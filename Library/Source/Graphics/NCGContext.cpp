/*	NAME:
		NCGContext.cpp

	DESCRIPTION:
		CoreGraphics context object.
	
	COPYRIGHT:
		Copyright (c) 2006, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NCoreGraphicsUtilities.h"
#include "NMathUtilities.h"
#include "NCGContext.h"





//============================================================================
//		Public constants
//----------------------------------------------------------------------------
const HIThemeTextInfo kThemeTextSystemFontLeft = {		kHIThemeTextInfoVersionZero,
														kThemeStateActive,
														kThemeSystemFont,
														kHIThemeTextHorizontalFlushLeft,
														kHIThemeTextVerticalFlushCenter,
														kHIThemeTextBoxOptionNone,
														kHIThemeTextTruncationNone,
														0,
														false };

const HIThemeTextInfo kThemeTextSystemFontCenter = {	kHIThemeTextInfoVersionZero,
														kThemeStateActive,
														kThemeSystemFont,
														kHIThemeTextHorizontalFlushCenter,
														kHIThemeTextVerticalFlushCenter,
														kHIThemeTextBoxOptionNone,
														kHIThemeTextTruncationNone,
														0,
														false };

const HIThemeTextInfo kThemeTextSystemFontRight = {		kHIThemeTextInfoVersionZero,
														kThemeStateActive,
														kThemeSystemFont,
														kHIThemeTextHorizontalFlushRight,
														kHIThemeTextVerticalFlushCenter,
														kHIThemeTextBoxOptionNone,
														kHIThemeTextTruncationNone,
														0,
														false };

const HIThemeTextInfo kThemeTextAppFontLeft = {			kHIThemeTextInfoVersionZero,
														kThemeStateActive,
														kThemeMenuTitleFont, // dair kThemeApplicationFont,
														kHIThemeTextHorizontalFlushLeft,
														kHIThemeTextVerticalFlushCenter,
														kHIThemeTextBoxOptionNone,
														kHIThemeTextTruncationNone,
														0,
														false };

const HIThemeTextInfo kThemeTextAppFontCenter = {		kHIThemeTextInfoVersionZero,
														kThemeStateActive,
														kThemeApplicationFont,
														kHIThemeTextHorizontalFlushCenter,
														kHIThemeTextVerticalFlushCenter,
														kHIThemeTextBoxOptionNone,
														kHIThemeTextTruncationNone,
														0,
														false };

const HIThemeTextInfo kThemeTextAppFontRight = {		kHIThemeTextInfoVersionZero,
														kThemeStateActive,
														kThemeApplicationFont,
														kHIThemeTextHorizontalFlushRight,
														kHIThemeTextVerticalFlushCenter,
														kHIThemeTextBoxOptionNone,
														kHIThemeTextTruncationNone,
														0,
														false };





//============================================================================
//		NCGContext::NCGContext : Constructor.
//----------------------------------------------------------------------------
NCGContext::NCGContext(const NCarbonEvent &theEvent, bool saveState)
{


	// Initialize ourselves
	InitializeSelf(theEvent.GetCGContext(), saveState, kHIThemeOrientationNormal);
}





//============================================================================
//		NCGContext::NCGContext : Constructor.
//----------------------------------------------------------------------------
NCGContext::NCGContext(CGContextRef cgContext, bool saveState, HIThemeOrientation theOrientation)
{


	// Initialize ourselves
	InitializeSelf(cgContext, saveState, theOrientation);
}





//============================================================================
//		NCGContext::NCGContext : Constructor.
//----------------------------------------------------------------------------
NCGContext::NCGContext(NData &theData, UInt32 theWidth, UInt32 theHeight, CGBitmapInfo theInfo, UInt32 rowBytes)
{	CGContextRef	cgContext;



	// Supply our defaults
	//
	// If the data already has a size, we assume it is correct for the image.
	if (rowBytes == 0)
		rowBytes = theWidth * sizeof(UInt32);

	if (theData.GetSize() == 0)
		theData.SetSize(rowBytes * theHeight);



	// Create the context
	//
	// The image data is 32bpp.
	cgContext = CGBitmapContextCreate(theData.GetData(), theWidth, theHeight, 8, rowBytes,
									  NCoreGraphicsUtilities::GetColorSpaceRGB(), theInfo);



	// Initialize ourselves
	//
	// We do not release the object when we initialize ourselves, so
	// we need to release it here to avoid leaving an extra reference.
	InitializeSelf(cgContext, false, kHIThemeOrientationInverted);
	
	CFSafeRelease(cgContext);
}





//============================================================================
//		NCGContext::NCGContext : Constructor.
//----------------------------------------------------------------------------
NCGContext::NCGContext(void)
{


	// Initialize ourselves
	InitializeSelf(NULL, false, kHIThemeOrientationNormal);
}





//============================================================================
//		NCGContext::~NCGContext : Destructor.
//----------------------------------------------------------------------------
NCGContext::~NCGContext(void)
{


	// Clean up
	if (mRestoreState)
		RestoreState();
}





//============================================================================
//		NCGContext::GetOrientation : Get the orientation.
//----------------------------------------------------------------------------
HIThemeOrientation NCGContext::GetOrientation(void) const
{


	// Get the orientation
	return(mOrientation);
}





//============================================================================
//		NCGContext::IsInverted : Is the context inverted?
//----------------------------------------------------------------------------
bool NCGContext::IsInverted(void) const
{


	// Check the orientation
	return(mOrientation == kHIThemeOrientationInverted);
}





//============================================================================
//		NCGContext::SaveState : Save the graphics state.
//----------------------------------------------------------------------------
void NCGContext::SaveState(void)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Save the graphics state
	CGContextSaveGState(*this);
}





//============================================================================
//		NCGContext::RestoreState : Restore the graphics state.
//----------------------------------------------------------------------------
void NCGContext::RestoreState(void)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Restore the graphics state
	CGContextRestoreGState(*this);
}





//============================================================================
//		NCGContext::GetCTM : Get the transform.
//----------------------------------------------------------------------------
CGAffineTransform NCGContext::GetCTM(void)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Get the transform
	return(CGContextGetCTM(*this));
}





//============================================================================
//		NCGContext::ConcatCTM : Concatenate a transform.
//----------------------------------------------------------------------------
void NCGContext::ConcatCTM(const CGAffineTransform &theTransform)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Concatenate the transform
	CGContextConcatCTM(*this, theTransform);
}





//============================================================================
//		NCGContext::ScaleCTM : Scale the context transform.
//----------------------------------------------------------------------------
void NCGContext::ScaleCTM(float x, float y)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Scale the transform
	CGContextScaleCTM(*this, x, y);
}





//============================================================================
//		NCGContext::TranslateCTM : Translate the context transform.
//----------------------------------------------------------------------------
void NCGContext::TranslateCTM(float x, float y)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Translate the transform
	CGContextTranslateCTM(*this, x, y);
}





//============================================================================
//		NCGContext::RotateCTM : Rotate the context transform.
//----------------------------------------------------------------------------
void NCGContext::RotateCTM(float theAngle)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Rotate the transform
	CGContextRotateCTM(*this, theAngle);
}





//============================================================================
//		NCGContext::ClipToRect : Clip to a rectangle.
//----------------------------------------------------------------------------
void NCGContext::ClipToRect(const CGRect &theRect)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Clip to the rectangle
	CGContextClipToRect(*this, theRect);
}





//============================================================================
//		NCGContext::SetLineWidth : Set the line width.
//----------------------------------------------------------------------------
void NCGContext::SetLineWidth(float theWidth)
{


	// Validate our parameters and state
	NN_ASSERT(theWidth > 0.0f);
	NN_ASSERT(IsValid());



	// Set the line width
	CGContextSetLineWidth(*this, theWidth);
}





//============================================================================
//		NCGContext::SetLineDash : Set the line dash.
//----------------------------------------------------------------------------
void NCGContext::SetLineDash(float thePhase, const float *dashLengths, UInt32 numLengths)
{


	// Validate our parameters and state
	NN_ASSERT(thePhase >= 0.0f);
	NN_ASSERT((dashLengths != NULL && numLengths != 0) || (dashLengths == NULL && numLengths == 0));
	NN_ASSERT(IsValid());



	// Set the line dash
	CGContextSetLineDash(*this, thePhase, dashLengths, numLengths);
}





//============================================================================
//		NCGContext::SetAlpha : Set the alpha.
//----------------------------------------------------------------------------
void NCGContext::SetAlpha(float theAlpha)
{


	// Validate our parameters and state
	NN_ASSERT(theAlpha > 0.0f);
	NN_ASSERT(IsValid());



	// Set the alpha
	CGContextSetAlpha(*this, theAlpha);
}





//============================================================================
//		NCGContext::SetStrokeColor : Set the stroke color.
//----------------------------------------------------------------------------
void NCGContext::SetStrokeColor(const NColor &theColor)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Set the stroke color
	CGContextSetRGBStrokeColor(*this, theColor.GetRed(), theColor.GetGreen(), theColor.GetBlue(), theColor.GetAlpha());
}





//============================================================================
//		NCGContext::SetStrokeColor : Set the stroke color.
//----------------------------------------------------------------------------
void NCGContext::SetStrokeColor(ThemeBrush theBrush)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Set the stroke color
	HIThemeSetStroke(theBrush, NULL, *this, mOrientation);
}





//============================================================================
//		NCGContext::SetFillColor : Set the fill color.
//----------------------------------------------------------------------------
void NCGContext::SetFillColor(const NColor &theColor)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Set the fill color
	CGContextSetRGBFillColor(*this, theColor.GetRed(), theColor.GetGreen(), theColor.GetBlue(), theColor.GetAlpha());
}





//============================================================================
//		NCGContext::SetFillColor : Set the fill color.
//----------------------------------------------------------------------------
void NCGContext::SetFillColor(ThemeBrush theBrush)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Set the fill color
	HIThemeSetFill(theBrush, NULL, *this, mOrientation);
}





//============================================================================
//		NCGContext::SetShadowColor : Set the shadow color.
//----------------------------------------------------------------------------
void NCGContext::SetShadowColor(const NColor &theColor, float theBlur, const CGSize &theOffset)
{	NCFObject	cfColor;



	// Set the shadow color
	if (cfColor.Set(theColor.CopyColor()))
		CGContextSetShadowWithColor(*this, theOffset, theBlur, cfColor);
}





//============================================================================
//		NCGContext::MoveToPoint : Move to a point.
//----------------------------------------------------------------------------
void NCGContext::MoveToPoint(float x, float y)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Move to the point
	CGContextMoveToPoint(*this, x, y);
}





//============================================================================
//		NCGContext::MoveToPoint : Move to a point.
//----------------------------------------------------------------------------
void NCGContext::MoveToPoint(const CGPoint &thePoint)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Move to the point
	CGContextMoveToPoint(*this, thePoint.x, thePoint.y);
}





//============================================================================
//		NCGContext::AddLineToPoint : Add a line to a point.
//----------------------------------------------------------------------------
void NCGContext::AddLineToPoint(float x, float y)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Add a line to the point
	CGContextAddLineToPoint(*this, x, y);
}





//============================================================================
//		NCGContext::AddLineToPoint : Add a line to a point.
//----------------------------------------------------------------------------
void NCGContext::AddLineToPoint(const CGPoint &thePoint)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Add a line to the point
	CGContextAddLineToPoint(*this, thePoint.x, thePoint.y);
}





//============================================================================
//		NCGContext::AddRect : Add a rectangle.
//----------------------------------------------------------------------------
void NCGContext::AddRect(const CGRect &theRect, float cornerRadius)
{	CGPoint		topMiddle, topLeft, topRight, bottomRight;



	// Validate our parameters and state
	NN_ASSERT(cornerRadius >= 0.0f);
	NN_ASSERT(IsValid());



	// Add a rectangle
	if (NMathUtilities::IsZero(cornerRadius))
		CGContextAddRect(*this, theRect);
	
	
	// Add a round-rect
	else
		{
		// Get the state we need
		topMiddle.x = theRect.origin.x;
		topMiddle.y = theRect.origin.y + (theRect.size.height / 2.0f);
		
		topLeft.x = theRect.origin.x;
		topLeft.y = theRect.origin.y + theRect.size.height;
		
		topRight.x = theRect.origin.x + theRect.size.width;
		topRight.y = theRect.origin.y + theRect.size.height;
		
		bottomRight.x = theRect.origin.x + theRect.size.width;
		bottomRight.y = theRect.origin.y;


		// Add the rect
		CGContextMoveToPoint(   *this,      topMiddle.x,      topMiddle.y);
		CGContextAddArcToPoint( *this, theRect.origin.x, theRect.origin.y,    bottomRight.x,    bottomRight.y, cornerRadius);
		CGContextAddArcToPoint( *this,    bottomRight.x,    bottomRight.y,       topRight.x,       topRight.y, cornerRadius);
		CGContextAddArcToPoint( *this,       topRight.x,       topRight.y,        topLeft.x,        topLeft.y, cornerRadius);
		CGContextAddArcToPoint( *this,        topLeft.x,        topLeft.y, theRect.origin.x, theRect.origin.y, cornerRadius);
		CGContextAddLineToPoint(*this,      topMiddle.x,      topMiddle.y);
		}
}





//============================================================================
//		NCGContext::AddCircle : Add a circle.
//----------------------------------------------------------------------------
void NCGContext::AddCircle(const CGPoint &theCentre, float theRadius)
{


	// Validate our parameters and state
	NN_ASSERT(theRadius > 0.0f);
	NN_ASSERT(IsValid());



	// Add a circle
	CGContextMoveToPoint(*this, theCentre.x + theRadius, theCentre.y);
	CGContextAddArc(     *this, theCentre.x,             theCentre.y, theRadius, 0.0f, M_PI * 2.0f, true);
}





//============================================================================
//		NCGContext::AddEllipse : Add an ellipse.
//----------------------------------------------------------------------------
void NCGContext::AddEllipse(const CGRect &theRect)
{	float	halfWidth, halfHeight;



	// Validate our state
	NN_ASSERT(IsValid());



	// Get the state we need
	halfWidth  = theRect.size.width  / 2.0f;
	halfHeight = theRect.size.height / 2.0f;


	// Add the ellipse
	CGContextSaveGState(   *this);
	CGContextTranslateCTM( *this, theRect.origin.x + halfWidth, theRect.origin.y + halfHeight);
	CGContextScaleCTM(     *this, halfWidth, halfHeight);
	CGContextAddArc(       *this, 0.0f, 0.0f, 1.0f, 0.0f, M_PI * 2.0f, true);
	CGContextRestoreGState(*this);
}





//============================================================================
//		NCGContext::AddPolygon : Add a polygon.
//----------------------------------------------------------------------------
void NCGContext::AddPolygon(const CGPointList &thePoints)
{


	// Validate our parameters and state
	NN_ASSERT(thePoints.size() >= 3);
	NN_ASSERT(IsValid());



	// Add the polygon
	CGContextAddLines(      *this, &thePoints[0],   thePoints.size());
	CGContextAddLineToPoint(*this,  thePoints[0].x, thePoints[0].y);
}





//============================================================================
//		NCGContext::ClearRect : Clear a rectangle.
//----------------------------------------------------------------------------
void NCGContext::ClearRect(const CGRect &theRect)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Clear the rectangle
	CGContextClearRect(*this, theRect);
}





//============================================================================
//		NCGContext::FillRect : Fill a rectangle.
//----------------------------------------------------------------------------
void NCGContext::FillRect(const CGRect &theRect)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Fill the rectangle
	CGContextFillRect(*this, theRect);
}





//============================================================================
//		NCGContext::StrokeRect : Stroke a rectangle.
//----------------------------------------------------------------------------
void NCGContext::StrokeRect(const CGRect &theRect)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Stroke the rectangle
	CGContextStrokeRect(*this, theRect);
}





//============================================================================
//		NCGContext::DrawPath : Draw the current path.
//----------------------------------------------------------------------------
void NCGContext::DrawPath(CGPathDrawingMode theMode)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Draw the path
	CGContextDrawPath(*this, theMode);
}





//============================================================================
//		NCGContext::FillPath : Fill the current path.
//----------------------------------------------------------------------------
void NCGContext::FillPath(void)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Fill the path
	CGContextFillPath(*this);
}





//============================================================================
//		NCGContext::StrokePath : Stroke the current path.
//----------------------------------------------------------------------------
void NCGContext::StrokePath(void)
{


	// Validate our state
	NN_ASSERT(IsValid());



	// Stroke the path
	CGContextStrokePath(*this);
}





//============================================================================
//		NCGContext::DrawImage : Draw an image.
//----------------------------------------------------------------------------
void NCGContext::DrawImage(const NCGImage &theImage, const CGRect &theRect)
{	OSStatus	theErr;



	// Validate our parameters and state
	NN_ASSERT(theImage.IsValid());
	NN_ASSERT(IsValid());



	// Draw the image
	if (IsInverted())
		CGContextDrawImage(*this, theRect, theImage);
	else
		{
		theErr = HIViewDrawCGImage(*this, &theRect, theImage);
		NN_ASSERT_NOERR(theErr);
		}
}





//============================================================================
//		NCGContext::DrawShading : Draw a shading.
//----------------------------------------------------------------------------
void NCGContext::DrawShading(const NCGShading &theShading)
{


	// Validate our parameters and state
	NN_ASSERT(IsValid());



	// Draw the shading
	if (theShading.GetMode() != kShadingNone)
		CGContextDrawShading(*this, theShading);
}





//============================================================================
//		NCGContext::DrawDragHilite : Draw a drag hilite.
//----------------------------------------------------------------------------
void NCGContext::DrawDragHilite(const CGRect &theRect)
{


	// Draw the drag hilite
	SetLineWidth(1.0f);
	SetStrokeColor(kThemeBrushDragHilite);

	StrokeRect(CGRectInset(theRect, 1.0f, 1.0f));
}





//============================================================================
//		NCGContext::DrawThemeText : Draw a theme text box.
//----------------------------------------------------------------------------
bool NCGContext::DrawThemeText(const NString			&theString,
								const HIRect			&theRect,
								const HIThemeTextInfo	&theInfo)
{	HIThemeTextInfo		mutableInfo;
	OSStatus			theErr;



	// Get the state we need
	mutableInfo = theInfo;



	// Draw the text box
	theErr = HIThemeDrawTextBox(theString, &theRect, &mutableInfo, *this, mOrientation);
	NN_ASSERT_NOERR(theErr);
	
	return(mutableInfo.truncationHappened);
}





//============================================================================
//		NCGContext::InitializeSelf : Initialize ourselves.
//----------------------------------------------------------------------------
#pragma mark -
void NCGContext::InitializeSelf(CGContextRef cgContext, bool saveState, HIThemeOrientation theOrientation)
{


	// Initialize ourselves
	mOrientation  = theOrientation;
	mRestoreState = false;

	Set(cgContext, false);



	// Save the state if necessary
	if (saveState)
		{
		mRestoreState = true;
		SaveState();
		}
}


