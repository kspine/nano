/*	NAME:
		NThreadUtilities.cpp

	DESCRIPTION:
		Thread utilities.
	
	COPYRIGHT:
		Copyright (c) 2006-2013, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NTargetThread.h"
#include "NMathUtilities.h"
#include "NThreadUtilities.h"





//============================================================================
//		NThreadUtilities::GetCPUCount : Get the number of CPUs.
//----------------------------------------------------------------------------
NIndex NThreadUtilities::GetCPUCount(void)
{


	// Get the CPU count
	return(NTargetThread::GetCPUCount());
}





//============================================================================
//		NThreadUtilities::DetachFunctor : Detach a functor.
//----------------------------------------------------------------------------
void NThreadUtilities::DetachFunctor(const NFunctor &theFunctor)
{


	// Detach the functor
	NTargetThread::ThreadCreate(theFunctor);
}





//============================================================================
//		NThreadUtilities::DelayFunctor : Delay a functor.
//----------------------------------------------------------------------------
void NThreadUtilities::DelayFunctor(const NFunctor &theFunctor, NTime theDelay, bool onMainThread)
{	NTimer		*theTimer;



	// Invoke immediately
	//
	// If we're to invoke on a new thread, or are already on the main
	// thread, we can invoke the functor directly without any delay.
	//
	// If we can't (we have a delay or we're not the main thread but
	// the functor must execute on the main thread), we fall through
	// to the timer case.
	if (NMathUtilities::IsZero(theDelay))
		{
		if (!onMainThread)
			{
			DetachFunctor(theFunctor);
			return;
			}
		
		else if (NThread::IsMain())
			{
			theFunctor();
			return;
			}
		}



	// Invoke with a delay
	theTimer = new NTimer;
	if (theTimer != NULL)
		theTimer->AddTimer(BindFunction(NThreadUtilities::DelayedFunctor, theTimer, theFunctor, onMainThread), theDelay);
}





//============================================================================
//		NThreadUtilities::AtomicCompareAndSwap32 : Atomic 32-bit compare-and-swap.
//----------------------------------------------------------------------------
bool NThreadUtilities::AtomicCompareAndSwap32(int32_t &theValue, int32_t oldValue, int32_t newValue)
{


	// Compare and swap
	return(NTargetThread::AtomicCompareAndSwap32(theValue, oldValue, newValue));
}





//============================================================================
//		NThreadUtilities::AtomicCompareAndSwapPtr : Atomic pointer compare-and-swap.
//----------------------------------------------------------------------------
bool NThreadUtilities::AtomicCompareAndSwapPtr(void *&theValue, void *oldValue, void *newValue)
{


	// Compare and swap
	return(NTargetThread::AtomicCompareAndSwapPtr(theValue, oldValue, newValue));
}





//============================================================================
//		NThreadUtilities::AtomicAdd32 : Atomic 32-bit add.
//----------------------------------------------------------------------------
int32_t NThreadUtilities::AtomicAdd32(int32_t &theValue, int32_t theDelta)
{


	// Add the value
	return(NTargetThread::AtomicAdd32(theValue, theDelta));
}





//============================================================================
//		NThreadUtilities::AtomicAnd32 : Atomic 32-bit and.
//----------------------------------------------------------------------------
void NThreadUtilities::AtomicAnd32(uint32_t &theValue, uint32_t theMask)
{


	// Update the value
	NTargetThread::AtomicAnd32(theValue, theMask);
}





//============================================================================
//		NThreadUtilities::AtomicXor32 : Atomic 32-bit xor.
//----------------------------------------------------------------------------
void NThreadUtilities::AtomicXor32(uint32_t &theValue, uint32_t theMask)
{


	// Update the value
	NTargetThread::AtomicXor32(theValue, theMask);
}





//============================================================================
//		NThreadUtilities::AtomicOr32 : Atomic 32-bit or.
//----------------------------------------------------------------------------
void NThreadUtilities::AtomicOr32(uint32_t &theValue, uint32_t theMask)
{


	// Update the value
	NTargetThread::AtomicOr32(theValue, theMask);
}





//============================================================================
//		NThreadUtilities::MemoryBarrier : Insert a memory barrier.
//----------------------------------------------------------------------------
void NThreadUtilities::MemoryBarrier(void)
{


	// Insert the barrier
	NTargetThread::MemoryBarrier();
}





#pragma mark private
//============================================================================
//		NThreadUtilities::DelayedFunctor : Execute a delayed functor.
//----------------------------------------------------------------------------
void NThreadUtilities::DelayedFunctor(NTimer *theTimer, const NFunctor &theFunctor, bool onMainThread)
{


	// Validate our state
	NN_ASSERT(NThread::IsMain());



	// Invoke the functor
	if (onMainThread)
		theFunctor();
	else
		NTargetThread::ThreadCreate(theFunctor);



	// Clean up
	delete theTimer;
}
