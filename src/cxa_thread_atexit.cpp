//===----------------------- cxa_thread_atexit.cpp ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "__config"
#ifndef _LIBCPP_HAS_NO_THREADS

#include "thread"
#include "utility"
#include "vector"
#ifdef _LIBCPP_HAS_THREAD_API_PTHREAD

static pthread_key_t tls_atexit_key;
static pthread_once_t tls_atexit_key_once = PTHREAD_ONCE_INIT;

typedef void (*tls_destructor_fn)(void*);
typedef std::pair<tls_destructor_fn, void *> tls_destructor_info;
typedef std::vector<tls_destructor_info> tls_destructor_info_list;

static void run_tls_atexit_funcs(void *ptr) {
  tls_destructor_info_list *il = (tls_destructor_info_list *) ptr;
  if (!ptr)
    return;

  for (auto &i : *il)
    i.first(i.second);

  delete il;
}

static void make_tls_atexit_key() {
  (void) pthread_key_create(&tls_atexit_key, run_tls_atexit_funcs);
}

namespace __cxxabiv1 {

// For BG/Q, __cxa_thread_atexit_impl is not available until glibc 2.18, but
// we're on 2.17. __cxa_thread_atexit is not in libsupc++ until 4.8.0, but
// we're on 4.7.2. So we'll just add a simple implementation here.

extern "C" {

_LIBCPP_FUNC_VIS int __cxa_thread_atexit(void (*dtor)(void *), void *obj,
                                            void *dso_symbol) throw() {
  (void) pthread_once(&tls_atexit_key_once, make_tls_atexit_key);

  void *ptr;
  if (!(ptr = pthread_getspecific(tls_atexit_key))) {
    tls_destructor_info_list *il = new tls_destructor_info_list;
    ptr = (void *) il;

    (void) pthread_setspecific(tls_atexit_key, ptr);
  }

  tls_destructor_info_list *il = (tls_destructor_info_list *) ptr;
  il->push_back(tls_destructor_info(dtor, obj));

  return 0;
}

} // extern "C"
} // namespace __cxxabiv1

#endif // _LIBCPP_HAS_THREAD_API_PTHREAD
#endif // _LIBCPP_HAS_NO_THREADS
