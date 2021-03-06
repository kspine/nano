/*	NAME:
		NBitVector.h

	DESCRIPTION:
		Bit vector.
	
	COPYRIGHT:
		Copyright (c) 2006-2013, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
#ifndef NBITVECTOR_HDR
#define NBITVECTOR_HDR
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NStringFormatter.h"
#include "NContainer.h"
#include "NComparable.h"
#include "NEncodable.h"
#include "NRange.h"
#include "NData.h"





//============================================================================
//		Class declaration
//----------------------------------------------------------------------------
class NBitVector :	public NContainer,
					public NEncodable,
					public NComparable<NBitVector> {
public:
										NENCODABLE_DECLARE(NBitVector);

										NBitVector(const NBitVector &theValue);

										NBitVector(void);
	virtual							   ~NBitVector(void);


	// Clear the vector
	void								Clear(void);


	// Compare the value
	NComparison							Compare(const NBitVector &theValue) const;


	// Get/set the vector size
	//
	// Increasing the size will append 0'd bits to the vector.
	//
	// To amortize the cost of repeatedly growing a bit vector, extra capacity can
	// be requested to reserve storage for at least that many extra bits. These bits
	// can not be accessed until a subsequent SetSize increases the vector size.
	NIndex								GetSize(void) const;
	void								SetSize(NIndex theSize, NIndex extraCapacity=0);


	// Get/set a bit
	inline bool							GetBit(NIndex theIndex) const;
	inline void							SetBit(NIndex theIndex, bool theValue);


	// Get/set the bits
	//
	// Bits are returned in the least-most-significant numBits bits of the result.
	NData								GetBits(void)                            const;
	inline uint32_t						GetBits(NIndex theIndex, NIndex numBits) const;

	void								SetBits(bool		 theValue);
	void								SetBits(const NData &theData);


	// Append bits
	//
	// Bits are read from the least-most-significant numBits of theValue.
	void								AppendBit( bool	    theValue);
	void								AppendBits(uint32_t theValue, NIndex numBits);


	// Flip bits
	void								FlipBit(NIndex theIndex);
	void								FlipBits(const NRange &theRange=kNRangeAll);


	// Test bits
	NIndex								CountBits(	bool theValue, const NRange &theRange=kNRangeAll) const;
	bool								ContainsBit(bool theValue, const NRange &theRange=kNRangeAll) const;


	// Find bits
	NIndex								FindFirstBit(bool theValue, const NRange &theRange=kNRangeAll) const;
	NIndex								FindLastBit( bool theValue, const NRange &theRange=kNRangeAll) const;


	// Operators
	const NBitVector&					operator = (const NBitVector &theObject);
										operator NFormatArgument(void) const;


protected:
	// Encode/decode the object
	void								EncodeSelf(      NEncoder &theEncoder) const;
	void								DecodeSelf(const NEncoder &theEncoder);


private:
	void								UpdateSize( NIndex numBits);
	NIndex								GetByteSize(NIndex numBits) const;

	inline uint8_t					   *GetByteForBit(NIndex theIndex, NIndex &bitIndex) const;


private:
	NData								mData;
	NIndex								mSize;
	mutable uint8_t					   *mBytes;
};





//============================================================================
//		NBitVector::GetByteForBit : Get the byte that contains a bit.
//----------------------------------------------------------------------------
//		Note : Inlined for performance.
//----------------------------------------------------------------------------
inline uint8_t *NBitVector::GetByteForBit(NIndex theIndex, NIndex &bitIndex) const
{	NIndex		byteIndex;



	// Validate our parameters
	NN_ASSERT(theIndex >= 0 && theIndex < GetSize());



	// Get the byte
	//
	// The bit index as the index within the byte, not the vector.
	byteIndex = theIndex / 8;
	bitIndex  = 7 - (theIndex - (byteIndex * 8));
	
	return(&mBytes[byteIndex]);
}





//============================================================================
//		NBitVector::GetBit : Get a bit.
//----------------------------------------------------------------------------
//		Note : Inlined for performance.
//----------------------------------------------------------------------------
inline bool NBitVector::GetBit(NIndex theIndex) const
{	NIndex		bitIndex;
	uint8_t		theByte;



	// Get the bit
	theByte = *GetByteForBit(theIndex, bitIndex);
	
	return((theByte >> bitIndex) & 0x01);
}





//============================================================================
//		NBitVector::GetBits : Get the bits.
//----------------------------------------------------------------------------
//		Note : Inlined for performance.
//----------------------------------------------------------------------------
inline uint32_t NBitVector::GetBits(NIndex theIndex, NIndex numBits) const
{	uint32_t	theResult, theMask;
	NIndex		n;



	// Validate our parameters
	NN_ASSERT((theIndex + numBits) <= GetSize());
	NN_ASSERT(numBits >= 1 && numBits <= 32);



	// Get the bits
	theResult = 0;
	theMask	  = (1 << (numBits-1));

	for (n = 0; n < numBits; n++)
		{
		if (GetBit(theIndex + n))
			theResult |= theMask;

		theMask >>= 1;
		}

	return(theResult);
}





//============================================================================
//		NBitVector::SetBit : Set a bit.
//----------------------------------------------------------------------------
//		Note : Inlined for performance.
//----------------------------------------------------------------------------
inline void NBitVector::SetBit(NIndex theIndex, bool theValue)
{	NIndex		bitIndex;
	uint8_t		*bytePtr;



	// Set the bit
	bytePtr	= GetByteForBit(theIndex, bitIndex);

	if (theValue)
		*bytePtr |=  (1 << bitIndex);
	else
		*bytePtr &= ~(1 << bitIndex);
}





#endif // NBITVECTOR_HDR


