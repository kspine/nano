/*	NAME:
		NSize.h

	DESCRIPTION:
		Size object.
	
	COPYRIGHT:
		Copyright (c) 2006-2013, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
#ifndef NSIZE_HDR
#define NSIZE_HDR
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NStringFormatter.h"
#include "NComparable.h"
#include "NEncodable.h"
#include "NVariant.h"





//============================================================================
//		Types
//----------------------------------------------------------------------------
// Classes
template <class T> class NSizeT;

typedef NSizeT<float64_t> NSize64;
typedef NSizeT<float32_t> NSize32;
typedef NSize32           NSize;


// Lists
typedef std::vector<NSize64>										NSize64List;
typedef NSize64List::iterator										NSize64ListIterator;
typedef NSize64List::const_iterator									NSize64ListConstIterator;

typedef std::vector<NSize32>										NSize32List;
typedef NSize32List::iterator										NSize32ListIterator;
typedef NSize32List::const_iterator									NSize32ListConstIterator;

typedef std::vector<NSize>											NSizeList;
typedef NSizeList::iterator											NSizeListIterator;
typedef NSizeList::const_iterator									NSizeListConstIterator;





//============================================================================
//		Class declaration
//----------------------------------------------------------------------------
template<class T> class NSizeT {
public:
										NSizeT(const NVariant &theValue);

										NSizeT(T width, T height);
										NSizeT(void);


	// Clear the size
	void								Clear(void);


	// Test the size
	bool								IsEmpty(void) const;


	// Compare the value
	NComparison							Compare(const NSizeT<T> &theValue) const;


	// Manipulate the size
	void								Normalize(void);
	void								MakeIntegral(void);
	void								Resize(T deltaX, T deltaY);
	void								Scale( T scaleBy);

	NSizeT<T>							GetNormalized(void)            const;
	NSizeT<T>							GetIntegral(void)              const;
	NSizeT<T>							GetResized(T deltaX, T deltaY) const;
	NSizeT<T>							GetScaled( T scaleBy)          const;


	// Operators
	NCOMPARABLE_OPERATORS(NSizeT<T>)

										operator NEncodable(     void) const;
										operator NFormatArgument(void) const;


public:
	T									width;
	T									height;
};





//============================================================================
//		Template files
//----------------------------------------------------------------------------
#define   NSIZE_CPP
#include "NSize.cpp"
#undef    NSIZE_CPP





#endif // NSIZE_HDR


