/*	NAME:
		NPoint.h

	DESCRIPTION:
		Point object.
	
	COPYRIGHT:
		Copyright (c) 2006-2009, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
#ifndef NPOINT_HDR
#define NPOINT_HDR
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NStringFormatter.h"
#include "NComparable.h"
#include "NEncodable.h"
#include "NNumber.h"





//============================================================================
//		Types
//----------------------------------------------------------------------------
// Classes
template<class T> class NVectorT;

class NPoint;
class NPoint32;
class NPoint64;


// Lists
typedef std::vector<NPoint>											NPointList;
typedef NPointList::iterator										NPointListIterator;
typedef NPointList::const_iterator									NPointListConstIterator;

typedef std::vector<NPoint32>										NPoint32List;
typedef NPoint32List::iterator										NPoint32ListIterator;
typedef NPoint32List::const_iterator								NPoint32ListConstIterator;

typedef std::vector<NPoint64>										NPoint64List;
typedef NPoint64List::iterator										NPoint64ListIterator;
typedef NPoint64List::const_iterator								NPoint64ListConstIterator;





//============================================================================
//		Class declaration
//----------------------------------------------------------------------------
template<class T> class NPointT : public NComparable< NPointT<T> > {
public:
										NPointT(T x, T y);

										NPointT(void);
	virtual							   ~NPointT(void);


	// Clear the point
	void								Clear(void);


	// Test the point
	bool								IsZero(void) const;


	// Compare the value
	NComparison							Compare(const NPointT<T> &theValue) const;


	// Add/subtract a vector
	void								Add(     const NVectorT<T> &theVector);
	void								Subtract(const NVectorT<T> &theVector);


	// Get the distance to a points
	T									GetDistance( const NPointT<T> &thePoint, bool getApprox=true) const;
	T									GetDistance2(const NPointT<T> &thePoint)                      const;


	// Operators
										operator NFormatArgument(void) const;


public:
	T									x;
	T									y;
};





//============================================================================
//		Class declaration
//----------------------------------------------------------------------------
class NPoint32 : public NPointT<Float32> {
public:
										NPoint32(Float32 x, Float32 y) : NPointT<Float32>(x, y) { }

										NPoint32(void) { }
	virtual							   ~NPoint32(void) { }
};





//============================================================================
//		Class declaration
//----------------------------------------------------------------------------
class NPoint64 : public NPointT<Float64> {
public:
										NPoint64(Float64 x, Float64 y) : NPointT<Float64>(x, y) { }

										NPoint64(void) { }
	virtual							   ~NPoint64(void) { }
};





//============================================================================
//		Class declaration
//----------------------------------------------------------------------------
class NPoint :	public NEncodable,
				public NPoint32 {
public:
										NENCODABLE_DECLARE(NPoint);

										NPoint(const NPoint32 &thePoint);
										NPoint(const NPoint64 &thePoint);

										NPoint(Float32 x, Float32 y);
										NPoint(Float64 x, Float64 y);

										NPoint(void);
	virtual							   ~NPoint(void);


	// Operators
										operator NPoint64(void) const;


protected:
	// Encode/decode the object
	void								EncodeSelf(      NEncoder &theEncoder) const;
	void								DecodeSelf(const NEncoder &theEncoder);
};





//============================================================================
//		Template files
//----------------------------------------------------------------------------
#define   NPOINT_CPP
#include "NPoint.cpp"
#undef    NPOINT_CPP





#endif // NPOINT_HDR

