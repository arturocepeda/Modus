
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.60
//  C++ Music Library
//
//  Copyright (c) 2012-2015 Arturo Cepeda
//
//  --------------------------------------------------------------------
//
//  This file is part of Modus. Permission is hereby granted, free 
//  of charge, to any person obtaining a copy of this software and 
//  associated documentation files (the "Software"), to deal in the 
//  Software without restriction, including without limitation the 
//  rights to use, copy, modify, merge, publish, distribute, 
//  sublicense, and/or sell copies of the Software, and to permit 
//  persons to whom the Software is furnished to do so, subject to 
//  the following conditions:
//
//  The above copyright notice and this permission notice shall be 
//  included in all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY 
//  KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
//  WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
//  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
//  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
//  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
////////////////////////////////////////////////////////////////////////

///\file

#ifndef _MTHREADS_H_
#define _MTHREADS_H_


//
//  Windows
//
#if defined(WIN32)

#include <windows.h>

#define MThread  HANDLE
#define MThreadFunction(FunctionName)  DWORD WINAPI FunctionName(LPVOID pData)
#define MCreateThread(Thread, FunctionName, DataPointer)  Thread = CreateThread(NULL, 0, FunctionName, DataPointer, 0, NULL)
#define MWaitForThread(Thread)  WaitForSingleObject(Thread, INFINITE)
#define MCloseThread(Thread)  CloseHandle(Thread)

#define MMutex  CRITICAL_SECTION
#define MInitMutex(Mutex)  InitializeCriticalSection(&Mutex)
#define MLockMutex(Mutex)  EnterCriticalSection(&Mutex)
#define MUnlockMutex(Mutex)  LeaveCriticalSection(&Mutex)
#define MDestroyMutex(Mutex)  DeleteCriticalSection(&Mutex)

#define MSemaphore  HANDLE
#define MInitSemaphore(Semaphore, Value)  Semaphore = CreateSemaphore(NULL, Value, Value, NULL)
#define MWaitSemaphore(Semaphore)  WaitForSingleObject(Semaphore, INFINITE)
#define MSignalSemaphore(Semaphore)  ReleaseSemaphore(Semaphore, 1, NULL)
#define MDestroySemaphore(Semaphore)  CloseHandle(Semaphore)

#define MSleep(Milliseconds)  Sleep(Milliseconds)


//
//  Unix
//
#else

#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define MThread  pthread_t
#define MThreadFunction(FunctionName)  void* FunctionName(void* pData)
#define MCreateThread(Thread, FunctionName, DataPointer)  pthread_create(&Thread, NULL, FunctionName, DataPointer)
#define MWaitForThread(Thread)  pthread_join(Thread, NULL)
#define MCloseThread(Thread)

#define MMutex  pthread_mutex_t
#define MInitMutex(Mutex)  pthread_mutex_init(&Mutex, NULL)
#define MLockMutex(Mutex)  pthread_mutex_lock(&Mutex)
#define MUnlockMutex(Mutex)  pthread_mutex_unlock(&Mutex)
#define MDestroyMutex(Mutex)  pthread_mutex_destroy(&Mutex)

#define MSemaphore  sem_t
#define MInitSemaphore(Semaphore, Value)  sem_init(&Semaphore, 0, Value)
#define MWaitSemaphore(Semaphore)  sem_wait(&Semaphore)
#define MSignalSemaphore(Semaphore)  sem_post(&Semaphore)
#define MDestroySemaphore(Semaphore)  sem_destroy(&Semaphore)

#define MSleep(Milliseconds)  usleep(Milliseconds * 1000)

#endif

#endif
