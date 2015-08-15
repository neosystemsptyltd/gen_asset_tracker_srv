#ifndef CUSTLOG_H
#define CUSTLOG_H

#include "Poco/Util/LoggingSubsystem.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Net/StreamSocket.h"

namespace Neo {
    
namespace neotrac {
        
namespace Log {

using namespace Poco;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Net::StreamSocket;

#define log_information(x) poco_information(Application::instance().logger(),x)
#define log_debug(x)       poco_debug(Application::instance().logger(),      x)
#define log_error(x)       poco_error(Application::instance().logger(),      x)
//#define log_critical(x)    poco_cricital(Application::instance().logger(),   x)
#define log_warning(x)     poco_warning(Application::instance().logger(),    x)
#define log_fatal(x)       poco_fatal(Application::instance().logger(),      x)
#define log_notice(x)      poco_notice(Application::instance().logger(),     x)
#define log_trace(x)       poco_trace(Application::instance().logger(),      x)
    
#define logbuf_error(buf,len)       BufferLog::Message('E',buf,len);
#define logbuf_information(buf,len) BufferLog::Message('I',buf,len);
#define logbuf_debug(buf,len)       BufferLog::Message('D',buf,len);
#define logbuf_warning(buf,len)     BufferLog::Message('W',buf,len);
#define logbuf_fatal(buf,len)       BufferLog::Message('F',buf,len);
#define logbuf_notice(buf,len)      BufferLog::Message('N',buf,len);
#define logbuf_trace(buf,len)       BufferLog::Message('T',buf,len);

class BufferLog {
    public: 
        static void Message(char T, void* buf, size_t len);
        
    private:
        static void LogMsg(char T, char* str);
    
};

}}}// close Neo::neotrac::Log


#endif // CUSTLOG_H

