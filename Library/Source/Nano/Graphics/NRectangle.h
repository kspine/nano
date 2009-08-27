/*	NAME:
		NRectangle.h

	DESCRIPTION:
		Rectangle object.
	
	COPYRIGHT:
		Copyright (c) 2006-2009, refNum Software
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
#include "NPoint.h"
#include "NSize.h"





//============================================================================
//		Types
//----------------------------------------------------------------------------
// Classes
class NRectangle;
class NRectangle32;
class NRectangle64;


// Lists
typedef std::vector<NRectangle>										NRectangleList;
typedef NRectangleList::iterator									NRectangleListIterator;
typedef NRectangleList::const_iterator								NRectangleListConstIterator;

typedef std::vector<NRectangle32>									NRectangle32List;
typedef NRectangle32List::iterator									NRectangle32ListIterator;
typedef NRectangle32List::const_iterator							NRectangle32ListConstIterator;

typedef std::vector<NRectangle64>									NRectangle64List;
typedef NRectangle64List::iterator									NRectangle64ListIterator;
typedef NRectangle64List::const_iterator							NRectangle64ListConstIterator;





//============================================================================
//		Class declaration
//----------------------------------------------------------------------------
template<class T> class NRectangleT : public NComparable< NRectangleT<T> > {
public:
										NRectangleT(const T x, T y, T width, T height);
										NRectangleT(const NPointT<T> &origin, const NSizeT<T> &size);

										NRectangleT(void);
	virtual							   ~NRectangleT(void);


	// Clear the rectangle
	void								Clear(void);


	// Test the rectangle
	bool								IsEmpty(void) const;


	// Compare the value
	NComparison							Compare(const NRectangleT<T> &theValue) const;


	// Does a rectangle contain another?
	bool								Contains(   const NRectangleT<T> &theRect) const;
	bool								ContainedBy(const NRectangleT<T> &theRect) const;


	// Does a rectangle intersect another?
	bool								Intersects(const NRectangleT<T> &theRect) const;


	// Manipulate the rectangle
	void								Normalize(void);
	void								UnionWith(    const NRectangleT<T> &theRect);
	void								IntersectWith(const NRectangleT<T> &theRect);

	NRectangleT<T>						GetNormalized(void)                       const;
	NRectangleT<T>						GetUnion(       const NRectangleT<T> &theRect) const;
	NRectangleT<T>						GetIntersection(const NRectangleT<T> &theRect) const;


	// Get the bounds
	T									GetMinX(void) const;
	T									GetMinY(void) const;
	T									GetMaxX(void) const;
	T									GetMaxY(void) const;


	// Operators
										operator NFormatArgument(void) const;


public:
	NPointT<T>							origin;
	NSizeT<T>							size;
};





//============================================================================
//		Class declaration
//----------------------------------------------------------------------------
class NRectangle32 : public NRectangleT<Float32> {
public:
										NRectangle32(Float32 x, Float32 y, Float32 width, Float32 height)	: NRectangleT<Float32>(x, y, width, height) { }
										NRectangle32(const NPoint32 &origin, const NSize32 &size)			: NRectangleT<Float32>(origin, size)        { }

										NRectangle32(void) { }
	virtual							   ~NRectangle32(void) { }
};





//============================================================================
//		Class declaration
//----------------------------------------------------------------------------
class NRectangle64 : public NRectangleT<Float64> {
public:
										NRectangle64(Float64 x, Float64 y, Float64 width, Float64 height)	: NRectangleT<Float64>(x, y, width, height) { }
										NRectangle64(const NPoint64 &origin, const NSize64 &size)			: NRectangleT<Float64>(origin, size)        { }

										NRectangle64(void) { }
	virtual							   ~NRectangle64(void) { }
};





//============================================================================
//		Class declaration
//----------------------------------------------------------------------------
class NRectangle :	public NEncodable,
					public NRectangle32 {
public:
										NENCODABLE_DECLARE(NRectangle);

										NRectangle(const NRectangle32 &theRectangle);
										NRectangle(const NRectangle64 &theRectangle);

										NRectangle(Float32 x, Float32 y, Float32 width, Float32 height);
										NRectangle(Float64 x, Float64 y, Float64 width, Float64 height);

										NRectangle(const NPoint32 &origin, const NSize32 &size);
										NRectangle(const NPoint64 &origin, const NSize64 &size);

										NRectangle(void);
	virtual							   ~NRectangle(void);


	// Operators
										operator NRectangle64(void) const;


protected:
	// Encode/decode the object
	void								EncodeSelf(      NEncoder &theEncoder) const;
	void								DecodeSelf(const NEncoder &theEncoder);
};





//============================================================================
//		Template files
//----------------------------------------------------------------------------
#define   NRECTANGLE_CPP
#include "NRectangle.cpp"
#undef    NRECTANGLE_CPP







#endif // NRECTANGLE_HDR

