#include <stdbool.h>
#include <string.h>
#include "helpers.h"

namespace Neo {
    
namespace neotrac {
        
namespace Helpers {

/*
 * @func bool CheckPrintableChars(const char * str);
 * @param const char * str - null terminated string to check
 * 
 */
bool HelperClass::CheckPrintableChars(const char * str, size_t maxlen)
{
    bool result = true;
    size_t i = 0;
    
    while((i<maxlen) && (str[i] != 0))
    {
        if ((str[i] < 32) || ((uint8_t)str[i] > 127)) result = false;
        i++;
    }
    
    return result;
}

/*
 * @func bool CheckPrintableChars(const char * str);
 * @param const char * str - null terminated string to check
 * 
 */
void HelperClass::RemoveCRLF(char * str, size_t maxlen)
{    
    size_t l = strlen(str);
    
    if (l > maxlen) return;
    
    if ((str[l-1] == '\n') || (str[l-1] == '\r')) str[l-1] = 0;
    if ((str[l-2] == '\n') || (str[l-2] == '\r')) str[l-2] = 0;
}

}}} // end namespace