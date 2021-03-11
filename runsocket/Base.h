#ifndef SOCKETDEF_H
#define SOCKETDEF_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <pthread.h>
#include <arpa/inet.h>

#include <string>
#include <new>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cwchar>
#include <cwctype>
#include <cctype>
#include <cmath>
#include <cstdarg>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <set>
#include <stack>
#include <map>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <limits>
#include <functional>
#include <bitset>
#include <typeinfo>
#include <thread>
#include <mutex>
#include <chrono>
#include <stddef.h>

// Common
#ifndef NULL
#define NULL     0
#endif

/*
   Avoid "unused parameter" warnings
*/
#define UNUSED(x) (void)x;

#define ASSERT(...) // assert(expression)
#define ERROR(...)

// Object deletion macro
#define SAFE_DELETE(x) \
    { \
        delete x; \
        x = NULL; \
    }

// Array deletion macro
#define SAFE_DELETE_ARRAY(x) \
    { \
        delete[] x; \
        x = NULL; \
    }

// Ref cleanup macro
#define SAFE_RELEASE(x) \
    if (x) \
    { \
        (x)->release(); \
        x = NULL; \
    }

#define DISABLE_COPY(Class) \
    Class(const Class &);\
    Class &operator=(const Class &);


#define CHECK_GL_ERROR_DEBUG()
#define CC_SAFE_DELETE(x) SAFE_DELETE(x)

// #define OS_ANDROID 0

#endif // SOCKETDEF_H

