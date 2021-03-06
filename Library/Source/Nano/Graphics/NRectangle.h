/*	NAME:
		NRectangle.h

	DESCRIPTION:
		Rectangle object.
	
	COPYRIGHT:
		Copyright (c) 2006-2013, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
#ifndef NRECTANGLE_HDR
#define NRECTANGLE_HDR
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NStringFormatter.h"
#include "NComparable.h"
#include "NEncodable.h"
#include "NVariant.h"
#include "NPoint.h"
#include "NSize.h"





//============================================================================
//		Constants
//----------------------------------------------------------------------------
// Positions
typedef enum {
	kNPositionAlert,
	kNPositionCenter,
	kNPositionLeft,
	kNPositionRight,
	kNPositionTop,
	kNPositionTopLeft,
	kNPositionTopRight,
	kNPositionBottom,
	kNPositionBottomLeft,
	kNPositionBottomRight
} NPosition;





//============================================================================
//		Types
//----------------------------------------------------------------------------
// Classes
template<class T> class NRectangleT;

typedef NRectangleT<float64_t> NRectangle64;
typedef NRectangleT<float32_t> NRectangle32;
typedef NRectangle32           NRectangle;


// Lists
typedef std::vector<NRectangle64>									NRectangle64List;
typedef NRectangle64List::iterator									NRectangle64ListIterator;
typedef NRectangle64List::const_iterator							NRectangle64ListConstIterator;

typedef std::vector<NRectangle32>									NRectangle32List;
typedef NRectangle32List::iterator									NRectangle32ListIterator;
typedef NRectangle32List::const_iterator							NRectangle32ListConstIterator;

typedef std::vector<NRectangle>										NRectangleList;
typedef NRectangleList::iterator									NRectangleListIterator;
typedef NRectangleList::const_iterator								NRectangleListConstIterator;





//============================================================================
//		Class declaration
//----------------------------------------------------------------------------
template<class T> class NRectangleT {
public:
										NRectangleT(const NVariant &theValue);

										NRectangleT(const T x, T y, T width, T height);
										NRectangleT(                T width, T height);

										NRectangleT(const NPointT<T> &origin, const NSizeT<T> &size);
										NRectangleT(                          const NSizeT<T> &size);

										NRectangleT(void);


	// Clear the rectangle
	void								Clear(void);


	// Test the rectangle
	bool								IsEmpty(void) const;


	// Compare the value
	NComparison							Compare(const NRectangleT<T> &theValue) const;


	// Does a rectangle contain an object?
	bool								Contains(const NPointT<T>     &thePoint) const;
	bool								Contains(const NRectangleT<T> &theRect)  const;


	// Does a rectangle intersect another?
	bool								Intersects(const NRectangleT<T> &theRect) const;


	// Position the rectangle
	void								ScaleToFit( const NRectangleT<T> &theRect);
	void								SetPosition(const NRectangleT<T> &theRect, NPosition thePosition);


	// Manipulate the rectangle
	void								Normalize(void);
	void								MakeIntegral(void);
	void								UnionWith(    const NRectangleT<T> &theRect);
	void								IntersectWith(const NRectangleT<T> &theRect);
	void								Inset( T deltaX, T deltaY);
	void								Offset(T deltaX, T deltaY);
	void								Scale( T scaleBy);

	NRectangleT<T>						GetNormalized(void)                            const;
	NRectangleT<T>						GetIntegral(void)                              const;
	NRectangleT<T>						GetUnion(       const NRectangleT<T> &theRect) const;
	NRectangleT<T>						GetIntersection(const NRectangleT<T> &theRect) const;
	NRectangleT<T>						GetInset( T deltaX, T deltaY)                  const;
	NRectangleT<T>						GetOffset(T deltaX, T deltaY)                  const;
	NRectangleT<T>						GetScaled(T scaleBy)                           const;


	// Query the rectangle
	NPointT<T>							GetCenter(void)                 const;
	NPointT<T>							GetPoint(NPosition thePosition) const;

	T									GetMinX(void) const;
	T									GetMinY(void) const;

	T									GetMidX(void) const;
	T									GetMidY(void) const;

	T									GetMaxX(void) const;
	T									GetMaxY(void) const;


	// Operators
	NCOMPARABLE_OPERATORS(NRectangleT<T>)

										operator NEncodable(     void) const;
										operator NFormatArgument(void) const;


public:
	NPointT<T>							origin;
	NSizeT<T>							size;
};





//============================================================================
//		Template files
//----------------------------------------------------------------------------
#define   NRECTANGLE_CPP
#include "NRectangle.cpp"
#undef    NRECTANGLE_CPP







#endif // NRECTANGLE_HDR


