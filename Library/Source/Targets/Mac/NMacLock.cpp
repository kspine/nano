/*	NAME:
		NMacLock.cpp

	DESCRIPTION:
		Mac lock support.
	
	COPYRIGHT:
		Copyright (c) 2006-2009, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NThreadUtilities.h"
#include "NMathUtilities.h"
#include "NTimeUtilities.h"
#include "NMacTarget.h"
#include "NTargetLock.h"





//============================================================================
//      NTargetLock::MutexCreate : Create a mutex lock.
//----------------------------------------------------------------------------
NLockRef NTargetLock::MutexCreate(void)
{	MPCriticalRegionID		mutexLock;
	OSStatus				theErr;



	// Validate our state
	NN_ASSERT(sizeof(mutexLock) == sizeof(NLockRef));



	// Create the lock
	theErr = MPCreateCriticalRegion(&mutexLock);
	NN_ASSERT_NOERR(theErr);
	
	return((NLockRef) mutexLock);
}





//============================================================================
//      NTargetLock::MutexDestroy : Destroy a mutex lock.
//----------------------------------------------------------------------------
void NTargetLock::MutexDestroy(NLockRef &theLock)
{	MPCriticalRegionID		mutexLock = (MPCriticalRegionID) theLock;
	OSStatus				theErr;



	// Validate our parameters
	NN_ASSERT(theLock != NULL);



	// Delete the lock
	theErr = MPDeleteCriticalRegion(mutexLock);
	NN_ASSERT_NOERR(theErr);



	// Clean up
	theLock = NULL;
}





//============================================================================
//      NTargetLock::MutexLock : Lock a mutex lock.
//----------------------------------------------------------------------------
NStatus NTargetLock::MutexLock(NLockRef &theLock, NTime waitFor)
{	MPCriticalRegionID		mutexLock = (MPCriticalRegionID) theLock;
	OSStatus				theErr;



	// Validate our parameters
	NN_ASSERT(theLock != NULL);



	// Acquire the lock
	theErr = MPEnterCriticalRegion(mutexLock, NMacTarget::GetDuration(waitFor));
	NN_ASSERT(theErr == noErr || theErr == kMPTimeoutErr);

	return(NMacTarget::GetStatus(theErr));
}





//============================================================================
//      NTargetLock::MutexUnlock : Unlock a mutex lock.
//----------------------------------------------------------------------------
void NTargetLock::MutexUnlock(NLockRef &theLock)
{	MPCriticalRegionID		mutexLock = (MPCriticalRegionID) theLock;
	OSStatus				theErr;



	// Validate our parameters
	NN_ASSERT(theLock != NULL);



	// Release the lock
	theErr = MPExitCriticalRegion(mutexLock);
	NN_ASSERT_NOERR(theErr);
}





//============================================================================
//      NTargetLock::ReadWriteCreate : Create a read-write lock.
//----------------------------------------------------------------------------
NLockRef NTargetLock::ReadWriteCreate(void)
{	pthread_rwlock_t		*rwLock;
	NStatus					theErr;



	// Validate our state
	NN_ASSERT(sizeof(rwLock) == sizeof(NLockRef));
	


	// Create the lock
	rwLock = (pthread_rwlock_t *) malloc(sizeof(pthread_rwlock_t));
	if (rwLock != NULL)
		{
		theErr = pthread_rwlock_init(rwLock, NULL);
		NN_ASSERT_NOERR(theErr);
		}

	return((NLockRef) rwLock);
}





//============================================================================
//      NTargetLock::ReadWriteDestroy : Destroy a read-write lock.
//----------------------------------------------------------------------------
void NTargetLock::ReadWriteDestroy(NLockRef &theLock)
{	pthread_rwlock_t		*rwLock = (pthread_rwlock_t *) theLock;
	NStatus					theErr;



	// Validate our parameters
	NN_ASSERT(theLock != NULL);



	// Destroy the lock
	theErr = pthread_rwlock_destroy(rwLock);
	NN_ASSERT_NOERR(theErr);
	
	free(rwLock);



	// Clean up
	theLock = NULL;
}





//============================================================================
//      NTargetLock::ReadWriteLock : Lock a read-write lock.
//----------------------------------------------------------------------------
NStatus NTargetLock::ReadWriteLock(NLockRef &theLock, NTime waitFor, bool forWrite)
{	pthread_rwlock_t		*rwLock = (pthread_rwlock_t *) theLock;
	NTime					stopTime;
	NStatus					theErr;



	// Validate our parameters
	NN_ASSERT(theLock != NULL);



	// Acquire with timeout
	if (NMathUtilities::NotEqual(waitFor, kNTimeForever))
		{
		stopTime = NTimeUtilities::GetTime() + waitFor;
		do
			{
			// Acquire the lock
			if (forWrite)
				theErr = pthread_rwlock_trywrlock(rwLock);
			else
				theErr = pthread_rwlock_tryrdlock(rwLock);


			// Handle failure
			if (theErr != kNoErr && NTimeUtilities::GetTime() < stopTime)
				{
				NThreadUtilities::Sleep(kNThreadSpinTime);
				theErr = kNoErr;
				}
			}
		while (theErr != kNoErr);
		}



	// Acquire the lock
	else
		{
		if (forWrite)
			theErr = pthread_rwlock_wrlock(rwLock);
		else
			theErr = pthread_rwlock_rdlock(rwLock);
		}



	// Convert the result
	if (theErr != kNoErr)
		theErr = kNErrTimeout;

	return(theErr);
}





//============================================================================
//      NTargetLock::ReadWriteUnlock : Unlock a read-write lock.
//----------------------------------------------------------------------------
void NTargetLock::ReadWriteUnlock(NLockRef &theLock)
{	pthread_rwlock_t		*rwLock = (pthread_rwlock_t *) theLock;
	NStatus					theErr;



	// Validate our parameters
	NN_ASSERT(theLock != NULL);



	// Release the lock
	theErr = pthread_rwlock_unlock(rwLock);
	NN_ASSERT_NOERR(theErr);
}

