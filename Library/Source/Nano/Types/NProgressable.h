/*	NAME:
		NProgressable.h

	DESCRIPTION:
		Mix-in class for objects that emit progress.

	COPYRIGHT:
		Copyright (c) 2006-2010, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
#ifndef NPROGRESSABLE_HDR
#define NPROGRESSABLE_HDR
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NFunctor.h"





//============================================================================
//		Constants
//----------------------------------------------------------------------------
static const float kNProgressNone									=  0.0f;
static const float kNProgressDone									=  1.0f;
static const float kNProgressUnknown								= -1.0f;





//============================================================================
//		Types
//----------------------------------------------------------------------------
typedef nfunctor<NStatus (float theProgress)>						NProgressFunctor;





//============================================================================
//		Class declaration
//----------------------------------------------------------------------------
class NProgressable {
public:
										NProgressable(void);
	virtual							   ~NProgressable(void);


	// Get/set the progress functor
	//
	// The operations that emit progress are determined by the derived class.
	NProgressFunctor					GetProgress(void) const;
	void								SetProgress(const NProgressFunctor &theFunctor);


protected:
	// Get/set the progress range
	//
	// Progress values range from 0.0 to 1.0 by default.
	//
	// To support multiple tasks within a single operation, progress values can be
	// offset and scaled to some range before being passed to the progress functor.
	void								GetProgressRange(float &theOffset, float &theScale) const;
	void								SetProgressRange(float  theOffset, float  theScale);


	// Begin/end the progress
	//
	// Progress updates are throttled to a sensible interval for UI updates (a few
	// times per second).
	//
	// Explicit begin/end updates are always dispatched, irrespective of the time.
	NStatus								BeginProgress(float theValue=kNProgressNone);
	void								EndProgress(  float theValue=kNProgressDone);


	// Update the progress
	NStatus								UpdateProgress(float  theValue);
	NStatus								UpdateProgress(NIndex theValue, NIndex maxValue);


private:
	NProgressFunctor					mProgress;
	NTime								mLastTime;
	float								mLastValue;
	float								mRangeOffset;
	float								mRangeScale;
};






#endif // NCOMPARABLE_HDR