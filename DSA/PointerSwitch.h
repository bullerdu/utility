#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

class RefCnt {
private:
  int cnt_;
public:
  RefCnt() : cnt_(0) {
  }

  virtual ~RefCnt() {
  }

  void AddRef() {
    __sync_fetch_and_add(&cnt_, 1);
  }

  void Release() {
    int cnt = __sync_add_and_fetch(&cnt_, -1);
    if(0 == cnt) {
      delete this;
    }
  }
};

class PointerSwitch {
private:
  pthread_rwlock_t l_;
  RefCnt* obj_;

public:
  PointerSwitch() : obj_(NULL) {
    pthread_rwlock_init(&l_, NULL);
  }

  virtual ~PointerSwitch() {
    if(NULL != obj_) {
      obj_->Release();
    }
    pthread_rwlock_destroy(&l_);
  }

  PointerSwitch(const PointerSwitch& ps){
    obj_ = const_cast<PointerSwitch*>(&ps)->Get();
    pthread_rwlock_init(&l_, NULL);
  }
  PointerSwitch& operator =(const PointerSwitch& ps){
    RefCnt* old = NULL;
    RefCnt* obj = const_cast<PointerSwitch*>(&ps)->Get();
    if(obj_ == obj){
      return *this;
    }
    pthread_rwlock_wrlock(&l_);
    old = obj_;
    obj_ = obj;
    pthread_rwlock_unlock(&l_);
    if(NULL != old) {
      old->Release();
    }
    return *this;
  } 

  RefCnt* Get() {
    RefCnt* obj = NULL;
    pthread_rwlock_rdlock(&l_);
    if(NULL != obj_) {
      obj_->AddRef();
    }
    obj = obj_;
    pthread_rwlock_unlock(&l_);
    return obj;
  }

  void Set(RefCnt* obj) {
    RefCnt* old = NULL;
    obj->AddRef();
    pthread_rwlock_wrlock(&l_);
    old = obj_;
    obj_ = obj;
    pthread_rwlock_unlock(&l_);
    if(NULL != old) {
      old->Release();
    }
  }
};

