/* 
 * File:   helpers.h
 * Author: armand
 *
 * Created on 14 August 2015, 9:23 PM
 */

#ifndef HELPERS_H
#define	HELPERS_H

#include <stdint.h>

namespace Neo {
    
namespace neotrac {
        
namespace Helpers {
    
class HelperClass{
    public:
        static bool CheckPrintableChars(const char * str, size_t maxlen);    
};

}}} // end namespace



#endif	/* HELPERS_H */

