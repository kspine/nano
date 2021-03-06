/*	NAME:
		NEncoder.cpp

	DESCRIPTION:
		Object encoder.

	COPYRIGHT:
		Copyright (c) 2006-2013, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NMathUtilities.h"
#include "NDataCompressor.h"
#include "NTargetPOSIX.h"
#include "NDataEncoder.h"
#include "NXMLEncoder.h"
#include "NEncodable.h"
#include "NEncoder.h"





//============================================================================
//		Internal constants
//----------------------------------------------------------------------------
// Magic
//
// These values are considered to be fixed, and will never change.
static const uint8_t kMagicXML_1_0[]								= { 0x3C, 0x3F, 0x78, 0x6D, 0x6C, 0x20  };
static const uint8_t kMagicBinary_1_0[]								= { 0x4E, 0x65, 0x4E, 0x63, 0x30, 0x30, 0x30, 0x31 };


// Tokens
static const NString kTokenBoolean									= "bool";
static const NString kTokenData										= "data";
static const NString kTokenEncoder									= "encoder";
static const NString kTokenNumber									= "number";
static const NString kTokenObject									= "object";
static const NString kTokenRoot										= "root";
static const NString kTokenString									= "string";

static const NString kTokenClass									= "class";
static const NString kTokenFalse									= "false";
static const NString kTokenKey										= "key";
static const NString kTokenTrue										= "true";
static const NString kTokenVersion									= "version";

static const NString kVersion_1_0									= "1.0";





//============================================================================
//		NEncoder::NEncoder : Constructor.
//----------------------------------------------------------------------------
NEncoder::NEncoder(void)
{


	// Initialise ourselves
	mState    = kNEncoderIdle;
	mNodeRoot = NULL;
}





//============================================================================
//		NEncoder::~NEncoder : Destructor.
//----------------------------------------------------------------------------
NEncoder::~NEncoder(void)
{


	// Validate our state
	NN_ASSERT(mState    == kNEncoderIdle);
	NN_ASSERT(mNodeRoot == NULL);
	NN_ASSERT(mNodeStack.empty());
}





//============================================================================
//		NEncoder::Encode : Encode an object.
//----------------------------------------------------------------------------
NData NEncoder::Encode(const NVariant &theObject, NEncoderFormat theFormat)
{	NXMLNode		*theRoot;
	NData			theData;



	// Validate our parameters and state
	NN_ASSERT(CanEncode(theObject));
	NN_ASSERT(mState == kNEncoderIdle);



	// Get the state we need
	mState    = kNEncoderEncoding;
	mNodeRoot = new NXMLNode(kNXMLNodeDocument, "");
	theRoot   = NULL;



	// Encode the object
	if (!CanEncode(theObject))
		goto cleanup;

	EncodeObject(kTokenRoot, theObject);

	if (mNodeRoot->GetChildren()->size() == 1)
		theRoot = mNodeRoot->GetChild(0);

	if (theRoot == NULL)
		{
		NN_LOG("Unable to encode object!");
		goto cleanup;
		}



	// Encode the data
	//
	// The encoded object is extracted as a root XML node.
	mNodeRoot->RemoveChild(theRoot, false);

	switch (theFormat) {
		case kNEncoderXML_1_0:
			theData = EncodeXML_1_0(theRoot);
			break;

		case kNEncoderBinary_1_0:
			theData = EncodeBinary_1_0(theRoot);
			break;

		default:
			NN_LOG("Unknown encoder format: %d", theFormat);
			break;
		}



	// Clean up
cleanup:
	delete mNodeRoot;
	delete theRoot;

	mNodeRoot = NULL;
	mState    = kNEncoderIdle;

	return(theData);
}





//============================================================================
//		NEncoder::Decode : Decode an object.
//----------------------------------------------------------------------------
NVariant NEncoder::Decode(const NData &theData)
{	NEncoderFormat		theFormat;
	NVariant			theObject;
	NXMLNode			*theRoot;



	// Get the state we need
	mState    = kNEncoderDecoding;
	mNodeRoot = new NXMLNode(kNXMLNodeDocument, "");
	theRoot   = NULL;



	// Decode the data
	theFormat = GetFormat(theData);

	switch (theFormat) {
		case kNEncoderInvalid:
			// Not an object
			break;

		case kNEncoderXML_1_0:
			theRoot = DecodeXML_1_0(theData);
			break;

		case kNEncoderBinary_1_0:
			theRoot = DecodeBinary_1_0(theData);
			break;

		default:
			NN_LOG("Unknown encoder format: %d", theFormat);
			break;
		}



	// Decode the object
	if (theRoot != NULL)
		{
		mNodeRoot->AddChild(theRoot);
		theObject = DecodeObject(kTokenRoot);
		}



	// Clean up
	delete mNodeRoot;

	mNodeRoot = NULL;
	mState    = kNEncoderIdle;
	
	return(theObject);
}





//============================================================================
//		NEncoder::CanEncode : Can an object be encoded?
//----------------------------------------------------------------------------
bool NEncoder::CanEncode(const NVariant &theValue)
{	NEncoderClassInfoMapConstIterator	theIter;
	NEncoderClassInfoMap				theInfo;



	// Validate our parameters
	NN_ASSERT(theValue.IsValid());



	// Get the state we need
	theInfo = GetClassesInfo();



	// Check the classes
	//
	// NVariants can't be cast back to an NEncodable, only to their leaf type, which
	// means we can't simply cast to see if the object is an NEncodable.
	//
	// In addition, encoders are registered as a set of functors rather than a class
	// instance (to allow encoders to encode other classes; see NPoint/etc), and so
	// we need to query the registered classes rather than theValue.
	for (theIter = theInfo.begin(); theIter != theInfo.end(); theIter++)
		{
		if (theIter->second.canEncode(theValue))
			return(true);
		}
	
	return(false);
}





#pragma mark protected
//============================================================================
//		NEncoder::GetKeys : Get the current object's keys.
//----------------------------------------------------------------------------
NStringList NEncoder::GetKeys(void) const
{	const NXMLNodeList				*theChildren;
	const NXMLNode					*theParent;
	NXMLNodeListConstIterator		theIter;
	NStringList						theKeys;
	NString							theKey;



	// Validate our state
	NN_ASSERT(mState != kNEncoderIdle);



	// Get the state we need
	theParent   = GetParentNode();
	theChildren = theParent->GetChildren();



	// Get the keys
	for (theIter = theChildren->begin(); theIter != theChildren->end(); theIter++)
		{
		theKey = (*theIter)->GetElementAttribute(kTokenKey);
		theKeys.push_back(theKey);
		}
	
	return(theKeys);
}





//============================================================================
//		NEncoder::GetValueType : Get a value type from the current object.
//----------------------------------------------------------------------------
NEncodedType NEncoder::GetValueType(const NString &theKey) const
{	const NXMLNode		*theChild;
	NEncodedType		theType;



	// Validate our state
	NN_ASSERT(mState != kNEncoderIdle);



	// Get the state we need
	theChild = GetChildNode(theKey);



	// Get the type
	if (theChild->IsElement(kTokenBoolean))
		theType = kNEncodedBoolean;
	
	else if (theChild->IsElement(kTokenNumber))
		theType = kNEncodedNumber;
	
	else if (theChild->IsElement(kTokenString))
		theType = kNEncodedString;
	
	else if (theChild->IsElement(kTokenData))
		theType = kNEncodedData;
	
	else if (theChild->IsElement(kTokenObject))
		theType = kNEncodedObject;
	
	else
		{
		NN_LOG("Unknown encoded type: (%d) %@", theChild->GetType(), theChild->GetTextValue());
		theType = kNEncodedUnknown;
		}
	
	return(theType);
}





//============================================================================
//		NEncoder::EncodeBoolean : Encode a boolean.
//----------------------------------------------------------------------------
void NEncoder::EncodeBoolean(const NString &theKey, bool theValue)
{	NString		valueText;



	// Validate our state
	NN_ASSERT(mState == kNEncoderEncoding);



	// Encode the value
	valueText = theValue ? kTokenTrue : kTokenFalse;

	EncodeChild(theKey, valueText, kTokenBoolean);
}





//============================================================================
//		NEncoder::EncodeNumber : Encode a number.
//----------------------------------------------------------------------------
void NEncoder::EncodeNumber(const NString &theKey, const NNumber &theValue)
{	NString		valueText;



	// Validate our state
	NN_ASSERT(mState == kNEncoderEncoding);
	
	
	
	// Encode the value
	valueText = theValue.GetString();
	
	EncodeChild(theKey, valueText, kTokenNumber);
}





//============================================================================
//		NEncoder::EncodeData : Encode data.
//----------------------------------------------------------------------------
void NEncoder::EncodeData(const NString &theKey, const NData &theValue)
{	NDataEncoder	theEncoder;
	NString			valueText;



	// Validate our state
	NN_ASSERT(mState == kNEncoderEncoding);



	// Encode the value
	valueText = theEncoder.Encode(theValue, kNDataEncodingB64);

	EncodeChild(theKey, valueText, kTokenData);
}





//============================================================================
//		NEncoder::EncodeString : Encode a string.
//----------------------------------------------------------------------------
void NEncoder::EncodeString(const NString &theKey, const NString &theValue)
{


	// Validate our state
	NN_ASSERT(mState == kNEncoderEncoding);



	// Encode the value
	EncodeChild(theKey, theValue, kTokenString);
}





//============================================================================
//		NEncoder::EncodeObject : Encode an object.
//----------------------------------------------------------------------------
void NEncoder::EncodeObject(const NString &theKey, const NVariant &theValue)
{	NEncoderClassInfo					classInfo;
	NString								className;
	NXMLNode							*theNode;
	NEncoderClassInfoMapConstIterator	theIter;
	NEncoderClassInfoMap				theInfo;



	// Validate our parameters and state
	NN_ASSERT(CanEncode(theValue));

	NN_ASSERT(mState == kNEncoderEncoding);



	// Get the state we need
	theInfo = GetClassesInfo();



	// Encode the object
	//
	// As per CanEncode, we need to iterate to find the class that can encode this object.
	//
	// If this proves a bottleneck then we could try to cast the object to an NEncodable,
	// and provide a method in NEncodable which could return the NEncoderClassInfo (and
	// have NEncodable::EncodableRegister use that method during initialisation).
	//
	// This would work for NEncodable objects, but a linear search would be required as a
	// fall-back to support classes-that-encode-other-classes (e.g., NPoint). 
	//
	// For now the list of encodable classes is small enough that a linear search is fine.
	for (theIter = theInfo.begin(); theIter != theInfo.end(); theIter++)
		{
		className = theIter->first;
		classInfo = theIter->second;
		
		if (classInfo.canEncode(theValue))
			{
			theNode = EncodeChild(theKey, "", kTokenObject);
			theNode->SetElementAttribute(kTokenClass, className);

			mNodeStack.push_back(theNode);
			classInfo.encodeObject(*this, theValue);
			mNodeStack.pop_back();
			}
		}
}





//============================================================================
//		NEncoder::DecodeBoolean : Decode a bool.
//----------------------------------------------------------------------------
bool NEncoder::DecodeBoolean(const NString &theKey) const
{	NString		valueText;
	bool		theValue;



	// Validate our state
	NN_ASSERT(mState == kNEncoderDecoding);
	
	
	
	// Decode the value
	valueText = DecodeChild(theKey, kTokenBoolean);
	theValue  = (valueText == kTokenTrue);
	
	return(theValue);
}





//============================================================================
//		NEncoder::DecodeNumber : Decode a number.
//----------------------------------------------------------------------------
NNumber NEncoder::DecodeNumber(const NString &theKey) const
{	NString		valueText;
	NNumber		theValue;



	// Validate our state
	NN_ASSERT(mState == kNEncoderDecoding);
	
	
	
	// Decode the value
	valueText = DecodeChild(theKey, kTokenNumber);
	theValue.SetValue(valueText);
	
	return(theValue);
}





//============================================================================
//		NEncoder::DecodeData : Decode data.
//----------------------------------------------------------------------------
NData NEncoder::DecodeData(const NString &theKey) const
{	NDataEncoder	theEncoder;
	NString			valueText;
	NData			theValue;



	// Validate our state
	NN_ASSERT(mState == kNEncoderDecoding);
	
	
	
	// Decode the value
	valueText = DecodeChild(theKey, kTokenData);
	theValue  = theEncoder.Decode(valueText, kNDataEncodingB64);
	
	return(theValue);
}





//============================================================================
//		NEncoder::DecodeString : Decode a string.
//----------------------------------------------------------------------------
NString NEncoder::DecodeString(const NString &theKey) const
{	NString		theValue;



	// Validate our state
	NN_ASSERT(mState == kNEncoderDecoding);
	
	
	
	// Decode the value
	theValue = DecodeChild(theKey, kTokenString);
	
	return(theValue);
}





//============================================================================
//		NEncoder::DecodeObject : Decode an object.
//----------------------------------------------------------------------------
NVariant NEncoder::DecodeObject(const NString &theKey) const
{	NString									className;
	const NXMLNode							*theNode;
	NVariant								theValue;
	NEncoderClassInfoMap					theInfo;
	NEncoderClassInfoMapConstIterator		theIter;



	// Validate our state
	NN_ASSERT(mState == kNEncoderDecoding);



	// Get the state we need
	theInfo = GetClassesInfo();
	theNode = GetChildNode(theKey);

	if (theNode == NULL)
		return(theValue);



	// Get the class
	className = theNode->GetElementAttribute(kTokenClass);
	theIter   = theInfo.find(className);

	if (theIter == theInfo.end())
		{
		NN_LOG("Unknown class (%@), skipping", className);
		return(theValue);
		}



	// Decode the value
	mNodeStack.push_back(const_cast<NXMLNode*>(theNode));
	theValue = theIter->second.decodeObject(*this);
	mNodeStack.pop_back();

	return(theValue);
}





//============================================================================
//		NEncoder::RegisterClass : Register a class.
//----------------------------------------------------------------------------
void NEncoder::RegisterClass(const NString &className, const NEncoderClassInfo &classInfo)
{	NEncoderClasses		*theClasses;



	// Validate our parameters
	NN_ASSERT(!IsKnownClass(className));
	NN_ASSERT(classInfo.canEncode    != NULL);
	NN_ASSERT(classInfo.encodeObject != NULL);
	NN_ASSERT(classInfo.decodeObject != NULL);
	
	
	
	// Register the class
	theClasses = GetClasses();
	theClasses->theLock.Lock();

		theClasses->theInfo[className] = classInfo;

	theClasses->theLock.Unlock();
}





#pragma mark private
//============================================================================
//		NEncoder::GetFormat : Get the format of encoded data.
//----------------------------------------------------------------------------
NEncoderFormat NEncoder::GetFormat(const NData &theData)
{	NEncoderFormat		theFormat;
	const uint8_t		*dataPtr;
	NIndex				dataSize;



	// Get the state we need
	theFormat = kNEncoderInvalid;
	dataSize  = theData.GetSize();
	dataPtr   = theData.GetData();



	// Identify the format
	#define MATCH_FORMAT(_magic, _format)													\
		do																					\
			{																				\
			if (theFormat == kNEncoderInvalid && dataSize >= NN_ARRAY_SIZE(_magic))			\
				{																			\
				if (memcmp(dataPtr, _magic, NN_ARRAY_SIZE(_magic)) == 0)					\
					theFormat = _format;													\
				}																			\
			}																				\
		while (0)

	MATCH_FORMAT(kMagicXML_1_0,    kNEncoderXML_1_0);
	MATCH_FORMAT(kMagicBinary_1_0, kNEncoderBinary_1_0);
	
	#undef MATCH_FORMAT
	
	return(theFormat);
}





//============================================================================
//		NEncoder::EncodeXML_1_0 : Encode to the XML 1.0 format.
//----------------------------------------------------------------------------
NData NEncoder::EncodeXML_1_0(NXMLNode *theRoot)
{	NXMLNode		*nodeDoc, *nodeEncoder;
	NXMLEncoder		xmlEncoder;
	NData			theData;
	NString			theXML;



	// Create the nodes
	nodeEncoder = new NXMLNode(kNXMLNodeElement, kTokenEncoder);
	nodeEncoder->SetElementAttribute(kTokenVersion, kVersion_1_0);
	nodeEncoder->AddChild(theRoot);

	nodeDoc = new NXMLNode(kNXMLNodeDocument, "");
	nodeDoc->AddChild(nodeEncoder);



	// Encode the XML
	theXML  = xmlEncoder.Encode(nodeDoc);
	theData = theXML.GetData();
	


	// Clean up
	nodeEncoder->RemoveChild(theRoot, false);
	delete nodeDoc;

	return(theData);
}





//============================================================================
//		NEncoder::DecodeXML_1_0 : Decode the XML 1.0 format.
//----------------------------------------------------------------------------
NXMLNode *NEncoder::DecodeXML_1_0(const NData &theData)
{	NXMLNode		*nodeDoc, *nodeEncoder, *nodeRoot;
	NXMLEncoder		xmlEncoder;
	NStatus			theErr;



	// Get the state we need
	theErr   = kNErrMalformed;
	nodeRoot = NULL;



	// Decode the XML
	nodeDoc = xmlEncoder.Decode(theData);
	if (nodeDoc == NULL || !nodeDoc->IsType(kNXMLNodeDocument) || nodeDoc->GetChildren()->size() != 1)
		goto cleanup;



	// Extract the encoder
	nodeEncoder = nodeDoc->GetChildren()->at(0);
	if (!nodeEncoder->IsElement(kTokenEncoder) || nodeEncoder->GetChildren()->size() != 1 || nodeEncoder->GetElementAttribute(kTokenVersion) != kVersion_1_0)
		goto cleanup;



	// Extract the root
	nodeRoot = nodeEncoder->GetChildren()->at(0);
	if (!nodeRoot->IsElement(kTokenObject) || nodeRoot->GetElementAttribute(kTokenKey) != kTokenRoot)
		goto cleanup;

	nodeEncoder->RemoveChild(nodeRoot, false);
	theErr = kNoErr;



	// Clean up
cleanup:
	if (theErr != kNoErr)
		NN_LOG("Unable to decode XML (%d)", theErr);

	delete nodeDoc;
	
	return(nodeRoot);
}





//============================================================================
//		NEncoder::EncodeBinary_1_0 : Encode to the binary 1.0 format.
//----------------------------------------------------------------------------
NData NEncoder::EncodeBinary_1_0(NXMLNode *theRoot)
{	NDataCompressor		theCompressor;
	NIndex				sizeMagic;
	NData				theData;



	// Encode the state
	theData = EncodeXML_1_0(theRoot);
	theData = theCompressor.Compress(theData, kNCompressionZLib);
	
	
	
	// Rewrite the header
	//
	// To save space our signature is overlaid on top of the existing NCompressionHeader.
	// If the compression header grows then a future version may need to leave that header
	// intact and prepend our own signature to the compressed block.
	sizeMagic = NN_ARRAY_SIZE(kMagicBinary_1_0);

	NN_ASSERT(sizeMagic                  == 8);
	NN_ASSERT(sizeof(NCompressionHeader) == 12);

	NN_ASSERT(offsetof(NCompressionHeader, compression) == 0);
	NN_ASSERT(offsetof(NCompressionHeader, reserved)    == 4);
	NN_ASSERT(offsetof(NCompressionHeader, origSize)    == 8);

	theData.ReplaceData(NRange(0, sizeMagic), sizeMagic, kMagicBinary_1_0);
	
	return(theData);
}





//============================================================================
//		NEncoder::DecodeBinary_1_0 : Decode the binary 1.0 format.
//----------------------------------------------------------------------------
NXMLNode *NEncoder::DecodeBinary_1_0(const NData &theData)
{	NDataCompressor			theCompressor;
	NCompressionHeader		theHeader;
	NXMLNode				*theRoot;
	NIndex					dataSize;
	const uint8_t			*dataPtr;
	NData					xmlData;



	// Get the state we need
	dataSize = theData.GetSize();
	dataPtr  = theData.GetData();

	NN_ASSERT(dataSize >= NN_ARRAY_SIZE(kMagicBinary_1_0));
	NN_ASSERT(memcmp(dataPtr, kMagicBinary_1_0, NN_ARRAY_SIZE(kMagicBinary_1_0)) == 0);



	// Prepare the header
	//
	// Due to our encoding approach, the NCompressionHeader is supplied externally.
	memcpy(&theHeader, dataPtr, sizeof(theHeader));
	
	theHeader.compression = kNCompressionZLib;
	theHeader.reserved    = 0;
	theHeader.origSize    = NSwapUInt32_BtoN(theHeader.origSize);

	dataSize -= sizeof(theHeader);
	dataPtr  += sizeof(theHeader);



	// Decode the state
	xmlData = theCompressor.Decompress(NData(dataSize, dataPtr, false), &theHeader);
	theRoot = DecodeXML_1_0(xmlData);
	
	return(theRoot);
}





//============================================================================
//		NEncoder::EncodeChild : Encode a child node.
//----------------------------------------------------------------------------
NXMLNode *NEncoder::EncodeChild(const NString &theKey, const NString &theValue, const NString &theType)
{	NXMLNode		*theParent, *theChild;



	// Validate our state
	NN_ASSERT(mState == kNEncoderEncoding);



	// Encode the child
	theChild = new NXMLNode(kNXMLNodeElement, theType);
	theChild->SetElementAttribute(kTokenKey, theKey);

	if (!theValue.IsEmpty())
		theChild->SetElementContents(theValue);



	// Attach to its parent
	theParent = const_cast<NXMLNode*>(GetParentNode());
	theParent->AddChild(theChild);

	return(theChild);
}





//============================================================================
//		NEncoder::DecodeChild : Decode a child node.
//----------------------------------------------------------------------------
NString NEncoder::DecodeChild(const NString &theKey, const NString &theType) const
{	const NXMLNode		*theNode;
	NString				theValue;



	// Validate our state
	NN_ASSERT(mState == kNEncoderDecoding);
	
	(void) theType;



	// Decode the child
	theNode = GetChildNode(theKey);
	if (theNode != NULL)
		{
		NN_ASSERT(theNode->GetTextValue() == theType);
		theValue = theNode->GetElementContents();
		}
	
	return(theValue);
}





//============================================================================
//		NEncoder::GetParentNode : Get the parent node.
//----------------------------------------------------------------------------
const NXMLNode *NEncoder::GetParentNode(void) const
{	NXMLNode	*theNode;



	// Validate our state
	NN_ASSERT(mNodeRoot != NULL);



	// Get the parent node
	if (mNodeStack.empty())
		theNode = mNodeRoot;
	else
		theNode = mNodeStack.back();
	
	return(theNode);
}





//============================================================================
//		NEncoder::GetChildNode : Get a child node.
//----------------------------------------------------------------------------
const NXMLNode *NEncoder::GetChildNode(const NString &theKey) const
{	const NXMLNodeList				*theChildren;
	NString							theAttribute;
	const NXMLNode					*theParent;
	NXMLNodeListConstIterator		theIter;



	// Validate our state
	NN_ASSERT(mState != kNEncoderIdle);



	// Get the state we need
	theParent   = GetParentNode();
	theChildren = theParent->GetChildren();



	// Find the child
	for (theIter = theChildren->begin(); theIter != theChildren->end(); theIter++)
		{
		theAttribute = (*theIter)->GetElementAttribute(kTokenKey);
		if (theAttribute == theKey)
			return(*theIter);
		}
	
	return(NULL);
}





//============================================================================
//		NEncoder::IsKnownClass : Is a class name known?
//----------------------------------------------------------------------------
bool NEncoder::IsKnownClass(const NString &className)
{	NEncoderClassInfoMap				theInfo;
	NEncoderClassInfoMapConstIterator	theIter;
	bool								isKnown;



	// Find the class
	theInfo = GetClassesInfo();
	theIter = theInfo.find(className);
	isKnown = (theIter != theInfo.end());
	
	return(isKnown);
}





//============================================================================
//		NEncoder::GetClasses : Get the classes.
//----------------------------------------------------------------------------
NEncoderClasses *NEncoder::GetClasses(void)
{	static NEncoderClasses		sClasses;



	// Get the classes
	//
	// We return a local static to ensure our state is constructed before
	// any other static initialisers.
	return(&sClasses);
}





//============================================================================
//		NEncoder::GetClassesInfo : Get the classes info.
//----------------------------------------------------------------------------
NEncoderClassInfoMap NEncoder::GetClassesInfo(void)
{	NEncoderClasses			*theClasses;
	NEncoderClassInfoMap	theInfo;



	// Get the state we need
	theClasses = GetClasses();
	theClasses->theLock.Lock();

		theInfo = theClasses->theInfo;

	theClasses->theLock.Unlock();
	
	return(theInfo);
}
