#ifndef SINGLETON_H_H
#define SINGLETON_H_H

#include <pthread.h>
#include <stdlib.h>

namespace utility
{
//  Private copy constructor and copy assignment ensure classes derived from  
//  class noncopyable cannot be copied.   
class noncopyable  
{  
  protected:  
    noncopyable() {}  
    ~noncopyable() {}  
  private: 
    noncopyable( const noncopyable& );  
    const noncopyable& operator=( const noncopyable& );  
};  

// singleton template
template<typename T>
class Singleton : public noncopyable
{
 public:
  static T& instance()
  {
    pthread_once(&ponce_, &Singleton::init);
    return *value_;
  }

 private:
  Singleton();
  ~Singleton();

  static void init()
  {
    value_ = new T();
    ::atexit(destroy);
  }

  static void destroy()
  {
    delete value_;
  }

 private:
  static pthread_once_t ponce_;
  static T*             value_;
};

template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;

}
#endif

