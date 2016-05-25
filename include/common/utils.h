#ifndef COPIABLE_H
#define COPIABLE_H

namespace utils
{
    class NonCopyable{
        NonCopyable(const NonCopyable &);
        NonCopyable & operator = (const NonCopyable &);
    public:
#if __cplusplus >= 20110103l
        NonCopyable(const NonCopyable &&){}
#endif
        NonCopyable(){}
    };
}//namespace utils

namespace deleters
{
    template <class T> inline void deleter(T&);//stub
}//namespace deleter

template <class T>
class ScopedHandle:
        utils::NonCopyable
{
    T handle;
public:
    ScopedHandle() : handle(0) {}
    ScopedHandle(const T &h): handle(h) {}
    T & getRef() {
        return handle;
    }

    ~ScopedHandle() {
        if (handle) {
            deleters::deleter<T>(handle);
        }
    }
};


#endif // COPIABLE_H

