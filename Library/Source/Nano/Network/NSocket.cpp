/*	NAME:
		NSocket.cpp

	DESCRIPTION:
		Network socket.
	
	COPYRIGHT:
		Copyright (c) 2006-2013, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "NThreadUtilities.h"
#include "NSTLUtilities.h"
#include "NTargetNetwork.h"
#include "NThread.h"
#include "NSocket.h"





//============================================================================
//      Internal constants
//----------------------------------------------------------------------------
static const NIndex kReadBufferSize									= 8 * kNKilobyte;





//============================================================================
//		NSocket::NSocket : Constructor.
//----------------------------------------------------------------------------
NSocket::NSocket(NSocketRef theSocket, NSocket *parentSocket)
{


	// Initialise ourselves
	InitialiseSelf(parentSocket->GetDelegate());
	
	mStatus = kNSocketOpening;
	mSocket = theSocket;
}





//============================================================================
//		NSocket::NSocket : Constructor.
//----------------------------------------------------------------------------
NSocket::NSocket(NSocketDelegate *theDelegate)
{


	// Initialise ourselves
	InitialiseSelf(theDelegate);
}





//============================================================================
//		NSocket::~NSocket : Destructor.
//----------------------------------------------------------------------------
NSocket::~NSocket(void)
{


	// Validate our state
	NN_ASSERT(mStatus == kNSocketClosed);
}





//============================================================================
//		NSocket::GetStatus : Get the status.
//----------------------------------------------------------------------------
NSocketStatus NSocket::GetStatus(void) const
{	StLock	acquireLock(mLock);



	// Get the status
	return(mStatus);
}





//============================================================================
//		NSocket::GetResult : Get the result.
//----------------------------------------------------------------------------
NStatus NSocket::GetResult(void) const
{	StLock	acquireLock(mLock);



	// Get the result
	return(mResult);
}





//============================================================================
//		NSocket::GetPendingWrites : Get the amount of pending write data.
//----------------------------------------------------------------------------
NIndex NSocket::GetPendingWrites(void) const
{


	// Get the state
	return(mWritesPending);
}





//============================================================================
//		NSocket::Open : Open the socket.
//----------------------------------------------------------------------------
void NSocket::Open(uint16_t thePort)
{


	// Open the socket
	Open("", thePort);
}





//============================================================================
//		NSocket::Open : Open the socket.
//----------------------------------------------------------------------------
void NSocket::Open(const NString &theHost, uint16_t thePort)
{	StLock		acquireLock(mLock);



	// Validate our state
	NN_ASSERT(mStatus == kNSocketClosed);
	
	
	
	// Open the socket
	mStatus = kNSocketOpening;
	mSocket = NTargetNetwork::SocketOpen(this, theHost, thePort);



	// Handle failure
	if (mSocket == NULL)
		SocketDidClose(kNErrInternal);
}





//============================================================================
//		NSocket::Close : Close the socket.
//----------------------------------------------------------------------------
void NSocket::Close(NStatus theErr)
{


	// Update our state
	//
	// SocketDidClose() will destroy the underlying target socket, which will
	// cause the target layer to stop dispatching events into this object.
	//
	// However since dispatching an event into this object must also acquire
	// our lock, this can lead to a deadlock if an event is dispatched between
	// Close() acquiring the lock and SocketDidClose() destroying the socket.
	//
	// If an event arrives on the target layer at that point, the target layer
	// will be blocked in SocketEvent() until Close() releases the lock.
	//
	// Since closing a socket may be a precursor to deleting the NSocket, the
	// target layer must both destroy the native socket and ensure its internal
	// thread is outside SocketEvent() before it can return.
	//
	//
	// To avoid this we maintain a (non-atomic) flag which is tested by the
	// target thread before it acquires our lock.
	//
	// Setting this flag ensures that no further attempts are made to acquire
	// our lock, making any further calls to SocketEvent() no-ops.
	//
	// The target layer can then turn off event dispatch for the target socket,
	// wait for its thread to exit SocketEvent(), then destroy the socket.
	mSocketClosing = true;



	// Close the socket
	//
	// The flag must be reset by us, not SocketDidClose(), since the native socket
	// may have dispatched a close event before we even set the flag (thus making
	// our call  a no-op).
	mLock.Lock();
		// Close the socket
		SocketDidClose(theErr);


		// Reset our state
		NN_ASSERT(mStatus == kNSocketClosed);
		mSocketClosing = false;
	mLock.Unlock();
}





//============================================================================
//		NSocket::Read : Submit a read request.
//----------------------------------------------------------------------------
void NSocket::Read(NIndex theSize)
{	NSocketRequest		*theRequest;



	// Validate our parameters and state
	NN_ASSERT(theSize   >= 1);
	NN_ASSERT(mDelegate != NULL);



	// Check our state
	if (GetStatus() != kNSocketOpened)
		{
		mDelegate->SocketFinishedRead(this, NData(), kNErrPermission);
		return;
		}



	// Submit the request
	theRequest = new NSocketRequest(NData(theSize));

	AddReadRequest(theRequest);



	// Clean up
	theRequest->Release();
}





//============================================================================
//		NSocket::Write : Submit a write request.
//----------------------------------------------------------------------------
void NSocket::Write(NIndex theSize, const void *thePtr)
{	NSocketRequest		*theRequest;



	// Validate our parameters and state
	NN_ASSERT(theSize   >= 1);
	NN_ASSERT(thePtr    != NULL);
	NN_ASSERT(mDelegate != NULL);



	// Check our state
	if (GetStatus() != kNSocketOpened)
		{
		mDelegate->SocketFinishedWrite(this, NData(), kNErrPermission);
		return;
		}



	// Submit the request
	theRequest = new NSocketRequest(NData(theSize, thePtr));

	AddWriteRequest(theRequest);



	// Clean up
	theRequest->Release();
}





//============================================================================
//		NSocket::ReadUInt8 : Read a uint8_t.
//----------------------------------------------------------------------------
NStatus NSocket::ReadUInt8(uint8_t &theValue)
{	NStatus		theErr;



	// Read the value
	theErr = ReadData(sizeof(theValue), &theValue);

	return(theErr);
}





//============================================================================
//		NSocket::ReadUInt16 : Read a uint16_t.
//----------------------------------------------------------------------------
NStatus NSocket::ReadUInt16(uint16_t &theValue, NEndianFormat wireFormat)
{	NStatus		theErr;



	// Read the value
	theErr = ReadData(sizeof(theValue), &theValue);
	if (theErr == kNoErr && wireFormat != kNEndianNative)
		NSwapUInt16(&theValue);
	
	return(theErr);
}





//============================================================================
//		NSocket::ReadUInt32 : Read a uint32_t.
//----------------------------------------------------------------------------
NStatus NSocket::ReadUInt32(uint32_t &theValue, NEndianFormat wireFormat)
{	NStatus		theErr;



	// Read the value
	theErr = ReadData(sizeof(theValue), &theValue);
	if (theErr == kNoErr && wireFormat != kNEndianNative)
		NSwapUInt32(&theValue);
	
	return(theErr);
}





//============================================================================
//		NSocket::ReadUInt64 : Read a uint64_t.
//----------------------------------------------------------------------------
NStatus NSocket::ReadUInt64(uint64_t &theValue, NEndianFormat wireFormat)
{	NStatus		theErr;



	// Read the value
	theErr = ReadData(sizeof(theValue), &theValue);
	if (theErr == kNoErr && wireFormat != kNEndianNative)
		NSwapUInt64(&theValue);
	
	return(theErr);
}





//============================================================================
//		NSocket::ReadInt8 : Read an int8_t.
//----------------------------------------------------------------------------
NStatus NSocket::ReadInt8(int8_t &theValue)
{	NStatus		theErr;



	// Write the value
	theErr = ReadData(sizeof(theValue), &theValue);

	return(theErr);
}





//============================================================================
//		NSocket::ReadInt16 : Read an int16_t.
//----------------------------------------------------------------------------
NStatus NSocket::ReadInt16(int16_t &theValue, NEndianFormat wireFormat)
{	NStatus		theErr;



	// Read the value
	theErr = ReadData(sizeof(theValue), &theValue);
	if (theErr == kNoErr && wireFormat != kNEndianNative)
		NSwapInt16(&theValue);
	
	return(theErr);
}





//============================================================================
//		NSocket::ReadInt32 : Read an int32_t.
//----------------------------------------------------------------------------
NStatus NSocket::ReadInt32(int32_t &theValue, NEndianFormat wireFormat)
{	NStatus		theErr;



	// Read the value
	theErr = ReadData(sizeof(theValue), &theValue);
	if (theErr == kNoErr && wireFormat != kNEndianNative)
		NSwapInt32(&theValue);
	
	return(theErr);
}





//============================================================================
//		NSocket::ReadInt64 : Read an int64_t.
//----------------------------------------------------------------------------
NStatus NSocket::ReadInt64(int64_t &theValue, NEndianFormat wireFormat)
{	NStatus		theErr;



	// Read the value
	theErr = ReadData(sizeof(theValue), &theValue);
	if (theErr == kNoErr && wireFormat != kNEndianNative)
		NSwapInt64(&theValue);
	
	return(theErr);
}





//============================================================================
//		NSocket::ReadData : Read data.
//----------------------------------------------------------------------------
NStatus NSocket::ReadData(NIndex theSize, void *thePtr)
{	NData		theData;
	NStatus		theErr;



	// Read the data
	theErr = ReadData(theSize, theData);

	if (theErr == kNoErr)
		{
		NN_ASSERT(theData.GetSize() == theSize);
		memcpy(thePtr, theData.GetData(), theSize);
		}
	
	return(theErr);
}





//============================================================================
//		NSocket::ReadData : Read data.
//----------------------------------------------------------------------------
NStatus NSocket::ReadData(NIndex theSize, NData &theData)
{	NSocketRequest		*theRequest;
	NStatus				theErr;



	// Validate our parameters
	NN_ASSERT(theSize >= 1);



	// Check our state
	if (NN_DEBUG && NThread::IsMain())
		NN_LOG("NSocket performing blocking read of %ld bytes on main thread!", theSize);

	if (GetStatus() != kNSocketOpened)
		return(kNErrPermission);



	// Submit the request
	theRequest = new NSocketRequest(NData(theSize));
	theRequest->SetSemaphore(&mReadSemaphore);
	
	AddReadRequest(theRequest);



	// Wait for it to finish
	mReadSemaphore.Wait();
	
	theErr = theRequest->GetStatus();
	
	if (theErr == kNoErr)
		{
		theData = theRequest->GetData();
		NN_ASSERT(theData.GetSize() == theSize);
		}



	// Clean up
	theRequest->Release();

	return(theErr);
}





//============================================================================
//		NSocket::WriteUInt8 : Write a uint8_t.
//----------------------------------------------------------------------------
NStatus NSocket::WriteUInt8(uint8_t theValue)
{


	// Write the value
	return(WriteData(sizeof(theValue), &theValue));
}





//============================================================================
//		NSocket::WriteUInt16 : Write a uint16_t.
//----------------------------------------------------------------------------
NStatus NSocket::WriteUInt16(uint16_t theValue, NEndianFormat wireFormat)
{


	// Write the value
	if (wireFormat != kNEndianNative)
		NSwapUInt16(&theValue);

	return(WriteData(sizeof(theValue), &theValue));
}





//============================================================================
//		NSocket::WriteUInt32 : Write a uint32_t.
//----------------------------------------------------------------------------
NStatus NSocket::WriteUInt32(uint32_t theValue, NEndianFormat wireFormat)
{


	// Write the value
	if (wireFormat != kNEndianNative)
		NSwapUInt32(&theValue);

	return(WriteData(sizeof(theValue), &theValue));
}





//============================================================================
//		NSocket::WriteUInt64 : Write a uint64_t.
//----------------------------------------------------------------------------
NStatus NSocket::WriteUInt64(uint64_t theValue, NEndianFormat wireFormat)
{


	// Write the value
	if (wireFormat != kNEndianNative)
		NSwapUInt64(&theValue);

	return(WriteData(sizeof(theValue), &theValue));
}





//============================================================================
//		NSocket::WriteInt8 : Write an int8_t.
//----------------------------------------------------------------------------
NStatus NSocket::WriteInt8(int8_t theValue)
{


	// Write the value
	return(WriteData(sizeof(theValue), &theValue));
}





//============================================================================
//		NSocket::WriteInt16 : Write an int16_t.
//----------------------------------------------------------------------------
NStatus NSocket::WriteInt16(int16_t theValue, NEndianFormat wireFormat)
{


	// Write the value
	if (wireFormat != kNEndianNative)
		NSwapInt16(&theValue);

	return(WriteData(sizeof(theValue), &theValue));
}





//============================================================================
//		NSocket::WriteInt32 : Write an int32_t.
//----------------------------------------------------------------------------
NStatus NSocket::WriteInt32(int32_t theValue, NEndianFormat wireFormat)
{


	// Write the value
	if (wireFormat != kNEndianNative)
		NSwapInt32(&theValue);

	return(WriteData(sizeof(theValue), &theValue));
}





//============================================================================
//		NSocket::WriteInt64 : Write an int64_t.
//----------------------------------------------------------------------------
NStatus NSocket::WriteInt64(int64_t theValue, NEndianFormat wireFormat)
{


	// Write the value
	if (wireFormat != kNEndianNative)
		NSwapInt64(&theValue);

	return(WriteData(sizeof(theValue), &theValue));
}





//============================================================================
//		NSocket::WriteData : Write data.
//----------------------------------------------------------------------------
NStatus NSocket::WriteData(NIndex theSize, const void *thePtr)
{


	// Write the data
	return(WriteData(NData(theSize, thePtr)));
}





//============================================================================
//		NSocket::WriteData : Write data.
//----------------------------------------------------------------------------
NStatus NSocket::WriteData(const NData &theData)
{	NSocketRequest		*theRequest;
	NStatus				theErr;



	// Validate our parameters
	NN_ASSERT(!theData.IsEmpty());



	// Check our state
	if (NN_DEBUG && NThread::IsMain())
		NN_LOG("NSocket performing blocking write of %ld bytes on main thread!", theData.GetSize());

	if (GetStatus() != kNSocketOpened)
		return(kNErrPermission);



	// Submit the request
	theRequest = new NSocketRequest(theData);
	theRequest->SetSemaphore(&mWriteSemaphore);
	
	AddWriteRequest(theRequest);



	// Wait for it to finish
	mWriteSemaphore.Wait();
	
	theErr = theRequest->GetStatus();



	// Clean up
	theRequest->Release();

	return(theErr);
}





//============================================================================
//		NSocket::GetDelegate : Get the delegate.
//----------------------------------------------------------------------------
NSocketDelegate *NSocket::GetDelegate(void) const
{


	// Get the delegate
	return(mDelegate);
}





//============================================================================
//		NSocket::SetDelegate : Set the delegate.
//----------------------------------------------------------------------------
void NSocket::SetDelegate(NSocketDelegate *theDelegate)
{


	// Set the delegate
	mDelegate = theDelegate;
}





//============================================================================
///		NSocket::GetOption : Get an option.
//----------------------------------------------------------------------------
int32_t NSocket::GetOption(NSocketOption theOption) const
{


	// Validate our state
	NN_ASSERT(GetStatus() == kNSocketOpened);
	
	
	
	// Get the option
	return(NTargetNetwork::SocketGetOption(mSocket, theOption));
}





//============================================================================
///		NSocket::GetOption : Get an option.
//----------------------------------------------------------------------------
NStatus NSocket::SetOption(NSocketOption theOption, int32_t theValue)
{


	// Validate our state
	NN_ASSERT(GetStatus() == kNSocketOpened);
	
	
	
	// Get the option
	return(NTargetNetwork::SocketSetOption(mSocket, theOption, theValue));
}





#pragma mark public (private)
//============================================================================
//		NSocket::SocketEvent : Handle events.
//----------------------------------------------------------------------------
void NSocket::SocketEvent(NSocketEvent theEvent, uintptr_t theValue)
{


	// Check our state
	//
	// See NSocket::Close().
	if (mSocketClosing)
		return;



	// Handle the event
	StLock	acquireLock(mLock);

	switch (theEvent) {
		case kNSocketDidOpen:
			SocketDidOpen();
			break;

		case kNSocketDidClose:
			SocketDidClose((NStatus) theValue);
			break;
		
		case kNSocketHasConnection:
			SocketHasConnection((NSocket *) theValue);
			break;

		case kNSocketCanRead:
			ContinueReading();
			break;

		case kNSocketCanWrite:
			ContinueWriting();
			break;

		default:
			NN_LOG("Unknown socket event: %d/%d", theEvent, theValue);
			break;
		}
}





#pragma mark private
//============================================================================
//		NSocket::InitialiseSelf : Initialise ourselves.
//----------------------------------------------------------------------------
void NSocket::InitialiseSelf(NSocketDelegate *theDelegate)
{


	// Initialise ourselves
	mStatus   = kNSocketClosed;
	mResult   = kNoErr;
	mDelegate = theDelegate;

	mSocket        = NULL;
	mSocketClosing = false;
	
	mReadRequest         = NULL;
	mReadBufferOffset    = 0;
	mReadBufferAvailable = 0;
	mReadBuffer.SetSize(kReadBufferSize);
	
	mWriteRequest  = NULL;
	mWritesPending = 0;
}





//============================================================================
//		NSocket::SocketDidOpen : The socket has opened.
//----------------------------------------------------------------------------
void NSocket::SocketDidOpen(void)
{


	// Validate our state
	NN_ASSERT(mStatus == kNSocketOpening);



	// Update our state
	mStatus = kNSocketOpened;

	if (mDelegate != NULL)
		mDelegate->SocketDidOpen(this);
}





//============================================================================
//		NSocket::SocketDidClose : The socket has closed.
//----------------------------------------------------------------------------
void NSocket::SocketDidClose(NStatus theErr)
{	NSocketRef		theSocket;



	// Close the socket
	//
	// The socket can be closed via Open(), Close(), or due to a kNSocketDidClose event.
	//
	// In all cases we are inside our lock, however the use of a lock means that the socket
	// may be closed by Close() on one thread which triggers a close event on another. As
	// such we need to check to see if the socket is already closed, and can ignore the
	// second notification.
	//
	// Similarly we need to check to see if we actually have a socket before destroying it,
	// as a failure in Open() simply means we should inform the delegate.
	if (mStatus != kNSocketClosed)
		{
		// Update our state
		mStatus = kNSocketClosed;
		mResult = theErr;

		theSocket = mSocket;
		mSocket   = NULL;

		RemoveRequests(kNErrCancelled);



		// Close the socket
		if (mDelegate != NULL)
			mDelegate->SocketDidClose(this, theErr);

		if (theSocket != NULL)
			NTargetNetwork::SocketClose(theSocket);
		}
}





//============================================================================
//		NSocket::SocketHasConnection : The socket has a connection.
//----------------------------------------------------------------------------
void NSocket::SocketHasConnection(NSocket *newSocket)
{	NSocketConnectionFunctor	theFunctor;



	// Validate our state
	NN_ASSERT(mStatus == kNSocketOpened);



	// Handle the connection
	if (mDelegate)
		theFunctor = mDelegate->SocketHasConnection(this, newSocket);

	NThreadUtilities::DetachFunctor(BindFunction(NSocket::ConnectionThread, theFunctor, newSocket));
}





//============================================================================
//		NSocket::RemoveRequests : Remove the active/pending requests.
//----------------------------------------------------------------------------
void NSocket::RemoveRequests(NStatus theErr)
{	NSocketRequestListConstIterator		theIter;



	// Flush the pending requests
	for (theIter = mReadRequests.begin(); theIter != mReadRequests.end(); theIter++)
		(*theIter)->Release();

	for (theIter = mWriteRequests.begin(); theIter != mWriteRequests.end(); theIter++)
		(*theIter)->Release();



	// Stop the active requests
	if (mReadRequest != NULL)
		{
		mReadRequest->SetStatus(theErr);
		FinishedReading();
		}
	
	if (mWriteRequest != NULL)
		{
		mWriteRequest->SetStatus(theErr);
		FinishedWriting();
		}
	
	
	
	// Reset our state
	mReadRequests.clear();
	mWriteRequests.clear();

	mWritesPending = 0;
}





//============================================================================
//		NSocket::AddReadRequest : Add a read request.
//----------------------------------------------------------------------------
void NSocket::AddReadRequest(NSocketRequest *theRequest)
{	StLock		acquireLock(mLock);



	// Add the request
	theRequest->Retain();
	mReadRequests.push_back(theRequest);

	ContinueReading();
}





//============================================================================
//		NSocket::AddWriteRequest : Add a write request.
//----------------------------------------------------------------------------
void NSocket::AddWriteRequest(NSocketRequest *theRequest)
{	StLock		acquireLock(mLock);



	// Add the request
	theRequest->Retain();

	mWriteRequests.push_back(theRequest);
	mWritesPending += theRequest->GetSize();

	ContinueWriting();
}





//============================================================================
//		NSocket::ContinueReading : Continue reading.
//----------------------------------------------------------------------------
void NSocket::ContinueReading(void)
{	NIndex		readSize, readOffset;



	// Continue reading
	//
	// Reading is done in three phases:
	//
	//	- Read data into the request from our buffer
	//	- Read data into the request from the network
	//	- Read data into the buffer  from the network
	//
	// After reading from the network into the request we read again into the buffer
	// since the request may have been be smaller than the data currently held by the
	// OS (so there may be more data available to read).
	//
	// This then requires us to loop, since the final read into the buffer may also
	// be the last part of the response (which means no further kNSocketCanRead events
	// would be seen, and ContinueReading not re-called).
	//
	//
	// However this looping also helps us to reduce notification overhead - on fast
	// networks the OS may have more data available to read by the time we finish
	// updating the request from the buffer.
	//
	// Rather than exit the event handler and have to wait for the OS to re-schedule
	// another event, our next pass will also check to see if there's more data.
	//
	// Ultimately we stop when we run out of requests to process, run out of data to
	// read, or fill the buffer.
	while (true)
		{
		// Start the next request
		if (mReadRequest == NULL)
			{
			if (mReadRequests.empty())
				break;
		
			mReadRequest = extract_front(mReadRequests);
			}



		// Read from the buffer
		if (mReadBufferAvailable != 0)
			{
			// Read into the request
			readSize = std::min(mReadBufferAvailable, mReadRequest->GetUnprocessedSize());
			memcpy(mReadRequest->GetUnprocessedData(), mReadBuffer.GetData(mReadBufferOffset), readSize);
			mReadRequest->ProcessedData(readSize);


			// Update our state
			mReadBufferOffset    += readSize;
			mReadBufferAvailable -= readSize;

			if (mReadBufferAvailable == 0)
				mReadBufferOffset = 0;
			}



		// Read from the network
		if (!mReadRequest->IsFinished())
			{
			if (!NTargetNetwork::SocketCanRead(mSocket))
				break;

			readSize = NTargetNetwork::SocketRead(mSocket, mReadRequest->GetUnprocessedSize(), mReadRequest->GetUnprocessedData());
			mReadRequest->ProcessedData(readSize);
			}



		// Read into the buffer
		if (NTargetNetwork::SocketCanRead(mSocket))
			{
			readOffset = mReadBufferOffset + mReadBufferAvailable;
			readSize   = mReadBuffer.GetSize() - readOffset;

			if (readSize != 0)
				{
				readSize              = NTargetNetwork::SocketRead(mSocket, readSize, mReadBuffer.GetData(readOffset));
				mReadBufferAvailable += readSize;
				}
			}



		// Finish the request
		if (mReadRequest->IsFinished())
			FinishedReading();
		}
}





//============================================================================
//		NSocket::ContinueWriting : Continue writing.
//----------------------------------------------------------------------------
void NSocket::ContinueWriting(void)
{	NIndex		writeSize;



	// Continue writing
	while (true)
		{
		// Start the next request
		if (mWriteRequest == NULL)
			{
			if (mWriteRequests.empty())
				break;
		
			mWriteRequest = extract_front(mWriteRequests);
			}



		// Write to the network
		if (!NTargetNetwork::SocketCanWrite(mSocket))
			break;
		
		writeSize = NTargetNetwork::SocketWrite(mSocket, mWriteRequest->GetUnprocessedSize(), mWriteRequest->GetUnprocessedData());
		mWriteRequest->ProcessedData(writeSize);



		// Finish the request
		if (mWriteRequest->IsFinished())
			FinishedWriting();
		}
}





//============================================================================
//		NSocket::FinishedReading : Finish a read request.
//----------------------------------------------------------------------------
void NSocket::FinishedReading(void)
{	NSemaphore		*theSemaphore;



	// Get the state we need
	theSemaphore = mReadRequest->GetSemaphore();



	// Inform the source
	//
	// Non-blocking reads require a delegate.
	if (theSemaphore != NULL)
		theSemaphore->Signal();
	else
		mDelegate->SocketFinishedRead(this, mReadRequest->GetData(), mReadRequest->GetStatus());



	// Finish the request
	mReadRequest->Release();
	mReadRequest = NULL;
}





//============================================================================
//		NSocket::FinishedWriting : Finish a write request.
//----------------------------------------------------------------------------
void NSocket::FinishedWriting(void)
{	NSemaphore		*theSemaphore;



	// Validate our state
	NN_ASSERT(mWritesPending >= (int) mWriteRequest->GetSize());



	// Get the state we need
	theSemaphore = mWriteRequest->GetSemaphore();



	// Inform the source
	//
	// Non-blocking writes require a delegate.
	if (theSemaphore != NULL)
		theSemaphore->Signal();
	else
		mDelegate->SocketFinishedWrite(this, mWriteRequest->GetData(), mWriteRequest->GetStatus());



	// Finish the request
	mWritesPending -= mWriteRequest->GetSize();

	mWriteRequest->Release();
	mWriteRequest = NULL;
}





//============================================================================
//		NSocket::ConnectionThread : Run a connection thread.
//----------------------------------------------------------------------------
void NSocket::ConnectionThread(const NSocketConnectionFunctor &theFunctor, NSocket *theSocket)
{


	// Wait for the socket to open
	while (theSocket->GetStatus() == kNSocketOpening)
		NThread::Sleep();



	// Handle the connection
	if (theSocket->GetStatus() == kNSocketOpened)
		{
		if (theFunctor != NULL)
			theFunctor(theSocket);
		}



	// Close the socket
	if (theSocket->GetStatus() == kNSocketOpened)
		theSocket->Close();



	// Clean up
	delete theSocket;
}





#pragma mark NSocketDelegate
//============================================================================
//		NSocketDelegate::NSocketDelegate : Constructor.
//----------------------------------------------------------------------------
NSocketDelegate::NSocketDelegate(void)
{
}





//============================================================================
//		NSocketDelegate::~NSocketDelegate : Destructor.
//----------------------------------------------------------------------------
NSocketDelegate::~NSocketDelegate(void)
{
}





//============================================================================
//		NSocketDelegate::SocketDidOpen : The socket has opened.
//----------------------------------------------------------------------------
void NSocketDelegate::SocketDidOpen(NSocket * /*theSocket*/)
{
}





//============================================================================
//		NSocketDelegate::SocketDidClose : The socket has closed.
//----------------------------------------------------------------------------
void NSocketDelegate::SocketDidClose(NSocket * /*theSocket*/, NStatus /*theErr*/)
{
}





//============================================================================
//		NSocketDelegate::SocketHasConnection : The socket has a connection.
//----------------------------------------------------------------------------
NSocketConnectionFunctor NSocketDelegate::SocketHasConnection(NSocket * /*theSocket*/, NSocket * /*newSocket*/)
{
	return(NULL);
}





//============================================================================
//		NSocketDelegate::SocketFinishedRead : The socket has finished a read.
//----------------------------------------------------------------------------
void NSocketDelegate::SocketFinishedRead( NSocket * /*theSocket*/, const NData &/*theData*/, NStatus /*theErr*/)
{
}





//============================================================================
//		NSocketDelegate::SocketFinishedWrite : The socket has finished a write.
//----------------------------------------------------------------------------
void NSocketDelegate::SocketFinishedWrite( NSocket * /*theSocket*/, const NData &/*theData*/, NStatus /*theErr*/)
{
}



