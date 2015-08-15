#ifndef CUSTLOG_H
#define CUSTLOG_H

#include "Poco/Util/LoggingSubsystem.h"

namespace Neo {
    
namespace neotrac {
        
namespace Log {

using namespace Poco;

#define log_information(x) poco_information(_app.logger(),std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)
#define log_debug(x)       poco_debug(_app.logger(),      std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)
#define log_error(x)       poco_error(_app.logger(),      std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)
#define log_critical(x)    poco_cricital(_app.logger(),   std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)
#define log_warning(x)     poco_warning(_app.logger(),    std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)
#define log_fatal(x)       poco_fatal(_app.logger(),    std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)
#define log_notice(x)      poco_notice(_app.logger(),     std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)
#define log_trace(x)       poco_trace(_app.logger(),      std::string(" [Addr]: ") + _socket.peerAddress().toString() + std::string(" : ")+x)
    
#define logbuf_error(buf,len) BufferLog::Message(buf,len);

class BufferLog {
    public: 
        static void Message(void* buf, size_t len);
        
    private:
        static void LogMsg(char T, char* Msg);
    
};

}}}// close Neo::neotrac::Log


#endif // CUSTLOG_H

