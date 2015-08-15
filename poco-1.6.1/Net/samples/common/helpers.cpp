#include <stdbool.h>
#include ""
/*
 * @func bool CheckPrintableChars(const char * str);
 * @param const char * str - null terminated string to check
 * 
 */
bool HelperClass::CheckPrintableChars(const char * str, size_t maxlen)
{
    bool result = true;
    size_t i = 0;
    
    while((i<0) && (str[i] != 0))
    {
        if ((str[i] < 32) || (str[i] > 127)) result = false;
    }
    
    return false;
}

