/*	NAME:
		NLinuxThread.cpp

	DESCRIPTION:
		Linux thread support.
	
	COPYRIGHT:
		Copyright (c) 2006-2013, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include <semaphore.h>
#include <syscall.h>

#include "NLinuxTarget.h"
#include "NTargetThread.h"





//============================================================================
//		Internal globals
//----------------------------------------------------------------------------
static ThreadFunctorList gMainThreadFunctors;





//============================================================================
//		Internal functions
//----------------------------------------------------------------------------
//		InvokeMainThreadFunctor : Invoke a main-thread functor.
//----------------------------------------------------------------------------
static void InvokeMainThreadFunctor(const NFunctor &theFunctor, NSemaphore *theSemaphore)
{


	// Validate our state
	NN_ASSERT(NTargetThread::ThreadIsMain());



	// Invoke the functor
	theFunctor();
	theSemaphore->Signal();
}





//============================================================================
//		InvokeMainThreadFunctors : Invoke the main-thread functors.
//----------------------------------------------------------------------------
static void InvokeMainThreadFunctors(void)
{	static bool			sIsInvoking = false;

	NFunctor			theFunctor;



	// Validate our state
	NN_ASSERT(NTargetThread::ThreadIsMain());



	// Invoke the functors
	//
	// If a main thread functor invokes NThread::Sleep (perhaps by waiting for
	// a lock) then we can be called recursively.
	//
	// This can lead to deadlocks, as we can try and call a functor which needs
	// to acquire a lock while executing another functor which also needs to
	// acquire the same lock.
	//
	// If the lock is meant to synchronise the main thread with a worker thread,
	// this can mean the main thread deadlocks itself.
	if (!sIsInvoking)
		{
		sIsInvoking = true;
		
		while (gMainThreadFunctors.PopFront(theFunctor))
			theFunctor();
		
		sIsInvoking = false;
		}
}





//============================================================================
//		ThreadEntry : Thread entry point.
//----------------------------------------------------------------------------
static void *ThreadEntry(void *userData)
{	NFunctor			*theFunctor;
	StAutoReleasePool	thePool;



	// Get the state we need
	theFunctor = (NFunctor *) userData;



	// Invoke the thread
	(*theFunctor)();
	delete theFunctor;

	return(NULL);
}





#pragma mark NTargetThread
//============================================================================
//		Public functions
//----------------------------------------------------------------------------
//		NTargetThread::GetCPUCount : Get the number of CPUs.
//----------------------------------------------------------------------------
UInt32 NTargetThread::GetCPUCount(void)
{	UInt32		theResult;



	// Get the CPU count
	theResult = sysconf(_SC_NPROCESSORS_ONLN);
	NN_ASSERT(theResult > 0);

	if (theResult == 0)
		theResult = 1;

	return(theResult);
}





//============================================================================
//		NTargetThread::AtomicCompareAndSwap32 : Atomic 32-bit compare-and-swap.
//----------------------------------------------------------------------------
bool NTargetThread::AtomicCompareAndSwap32(UInt32 &theValue, UInt32 oldValue, UInt32 newValue)
{


	// Validate our parameters
	NN_ASSERT_ALIGNED_4(&theValue);



	// Compare and swap
	return(__sync_bool_compare_and_swap(&theValue, oldValue, newValue));
}





//============================================================================
//		NTargetThread::AtomicAdd32 : Atomic 32-bit add.
//----------------------------------------------------------------------------
SInt32 NTargetThread::AtomicAdd32(SInt32 &theValue, SInt32 theDelta)
{


	// Validate our parameters
	NN_ASSERT_ALIGNED_4(&theValue);



	// Add the value
	return(__sync_add_and_fetch(&theValue, theDelta));
}





//============================================================================
//		NTargetThread::AtomicAnd32 : Atomic 32-bit and.
//----------------------------------------------------------------------------
void NTargetThread::AtomicAnd32(UInt32 &theValue, UInt32 theMask)
{


	// Validate our parameters
	NN_ASSERT_ALIGNED_4(&theValue);



	// AND the value
	__sync_fetch_and_and(&theValue, theMask);
}





//============================================================================
//		NTargetThread::AtomicOr32 : Atomic 32-bit or.
//----------------------------------------------------------------------------
void NTargetThread::AtomicOr32(UInt32 &theValue, UInt32 theMask)
{


	// Validate our parameters
	NN_ASSERT_ALIGNED_4(&theValue);



	// OR the value
	__sync_fetch_and_or(&theValue, theMask);
}





//============================================================================
//		NTargetThread::AtomicXor32 : Atomic 32-bit xor.
//----------------------------------------------------------------------------
void NTargetThread::AtomicXor32(UInt32 &theValue, UInt32 theMask)
{


	// Validate our parameters
	NN_ASSERT_ALIGNED_4(&theValue);



	// XOR the value
	__sync_fetch_and_xor(&theValue, theMask);
}





//============================================================================
//		NTargetThread::MemoryBarrier : Insert a memory barrier.
//----------------------------------------------------------------------------
void NTargetThread::MemoryBarrier(void)
{


	// Insert the barrier
	__sync_synchronize();
}





//============================================================================
//		NTargetThread::ThreadIsMain : Is this the main thread?
//----------------------------------------------------------------------------
bool NTargetThread::ThreadIsMain(void)
{


	// Check our state
	return(syscall(SYS_gettid) == getpid());
}





//============================================================================
//		NTargetThread::ThreadSleep : Sleep the current thread.
//----------------------------------------------------------------------------
void NTargetThread::ThreadSleep(NTime theTime)
{


	// Sleep the thread
	usleep((useconds_t) (theTime / kNTimeMicrosecond));



	// Invoke the functors
	//
	// Sleeping the main thread will prevent functors due to be executed on the main
	// thread from firing.
	//
	// To avoid deadlocks where the main thread is waiting for a thread to exit and
	// that thread is waiting inside InvokeMain for a functor to complete, sleeping
	// the main thread will also invoke any queued functors.
	if (ThreadIsMain())
		InvokeMainThreadFunctors();
}





//============================================================================
//		NTargetThread::ThreadCreate : Create a thread.
//----------------------------------------------------------------------------
NStatus NTargetThread::ThreadCreate(const NFunctor &theFunctor)
{	NFunctor		*tmpFunctor;
	pthread_t		threadID;
	int				sysErr;



	// Get the state we need
	tmpFunctor = new NFunctor(theFunctor);



	// Create the thread
	sysErr = pthread_create(&threadID, NULL, ThreadEntry, tmpFunctor);
	NN_ASSERT_NOERR(sysErr);
	
	return(NLinuxTarget::ConvertSysErr(sysErr));
}





//============================================================================
//		NTargetThread::ThreadInvokeMain : Invoke the main thread.
//----------------------------------------------------------------------------
void NTargetThread::ThreadInvokeMain(const NFunctor &theFunctor)
{	NFunctor				invokeFunctor;
	NSemaphore				theSemaphore;
// dair
//	CFAbsoluteTime			fireTime;
//	CFRunLoopTimerRef		cfTimer;
	bool					wasDone;



	// Invoke the functor
	if (ThreadIsMain())
		theFunctor();


	// Pass it to the main thread
	//
	// An invoker is used to invoke the functor then set our semaphore.
	//
	// The invoker is executed by the main thread, via InvokeMainThreadFunctors, either
	// due to the main thread being blocked in ThreadSleep or due to the event loop
	// running as normal and executing our timer.
	else
		{
		// Save the functor
		gMainThreadFunctors.PushBack(BindFunction(InvokeMainThreadFunctor, theFunctor, &theSemaphore));


// dair, todo
NN_LOG("NTargetThread::ThreadInvokeMain skipping runloop!");
/*
		// Schedule the timer
		fireTime = CFRunLoopGetNextTimerFireDate(CFRunLoopGetMain(), kCFRunLoopCommonModes);
		cfTimer  = CFRunLoopTimerCreate(kCFAllocatorNano, fireTime, 0.0f, 0, 0, MainThreadFunctorsTimer, NULL);

		CFRunLoopAddTimer(CFRunLoopGetMain(), cfTimer, kCFRunLoopCommonModes);
		CFSafeRelease(cfTimer);
*/


		// Wait for the functor to be processed
		wasDone = theSemaphore.Wait();
		NN_ASSERT(wasDone);
		}
}





//============================================================================
//		NTargetThread::LocalCreate : Create a thread-local value.
//----------------------------------------------------------------------------
NThreadLocalRef NTargetThread::LocalCreate(void)
{	pthread_key_t	keyRef;
	int				sysErr;



	// Validate our state
	NN_ASSERT(sizeof(pthread_key_t) <= sizeof(NThreadLocalRef));



	// Create the key
	keyRef = 0;
	sysErr = pthread_key_create(&keyRef, NULL);
	NN_ASSERT_NOERR(sysErr);

	return((NThreadLocalRef) keyRef);
}





//============================================================================
//		NTargetThread::LocalDestroy : Destroy a thread-local value.
//----------------------------------------------------------------------------
void NTargetThread::LocalDestroy(NThreadLocalRef theKey)
{	pthread_key_t	keyRef = (pthread_key_t) theKey;
	int				sysErr;



	// Destroy the key
	sysErr = pthread_key_delete(keyRef);
	NN_ASSERT_NOERR(sysErr);
}





//============================================================================
//		NTargetThread::LocalGetValue : Get a thread-local value.
//----------------------------------------------------------------------------
void *NTargetThread::LocalGetValue(NThreadLocalRef theKey)
{	pthread_key_t	keyRef = (pthread_key_t) theKey;
	void			*theValue;



	// Get the value
	theValue = pthread_getspecific(keyRef);

	return(theValue);
}





//============================================================================
//		NTargetThread::LocalSetValue : Set a thread-local value.
//----------------------------------------------------------------------------
void NTargetThread::LocalSetValue(NThreadLocalRef theKey, void *theValue)
{	pthread_key_t	keyRef = (pthread_key_t) theKey;
	int				sysErr;



	// Set the value
	sysErr = pthread_setspecific(keyRef, theValue);
	NN_ASSERT_NOERR(sysErr);
}





//============================================================================
//		NTargetThread::SemaphoreCreate : Create a semaphore.
//----------------------------------------------------------------------------
NSemaphoreRef NTargetThread::SemaphoreCreate(NIndex theValue)
{	sem_t		*semRef;
	int			sysErr;



	// Create the semaphore
	semRef = (sem_t *) malloc(sizeof(sem_t));
	sysErr = sem_init(semRef, 0, theValue);
	NN_ASSERT_NOERR(sysErr);

	return((NSemaphoreRef) semRef);
}





//============================================================================
//		NTargetThread::SemaphoreDestroy : Destroy a semaphore.
//----------------------------------------------------------------------------
void NTargetThread::SemaphoreDestroy(NSemaphoreRef theSemaphore)
{	sem_t		*semRef = (sem_t *) theSemaphore;
	int			sysErr;



	// Destroy the semaphore
	sysErr = sem_destroy(semRef);
	NN_ASSERT_NOERR(sysErr);
	
	free(semRef);
}





//============================================================================
//		NTargetThread::SemaphoreSignal : Signal a semaphore.
//----------------------------------------------------------------------------
void NTargetThread::SemaphoreSignal(NSemaphoreRef theSemaphore)
{	sem_t		*semRef = (sem_t *) theSemaphore;
	int			sysErr;



	// Signal the semaphore
    sysErr = sem_post(semRef);
	NN_ASSERT_NOERR(sysErr);
}





//============================================================================
//		NTargetThread::SemaphoreWait : Wait for a semaphore.
//----------------------------------------------------------------------------
NStatus NTargetThread::SemaphoreWait(NSemaphoreRef theSemaphore, NTime waitFor)
{	sem_t				*semRef = (sem_t *) theSemaphore;
	NTime				timeSecs, timeFrac;
	struct timespec		waitTime;
	int					sysErr;



	// Wait with timeout
	if (!NMathUtilities::AreEqual(waitFor, kNTimeForever))
		{
		timeSecs = floor(waitFor);
		timeFrac = waitFor - timeSecs;
		
		waitTime.tv_sec  = (time_t) timeSecs;
		waitTime.tv_nsec = (long) (timeFrac / kNTimeNanosecond);
		
		sysErr = sem_timedwait(semRef, &waitTime);
		}


	// Wait for the semaphore
	else
		sysErr = sem_wait(semRef);

	return(NLinuxTarget::ConvertSysErr(sysErr));
}





//============================================================================
//      NTargetThread::MutexCreate : Create a mutex lock.
//----------------------------------------------------------------------------
NLockRef NTargetThread::MutexCreate(void)
{	pthread_mutexattr_t		lockAttr;
	pthread_mutex_t			*lockRef;
	NStatus					theErr;



	// Validate our state
	NN_ASSERT(sizeof(lockRef) == sizeof(NLockRef));
	
	
	
	// Get the state we need
	pthread_mutexattr_init(   &lockAttr);
	pthread_mutexattr_settype(&lockAttr, PTHREAD_MUTEX_RECURSIVE);



	// Create the lock
	lockRef = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	if (lockRef != NULL)
		{
		theErr = pthread_mutex_init(lockRef, &lockAttr);
		NN_ASSERT_NOERR(theErr);
		}

	return((NLockRef) lockRef);
}





//============================================================================
//      NTargetThread::MutexDestroy : Destroy a mutex lock.
//----------------------------------------------------------------------------
void NTargetThread::MutexDestroy(NLockRef theLock)
{	pthread_mutex_t		*lockRef = (pthread_mutex_t *) theLock;
	NStatus				theErr;



	// Destroy the lock
	theErr = pthread_mutex_destroy(lockRef);
	NN_ASSERT_NOERR(theErr);
	
	free(lockRef);
}





//============================================================================
//      NTargetThread::MutexLock : Lock a mutex lock.
//----------------------------------------------------------------------------
NStatus NTargetThread::MutexLock(NLockRef theLock, bool canBlock)
{	pthread_mutex_t		*lockRef = (pthread_mutex_t *) theLock;
	int					sysErr;
	NStatus				theErr;



	// Acquire the lock
	if (canBlock)
		sysErr = pthread_mutex_lock(   lockRef);
	else
		sysErr = pthread_mutex_trylock(lockRef);
	
	theErr = (sysErr == 0) ? kNoErr : kNErrTimeout;

	return(theErr);
}





//============================================================================
//      NTargetThread::MutexUnlock : Unlock a mutex lock.
//----------------------------------------------------------------------------
void NTargetThread::MutexUnlock(NLockRef theLock)
{	pthread_mutex_t		*lockRef = (pthread_mutex_t *) theLock;
	NStatus				theErr;



	// Release the lock
	theErr = pthread_mutex_unlock(lockRef);
	NN_ASSERT_NOERR(theErr);
}





//============================================================================
//      NTargetThread::ReadWriteCreate : Create a read-write lock.
//----------------------------------------------------------------------------
NLockRef NTargetThread::ReadWriteCreate(void)
{	pthread_rwlock_t		*lockRef;
	NStatus					theErr;



	// Validate our state
	NN_ASSERT(sizeof(lockRef) == sizeof(NLockRef));
	


	// Create the lock
	lockRef = (pthread_rwlock_t *) malloc(sizeof(pthread_rwlock_t));
	if (lockRef != NULL)
		{
		theErr = pthread_rwlock_init(lockRef, NULL);
		NN_ASSERT_NOERR(theErr);
		}

	return((NLockRef) lockRef);
}





//============================================================================
//      NTargetThread::ReadWriteDestroy : Destroy a read-write lock.
//----------------------------------------------------------------------------
void NTargetThread::ReadWriteDestroy(NLockRef theLock)
{	pthread_rwlock_t		*lockRef = (pthread_rwlock_t *) theLock;
	NStatus					theErr;



	// Destroy the lock
	theErr = pthread_rwlock_destroy(lockRef);
	NN_ASSERT_NOERR(theErr);
	
	free(lockRef);
}





//============================================================================
//      NTargetThread::ReadWriteLock : Lock a read-write lock.
//----------------------------------------------------------------------------
NStatus NTargetThread::ReadWriteLock(NLockRef theLock, bool forRead, bool canBlock)
{	pthread_rwlock_t	*lockRef = (pthread_rwlock_t *) theLock;
	int					sysErr;
	NStatus				theErr;



	// Acquire the lock
	if (canBlock)
		{
		if (forRead)
			sysErr = pthread_rwlock_rdlock(lockRef);
		else
			sysErr = pthread_rwlock_wrlock(lockRef);
		}
	else
		{
		if (forRead)
			sysErr = pthread_rwlock_tryrdlock(lockRef);
		else
			sysErr = pthread_rwlock_trywrlock(lockRef);
		}
	
	theErr = (sysErr == 0) ? kNoErr : kNErrTimeout;

	return(theErr);
}





//============================================================================
//      NTargetThread::ReadWriteUnlock : Unlock a read-write lock.
//----------------------------------------------------------------------------
void NTargetThread::ReadWriteUnlock(NLockRef theLock, bool /*forRead*/)
{	pthread_rwlock_t		*lockRef = (pthread_rwlock_t *) theLock;
	NStatus					theErr;



	// Release the lock
	theErr = pthread_rwlock_unlock(lockRef);
	NN_ASSERT_NOERR(theErr);
}





//============================================================================
//      NTargetThread::SpinCreate : Create a spin lock.
//----------------------------------------------------------------------------
NLockRef NTargetThread::SpinCreate(void)
{	spinlock_t		*lockRef;



	// Validate our state
	NN_ASSERT(sizeof(lockRef) == sizeof(NLockRef));



	// Create the lock
	lockRef = (spinlock_t *) malloc(sizeof(spinlock_t));
	if (lockRef != NULL)
		spin_lock_init(lockRef);

	return((NLockRef) lockRef);
}





//============================================================================
//      NTargetThread::SpinDestroy : Destroy a spin lock.
//----------------------------------------------------------------------------
void NTargetThread::SpinDestroy(NLockRef theLock)
{	spinlock_t		*lockRef = (spinlock_t *) theLock;



	// Destroy the lock
	free(lockRef);
}





//============================================================================
//      NTargetThread::SpinLock : Lock a spin lock.
//----------------------------------------------------------------------------
NStatus NTargetThread::SpinLock(NLockRef theLock, bool canBlock)
{	spinlock_t		*lockRef = (spinlock_t *) theLock;
	NStatus			theErr;



	// Acquire the lock
	theErr = kNoErr;
	
	if (canBlock)
		spin_lock(lockRef);
	else
		{
		if (!spin_trylock(lockRef))
			theErr = kNErrTimeout;
		}

	return(theErr);
}





//============================================================================
//      NTargetThread::SpinUnlock : Unlock a spin lock.
//----------------------------------------------------------------------------
void NTargetThread::SpinUnlock(NLockRef theLock)
{	spinlock_t		*lockRef = (spinlock_t *) theLock;



	// Release the lock
	spin_unlock(lockRef);
}


