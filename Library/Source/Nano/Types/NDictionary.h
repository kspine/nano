/*	NAME:
		NDictionary.h

	DESCRIPTION:
		Dictionary object.
	
	COPYRIGHT:
		Copyright (c) 2006-2013, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
#ifndef NDICTIONARY_HDR
#define NDICTIONARY_HDR
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NStringFormatter.h"
#include "NSharedValue.h"
#include "NContainer.h"
#include "NDebuggable.h"
#include "NEncodable.h"
#include "NString.h"
#include "NPoint.h"
#include "NSize.h"
#include "NRectangle.h"
#include "NColor.h"
#include "NDate.h"
#include "NArray.h"
#include "NVariant.h"
#include "NData.h"





//============================================================================
//		Types
//----------------------------------------------------------------------------
// Functors
typedef nfunctor<void (const NString &theKey, const NVariant &theValue)>	NDictionaryForEachFunctor;


// Lists
typedef std::map<NString, NVariant, NStringHashCompare>				NDictionaryValue;
typedef NDictionaryValue::iterator									NDictionaryValueIterator;
typedef NDictionaryValue::const_iterator							NDictionaryValueConstIterator;


// Value
typedef NSharedValue<NDictionaryValue>								NSharedValueDictionary;





//============================================================================
//		Class declaration
//----------------------------------------------------------------------------
class NDictionary :	public NContainer,
					public NEncodable,
					public NDebuggable,
					public NComparable<NDictionary>,
					public NSharedValueDictionary {
public:
										NENCODABLE_DECLARE(NDictionary);

										NDictionary(void);
	virtual							   ~NDictionary(void);


	// Get the size
	NIndex								GetSize(void) const;


	// Compare the value
	NComparison							Compare(const NDictionary &theValue) const;


	// Join two dictionaries
	//
	// Existing keys will have their values replaced.
	void								Join(const NDictionary &theValue);


	// Invert the dictionary
	//
	// A dictionary can only be inverted if all values are unique, non-empty, strings.
	bool								Invert(void);


	// Process each item
	void								ForEach(const NDictionaryForEachFunctor &theFunctor) const;


	// Does a key exist?
	bool								HasKey(const NString &theKey) const;


	// Remove a key
	void								RemoveKey(const NString &theKey);


	// Get the keys
	NStringList							GetKeys(bool sortKeys=false) const;


	// Get/set a value
	NVariant							GetValue(const NString &theKey) const;
	void								SetValue(const NString &theKey, const NVariant &theValue);


	// Get a type'd value
	//
	// If the value does not exist, or can not be returned as the specified type, 0/empty is returned.
	bool								GetValueBoolean(   const NString &theKey) const;
	int32_t								GetValueInt32(    const NString &theKey) const;
	int64_t								GetValueInt64(    const NString &theKey) const;
	float32_t							GetValueFloat32(   const NString &theKey) const;
	float64_t							GetValueFloat64(   const NString &theKey) const;
	NString								GetValueString(    const NString &theKey) const;
	NData								GetValueData(      const NString &theKey) const;
	NDate								GetValueDate(      const NString &theKey) const;
	NColor								GetValueColor(     const NString &theKey) const;
	NPoint								GetValuePoint(     const NString &theKey) const;
	NSize								GetValueSize(      const NString &theKey) const;
	NRectangle							GetValueRectangle( const NString &theKey) const;
	NVector								GetValueVector(    const NString &theKey) const;
	NArray								GetValueArray(     const NString &theKey) const;
	NDictionary							GetValueDictionary(const NString &theKey) const;


	// Operators
										operator NFormatArgument(void) const;


protected:
	// Get the null value
	const NDictionaryValue				*GetNullValue(void) const;


	// Encode/decode the object
	void								EncodeSelf(      NEncoder &theEncoder) const;
	void								DecodeSelf(const NEncoder &theEncoder);


private:
	void								ValueChanged(NDictionaryValue *theValue);

	NString								GetDebugID(const NString &theKey) const;


private:


};





#endif // NDICTIONARY_HDR


