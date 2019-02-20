
#pragma once

#ifdef POSIX
#include <pthread.h>
#endif

#include <Windows.h>


#ifdef _DEBUG
#define CS_TRACK_OWNER 1
#endif  // _DEBUG

#if CS_TRACK_OWNER
#define TRACK_OWNER(x) x
#else  // !CS_TRACK_OWNER
#define TRACK_OWNER(x)
#endif  // !CS_TRACK_OWNER

namespace base {

#ifdef WIN32
class CriticalSection {
public:
  CriticalSection() {
    InitializeCriticalSection(&crit_);
    // Windows docs say 0 is not a valid thread id
    TRACK_OWNER(thread_ = 0);
  }
  ~CriticalSection() {
    DeleteCriticalSection(&crit_);
  }
  void Enter() {
    EnterCriticalSection(&crit_);
    TRACK_OWNER(thread_ = GetCurrentThreadId());
  }
  void Leave() {
    TRACK_OWNER(thread_ = 0);
    LeaveCriticalSection(&crit_);
  }

#if CS_TRACK_OWNER
  bool CurrentThreadIsOwner() const { return thread_ == GetCurrentThreadId(); }
#endif  // CS_TRACK_OWNER

private:
  CRITICAL_SECTION crit_;
  TRACK_OWNER(DWORD thread_);  // The section's owning thread id
};
#endif // WIN32

#ifdef POSIX
class CriticalSection {
public:
  CriticalSection() {
    pthread_mutexattr_t mutex_attribute;
    pthread_mutexattr_settype(&mutex_attribute, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex_, &mutex_attribute);
  }
  ~CriticalSection() {
    pthread_mutex_destroy(&mutex_);
  }
  void Enter() {
    pthread_mutex_lock(&mutex_);
  }
  void Leave() {
    pthread_mutex_unlock(&mutex_);
  }
private:
  pthread_mutex_t mutex_;
};
#endif // POSIX

// CritScope, for serializing exection through a scope

class CritScope {
public:
  CritScope(CriticalSection *pcrit) {
    pcrit_ = pcrit;
    pcrit_->Enter();
  }
  ~CritScope() {
    pcrit_->Leave();
  }
private:
  CriticalSection *pcrit_;
};

} // namespace
