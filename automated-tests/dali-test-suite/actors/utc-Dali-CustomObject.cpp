//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C"
{
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum
{
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliCustomObjectDownCast, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomObjectDownCastNegative, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomObjectGetImplementation, POSITIVE_TC_IDX );


// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

/**
 * Test custom object impl
 */
class TestCustomObjectImpl : public CustomObjectImpl
{
public:

  /**
   * Constructor
   */
  TestCustomObjectImpl( const std::string testName )
  : mName( testName )
  {
  }

protected:

  /**
   * Destructor
   */
  virtual ~TestCustomObjectImpl()
  {
  }

public:

  std::string mName;
};

/**
 * Test custom object handle
 */
class TestCustomHandle : public CustomHandle
{
public:

  static TestCustomHandle New( const std::string testName )
  {
    TestCustomObjectImpl* impl = new TestCustomObjectImpl( testName );

    TestCustomHandle handle( *impl ); // takes ownership

    return handle;
  }

private:

  TestCustomHandle( TestCustomObjectImpl& impl ) : CustomHandle( impl )
  {
  }
};

static void UtcDaliCustomObjectDownCast()
{
  TestApplication application;

  Handle handle = TestCustomHandle::New( "DownCastTester" );

  CustomHandle customHandle1 = CustomHandle::DownCast( handle );
  DALI_TEST_CHECK( customHandle1 );

  CustomHandle customHandle2 = DownCast< CustomHandle >( handle );
  DALI_TEST_CHECK( customHandle2 );
}

static void UtcDaliCustomObjectDownCastNegative()
{
  TestApplication application;

  Actor actor = Actor::New();
  CustomHandle customHandle1 = CustomHandle::DownCast( actor );
  DALI_TEST_CHECK( ! customHandle1 );

  CustomHandle empty;
  CustomHandle customHandle2 = CustomHandle::DownCast( empty );
  DALI_TEST_CHECK( ! customHandle2 );
}

static void UtcDaliCustomObjectGetImplementation()
{
  TestApplication application;

  CustomHandle handle = TestCustomHandle::New( "GetImplementationTester" );

  CustomObjectImpl& impl = handle.GetImplementation();
  TestCustomObjectImpl& testImpl = static_cast<TestCustomObjectImpl&>( impl );
  DALI_TEST_CHECK( testImpl.mName == "GetImplementationTester" );

  const CustomHandle handle2 = TestCustomHandle::New( "GetImplementationTester2" );

  const CustomObjectImpl& impl2 = handle2.GetImplementation();
  const TestCustomObjectImpl& testImpl2 = static_cast<const TestCustomObjectImpl&>( impl2 );
  DALI_TEST_CHECK( testImpl2.mName == "GetImplementationTester2" );
}

