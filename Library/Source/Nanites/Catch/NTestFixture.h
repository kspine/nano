/*	NAME:
		NTestFixture.h

	DESCRIPTION:
		Nano test fixture for catch.
		
		Catch is a header-only library, distributed under the boost licence.

		It has been included directly with this Nanite as this is compatible
		with the Nano licence.

	COPYRIGHT:
		Copyright (c) 2006-2013, refNum Software
		<http://www.refnum.com/>

		All rights reserved. Released under the terms of licence.html.
	__________________________________________________________________________
*/
#ifndef NTESTFIXTURE_HDR
#define NTESTFIXTURE_HDR
//============================================================================
//		Include files
//----------------------------------------------------------------------------
#include "catch.hpp"





//============================================================================
//		Catch
//----------------------------------------------------------------------------
// Classes
namespace Catch {
	class Fixture {
	public:
		Fixture(void) { }
		virtual ~Fixture(void) { }
		virtual void setUp(void) { }
		virtual void tearDown(void) { }
	};
}


// Macros
#define TEST_CASE_FIXTURE( ClassName, TestName, Desc )\
    namespace{ \
        class INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ ) : ClassName{ \
		public: \
			virtual ~INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ )(void) { } \
            void invokeFixture(); \
            void test(); \
        }; \
        Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar ) ( &INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ )::invokeFixture, #ClassName, TestName, Desc, CATCH_INTERNAL_LINEINFO ); \
    } \
    void INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ )::invokeFixture() \
	{ \
		setUp(); \
		test(); \
		tearDown(); \
	} \
	\
    void INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ )::test()


#define SETUP														virtual void setUp(void)

#define TEARDOWN													virtual void tearDown(void)





//============================================================================
//		Macros
//----------------------------------------------------------------------------
// Fixture macros
//
// A fixture derived from NTestFixture can be defined with:
//
//		NANO_FIXTURE(SomeFixture)
//		{
//			UInt8 *someData;
//
//			SETUP
//			{
//				someData = NULL;
//			}
//
//			TEARDOWN
//			{
//				if (someData != NULL)
//					free(someData);
//			}
//		}
//
// Both SETUP and TEARDOWN sections are optional.
//
//
// A test that uses this fixture can be defined with:
//
//		NANO_TEST(SomeFixture, "Malloc", "Testing malloc")
//		{
//			someData = (UInt8 *) malloc(10);
//			REQUIRE(someData != NULL);
//		}
//
// This will create a "Nano/SomeFixture/Malloc" test based on SomeFixture.
#define NANO_FIXTURE(_class)										struct _class : public NTestFixture
#define NANO_TEST(_class, _name, _desc)								TEST_CASE_FIXTURE(_class, "Nano/" #_class "/" _name, _desc)


// Assertions
#define REQUIRE_NOERR(_err)											REQUIRE((_err) == kNoErr)





//============================================================================
//		Class declaration
//----------------------------------------------------------------------------
class NTestFixture : public Catch::Fixture {
public:
										NTestFixture(void);
	virtual							   ~NTestFixture(void);


	// Reset the time
	void								ResetTime(void);


	// Get the elapsed time
	NTime								GetElapsedTime(void) const;


	// Test the time
	bool								TimeUnder(NTime theTime) const;
	bool								TimeOver( NTime theTime) const;


private:
	NTime								mTimeStart;
};





#endif // NTESTFIXTURE_HDR

