/*	NAME:
		NTargetFile.h

	DESCRIPTION:
		File support.
	
	COPYRIGHT:
		Copyright (c) 2006-2013, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
#ifndef NTARGET_FILE_HDR
#define NTARGET_FILE_HDR
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NFileUtilities.h"
#include "NDictionary.h"
#include "NFileMap.h"
#include "NFile.h"





//============================================================================
//		Class declaration
//----------------------------------------------------------------------------
class NTargetFile {
public:
	// Is this a file?
	static bool							IsFile(const NString &thePath);


	// Is this a directory?
	static bool							IsDirectory(const NString &thePath);


	// Is this a link?
	static bool							IsLink(const NString &thePath);


	// Is a file writeable?
	static bool							IsWriteable(const NString &thePath);


	// Does a file exist?
	static bool							Exists(const NString &thePath);


	// Get/set a file's name
	static NString						GetName(const NString &thePath, bool displayName);
	static NStatus						SetName(const NString &thePath, const NString &fileName, bool renameFile, bool isPath, NString &newPath);


	// Get/set a file's size
	static uint64_t						GetSize(const NString &thePath);
	static NStatus						SetSize(const NString &thePath, NFileRef theFile, uint64_t theSize);


	// Get a file's timestamps
	static NDate						GetCreationTime(    const NString &thePath);
	static NDate						GetAccessTime(      const NString &thePath);
	static NDate						GetModificationTime(const NString &thePath);


	// Get the child/parent of a path
	static NString						GetChild( const NString &thePath, const NString &fileName);
	static NString						GetParent(const NString &thePath);


	// Get the target of a path
	static NString						GetTarget(const NString &thePath);


	// Get the children of a path
	static NFileList					GetChildren(const NString &thePath);
	

	// Delete the file
	static NStatus						Delete(const NString &thePath, bool moveToTrash);


	// Get a directory
	static NFile						GetDirectory(NDirectoryDomain theDomain, NDirectoryLocation theLocation);


	// Create a directory
	static NStatus						CreateDirectory(const NString &thePath);


	// Create a link
	static NStatus						CreateLink(const NString &thePath, const NString &targetPath, NFileLink theType);


	// Exchange two files
	static NStatus						ExchangeWith(const NString &srcPath, const NString &dstPath);


	// Unmount a volume
	static NStatus						UnmountVolume(const NString &thePath);


	// File I/O
	static NFileRef						FileOpen(const NString &thePath, NFilePermission thePermission);
	static void							FileClose(      NFileRef theFile);
	static uint64_t						FileGetPosition(NFileRef theFile);
	static NStatus						FileSetPosition(NFileRef theFile, int64_t theOffset, NFilePosition thePosition);
	static NStatus						FileRead(       NFileRef theFile, uint64_t theSize,       void *thePtr, uint64_t &numRead,    int64_t theOffset, NFilePosition thePosition, NFileFlags theFlags);
	static NStatus						FileWrite(      NFileRef theFile, uint64_t theSize, const void *thePtr, uint64_t &numWritten, int64_t theOffset, NFilePosition thePosition, NFileFlags theFlags);


	// File mapping
	static NFileRef						MapOpen(const NFile &theFile, NMapAccess theAccess);
	static void							MapClose(  NFileRef theFile);
	static void						   *MapFetch(  NFileRef theFile,  NMapAccess theAccess, uint64_t    theOffset, uint32_t theSize, bool noCache);
	static void							MapDiscard(NFileRef theFile,  NMapAccess theAccess, const void *thePtr,    uint32_t theSize);


	// Bundles
	static NDictionary					BundleGetInfo(      const NFile &theBundle);
	static NFile						BundleGetExecutable(const NFile &theBundle, const NString &theName);
	static NFile						BundleGetResources( const NFile &theBundle);
};





#endif // NTARGET_FILE_HDR


