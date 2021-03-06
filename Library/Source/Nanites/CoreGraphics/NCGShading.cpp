/*	NAME:
		NCGShading.cpp

	DESCRIPTION:
		CoreGraphics shading object.
	
	COPYRIGHT:
		Copyright (c) 2006-2013, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NGeometryUtilities.h"
#include "NCoreGraphics.h"
#include "NCGShading.h"
#include "NCGColor.h"





//============================================================================
//		Public constants
//----------------------------------------------------------------------------
// Shadings
const NShadingSample kShadingBlackWhite[] =					{ { 0.0f, NColor(0.0f, 0.0f, 0.0f) },
															  { 1.0f, NColor(1.0f, 1.0f, 1.0f) } };

const NShadingSample kShadingWhiteBlack[] =					{ { 0.0f, NColor(1.0f, 1.0f, 1.0f) },
															  { 1.0f, NColor(0.0f, 0.0f, 0.0f) } };

const NShadingSample kShadingFrontRow[] =					{ { 0.0f, NColor(0.000f, 0.000f, 0.000f) },
															  { 0.5f, NColor(0.000f, 0.000f, 0.000f) },
															  { 1.0f, NColor(0.298f, 0.298f, 0.298f) } };





//============================================================================
//		NCGShading::NCGShading : Constructor.
//----------------------------------------------------------------------------
NCGShading::NCGShading(const NCGShading &theShading)
{


	// Initialize ourselves
	CopyShading(theShading);
}





//============================================================================
//		NCGShading::NCGShading : Constructor.
//----------------------------------------------------------------------------
NCGShading::NCGShading(NShadingMode theMode)
{


	// Initialize ourselves
	InitializeSelf();
	SetMode(theMode);
}





//============================================================================
//		NCGShading::~NCGShading : Destructor.
//----------------------------------------------------------------------------
NCGShading::~NCGShading(void)
{
}





//============================================================================
//		NCGShading::GetMode : Get the shading mode.
//----------------------------------------------------------------------------
NShadingMode NCGShading::GetMode(void) const
{


	// Get the mode
	return(mMode);
}





//============================================================================
//		NCGShading::SetMode : Set the shading mode.
//----------------------------------------------------------------------------
void NCGShading::SetMode(NShadingMode theMode)
{


	// Set the mode
	if (theMode != mMode)
		{
		mMode = theMode;
		ResetShading();
		}
}





//============================================================================
//		NCGShading::GetSamples : Get the shading samples.
//----------------------------------------------------------------------------
NShadingSampleList NCGShading::GetSamples(void) const
{


	// Get the samples
	return(mSamples);
}





//============================================================================
//		NCGShading::SetSamples : Set the shading samples.
//----------------------------------------------------------------------------
void NCGShading::SetSamples(const NShadingSampleList &theSamples)
{	NIndex		n, numItems;
	bool		didChange;



	// Get the state we need
	didChange = (mSamples.size() != theSamples.size());
	
	if (!didChange)
		{
		numItems = mSamples.size();

		for (n = 0; n < numItems && !didChange; n++)
			didChange = (mSamples[n] != theSamples[n]);
		}



	// Set the samples
	if (didChange)
		{
		mSamples = theSamples;
		ResetShading();
		}
}





//============================================================================
//		NCGShading::SetSamples : Set the shading samples.
//----------------------------------------------------------------------------
void NCGShading::SetSamples(const NShadingSample *theSamples)
{	NShadingSampleList		theList;



	// Validate our parameters
	NN_ASSERT(theSamples != NULL);



	// Set the samples
	while (true)
		{
		theList.push_back(*theSamples);
		if (theSamples->theValue >= 1.0f)
			break;

		theSamples++;
		}
	
	SetSamples(theList);
}





//============================================================================
//		NCGShading::GetStartPoint : Get the start point.
//----------------------------------------------------------------------------
NPoint NCGShading::GetStartPoint(void) const
{


	// Get the start point
	return(mStartPoint);
}





//============================================================================
//		NCGShading::SetStartPoint : Set the start point.
//----------------------------------------------------------------------------
void NCGShading::SetStartPoint(const NPoint &thePoint)
{


	// Set the start point
	if (thePoint != mStartPoint)
		{
		mStartPoint = thePoint;
		ResetShading();
		}
}





//============================================================================
//		NCGShading::GetStartExtend : Get the start extend.
//----------------------------------------------------------------------------
bool NCGShading::GetStartExtend(void) const
{


	// Get the start extend
	return(mStartExtend);
}





//============================================================================
//		NCGShading::SetStartExtend : Set the start extend.
//----------------------------------------------------------------------------
void NCGShading::SetStartExtend(bool theValue)
{


	// Set the start extend
	if (theValue != mStartExtend)
		{
		mStartExtend = theValue;
		ResetShading();
		}
}





//============================================================================
//		NCGShading::GetStartRadius : Get the start radius.
//----------------------------------------------------------------------------
float NCGShading::GetStartRadius(void) const
{


	// Get the start radius
	return(mStartRadius);
}





//============================================================================
//		NCGShading::SetStartRadius : Set the start radius.
//----------------------------------------------------------------------------
void NCGShading::SetStartRadius(float theValue)
{


	// Validate our state
	NN_ASSERT(mMode == kShadingRadial);
	


	// Set the start radius
	if (!NMathUtilities::AreEqual(theValue, mStartRadius))
		{
		mStartRadius = theValue;
		ResetShading();
		}
}





//============================================================================
//		NCGShading::GetEndPoint : Get the end point.
//----------------------------------------------------------------------------
NPoint NCGShading::GetEndPoint(void) const
{


	// Get the end point
	return(mEndPoint);
}





//============================================================================
//		NCGShading::SetEndPoint : Set the end point.
//----------------------------------------------------------------------------
void NCGShading::SetEndPoint(const NPoint &thePoint)
{


	// Set the end point
	if (thePoint != mEndPoint)
		{
		mEndPoint = thePoint;
		ResetShading();
		}
}





//============================================================================
//		NCGShading::GetEndExtend : Get the end extend.
//----------------------------------------------------------------------------
bool NCGShading::GetEndExtend(void) const
{


	// Get the end extend
	return(mEndExtend);
}





//============================================================================
//		NCGShading::SetEndExtend : Set the end extend.
//----------------------------------------------------------------------------
void NCGShading::SetEndExtend(bool theValue)
{


	// Set the end extend
	if (theValue != mEndExtend)
		{
		mEndExtend = theValue;
		ResetShading();
		}
}





//============================================================================
//		NCGShading::GetEndRadius : Get the end radius.
//----------------------------------------------------------------------------
float NCGShading::GetEndRadius(void) const
{


	// Get the end radius
	return(mEndRadius);
}





//============================================================================
//		NCGShading::SetEndRadius : Set the end radius.
//----------------------------------------------------------------------------
void NCGShading::SetEndRadius(float theValue)
{


	// Validate our state
	NN_ASSERT(mMode == kShadingRadial);
	


	// Set the end radius
	if (!NMathUtilities::AreEqual(theValue, mEndRadius))
		{
		mEndRadius = theValue;
		ResetShading();
		}
}





//============================================================================
//		NCGShading::SetStartEndPoints : Set the start/end points.
//----------------------------------------------------------------------------
void NCGShading::SetStartEndPoints(const NRectangle &theRect, NPosition startPos, NPosition endPos)
{


	// Update our state
	SetStartPoint(theRect.GetPoint(startPos));
	SetEndPoint(  theRect.GetPoint(endPos));
}





//============================================================================
//		NCGShading::operator CGShadingRef : CGShadingRef-cast operator.
//----------------------------------------------------------------------------
NCGShading::operator CGShadingRef(void) const
{


	// Update the shading
	//
	// Since multiple attributes may be modified before a shading is ready
	// for use, we defer eveluating the shading until it's needed by CG.
	if (!mShading.IsValid())
		UpdateShading();



	// Get the shading
	return(mShading);
}





//============================================================================
//		NCGShading::= : Assignment operator.
//----------------------------------------------------------------------------
const NCGShading& NCGShading::operator = (const NCGShading &theShading)
{


	// Assign the shading
	if (this != &theShading)
		CopyShading(theShading);

	return(*this);
}





//============================================================================
//		NCGShading::Evaluate : Evaluate the shading.
//----------------------------------------------------------------------------
#pragma mark -
NColor NCGShading::Evaluate(float theSample)
{	NColor		theResult, colorOne, colorTwo;
	float		theRange, theWeight;
	NIndex		n, numSamples;



	// Evaluate the shading
	//
	// Provides a linear interpolation through the color sample list, by
	// identifying the relevant pair of colors for this sample and using
	// the color values to determine where each color starts/stops.
	//
	// We special case shadings of 1 or 2 colors, and ignore the sample
	// values to perform a straight no/pair shading.
	switch (mSamples.size()) {
		case 0:
			theResult = kNColorBlack;
			break;

		case 1:
			theResult = mSamples[0].theColor;
			break;
		
		case 2:
			theResult = NColor::Interpolate(mSamples[0].theColor, mSamples[1].theColor, theSample);
			break;
		
		default:
			// Find the appropriate pair
			numSamples = mSamples.size();
			theResult  = mSamples[0].theColor;
	
			for (n = numSamples-1; n >= 0; n--)
				{
				if (theSample >= mSamples[n].theValue)
					{
					// Use the last color
					if (n == (numSamples -1))
						theResult = mSamples[n].theColor;
				
				
					// Use the pair of colors
					else
						{
						colorOne = mSamples[n + 0].theColor;
						colorTwo = mSamples[n + 1].theColor;
					
						theRange  = mSamples[n+1].theValue - mSamples[n].theValue;
						theWeight = (theSample - mSamples[n].theValue) / theRange;
					
						theResult = NColor::Interpolate(colorOne, colorTwo, theWeight);
						}
				
					break;
					}
				}
			break;
		}
	
	return(theResult);
}





//============================================================================
//		NCGShading::InitializeSelf : Initialize ourselves.
//----------------------------------------------------------------------------
#pragma mark -
void NCGShading::InitializeSelf(void)
{


	// Initialize ourselves
	mMode = kShadingNone;
	
	ResetShading();
	
	mStartExtend = true;
	mStartRadius = 0.0f;

	mEndExtend = true;
	mEndRadius = 1.0f;
}





//============================================================================
//		NCGShading::ResetShading : Reset the shading.
//----------------------------------------------------------------------------
void NCGShading::ResetShading(void) const
{


	// Work around rdar://5604593
	//
	// 10.5 incorrectly caches the results of shading evaluation callbacks:
	//
	//		<http://lists.apple.com/archives/quartz-dev/2007/Nov/msg00052.html>
	//
	// To fully reset the shading object, we must also re-create the evaluate
	// callback to ensure the cached results will be discarded.
	mShading.SetObject(NULL);
	mEvaluate.SetObject(CreateEvaluateCallback());
}





//============================================================================
//		NCGShading::UpdateShading : Update the shading.
//----------------------------------------------------------------------------
void NCGShading::UpdateShading(void) const
{	CGColorSpaceRef		cgColorSpace;



	// Validate our state
	NN_ASSERT(!mShading.IsValid());



	// Create the new shading
	cgColorSpace = NCGColor::GetDeviceRGB();

	switch (mMode) {
		case kShadingNone:
			ResetShading();
			break;

		case kShadingLinear:
			mShading.SetObject(CGShadingCreateAxial( cgColorSpace, ToCG(mStartPoint), ToCG(mEndPoint), mEvaluate, mStartExtend, mEndExtend));
			break;

		case kShadingRadial:
			mShading.SetObject(CGShadingCreateRadial(cgColorSpace, ToCG(mStartPoint), mStartRadius, ToCG(mEndPoint), mEndRadius, mEvaluate, mStartExtend, mEndExtend));
			break;

		default:
			NN_LOG("Unknown shading mode: %d", mMode);
			break;
		}
}





//============================================================================
//		NCGShading::CopyShading : Copy a shading.
//----------------------------------------------------------------------------
void NCGShading::CopyShading(const NCGShading &theShading)
{


	// Reset our state
	//
	// The shading object and evaluate callback are instance-specific, since
	// the evaluate callback's user data is our this pointer.
	//
	// Those objects are never copied, and we initialize ourselves prior to
	// copying to allow CopyShading to be used from our copy constructor.
	InitializeSelf();



	// Copy the shading
	mMode = theShading.mMode;
	mSamples = theShading.mSamples;
	
	mStartPoint  = theShading.mStartPoint;
	mStartExtend = theShading.mStartExtend;
	mStartRadius = theShading.mStartRadius;

	mEndPoint  = theShading.mEndPoint;
	mEndExtend = theShading.mEndExtend;
	mEndRadius = theShading.mEndRadius;
}





//============================================================================
//		NCGShading::CreateEvaluateCallback : Create the evaluate callback.
//----------------------------------------------------------------------------
CGFunctionRef NCGShading::CreateEvaluateCallback(void) const
{	static const CGFunctionCallbacks	kCallbackInfo     = { 0, NCGShading::EvaluateCallback, NULL };
	static const CGFloat				kCallbackRange[]  = { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
	static const CGFloat				kCallbackDomain[] = { 0.0f, 1.0f };



	// Create the callback
	return(CGFunctionCreate((void *) this, 1, kCallbackDomain, 4, kCallbackRange, &kCallbackInfo));
}





//============================================================================
//		NCGShading::EvaluateCallback : Evaluate callback.
//----------------------------------------------------------------------------
void NCGShading::EvaluateCallback(void *info, const CGFloat *in, CGFloat *out)
{	float32_t		colRed, colGreen, colBlue, colAlpha;
	NCGShading		*thisPtr = (NCGShading *) info;
	NColor			theColor;



	// Validate our parameters
	NN_ASSERT(in[0] >= 0.0f && in[0] <= 1.0f);



	// Evaluate the shading
	theColor = thisPtr->Evaluate(in[0]);
	theColor.GetColor(colRed, colGreen, colBlue, colAlpha);

	out[0] = (CGFloat) colRed;
	out[1] = (CGFloat) colGreen;
	out[2] = (CGFloat) colBlue;
	out[3] = (CGFloat) colAlpha;
}

