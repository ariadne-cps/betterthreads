
#ifndef BETTERTHREADS_MACROS_HPP
#define BETTERTHREADS_MACROS_HPP

#include <sstream>
#include <stdexcept>

#include "typedefs.hpp"

#define BETTERTHREADS_USING_CONSTRUCTORS(Class,Base) \
    template<class T,typename std::enable_if<std::is_convertible<T,Base>::value,int>::type=0> \
    Class(const T& t) : Base(t) { } \
    template<class T,typename std::enable_if<std::is_constructible<T,Base>::value and not std::is_convertible<T,Base>::value,int>::type=0> \
    explicit Class(const T& t) : Base(t) { } \
    template<class ...Args> Class(Args&&... args) : Base(std::forward<Args>(args)...) { } \

#define BETTERTHREADS_THROW(except,func,msg)          \
    { \
        StringStream ss; \
        ss << #except " in " << func << ": " << msg;    \
        throw except(ss.str()); \
    } \

#define BETTERTHREADS_ASSERT(expression) \
    { \
        bool assertion_result = static_cast<bool>(expression); \
        if(!assertion_result) { \
            BETTERTHREADS_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<__FUNCTION__,"Assertion `" << #expression << "' failed."); \
        } \
    } \


#ifndef NDEBUG
#define BETTERTHREADS_DEBUG_ASSERT_MSG(expression,error) \
    { \
        bool assertion_result = static_cast<bool>(expression); \
        if(!assertion_result) { \
            BETTERTHREADS_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<BETTERTHREADS_PRETTY_FUNCTION,"Assertion `" << #expression << "' failed.\n"<<"  "<<error); \
        } \
    } \

#else
#define BETTERTHREADS_DEBUG_ASSERT_MSG(expression,error) \
    { }
#endif


#ifndef NDEBUG
#define BETTERTHREADS_DEBUG_ASSERT(expression) \
    { \
        bool assertion_result = static_cast<bool>(expression); \
        if(!assertion_result) { \
            BETTERTHREADS_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<__FUNCTION__,"Assertion `" << #expression << "' failed."); \
        } \
    } \

#else
#define BETTERTHREADS_DEBUG_ASSERT(expression) \
    { }
#endif


#define BETTERTHREADS_PRECONDITION_MSG(expression,error)             \
    { \
        bool assertion_result = static_cast<bool>(expression); \
        if(!assertion_result) { \
            BETTERTHREADS_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<BETTERTHREADS_PRETTY_FUNCTION,"Precondition `" << #expression << "' failed.\n"<<"  "<<error); \
        } \
    } \

#define BETTERTHREADS_PRECONDITION(expression)             \
    { \
        bool assertion_result = static_cast<bool>(expression); \
        if(!assertion_result) { \
            BETTERTHREADS_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<BETTERTHREADS_PRETTY_FUNCTION,"Precondition `" << #expression << "' failed."); \
        } \
    } \

#ifndef NDEBUG
#define BETTERTHREADS_DEBUG_PRECONDITION(expression) \
    { \
        bool result = static_cast<bool>(expression); \
        if(!result) { \
            BETTERTHREADS_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<__FUNCTION__,"Precondition `" << #expression << "' failed."); \
        } \
    } \

#else
#define BETTERTHREADS_DEBUG_PRECONDITION(expression) \
    { }
#endif

#define BETTERTHREADS_FAIL_MSG(error)             \
    { \
        BETTERTHREADS_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<BETTERTHREADS_PRETTY_FUNCTION,"ErrorTag "<<error); \
    } \

#define BETTERTHREADS_ASSERT_MSG(expression,error)             \
    { \
        bool assertion_result = static_cast<bool>(expression); \
        if(!assertion_result) { \
            BETTERTHREADS_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<BETTERTHREADS_PRETTY_FUNCTION,"Assertion `" << #expression << "' failed.\n"<<"  "<<error); \
        } \
    } \

#define BETTERTHREADS_ASSERT_EQUAL(expression1,expression2)    \
    { \
        bool assertion_result = static_cast<bool>((expression1) == (expression2));       \
        if(!assertion_result) { \
            BETTERTHREADS_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<BETTERTHREADS_PRETTY_FUNCTION,"Assertion `" << #expression1 << "==" << #expression2 << "' failed.\n"<<"  "<<expression1<<" != "<<expression2); \
        } \
    } \

#define BETTERTHREADS_NOT_IMPLEMENTED                 \
    throw std::runtime_error(StringType("Not implemented: ")+BETTERTHREADS_PRETTY_FUNCTION);

#define BETTERTHREADS_DEPRECATED(fn,msg)          \
    static bool first_time=true; \
    if(first_time) { \
        first_time=false; \
        std::cerr << "DEPRECATED: Function " << #fn << " is deprecated. " << #msg << std::endl; \
    } \

#define BETTERTHREADS_NOTIFY(msg)          \
    {                                                                \
        std::cerr << "NOTIFICATION: " << msg << "" << std::endl;                \
    }

#define BETTERTHREADS_WARN(msg)          \
    {                                                                \
        std::cerr << "WARNING: " << msg << "" << std::endl;                \
    }

#define BETTERTHREADS_WARN_ONCE(msg)          \
    static bool first_time=true; \
    if(first_time) { \
        first_time=false; \
        std::cerr << "WARNING: " << msg << "" << std::endl; \
    } \

#define BETTERTHREADS_ERROR(msg)          \
    {                                                                \
        std::cerr << "ERROR: " << msg << "" << std::endl;                \
    }
                                                                  \
#if defined(linux) || defined(__linux) || defined(__linux__)
#define BETTERTHREADS_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define BETTERTHREADS_PRETTY_FUNCTION __FUNCTION__
#elif defined(darwin) || defined(__darwin) || defined(__darwin__) || defined(__APPLE__)
#define BETTERTHREADS_PRETTY_FUNCTION __PRETTY_FUNCTION__
#else
#define BETTERTHREADS_PRETTY_FUNCTION ""
#endif


#endif // BETTERTHREADS_MACROS_HPP
