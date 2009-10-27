/*	NAME:
		NCocoa.h

	DESCRIPTION:
		Cocoa support.
	
	COPYRIGHT:
		Copyright (c) 2006-2009, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
#ifndef NCOCOA_HDR
#define NCOCOA_HDR
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NPoint.h"
#include "NSize.h"
#include "NRectangle.h"
#include "NRange.h"





//============================================================================
//		Inline functions
//----------------------------------------------------------------------------
// Convert to Cocoa
inline NSPoint ToNS(const NPoint &thePoint)
{
	return(NSMakePoint(thePoint.x, thePoint.y));
}

inline NSSize ToNS(const NSize &theSize)
{
	return(NSMakeSize(theSize.width, theSize.height));
}

inline NSRect ToNS(const NRectangle &theRect)
{
	return(NSMakeRect(theRect.origin.x, theRect.origin.y, theRect.size.width, theRect.size.height));
}

inline NSRange ToNS(const NRange &theRange)
{
	return(NSMakeRange(theRange.GetLocation(), theRange.GetSize()));
}



// Convert from Cocoa
inline NPoint ToNN(const NSPoint &thePoint)
{
	return(NPoint(thePoint.x, thePoint.y));
}

inline NSize ToNN(const NSSize &theSize)
{
	return(NSize(theSize.width, theSize.height));
}

inline NRectangle ToNN(const NSRect &theRect)
{
	return(NRectangle(theRect.origin.x, theRect.origin.y, theRect.size.width, theRect.size.height));
}

inline NRange ToNN(const NSRange &theRange)
{
	return(NRange(theRange.location, theRange.length));
}





#endif // NCOCOA_HDR




