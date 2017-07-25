
#ifndef UTILS_DBG_H_
#define UTILS_DBG_H_

#include <iostream>

/**
 *  TRACE and DBG macros
 */
#if defined(DBG)
    #error DBG was already defined elsewhere
#elif defined(TRACE)
    #error TRACE was already defined elsewhere
#elif defined(__FILENAME__)
    #error __FILENAME__ was already defined elsewhere
#else
    #define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

    #define TRACE \
        do { std::cout << "TRACE: " << __FILENAME__ << "(" << __LINE__ << "): " << __PRETTY_FUNCTION__ << std::endl; } while(0)

    #define DBG(what) \
        do { std::cout << #what << "="<< (what) << std::endl; } while(0)

#endif

#endif /* UTILS_DBG_H_ */
