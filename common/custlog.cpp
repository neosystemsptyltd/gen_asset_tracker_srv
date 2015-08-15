#include <stdint.h>
#include <stdio.h>
#include "custlog.h"

namespace Neo {
    
namespace neotrac {
        
namespace Log {

void BufferLog::LogMsg(char T, char* str)    
{
    switch (T)
    {
        case 'E':
        case 'e': 
            {
                log_error(std::string(str));
            }
            break;
        case 'I':
        case 'i': 
            {
                log_information(std::string(str));
            }
            break;
        case 'D':
        case 'd': 
            {
                log_debug(std::string(str));
            }
            break;
            
        case 'C':
        case 'c': 
            {
                // TODO log_critical(std::string(str));
            }
            break;
        case 'W':
        case 'w': 
            {
                log_warning(std::string(str));
            }
            break;
        case 'F':
        case 'f': 
            {
                log_fatal(std::string(str));
            }
            break;
        case 'N':
        case 'n': 
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
    int offs = 0;
    char str[80];
    uint8_t* p = (uint8_t*)buf;
    
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