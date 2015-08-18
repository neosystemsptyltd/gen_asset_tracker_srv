//
// neotracsrvtcp.cpp
//
// $Id: //poco/1.4/Net/samples/neotracsrvtcp/src/neotracsrvtcp.cpp#1 $
//
// This sample demonstrates the SocketReactor and SocketAcceptor classes.
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/NObserver.h"
#include "Poco/Exception.h"
#include "Poco/Thread.h"
#include "Poco/FIFOBuffer.h"
#include "Poco/Delegate.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Base64Decoder.h"
#include "Poco/StreamCopier.h"

#include <boost/tokenizer.hpp>

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdint.h>
#include "../common/custlog.h"
#include "../common/helpers.h"
#include "../common/neoprotocol.h"
#include "../common/database.h"


using Poco::Net::SocketReactor;
using Poco::Net::SocketAcceptor;
using Poco::Net::ReadableNotification;
using Poco::Net::WritableNotification;
using Poco::Net::ShutdownNotification;
using Poco::Net::TimeoutNotification;
using Poco::Net::ErrorNotification;
using Poco::Net::ShutdownNotification;
using Poco::Net::ServerSocket;
using Poco::Net::StreamSocket;
using Poco::NObserver;
using Poco::AutoPtr;
using Poco::Thread;
using Poco::FIFOBuffer;
using Poco::delegate;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Base64Decoder;
using Poco::StreamCopier;
using namespace std;
using namespace Neo::neotrac::Log;
using namespace Neo::neotrac::Helpers;
using namespace Neo::neotrac::Protocol;
using namespace Neo::neotrac::Database;
        
#define netlog_information(x) log_information(std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)
#define netlog_debug(x)       log_debug(      std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)
#define netlog_error(x)       log_error(      std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)
#define netlog_critical(x)    log_critical(   std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)
#define netlog_warning(x)     log_warning(    std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)
#define netlog_fatal(x)       log_fatal(      std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)
#define netlog_notice(x)      log_notice(     std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)
#define netlog_trace(x)       log_trace(      std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)

class ServerContext 
{
public:
    
    ServerContext()
    {
        log_trace("Server context created");   
        
        db_user = "postgres";
        db_passwd = "";  
        DatabaseConnected = false;
    }
    
    NeotracDataBase* getDatabase()
    {
        if (!DatabaseConnected)
        {
            CreateDatabaseConn();
            DatabaseConnected = true;
        }
        return _db.get();
    }
    
private:
    string db_user;
    string db_passwd;
    bool   DatabaseConnected;
    
    unique_ptr<NeotracDataBase> _db;    
    
    void CreateDatabaseConn()
    {
        log_trace("Database not opened before");
        unique_ptr<NeotracDataBase> tempdb(new NeotracDataBase(db_user,db_passwd));
        _db = std::move(tempdb);        
    }
};

class NeoSocketReactor  : public SocketReactor {
    
public:
    NeoSocketReactor(ServerContext& srv) : SocketReactor(),
        _srv(srv)
    {
        log_trace("Neo Socket reactor created");
    }
    
    ServerContext& GetServer()
    {
        return _srv;
    }
    
private:
    ServerContext& _srv;    
};

class neotracsrvServiceHandler
	/// I/O handler class. This class (un)registers handlers for I/O based on
	/// data availability. To ensure non-blocking behavior and alleviate spurious
	/// socket writability callback triggering when no data to be sent is available,
	/// FIFO buffers are used. I/O FIFOBuffer sends notifications on transitions
	/// from [1] non-readable (i.e. empty) to readable, [2] writable to non-writable 
	/// (i.e. full) and [3] non-writable (i.e. full) to writable.
	/// Based on these notifications, the handler member functions react by
	/// enabling/disabling respective reactor framework notifications.
{
public:
	neotracsrvServiceHandler(StreamSocket& socket, SocketReactor& reactor):
		_socket(socket),
		_reactor(reactor),
		_fifoIn(BUFFER_SIZE, true),
		_fifoOut(BUFFER_SIZE, true),
		_app(Application::instance()),
		mybuffer(BUFFER_SIZE),
		outbuffer(BUFFER_SIZE),
        _neoSocketReactor(dynamic_cast<NeoSocketReactor&>(reactor))
	{
		netlog_information("New Connection");

		_reactor.addEventHandler(_socket, NObserver<neotracsrvServiceHandler, ReadableNotification>(*this, &neotracsrvServiceHandler::onSocketReadable));
		_reactor.addEventHandler(_socket, NObserver<neotracsrvServiceHandler, ShutdownNotification>(*this, &neotracsrvServiceHandler::onSocketShutdown));
		_reactor.addEventHandler(_socket, NObserver<neotracsrvServiceHandler, ErrorNotification>(*this, &neotracsrvServiceHandler::onSocketError));
		_reactor.addEventHandler(_socket, NObserver<neotracsrvServiceHandler, TimeoutNotification>(*this, &neotracsrvServiceHandler::onSocketTimeout));
  
		_fifoOut.readable += delegate(this, &neotracsrvServiceHandler::onFIFOOutReadable);
		_fifoIn.writable += delegate(this, &neotracsrvServiceHandler::onFIFOInWritable);
                
		MsgIdx = 0;
		u32Timeout = CONNECTION_TIMEOUT;
        _Closerequested = false;        
        
        netlog_trace("Database information: " + _neoSocketReactor.GetServer().getDatabase()->getDescr());
	}
	
	~neotracsrvServiceHandler()
	{
		netlog_information("Disconnecting");
		
		_reactor.removeEventHandler(_socket, NObserver<neotracsrvServiceHandler, ReadableNotification>(*this, &neotracsrvServiceHandler::onSocketReadable));
		_reactor.removeEventHandler(_socket, NObserver<neotracsrvServiceHandler, WritableNotification>(*this, &neotracsrvServiceHandler::onSocketWritable));
		_reactor.removeEventHandler(_socket, NObserver<neotracsrvServiceHandler, ShutdownNotification>(*this, &neotracsrvServiceHandler::onSocketShutdown));
		_reactor.removeEventHandler(_socket, NObserver<neotracsrvServiceHandler, ErrorNotification>(*this, &neotracsrvServiceHandler::onSocketError));
		_reactor.removeEventHandler(_socket, NObserver<neotracsrvServiceHandler, TimeoutNotification>(*this, &neotracsrvServiceHandler::onSocketTimeout));

		_fifoOut.readable -= delegate(this, &neotracsrvServiceHandler::onFIFOOutReadable);
		_fifoIn.writable -= delegate(this, &neotracsrvServiceHandler::onFIFOInWritable);
	}
	
	void onFIFOOutReadable(bool& b)
	{
		if (b)
			_reactor.addEventHandler(_socket, NObserver<neotracsrvServiceHandler, WritableNotification>(*this, &neotracsrvServiceHandler::onSocketWritable));
		else
			_reactor.removeEventHandler(_socket, NObserver<neotracsrvServiceHandler, WritableNotification>(*this, &neotracsrvServiceHandler::onSocketWritable));
	}
	
	void onFIFOInWritable(bool& b)
	{
		if (b)                    
			_reactor.addEventHandler(_socket, NObserver<neotracsrvServiceHandler, ReadableNotification>(*this, &neotracsrvServiceHandler::onSocketReadable));
		else
			_reactor.removeEventHandler(_socket, NObserver<neotracsrvServiceHandler, ReadableNotification>(*this, &neotracsrvServiceHandler::onSocketReadable));
	}
	
    /*
     * @method: void ProcessTextCommand(void)
     * @desc Process text commands on this server
     */
    void ProcessTextCommand(void)
    {        
		char msgbuf[80];
        char* msg = msgbuf;
    
        HelperClass::RemoveCRLF(_message,sizeof(_message));
        if (!HelperClass::CheckPrintableChars(_message,sizeof(_message)))
        {
            netlog_error("Text command contains non printable characters");
            logbuf_error(_message,MsgIdx);
            return;
        }
        
		netlog_information("Incoming message: " + std::string(_message));
            
		memset(msgbuf,0,sizeof(msgbuf));
		if (strstr(_message,"$HELP") == _message)
		{
            netlog_trace("Processing HELP command");            
			sprintf(msg,"so many commands so little time\r\n");
		}
		else if (strstr(_message,"$ECHO") == _message)
		{
            netlog_trace("Processing ECHO command");            
			sprintf(msg,"ECHO Command\r\n");
		}
		else if (strstr(_message,"$TEST") == _message)
		{
            netlog_trace("Processing TEST command");            
			sprintf(msg,"Server connection OK\r\n");
		}
		else if (strstr(_message,"$CLOSE") == _message)
		{
            netlog_trace("Processing CLOSE command");   
            _Closerequested = true;
		}
		else
		{
            netlog_trace("unknown text command");            
			sprintf(msg,"Error: unknown command\r\n");
		}

		if (strlen(msg) > 0)
		{
            std::string s("Transmit response: ");
            s += msg;
            
            netlog_trace(s);
			outbuffer.assign(msg,strlen(msg));
			_fifoOut.write(outbuffer);
		}        
    }

    /*
     * @method: void ProcessRegistration(void)
     * @descr: Process a registration message
     */
    void ProcessRegistration(void)
    {
        netlog_trace("Process registration message");
    }
    
    /*
     * @method: void ProcessEventReport(void)
     * @descr: Process and event report
     */
    void ProcessEventReport(void)
    {
        netlog_trace("Process event report");
    }
    
    /*
     * @method: void ProcessDecodedMessge(void)
     * @desr: This is where the decoded messages are processed
     * 
     * @note: short messages are message that will also fit into an SMS if needed
     */
    void ProcessShortDecodedMessage(void)
    {
        netlog_trace("Process short decoded message");
        switch(ShortPacket.MessageTypeE)
        {
            case NEOPROTOCOL_MESSAGE_TYPE_E_REGISTRATION:
                ProcessRegistration();
                break;
                
            case NEOPROTOCOL_MESSAGE_TYPE_E_REPORT_EVENT:
                ProcessEventReport();
                break;
                
            default:
                netlog_trace("message type not recognisede");
                break;
        }
    }
    
    /**
     * @void ProcessSMSGatewayMessage(void)
     * process a message from the SMS gateway
     */
    void ProcessSMSGatewayMessage(void)
    {
        
        // str = from the second character in _message
        std::string str(_message+1);
        
        netlog_trace("Processing SMS gateway message");
        
        // declare the tokenizer type
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        
        // specify the separator characters
        boost::char_separator<char> sep(", ");
        
        SMSSource = "";
        SMSTime = "";
        SMSDecoded = "";
        SMSEncoded = "";
        
        tokenizer tokens(str, sep);
        int state = 0;
        
        for (tokenizer::iterator tok_iter = tokens.begin(); (tok_iter != tokens.end()) && (state <= 2); ++tok_iter)
        {
            if (state == 0)
            {
                // this is the SMS number
                netlog_trace("Received from SMS Number: " + *tok_iter);
                SMSSource = *tok_iter;
                state = 1;
            }
            else if (state == 1)
            {
                // this is the time
                netlog_trace("Received time: " + *tok_iter);
                SMSTime = *tok_iter;
                state = 2;
            }
            else if (state == 2)
            {
                // this is the message
                netlog_trace("Message: " + *tok_iter);
                SMSEncoded = *tok_iter;
                state = 3;
            }
        }        
        if (state == 3)
        {            
            netlog_trace("Decode the message");
            
            logbuf_trace(&ShortPacket,sizeof(ShortPacket));
            
            ProcessShortDecodedMessage();
        }
    }
    
    /*
     * @method: void ProcessIncomingMessage(void)
     * 
     * @note Processes incoming messages
     */
	void ProcessIncomingMessage(void)
	{
        // determine the type of message, based on the first character
        
        switch (_message[0])
        {
            case '$': // human readable text commands
                {
                    netlog_trace("Processing text command");
                    ProcessTextCommand();
                }
                break;
                
            case '#': // base91 encoded message
                {
                    netlog_trace("Processing base91 encoded message");

                }
                break;
                
            case '@': // base64 encoded message
                {
                    netlog_trace("Processing base64 encoded message");

                }
                break;
                
            case '&': // escaped binary message
                {
                    netlog_trace("Processing escaped binary message");

                }
                break;
                
            case '*': // SMS gateway message
                {
                    netlog_trace("Processing message from SMS gateway");
                    ProcessSMSGatewayMessage();
                }
                break;
            default: // not understood - no response
                {
                    netlog_trace("Incoming message header unknown - ignored");
                }
                break;                    
        }        
	}
            
    /*
     * @method void onSocketReadable(const AutoPtr<ReadableNotification>& pNf)
     * @param const AutoPtr<ReadableNotification>& pNf = readable notification ref
     * 
     * @note gets called when data is ready to be read
     */
	void onSocketReadable(const AutoPtr<ReadableNotification> __attribute__((unused)) & pNf)
	{		
		try
		{
			// some socket implementations (windows) report available 
			// bytes on client disconnect, so we  double-check here
			if (_socket.available())
			{
				int n = _socket.receiveBytes(_fifoIn);
				//_fifoIn.drain(_fifoOut.write(_fifoIn.buffer()));
													
				int i=0;
				u32Timeout = CONNECTION_TIMEOUT;

				_fifoIn.read(mybuffer);
				if (n > 0)
				{            
					while((n > 0) && (!_Closerequested))
					{
                        _message[MsgIdx] = mybuffer[i];
                        if (MsgIdx < (BUFFER_SIZE-2))
                        {
                            MsgIdx++;
                        }
						if (mybuffer[i] == '\n')
						{
							_message[MsgIdx] = 0;
							ProcessIncomingMessage();
							MsgIdx = 0;
						}
						i++;
						n--;
					}
				}		
				else
				{
					netlog_information("Connection reset by peer normally");
				}
                _fifoIn.drain();
                
                if (_Closerequested)
                {
                    netlog_information("Remote requested close");
                    delete this;                    
                }
			}
			else
			{
				netlog_information("Nothing to be read - connection closed");
				delete this;
			}
		}
		catch(Poco::Net::NetException& exc)
		{
			netlog_information("ERROR: Exception: " + exc.displayText());
			delete this;
		}
			
	}
	
	void onSocketWritable(const AutoPtr<WritableNotification> __attribute__((unused)) & pNf)
	{
		_socket.sendBytes(_fifoOut);
	}

	void onSocketShutdown(const AutoPtr<ShutdownNotification> __attribute__((unused)) & pNf)
	{
		netlog_information("Shutdown - conn close");		
		
		delete this;
	}
	
	/**
	* @Brief Event Handler when Socket throws an error
	*/
	void onSocketError(const AutoPtr<ErrorNotification> __attribute__((unused)) & pNf)
	{
		netlog_information("Error occurred");		
	}

	/**
	* @Brief Event Handler when Socket times-out
	*/
	void onSocketTimeout(const AutoPtr<TimeoutNotification> __attribute__((unused)) & pNf)
	{
		char intstr[40];
		try
		{
			if (u32Timeout > 0) 
			{
				u32Timeout--;
			}
			if ((u32Timeout % 10) == 0)
			{
				sprintf(intstr,"Timeout %d on",u32Timeout);
				netlog_debug(intstr);		
			}
			if (u32Timeout == 0)
			{
				netlog_information("Connection Timeout");		
				delete this;
			}
		}
		catch (...)
		{
            netlog_trace("Exception");            
		}
	}
	
private:
	enum
	{
		BUFFER_SIZE = 1024
	};
	
	enum
	{
		CONNECTION_TIMEOUT = 100
	};
	
	StreamSocket   	_socket;
	SocketReactor& 	_reactor;
	FIFOBuffer     	_fifoIn;
	FIFOBuffer     	_fifoOut;
	uint32_t       	u32Timeout;
	Application& 	_app;
	bool            _Closerequested;
    
	Poco::Buffer<char>  mybuffer;
	Poco::Buffer<char>  outbuffer;

    NeoSocketReactor& _neoSocketReactor;

	int MsgIdx;
	char _message[BUFFER_SIZE];
    
    NEOPROTOCOL_ST_SHORT_PACKET ShortPacket;
    
    std::string SMSSource;
    std::string SMSTime;
    std::string SMSEncoded;
    std::string SMSDecoded;       
};

class neotracsrv: public Poco::Util::ServerApplication
	/// The main application class.
	///
	/// This class handles command-line arguments and
	/// configuration files.
	/// Start the neotracsrv executable with the help
	/// option (/help on Windows, --help on Unix) for
	/// the available command line options.
	///
	/// To use the sample configuration file (neotracsrv.properties),
	/// copy the file to the directory where the neotracsrv executable
	/// resides. If you start the debug version of the neotracsrv
	/// (neotracsrvd[.exe]), you must also create a copy of the configuration
	/// file named neotracsrvd.properties. In the configuration file, you
	/// can specify the port on which the server is listening (default
	/// 9977) and the format of the date/time string sent back to the client.
	///
	/// To test the neotracsrv you can use any telnet client (telnet localhost 9977).
{
public:
	neotracsrv(): _helpRequested(false)
	{
	}
	
	~neotracsrv()
	{
	}

protected:
	void initialize(Application& self)
	{
		loadConfiguration(); // load default configuration files, if present
		ServerApplication::initialize(self);
	}
		
	void uninitialize()
	{
		ServerApplication::uninitialize();
	}

	void defineOptions(OptionSet& options)
	{
		ServerApplication::defineOptions(options);
		
		options.addOption(
			Option("help", "h", "display help information on command line arguments")
				.required(false)
				.repeatable(false));
	}

	void handleOption(const std::string& name, const std::string& value)
	{
		ServerApplication::handleOption(name, value);

		if (name == "help")
			_helpRequested = true;
	}

	void displayHelp()
	{
		HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader("Neo Systems (Pty) Ltd tracking server - tcp edition.");
		helpFormatter.format(std::cout);
	}

	int main(const std::vector<std::string> __attribute__((unused)) & args)
	{
		if (_helpRequested)
		{
			displayHelp();
		}
		else
		{
            ServerContext srv;
                        
			// get parameters from configuration file
			unsigned short port = (unsigned short) config().getInt("neotracsrv.port", 9977);
			
			// set-up a server socket
			ServerSocket svs(port);
			// set-up a SocketReactor...
			NeoSocketReactor reactor(srv);
			// ... and a SocketAcceptor
			SocketAcceptor<neotracsrvServiceHandler> acceptor(svs,reactor);
			// run the reactor in its own thread so that we can wait for 
			// a termination request
			Thread thread;
			thread.start(reactor);
			
			Application& app = Application::instance();
			poco_information(app.logger(),string("Compiler version: ") + to_string(__GNUC__));
			poco_information(app.logger(),string("C++ version: ") + to_string(__cplusplus));
			poco_information(app.logger(),string("Build date & Time: ") + __DATE__ + ", " + __TIME__);
			poco_information(app.logger(),"Server started");
			
			// wait for CTRL-C or kill
			waitForTerminationRequest();
			poco_information(app.logger(),"Termination requested.");
			
			// Stop the SocketReactor
			reactor.stop();
			poco_information(app.logger(),"Reactor stopped.");
			
			thread.join();
			poco_information(app.logger(),"Thread joined.");
			
			poco_information(app.logger(),"Server stopped.");
		}
		return Application::EXIT_OK;
	}
	
private:
	bool _helpRequested;
};


int main(int argc, char** argv)
{
	neotracsrv app;
	return app.run(argc, argv);
}


/*
 * 
 * 
 * 
 * template <class ServiceHandler> class NeoSocketAcceptor : SocketAcceptor<ServiceHandler>
{
public:
    NeoSocketAcceptor(ServerSocket& socket, SocketReactor& reactor, ServerContext& srv):
        SocketAcceptor<ServiceHandler>(socket,reactor),
            _reactor(reactor),
            _srv(srv)
        {
            
        }
        
protected:
	virtual ServiceHandler* createServiceHandler(StreamSocket& socket)
		/// Create and initialize a new ServiceHandler instance.
		///
		/// Subclasses can override this method.
	{
		return new ServiceHandler(socket,_reactor,_srv);
	}
    
    
    SocketReactor& _reactor;
    ServerContext& _srv;
};

 */