#include <stdint.h>
#include <stdio.h>
#include "custlog.h"

namespace Neo {
    
namespace neotrac {
        
namespace Log {

void BufferLog::LogMsg(char T, char* Msg)    
{
    switch (T)
    {
        case 'E','e': 
            {
                log_error(std::string(str));
            }
            break;
        case 'I','i': 
            {
                log_information(std::string(str));
            }
            break;
        case 'D','d': 
            {
                log_debug(std::string(str));
            }
            break;
        case 'C','c': 
            {
                log_critical(std::string(str));
            }
            break;
        case 'W','w': 
            {
                log_warning(std::string(str));
            }
            break;
        case 'F','f': 
            {
                log_fatal(std::string(str));
            }
            break;
        case 'N','n': 
            {
                log_fatal(std::string(str));
            }
            break;
        default: //case 'N','n': & all others
            {
                log_trace(std::string(str));
            }
            break;
    }
    
}

void BufferLog::Message(char T, void* buf, size_t len)
{
    size_t i;
    int offs
    char str[80];
    uint8_t p = (uint8_t*)buf;
    
    for(i=0; i<len; i++)
    {
        if ((i%16) == 0)
        {
            offs = 0;
            memset(str,0,sizeof(str));
        }
        offs += snprintf(str+offs,sizeof(str)-offs,"%02X ",p[i]);
        if ((i%16) == 15)
        {
            offs += snprintf(str+offs,sizeof(str)-offs,"\n");

            LogMsg(T,str);
        }
    }
    if ((i%16) != 0)
    {
        offs += snprintf(str+offs,sizeof(str)-offs,"\n");

        LogMsg(T,str);
    }
}

}}} // end namespace