/*	NAME:
		NPropertyStore.cpp

	DESCRIPTION:
		Property store.
	
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
#include "NPropertyStore.h"





//============================================================================
//		Public constants
//----------------------------------------------------------------------------
const NString kNPropertyStoreChangedAllKey							= "*";





//============================================================================
//		NPropertyStore::NPropertyStore : Constructor.
//----------------------------------------------------------------------------
NPropertyStore::NPropertyStore(void)
{
}





//============================================================================
//		NPropertyStore::~NPropertyStore : Destructor.
//----------------------------------------------------------------------------
NPropertyStore::~NPropertyStore(void)
{
}





//============================================================================
//		NPropertyStore::HasKey : Does a key exist?
//----------------------------------------------------------------------------
bool NPropertyStore::HasKey(const NString &theKey, bool checkDefaults) const
{	bool	hasKey;



	// Validate our parameters
	NN_ASSERT(!theKey.IsEmpty());



	// Check the key
	hasKey = mProperties.HasKey(theKey);
	if (!hasKey && checkDefaults)
		hasKey = mDefaults.HasKey(theKey);
	
	return(hasKey);
}





//============================================================================
//		NPropertyStore::RemoveKey : Remove a key.
//----------------------------------------------------------------------------
void NPropertyStore::RemoveKey(const NString &theKey)
{


	// Validate our parameters
	NN_ASSERT(!theKey.IsEmpty());



	// Remove the key
	mProperties.RemoveKey(theKey);
}





//============================================================================
//		NPropertyStore::GetValue : Get a value.
//----------------------------------------------------------------------------
NVariant NPropertyStore::GetValue(const NString &theKey) const
{	NVariant		theValue;



	// Validate our parameters
	NN_ASSERT(!theKey.IsEmpty());
	


	// Get the value
	theValue = mProperties.GetValue(theKey);
	if (!theValue.IsValid())
		theValue = mDefaults.GetValue(theKey);
	
	return(theValue);
}





//============================================================================
//		NPropertyStore::SetValue : Set a value.
//----------------------------------------------------------------------------
void NPropertyStore::SetValue(const NString &theKey, const NVariant &theValue)
{


	// Validate our parameters
	NN_ASSERT(!theKey.IsEmpty());
	


	// Set the value
	mProperties.SetValue(theKey, theValue);
	BroadcastMessage(kMsgNPropertyStoreValueChanged, &theKey);
}





//============================================================================
//		NPropertyStore::GetValueBoolean : Get a boolean value.
//----------------------------------------------------------------------------
bool NPropertyStore::GetValueBoolean(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetBoolean(GetValue(theKey), theKey));
}





//============================================================================
//		NPropertyStore::GetValueInt32 : Get an int32_t value.
//----------------------------------------------------------------------------
int32_t NPropertyStore::GetValueInt32(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetInt32(GetValue(theKey), theKey));
}





//============================================================================
//		NPropertyStore::GetValueInt64 : Get an int64_t value.
//----------------------------------------------------------------------------
int64_t NPropertyStore::GetValueInt64(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetInt64(GetValue(theKey), theKey));
}





//============================================================================
//		NPropertyStore::GetValueFloat32 : Get a float32_t value.
//----------------------------------------------------------------------------
float32_t NPropertyStore::GetValueFloat32(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetFloat32(GetValue(theKey), theKey));
}





//============================================================================
//		NPropertyStore::GetValueFloat64 : Get a float64_t value.
//----------------------------------------------------------------------------
float64_t NPropertyStore::GetValueFloat64(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetFloat64(GetValue(theKey), theKey));
}





//============================================================================
//		NPropertyStore::GetValueString : Get a string value.
//----------------------------------------------------------------------------
NString NPropertyStore::GetValueString(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetString(GetValue(theKey), theKey));
}





//============================================================================
//		NPropertyStore::GetValueData : Get a data value.
//----------------------------------------------------------------------------
NData NPropertyStore::GetValueData(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetData(GetValue(theKey), theKey));
}





//============================================================================
//		NPropertyStore::GetValueDate : Get a date value.
//----------------------------------------------------------------------------
NDate NPropertyStore::GetValueDate(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetDate(GetValue(theKey), theKey));
}





//============================================================================
//		NPropertyStore::GetValueColor : Get a color value.
//----------------------------------------------------------------------------
NColor NPropertyStore::GetValueColor(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetColor(GetValue(theKey), theKey));
}





//============================================================================
//		NPropertyStore::GetValuePoint : Get a point value.
//----------------------------------------------------------------------------
NPoint NPropertyStore::GetValuePoint(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetPoint(GetValue(theKey), theKey));
}





//============================================================================
//		NPropertyStore::GetValueSize : Get a size value.
//----------------------------------------------------------------------------
NSize NPropertyStore::GetValueSize(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetSize(GetValue(theKey), theKey));
}





//============================================================================
//		NPropertyStore::GetValueRectangle : Get a rectangle value.
//----------------------------------------------------------------------------
NRectangle NPropertyStore::GetValueRectangle(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetRectangle(GetValue(theKey), theKey));
}





//============================================================================
//		NPropertyStore::GetValueVector : Get a vector value.
//----------------------------------------------------------------------------
NVector NPropertyStore::GetValueVector(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetVector(GetValue(theKey), theKey));
}





//============================================================================
//		NPropertyStore::GetValueArray : Get an array value.
//----------------------------------------------------------------------------
NArray NPropertyStore::GetValueArray(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetArray(GetValue(theKey), theKey));
}





//============================================================================
//		NPropertyStore::GetValueDictionary : Get a dictionary value.
//----------------------------------------------------------------------------
NDictionary NPropertyStore::GetValueDictionary(const NString &theKey) const
{


	// Get the value
	return(NSystemUtilities::GetDictionary(GetValue(theKey), theKey));
}





//============================================================================
//		NPropertyStore::GetProperties : Get the properties.
//----------------------------------------------------------------------------
NDictionary NPropertyStore::GetProperties(void) const
{


	// Get the properties
	return(mProperties);
}





//============================================================================
//		NPropertyStore::SetProperties : Set the properties.
//----------------------------------------------------------------------------
void NPropertyStore::SetProperties(const NDictionary &theProperties)
{


	// Set the properties
	mProperties = theProperties;
	BroadcastMessage(kMsgNPropertyStoreValueChanged, &kNPropertyStoreChangedAllKey);
}





//============================================================================
//		NPropertyStore::GetDefaults : Get the defaults.
//----------------------------------------------------------------------------
NDictionary NPropertyStore::GetDefaults(void)
{


	// Get the defaults
	return(mDefaults);
}





//============================================================================
//		NPropertyStore::SetDefaults : Set the defaults.
//----------------------------------------------------------------------------
void NPropertyStore::SetDefaults(const NDictionary &theDefaults)
{


	// Set the defaults
	mDefaults = theDefaults;
	BroadcastMessage(kMsgNPropertyStoreValueChanged, &kNPropertyStoreChangedAllKey);
}

