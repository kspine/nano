/*	NAME:
		NDictionary.cpp

	DESCRIPTION:
		Dictionary object.
	
	COPYRIGHT:
		Copyright (c) 2006-2013, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NSystemUtilities.h"
#include "NSTLUtilities.h"
#include "NNumber.h"
#include "NEncoder.h"
#include "NDictionary.h"





//============================================================================
//		Implementation
//----------------------------------------------------------------------------
NENCODABLE_DEFINE(NDictionary);





//============================================================================
//		NDictionary::NDictionary : Constructor.
//----------------------------------------------------------------------------
NDictionary::NDictionary(void)
{


	// Initialise ourselves
	ValueChanged(NULL);
}





//============================================================================
//		NDictionary::~NDictionary : Destructor.
//----------------------------------------------------------------------------
NDictionary::~NDictionary(void)
{
}





//============================================================================
//		NDictionary::GetSize : Get the size.
//----------------------------------------------------------------------------
NIndex NDictionary::GetSize(void) const
{


	// Get the size
	return((NIndex) GetImmutable()->size());
}





//============================================================================
//		NDictionary::Compare : Compare the value.
//----------------------------------------------------------------------------
NComparison NDictionary::Compare(const NDictionary &theValue) const
{	const NDictionaryValue		*ourValue, *otherValue;
	NIndex						ourSize, otherSize;
	NComparison					theResult;



	// Get the state we need
	ourValue   =          GetImmutable();
	otherValue = theValue.GetImmutable();

	ourSize   = (NIndex) ourValue->size();
	otherSize = (NIndex) otherValue->size();



	// Compare the value
	//
	// We have no natural order, so the only real comparison is equality.
	theResult = GetComparison(ourSize, otherSize);

	if (theResult == kNCompareEqualTo)
		theResult = GetComparison(ourValue, otherValue);

	return(theResult);
}





//============================================================================
//		NDictionary::Join : Join two dictionaries.
//----------------------------------------------------------------------------
void NDictionary::Join(const NDictionary &theValue)
{	const NDictionaryValue				*theDict;
	NDictionaryValueConstIterator		theIter;



	// Get the state we need
	theDict = theValue.GetImmutable();



	// Join the dictionaries
	for (theIter = theDict->begin(); theIter != theDict->end(); theIter++)
		SetValue(theIter->first, theIter->second);
}





//============================================================================
//		NDictionary::Invert : Invert the dictinary.
//----------------------------------------------------------------------------
bool NDictionary::Invert(void)
{	NString							theKey, theValue;
	NDictionary						theResult;
	bool							canInvert;
	NStringList						theKeys;
	NStringListConstIterator		theIter;



	// Get the state we need
	theKeys   = GetKeys();
	canInvert = true;



	// Create the inverted dictionary
	for (theIter = theKeys.begin(); theIter != theKeys.end() && canInvert; theIter++)
		{
		theKey   = *theIter;
		theValue = GetValueString(theKey);

		canInvert = !theValue.IsEmpty() && !theResult.HasKey(theValue);
		if (canInvert)
			theResult.SetValue(theValue, theKey);
		}
	
	
	
	// Update our state
	if (canInvert)
		*this = theResult;
	
	return(canInvert);
}





//============================================================================
//		NDictionary::ForEach : Process each item.
//----------------------------------------------------------------------------
void NDictionary::ForEach(const NDictionaryForEachFunctor &theFunctor) const
{	const	NDictionaryValue			*theDict;
	NDictionaryValueConstIterator		theIter;



	// Get the state we need
	theDict = GetImmutable();



	// Process the array
	for (theIter = theDict->begin(); theIter != theDict->end(); theIter++)
		theFunctor(theIter->first, theIter->second);
}





//============================================================================
//		NDictionary::HasKey : Does a key exist?
//----------------------------------------------------------------------------
bool NDictionary::HasKey(const NString &theKey) const
{	bool								hasValue;
	const NDictionaryValue				*theDict;
	NDictionaryValueConstIterator		theIter;



	// Validate our parameters
	NN_ASSERT(!theKey.IsEmpty());



	// Find the value
	theDict  = GetImmutable();
	theIter  = theDict->find(theKey);
	hasValue = (theIter != theDict->end());

	return(hasValue);
}





//============================================================================
//		NDictionary::RemoveKey : Remove a key.
//----------------------------------------------------------------------------
void NDictionary::RemoveKey(const NString &theKey)
{	NDictionaryValue	*theDict;



	// Validate our parameters
	NN_ASSERT(!theKey.IsEmpty());



	// Get the state we need
	theDict = GetMutable();



	// Remove the key
	theDict->erase(theKey);
	
	ValueChanged(theDict);
}





//============================================================================
//		NDictionary::GetKeys : Get the keys.
//----------------------------------------------------------------------------
NStringList NDictionary::GetKeys(bool sortKeys) const
{	const NDictionaryValue				*theDict;
	NDictionaryValueConstIterator		theIter;
	NStringList							theKeys;



	// Get the state we need
	theDict = GetImmutable();



	// Get the keys
	for (theIter = theDict->begin(); theIter != theDict->end(); theIter++)
		theKeys.push_back(theIter->first);

	if (sortKeys)
		sort(theKeys);

	return(theKeys);
}





//============================================================================
//		NDictionary::GetValue : Get a value.
//----------------------------------------------------------------------------
NVariant NDictionary::GetValue(const NString &theKey) const
{	const NDictionaryValue				*theDict;
	NVariant							theValue;
	NDictionaryValueConstIterator		theIter;



	// Validate our parameters
	NN_ASSERT(!theKey.IsEmpty());



	// Get the value
	theDict = GetImmutable();
	theIter = theDict->find(theKey);

	if (theIter != theDict->end())
		{
		theValue = theIter->second;
		NN_ASSERT(theValue.IsValid());
		}

	return(theValue);
}





//============================================================================
//		NDictionary::SetValue : Set a value.
//----------------------------------------------------------------------------
void NDictionary::SetValue(const NString &theKey, const NVariant &theValue)
{	NDictionaryValue		*theDict;



	// Validate our parameters
	NN_ASSERT(!theKey.IsEmpty());
	NN_ASSERT(theValue.IsValid());



	// Set the value
	theDict            = GetMutable();
	(*theDict)[theKey] = theValue;

	ValueChanged(theDict);
}





//============================================================================
//		NDictionary::GetValueBoolean : Get a boolean value.
//----------------------------------------------------------------------------
bool NDictionary::GetValueBoolean(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetBoolean(GetValue(theKey), GetDebugID(theKey)));
}





//============================================================================
//		NDictionary::GetValueInt32 : Get an int32_t value.
//----------------------------------------------------------------------------
int32_t NDictionary::GetValueInt32(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetInt32(GetValue(theKey), GetDebugID(theKey)));
}





//============================================================================
//		NDictionary::GetValueInt64 : Get an int64_t value.
//----------------------------------------------------------------------------
int64_t NDictionary::GetValueInt64(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetInt64(GetValue(theKey), GetDebugID(theKey)));
}





//============================================================================
//		NDictionary::GetValueFloat32 : Get a float32_t value.
//----------------------------------------------------------------------------
float32_t NDictionary::GetValueFloat32(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetFloat32(GetValue(theKey), GetDebugID(theKey)));
}





//============================================================================
//		NDictionary::GetValueFloat64 : Get a float64_t value.
//----------------------------------------------------------------------------
float64_t NDictionary::GetValueFloat64(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetFloat64(GetValue(theKey), GetDebugID(theKey)));
}





//============================================================================
//		NDictionary::GetValueString : Get a string value.
//----------------------------------------------------------------------------
NString NDictionary::GetValueString(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetString(GetValue(theKey), GetDebugID(theKey)));
}





//============================================================================
//		NDictionary::GetValueData : Get a data value.
//----------------------------------------------------------------------------
NData NDictionary::GetValueData(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetData(GetValue(theKey), GetDebugID(theKey)));
}





//============================================================================
//		NDictionary::GetValueDate : Get a date value.
//----------------------------------------------------------------------------
NDate NDictionary::GetValueDate(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetDate(GetValue(theKey), GetDebugID(theKey)));
}





//============================================================================
//		NDictionary::GetValueColor : Get a color value.
//----------------------------------------------------------------------------
NColor NDictionary::GetValueColor(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetColor(GetValue(theKey), GetDebugID(theKey)));
}





//============================================================================
//		NDictionary::GetValuePoint : Get a point value.
//----------------------------------------------------------------------------
NPoint NDictionary::GetValuePoint(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetPoint(GetValue(theKey), GetDebugID(theKey)));
}





//============================================================================
//		NDictionary::GetValueSize : Get a size value.
//----------------------------------------------------------------------------
NSize NDictionary::GetValueSize(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetSize(GetValue(theKey), GetDebugID(theKey)));
}





//============================================================================
//		NDictionary::GetValueRectangle : Get a rectangle value.
//----------------------------------------------------------------------------
NRectangle NDictionary::GetValueRectangle(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetRectangle(GetValue(theKey), GetDebugID(theKey)));
}





//============================================================================
//		NDictionary::GetValueVector : Get a vector value.
//----------------------------------------------------------------------------
NVector NDictionary::GetValueVector(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetVector(GetValue(theKey), GetDebugID(theKey)));
}





//============================================================================
//		NDictionary::GetValueArray : Get an array value.
//----------------------------------------------------------------------------
NArray NDictionary::GetValueArray(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetArray(GetValue(theKey), GetDebugID(theKey)));
}





//============================================================================
//		NDictionary::GetValueDictionary : Get a dictionary value.
//----------------------------------------------------------------------------
NDictionary NDictionary::GetValueDictionary(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetDictionary(GetValue(theKey), GetDebugID(theKey)));
}





//============================================================================
//		NDictionary::NFormatArgument : NFormatArgument operator.
//----------------------------------------------------------------------------
NDictionary::operator NFormatArgument(void) const
{	NString		theResult;



	// Get the value
	theResult.Format("{size=%ld}", GetSize());

	return(theResult);
}





#pragma mark protected
//============================================================================
//		NDictionary::GetNullValue : Get the null value.
//----------------------------------------------------------------------------
const NDictionaryValue *NDictionary::GetNullValue(void) const
{	static NDictionaryValue		sNullValue;



	// Get the value
	return(&sNullValue);
}





//============================================================================
//      NDictionary::EncodeSelf : Encode the object.
//----------------------------------------------------------------------------
void NDictionary::EncodeSelf(NEncoder &theEncoder) const
{	bool								valueBoolean;
	NString								valueString;
	NData								valueData;
	const NDictionaryValue				*theDict;
	NVariant							theValue;
	NDictionaryValueConstIterator		theIter;
	NString								theKey;



	// Get the state we need
	theDict = GetImmutable();



	// Encode the object
	for (theIter = theDict->begin(); theIter != theDict->end(); theIter++)
		{
		theKey   = theIter->first;
		theValue = theIter->second;
		
		if (theValue.GetValue(valueBoolean))
			theEncoder.EncodeBoolean(theKey, valueBoolean);

		else if (theValue.IsNumeric())
			theEncoder.EncodeNumber(theKey, NNumber(theValue));

		else if (theValue.GetValue(valueString))
			theEncoder.EncodeString(theKey, valueString);

		else if (theValue.GetValue(valueData))
			theEncoder.EncodeData(theKey, valueData);

		else
			{
			if (NEncoder::CanEncode(theValue))
				theEncoder.EncodeObject(theKey, theValue);
			else
 				NN_LOG("Unable to encode object '%@' (%s)", theKey, theValue.GetType().name());
			}
		}
}





//============================================================================
//      NDictionary::DecodeSelf : Decode the object.
//----------------------------------------------------------------------------
void NDictionary::DecodeSelf(const NEncoder &theEncoder)
{	NNumber							theNumber;
	NStringList						theKeys;
	NEncodedType					theType;
	NStringListConstIterator		theIter;
	NString							theKey;



	// Get the state we need
	theKeys = theEncoder.GetKeys();



	// Decode the object
	for (theIter = theKeys.begin(); theIter != theKeys.end(); theIter++)
		{
		// Get the state we need
		theKey  = *theIter;
		theType = theEncoder.GetValueType(theKey);



		// Decode the value
		switch (theType) {
			case kNEncodedBoolean:
				SetValue(theKey, theEncoder.DecodeBoolean(theKey));
				break;

			case kNEncodedNumber:
				SetValue(theKey, theEncoder.DecodeNumber(theKey));
				break;

			case kNEncodedString:
				SetValue(theKey, theEncoder.DecodeString(theKey));
				break;

			case kNEncodedData:
				SetValue(theKey, theEncoder.DecodeData(theKey));
				break;

			case kNEncodedObject:
				SetValue(theKey, theEncoder.DecodeObject(theKey));
				break;
			
			case kNEncodedUnknown:
			default:
				NN_LOG("Unknown encoder type %d (%@) - skipping", theType, theKey);
				break;
			}
		}
}





#pragma mark private
//============================================================================
//      NDictionary::ValueChanged : Our value has been changed.
//----------------------------------------------------------------------------
void NDictionary::ValueChanged(NDictionaryValue *theValue)
{	NIndex		theSize;



	// Compiler warnings
	NN_UNUSED(theSize);
	NN_UNUSED(theValue);



	// Update the debug summary
#if NN_DEBUG
	theSize = (theValue == NULL) ? 0 : (theValue->size());

	UpdateSummary("%ld %s", theSize, theSize == 1 ? "value" : "values");
#endif
}





//============================================================================
//		NDictionary::GetDebugID : Get the debug ID for a value.
//----------------------------------------------------------------------------
NString NDictionary::GetDebugID(const NString &theKey) const
{	NString		theID;



	// Validate our parameters
	NN_ASSERT(!theKey.IsEmpty());

	NN_UNUSED(theKey);



	// Get the ID
#if NN_DEBUG
	if (HasKey(theKey))
		theID = theKey;
#endif

	return(theID);
}


