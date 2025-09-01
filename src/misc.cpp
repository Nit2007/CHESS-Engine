#include "defs.h"
#include "struct.h"
#include <ctime>       // C++ version of time.h, provides basic time functions like clock()
#include <chrono>      // For high-resolution timing (C++11) {more accurate than old ctime, supports nanosecond precision}

// Platform-specific headers for time functions
#ifdef WIN32
    #include <windows.h>    // Windows-specific timing functions { GetTickCount(), Sleep()}
#else// Unix/Linux/Mac
    #include <sys/time.h>   // Unix/Linux timing functions {gettimeofday(), struct timeval}
    #include <sys/select.h> // For input checking on Unix/Linux {select() function for checking if input is ready}
    #include <unistd.h>     // Unix standard definitions {Contains constants like STDIN_FILENO, basic Unix functions}
    #include <string.h>     // String manipulation functions {strlen(), strcpy(), strcmp()}
#endif
                          // -------TIME MEASUREMENT FUNCTIONS-------
int GetTimeMs() {//Get current time in milliseconds,This function provides cross-platform time measurement in milliseconds.
#ifdef WIN32
    // Windows implementation using GetTickCount()
    // GetTickCount() returns milliseconds since system startup
    // Simple and reliable for timing differences
    return GetTickCount();
#else
    // Unix/Linux implementation using gettimeofday()
    struct timeval t;
  /*struct timeval {
    time_t tv_sec;  // seconds
    long   tv_usec; // microseconds
    };*/
    gettimeofday(&t, NULL);
    
    // Convert seconds to milliseconds and add microseconds converted to milliseconds
    // t.tv_sec = seconds since January 1, 1970 (Unix epoch)
    // t.tv_usec = microseconds (need to divide by 1000 to get milliseconds)
    return t.tv_sec * 1000 + t.tv_usec / 1000;
#endif
}
