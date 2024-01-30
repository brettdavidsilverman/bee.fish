/*

Copyright (c) 2014, NVIDIA Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef ATOMIC_FLAG_HPP
#define ATOMIC_FLAG_HPP

#include <atomic>
#include <chrono>
#include <thread>
#include <cassert>

#ifdef WIN32
#include <windows.h>
#endif //WIN32

#ifdef __linux__
#include <time.h>
#include <cstring>
#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <climits>
#endif //__linux__

namespace std {
    namespace experimental {
        inline namespace concurrency_v2 {

#ifdef __linux__
            inline void __atomic_yield() { sched_yield(); }
#else
            inline void __atomic_yield() { this_thread::yield(); }
#endif

#if defined(_MSC_VER)
#if (defined(_M_X86) || defined(_M_X64))
#define __atomic_x86
#elif defined(_M_ARM)
#define __atomic_arm
#endif
#elif defined(__GNUC__) 
#if (defined(__i386__) || defined(__x86_64__))
#define __atomic_x86
#elif defined(__arm__)
#define __atomic_arm
#endif
#endif

#ifdef __GNUC__
#define __atomic_gcc_inline __attribute__((always_inline))
#define __atomic_gcc_dont_inline __attribute__((noinline))
#else
#define __atomic_gcc_inline inline
#define __atomic_gcc_dont_inline
#endif

#if defined(__GNUC__)
#define __atomic_expect __builtin_expect
#else
#define __atomic_expect(c,e) (c)
#endif

#ifdef __linux__
            // On Linux, we make use of the kernel memory wait operations. These have been available for a long time.
            template < class Rep, class Period>
            timespec __atomic_to_timespec(chrono::duration<Rep, Period> const& delta) {
                struct timespec ts;
                ts.tv_sec = static_cast<long>(chrono::duration_cast<chrono::seconds>(delta).count());
                ts.tv_nsec = static_cast<long>(chrono::duration_cast<chrono::nanoseconds>(delta).count());
                return ts;
            }
            template <class A>
            inline const void* __atomic_fixalign(A& a) {
                static_assert(sizeof(A) <= sizeof(int), "Linux only supports 'int' for Futex.");
                return (const void*)((intptr_t)&a & ~(sizeof(int) - 1));
            }
            inline int __atomic_readint(const void* p) {
                int i;
                memcpy(&i, p, sizeof(int));
                return i;
            }
            template <class A, class V>
            inline void __atomic_wait(A& a, V v) {
                auto p = __atomic_fixalign(a);
                auto i = __atomic_readint(p);
                asm volatile("" ::: "memory");
                if (a.load(memory_order_relaxed) != v) return;
                syscall(SYS_futex, p, FUTEX_WAIT_PRIVATE, i, 0, 0, 0);
            }
            template <class A, class V, class Rep, class Period>
            void __atomic_wait_timed(A& a, V v, const chrono::duration<Rep, Period>& t) {
                auto p = __atomic_fixalign(a);
                auto i = __atomic_readint(p);
                asm volatile("" ::: "memory");
                if (a.load(memory_order_relaxed) != v) return;
                syscall(SYS_futex, p, FUTEX_WAIT_PRIVATE, i, __atomic_to_timespec(t), 0, 0);
            }
            template <class A>
            inline void __atomic_wake_one(A& a) {
                syscall(SYS_futex, __atomic_fixalign(a), FUTEX_WAKE_PRIVATE, 1, 0, 0, 0);
            }
            template <class A>
            inline void __atomic_wake_all(A& a) {
                syscall(SYS_futex, __atomic_fixalign(a), FUTEX_WAKE_PRIVATE, INT_MAX, 0, 0, 0);
            }
            template <class A, class V>
            inline void __atomic_wait(volatile A& a, V v) {
                auto p = __atomic_fixalign(a);
                auto i = __atomic_readint(p);
                asm volatile("" ::: "memory");
                if (a.load(memory_order_relaxed) != v) return;
                syscall(SYS_futex, p, FUTEX_WAIT, i, 0, 0, 0);
            }
            template <class A, class V, class Rep, class Period>
            void __atomic_wait_timed(volatile A& a, V v, const chrono::duration<Rep, Period>& t) {
                auto p = __atomic_fixalign(a);
                auto i = __atomic_readint(p);
                asm volatile("" ::: "memory");
                if (a.load(memory_order_relaxed) != v) return;
                syscall(SYS_futex, p, FUTEX_WAIT, i, __atomic_to_timespec(t), 0, 0);
            }
            template <class A>
            inline void __atomic_wake_one(volatile A& a) {
                syscall(SYS_futex, __atomic_fixalign(a), FUTEX_WAKE, 1, 0, 0, 0);
            }
            template <class A>
            inline void __atomic_wake_all(volatile A& a) {
                syscall(SYS_futex, __atomic_fixalign(a), FUTEX_WAKE, INT_MAX, 0, 0, 0);
            }
#define __atomic_flag_fast_path
#endif // __linux__

#if defined(WIN32) && _WIN32_WINNT >= 0x0602
            // On Windows, we make use of the kernel memory wait operations as well. These first became available with Windows 8.
            template <class A, class V>
            void __atomic_wait(A& a, V v) {
                static_assert(sizeof(V) <= 8, "Windows only allows sizes between 1B and 8B for WaitOnAddress.");
                WaitOnAddress((PVOID)&a, (PVOID)&v, sizeof(v), -1);
            }
            template <class A, class V, class Rep, class Period>
            void __atomic_wait_timed(A& a, V v, chrono::duration<Rep, Period> const& delta) {
                static_assert(sizeof(V) <= 8, "Windows only allows sizes between 1B and 8B for WaitOnAddress.");
                WaitOnAddress((PVOID)&a, (PVOID)&v, sizeof(v), chrono::duration_cast<chrono::milliseconds>(delta).count());
            }
            template <class A>
            inline void __atomic_wake_one(A& a) {
                WakeByAddressSingle((PVOID)&a);
            }
            template <class A>
            inline void __atomic_wake_all(A& a) {
                WakeByAddressAll((PVOID)&a);
            }

#define __atomic_flag_fast_path
#endif // defined(WIN32) && _WIN32_WINNT >= 0x0602

            //#undef __atomic_flag_fast_path

                        // A simple exponential back-off helper that is designed to cover the space between (1<<__magic_number_3) and __magic_number_4
            class __atomic_exponential_backoff {
                int microseconds = 50;
            public:
                void sleep(int us = 0) {
                    if (us != 0)
                        microseconds = us;
                    this_thread::sleep_for(chrono::microseconds(microseconds));
                    // Avoiding the use of std::min here, to keep includes minimal
                    auto next_microseconds = microseconds + (microseconds >> 2);
                    microseconds = next_microseconds < 8192 ? next_microseconds : 8192;
                }
            };

            enum class atomic_notify {
                all, one, none
            };

            typedef struct atomic_flag {

                typedef uint8_t __base_t;

                static constexpr __base_t __valubit = 1;
#ifdef __atomic_flag_fast_path
                static constexpr __base_t __contbit = 2;
                static constexpr __base_t __lockbit = 4;
#endif

#ifdef __atomic_flag_fast_path
                template <class A>
                __atomic_gcc_dont_inline static bool __test_and_set_slow(A& atom, __base_t old, memory_order order, atomic_notify notify) noexcept {
                    while ((old & __valubit) == 0) {
                        old &= __contbit;
                        __base_t const lock = (old & __contbit ? __lockbit : 0);
                        if (atom.compare_exchange_weak(old, __valubit | lock, order, memory_order_relaxed)) {
                            if (lock) {
                                switch (notify) {
                                case atomic_notify::all: __atomic_wake_all(atom); break;
                                case atomic_notify::one: __atomic_wake_one(atom); break;
                                case atomic_notify::none: break;
                                }
                                atom.fetch_and(~lock, memory_order_relaxed);
                            }
                            return false;
                        }
                    }
                    return true;
                }
#endif

                template <class A>
                __atomic_gcc_inline static bool __test_and_set(A& atom, memory_order order, atomic_notify notify) noexcept {
                    __base_t old = 0;
                    bool const success = atom.compare_exchange_weak(old, __valubit, order, memory_order_relaxed);
                    bool retcode = (old & __valubit) == 1;
#ifdef __atomic_flag_fast_path
                    if (__atomic_expect(!success && !retcode, 0))
                        retcode = __test_and_set_slow(atom, old, order, notify);
#endif
#ifdef __atomic_arm
                    if (!retcode) {
                        __asm__ __volatile__(
                            "   dsb\n"
                            "   sev"
                        );
                    }
#endif
                    return retcode;
                }

                inline bool test_and_set(memory_order order = memory_order_seq_cst, atomic_notify notify = atomic_notify::all) noexcept {

                    return __test_and_set(atom, order, notify);
                }
                inline bool test_and_set(memory_order order = memory_order_seq_cst, atomic_notify notify = atomic_notify::all) volatile noexcept {

                    return __test_and_set(atom, order, notify);
                }

#ifdef __atomic_flag_fast_path
                template <class A>
                __atomic_gcc_dont_inline static void __clear_slow(A& atom, __base_t old, memory_order order, atomic_notify notify) noexcept {
                    while (1) {
                        old &= (__contbit | __valubit);
                        __base_t const lock = (old & __contbit) ? __lockbit : 0;
                        if (atom.compare_exchange_weak(old, lock, order, memory_order_relaxed)) {
                            if (lock) {
                                switch (notify) {
                                case atomic_notify::all: __atomic_wake_all(atom); break;
                                case atomic_notify::one: __atomic_wake_one(atom); break;
                                case atomic_notify::none: break;
                                }
                                atom.fetch_and(~lock, memory_order_relaxed);
                            }
                            break;
                        }
                    }
                }
#endif

                template <class A>
                __atomic_gcc_inline static void __clear(A& atom, memory_order order, atomic_notify notify) noexcept {
#ifdef __atomic_flag_fast_path
                    __base_t old = __valubit;
                    bool const success = atom.compare_exchange_weak(old, 0, order, memory_order_relaxed);
                    if (__atomic_expect(!success, 0)) {
                        bool const success2 = ((old & ~__valubit) == 0) && atom.compare_exchange_weak(old, 0, order, memory_order_relaxed);
                        if (__atomic_expect(!success2, 0))
                            __clear_slow(atom, old, order, notify);
                    }
#else
                    atom.store(0, order);
#endif
#ifdef __atomic_arm
                    __asm__ __volatile__(
                        "   dsb\n"
                        "   sev"
                    );
#endif
                }

                inline void clear(memory_order order = memory_order_seq_cst, atomic_notify notify = atomic_notify::all) noexcept {

                    __clear(atom, order, notify);
                }

                inline void clear(memory_order order = memory_order_seq_cst, atomic_notify notify = atomic_notify::all) volatile noexcept {

                    __clear(atom, order, notify);
                }

                inline void set(bool state, memory_order order = memory_order_seq_cst, atomic_notify notify = atomic_notify::all) noexcept {

                    if (state)
                        test_and_set(order, notify);
                    else
                        clear(order, notify);
                }
                inline void set(bool state, memory_order order = memory_order_seq_cst, atomic_notify notify = atomic_notify::all) volatile noexcept {

                    if (state)
                        test_and_set(order, notify);
                    else
                        clear(order, notify);
                }

                template <class A>
                __atomic_gcc_dont_inline static void __wait_slow(A& atom, bool set, memory_order order) noexcept {

                    __base_t old = atom.load(order);
                    __base_t const expectbit = (set ? __valubit : 0);
                    if ((old & __valubit) != expectbit) {
                        __atomic_exponential_backoff b;
#ifdef __atomic_flag_fast_path
                        for (int i = 0; i < 2; ++i) {
#else
                        while (1) {
#endif
                            b.sleep();
                            old = atom.load(order);
                            if ((old & __valubit) == expectbit) break;
                        }
                    }
#ifdef __atomic_flag_fast_path
                    if ((old & __valubit) != expectbit) {
                        while (1) {
                            old = atom.fetch_or(__contbit, memory_order_relaxed) | __contbit;
                            if ((old & __valubit) == expectbit) break;
                            __atomic_wait(atom, old);
                            old = atom.load(order);
                            if ((old & __valubit) == expectbit) break;
                        }
                    }
                    while (old & __lockbit)
                        old = atom.load(memory_order_relaxed);
#endif
                }

                template <class A>
                __atomic_gcc_inline static bool __wait_fast(A& atom, bool set, memory_order order) noexcept {

                    __base_t old = atom.load(order);
                    __base_t const expectbit = (set ? __valubit : 0);
                    if (__atomic_expect(old == expectbit, 1))
                        return true;
#ifdef __atomic_arm
                    if ((old & __valubit) != expectbit)
                        for (int i = 0; i < 4; ++i) {
                            __base_t const tmp = old;
                            __asm__ __volatile__(
                                "ldrex %0, [%1]\n"
                                "cmp %0, %2\n"
                                "it eq\n"
                                "wfeeq.n\n"
                                "nop.w\n"
                                : "=&r" (old) : "r" (&atom), "r" (tmp) : "cc"
                            );
                            if ((old & __valubit) == expectbit) { atomic_thread_fence(order); break; }
                        }
#endif
                    if ((old & __valubit) != expectbit)
                        for (int i = 0; i < 32; ++i) {
                            __atomic_yield();
                            old = atom.load(order);
                            if ((old & __valubit) == expectbit) break;
                        }
                    return old == expectbit;
                }

                inline void wait(bool set, memory_order order = memory_order_seq_cst) const noexcept {

                    bool const success = __wait_fast(atom, set, order);
                    if (__atomic_expect(!success, 0))
                        __wait_slow(atom, set, order);
                }
                inline void wait(bool set, memory_order order = memory_order_seq_cst) const volatile noexcept {

                    bool const success = __wait_fast(atom, set, order);
                    if (__atomic_expect(!success, 0))
                        __wait_slow(atom, set, order);
                }

                bool test(memory_order order = memory_order_seq_cst) const noexcept {

                    return atom.load(order) & __valubit;
                }
                bool test(memory_order order = memory_order_seq_cst) const volatile noexcept {

                    return atom.load(order) & __valubit;
                }

                using clock = conditional<chrono::high_resolution_clock::is_steady,
                    chrono::high_resolution_clock, chrono::steady_clock>::type;

                template <class A, class Clock, class Duration>
                __atomic_gcc_dont_inline static bool __wait_slow_timed(A& atom, bool set, chrono::time_point<Clock, Duration> const& abs_time, memory_order order) noexcept {

                    __base_t old = atom.load(order);
                    __base_t const expectbit = (set ? __valubit : 0);
                    if ((old & __valubit) != expectbit) {
                        __atomic_exponential_backoff b;
#ifdef __atomic_flag_fast_path
                        for (int i = 0; i < 2; ++i) {
#else
                        while (1) {
#endif
                            if (clock::now() > abs_time)
                                return false;
                            b.sleep();
                            old = atom.load(order);
                            if ((old & __valubit) == expectbit)
                                break;
                        }
                    }
#ifdef __atomic_flag_fast_path
                    if ((old & __valubit) != expectbit) {
                        while (1) {
                            old = atom.fetch_or(__contbit, memory_order_relaxed) | __contbit;
                            if ((old & __valubit) == expectbit)
                                break;
                            auto const delay = abs_time - clock::now();
                            if (delay < 0)
                                return false;
                            __atomic_wait_timed(atom, old, delay);
                            old = atom.load(order);
                            if ((old & __valubit) == expectbit)
                                break;
                        }
                    }
                    while (old & __lockbit)
                        old = atom.load(memory_order_relaxed);
#endif
                    return true;
                }

                template <class Clock, class Duration>
                bool wait_until(bool set, chrono::time_point<Clock, Duration> const& abs_time, memory_order order = memory_order_seq_cst) const {

                    bool success = __wait_fast(atom, set, order);
                    if (__atomic_expect(!success, 0))
                        success = __wait_slow_timed(atom, set, order, abs_time);
                    return success;
                }
                template <class Clock, class Duration>
                bool wait_until(bool set, chrono::time_point<Clock, Duration> const& abs_time, memory_order order = memory_order_seq_cst) const volatile {

                    bool success = __wait_fast(atom, set, order);
                    if (__atomic_expect(!success, 0))
                        success = __wait_slow_timed(atom, set, order, abs_time);
                    return success;
                }
                template <class Rep, class Period>
                bool wait_for(bool set, chrono::duration<Rep, Period> const& rel_time, memory_order order = memory_order_seq_cst) const {

                    bool success = __wait_fast(atom, set, order);
                    if (__atomic_expect(!success, 0))
                        success = __wait_slow_timed(atom, set, order, clock::now() + rel_time);
                    return success;
                }
                template <class Rep, class Period>
                bool wait_for(bool set, chrono::duration<Rep, Period> const& rel_time, memory_order order = memory_order_seq_cst) const volatile {

                    bool success = __wait_fast(atom, set, order);
                    if (__atomic_expect(!success, 0))
                        success = __wait_slow_timed(atom, set, order, clock::now() + rel_time);
                    return success;
                }

                atomic_flag(__base_t init) noexcept : atom(init) { }
                atomic_flag() noexcept = default;
                atomic_flag(const atomic_flag&) = delete;
                atomic_flag& operator=(const atomic_flag&) = delete;
#ifndef _MSC_VER
                atomic_flag& operator=(const atomic_flag&) volatile = delete;
#endif

                mutable std::atomic<__base_t> atom;

            } atomic_flag;

            inline bool atomic_flag_test_and_set(volatile atomic_flag* f) noexcept {
                return f->test_and_set();
            }
            inline bool atomic_flag_test_and_set(atomic_flag* f) noexcept {
                return f->test_and_set();
            }
            inline bool atomic_flag_test_and_set_explicit(volatile atomic_flag* f, memory_order order) noexcept {
                return f->test_and_set(order);
            }
            inline bool atomic_flag_test_and_set_explicit(atomic_flag* f, memory_order order) noexcept {
                return f->test_and_set(order);
            }
            inline bool atomic_flag_test_and_set_explicit_notify(volatile atomic_flag* f, memory_order order, atomic_notify notify) noexcept {
                return f->test_and_set(order, notify);
            }
            inline bool atomic_flag_test_and_set_explicit_notify(atomic_flag* f, memory_order order, atomic_notify notify) noexcept {
                return f->test_and_set(order, notify);
            }

            inline void atomic_flag_clear(volatile atomic_flag* f) noexcept {
                f->clear();
            }
            inline void atomic_flag_clear(atomic_flag* f) noexcept {
                f->clear();
            }
            inline void atomic_flag_clear_explicit(volatile atomic_flag* f, memory_order order) noexcept {
                f->clear(order);
            }
            inline void atomic_flag_clear_explicit(atomic_flag* f, memory_order order) noexcept {
                f->clear(order);
            }
            inline void atomic_flag_clear_explicit_notify(volatile atomic_flag* f, memory_order order, atomic_notify notify) noexcept {
                f->clear(order, notify);
            }
            inline void atomic_flag_clear_explicit_notify(atomic_flag* f, memory_order order, atomic_notify notify) noexcept {
                f->clear(order, notify);
            }

            inline bool atomic_flag_test(const volatile atomic_flag* f) noexcept {
                return f->test();
            }
            inline bool atomic_flag_test(const atomic_flag* f) noexcept {
                return f->test();
            }
            inline bool atomic_flag_test_explicit(const volatile atomic_flag* f, memory_order order) noexcept {
                return f->test(order);
            }
            inline bool atomic_flag_test_explicit(const atomic_flag* f, memory_order order) noexcept {
                return f->test(order);
            }

            inline void atomic_flag_wait(const volatile atomic_flag* f, bool set) {
                f->wait(set);
            }
            inline void atomic_flag_wait(const atomic_flag* f, bool set) {
                f->wait(set);
            }
            inline void atomic_flag_wait_explicit(const volatile atomic_flag* f, bool set, memory_order order) {
                f->wait(set, order);
            }
            inline void atomic_flag_wait_explicit(const atomic_flag* f, bool set, memory_order order) {
                f->wait(set, order);
            }

#ifndef ATOMIC_FLAG_INIT
#define ATOMIC_FLAG_INIT 0
#endif

        } // namespace concurrency_v2
    } // namespace experimental
} // namespace std

#endif //ATOMIC_FLAG_HPP
