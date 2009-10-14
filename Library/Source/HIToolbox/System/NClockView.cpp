/*	NAME:
		NClockView.cpp

	DESCRIPTION:
		Clock view.
	
	COPYRIGHT:
		Copyright (c) 2006, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NClockView.h"





//============================================================================
//		Internal macros
//----------------------------------------------------------------------------
DEFINE_HIVIEW_SUBCLASS(NClockView, kHIClockViewClassID, NULL);





//============================================================================
//		NClockView::NClockView : Constructor.
//----------------------------------------------------------------------------
NClockView::NClockView(HIWindowRef			theWindow,
						const HIRect		&theRect,
						ControlClockType	clockType,
						ControlClockFlags	clockFlags,
						bool				isAnimating)
{	HIViewRef	theView;
	OSStatus	theErr;



	// Create the view
	theErr = CreateClockControl(NULL, &kQDRectZero, clockType, clockFlags, &theView);
	NN_ASSERT_NOERR(theErr);
	
	ConstructView(theView, theWindow, &theRect);



	// Configure the view
	SetAnimating(isAnimating);
}





//============================================================================
//		NClockView::~NClockView : Destructor.
//----------------------------------------------------------------------------
NClockView::~NClockView(void)
{
}





//============================================================================
//		NClockView::GetDate : Get the date.
//----------------------------------------------------------------------------
NCFDate NClockView::GetDate(void) const
{	LongDateRec		longDate;
	NCFDate			theDate;



	// Get the date
	GetControlData(kControlEntireControl, kControlClockLongDateTag, sizeof(longDate), &longDate);
	
	if (longDate.ld.year != 0)
		theDate = NCFDate(NCFDate::GetGregorianDate(longDate));
	
	return(theDate);
}





//============================================================================
//		NClockView::SetDate : Set the date.
//----------------------------------------------------------------------------
void NClockView::SetDate(const NCFDate &theDate)
{	LongDateRec		longDate;



	// Set the date
	longDate = NCFDate::GetLongDate(theDate.GetDate());
	
	SetControlData(kControlEntireControl, kControlClockLongDateTag, sizeof(longDate), &longDate);
}





//============================================================================
//		NClockView::IsAnimating : Is the view animating?
//----------------------------------------------------------------------------
bool NClockView::IsAnimating(void) const
{


	// Get the flag
	return(GetControlBoolean(kControlEntireControl, kControlClockAnimatingTag));
}





//============================================================================
//		NClockView::SetAnimating : Set the animating state.
//----------------------------------------------------------------------------
void NClockView::SetAnimating(bool isAnimating)
{


	// Set the flag
	SetControlBoolean(kControlEntireControl, kControlClockAnimatingTag, isAnimating);
}

