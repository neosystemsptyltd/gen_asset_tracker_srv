#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/DialogSocket.h"
#include "Poco/StreamCopier.h"
#include <iostream>
#include <string>
#include <stdint.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;

class Tester
{
public:
    typedef enum class
    {
        EXACT_MATCH,
        EXACT_CONTAINS,
        EXACT_STARTS_WITH,
        EXACT_ENDS_WITH,

        LAST        
    } RESPONSE_TYPE_E;
    
    Tester(const string& addr, Poco::UInt16 portNumber):
        _sa(addr, portNumber),
        _socket(_sa),
        _str(_socket)
    {
        
    }
    bool TestTextCommand(string cmd, string expect, RESPONSE_TYPE_E ResponseTypeE, Poco::Timespan t)
    {
        int rxcnt = 0;
        bool Result = false;
        string resp;
        
        try
        {
            _str.setReceiveTimeout(t);
            _str.sendString(cmd);

            rxcnt = _str.receiveMessage(resp);

            cout << "Rx count: " << rxcnt << ", string: " << resp << endl;
            
            if (rxcnt > 0)
            {
                switch (ResponseTypeE)
                {
                    case RESPONSE_TYPE_E::EXACT_MATCH:
                        if (resp == expect) Result = true;
                        break;

                    case RESPONSE_TYPE_E::EXACT_CONTAINS:
                        try 
                        {    
                            if (boost::contains(resp,expect)) Result = true;
                        } 
                        catch (boost::bad_lexical_cast) 
                        {
                            // bad parameter
                        }
                        break;

                    case RESPONSE_TYPE_E::EXACT_STARTS_WITH:
                        try 
                        {    
                            if (boost::starts_with(resp,expect)) Result = true;
                        } 
                        catch (boost::bad_lexical_cast) 
                        {
                            // bad parameter
                        }
                        break;

                    case RESPONSE_TYPE_E::EXACT_ENDS_WITH:
                        try 
                        {    
                            if (boost::ends_with(resp,expect)) Result = true;
                        } 
                        catch (boost::bad_lexical_cast) 
                        {
                            // bad parameter
                        }
                        break;

                    case RESPONSE_TYPE_E::LAST:
                        break;

                    default:
                        break;
                }
            }
        }
        catch(std::exception& exc)
        {
            cout << "Rx count: " << rxcnt << ", string: " << resp << endl;            
        }
    }
    
    int Run()
    {
        try
        {
            TestTextCommand("$HELP","so many commands so little time",RESPONSE_TYPE_E::EXACT_MATCH,Poco::Timespan(5,0));
        }
        catch(std::exception& exc)
        {
            cout << "Test failed: " << exc.what() << endl;
        }
    }
    
private:
    Poco::Net::SocketAddress _sa;
    Poco::Net::StreamSocket  _socket;
    Poco::Net::DialogSocket  _str;
};



int main(int argc, char** argv)
{
    try
    {
        Tester t("127.0.0.1",9911);
        
        t.Run();        
    }
    catch(std::exception& exc)
    {
        cout << "Error: " << exc.what() << endl;
    }
}
