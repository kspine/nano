/*	NAME:
		NQt.h

	DESCRIPTION:
		Qt support.
	
	COPYRIGHT:
		Copyright (c) 2006-2009, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
#ifndef NQT_HDR
#define NQT_HDR
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NRectangle.h"
#include "NPoint.h"
#include "NSize.h"
#include "NBundle.h"
#include "NString.h"





//============================================================================
//		Inline functions
//----------------------------------------------------------------------------
// Nano to Qt
inline QSizeF ToNS(const NSize &theSize)
{
	return(QSizeF(theSize.width, theSize.height));
}

inline QPointF ToNS(const NPoint &thePoint)
{
	return(QPointF(thePoint.x, thePoint.y));
}

inline QRectF ToNS(const NRectangle &theRect)
{
	return(QRectF(theRect.origin.x, theRect.origin.y, theRect.size.width, theRect.size.height));
}

inline QString ToQt(const NString &theString)
{
	return(QString::fromUtf8(theString.GetUTF8()));
}



// Qt to Nano
inline NPoint ToNN(const QPoint &thePoint)
{
	return(NPoint(thePoint.x(), thePoint.y()));
}

inline NSize ToNN(const QSize &theSize)
{
	return(NSize(theSize.width(), theSize.height()));
}

inline NRectangle ToNN(const QRect &theRect)
{
	return(NRectangle(theRect.x(), theRect.y(), theRect.width(), theRect.height()));
}

inline NPoint ToNN(const QPointF &thePoint)
{
	return(NPoint(thePoint.x(), thePoint.y()));
}

inline NSize ToNN(const QSizeF &theSize)
{
	return(NSize(theSize.width(), theSize.height()));
}

inline NRectangle ToNN(const QRectF &theRect)
{
	return(NRectangle(theRect.x(), theRect.y(), theRect.width(), theRect.height()));
}

inline NString ToNN(const QString &theString)
{
	return(NString(theString.toUtf8().data()));
}



// Bundles
inline QString NQtBundleResource(const NString &theName, const NString &theType="", const NString &subDir="")
{	QString		thePath;
	NFile		theFile;

	theFile = NBundleResource(theName, theType, subDir);
	if (theFile.Exists())
		thePath = ToQt(theFile.GetPath());
	
	return(thePath);
}

inline QString NQtBundleString(const NString &theKey, const NString &defaultValue="", const NString &tableName="")
{
	return(ToQt(NBundleString(theKey, defaultValue, tableName)));
}

inline QPixmap NQtBundlePixmap(const NString &theKey, const NString &defaultValue="", const NString &tableName="")
{
	return(QPixmap(NQtBundleResource(theKey, defaultValue, tableName)));
}

inline QIcon NQtBundleIcon(const NString &theKey, const NString &defaultValue="", const NString &tableName="")
{
	return(QIcon(NQtBundleResource(theKey, defaultValue, tableName)));
}





#endif // NQT_HDR





