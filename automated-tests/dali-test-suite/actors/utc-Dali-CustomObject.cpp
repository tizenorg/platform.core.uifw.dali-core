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

#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/mouse-wheel-event-integ.h>
#include <dali/integration-api/events/key-event-integ.h>

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
TEST_FUNCTION( UtcDaliCustomActorDestructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorImplDestructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorDownCast, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorDownCastNegative, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorGetImplementation, POSITIVE_TC_IDX );


// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

std::vector< std::string > MasterCallStack;

// TypeRegistry needs custom actor Implementations to have the same name (namespaces are ignored so we use one here)
namespace Impl
{

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
class TestCustomObject : public CustomObject
{
public:

  static TestCustomObject New( const std::string testName )
  {
    TestCustomObjectImpl* impl = new TestCustomObjectImpl( testName );

    TestCustomObject handle( *impl ); // takes ownership

    return handle;
  }

private:

  TestCustomObject( TestCustomObjectImpl& impl ) : CustomObject( impl )
  {
  }
};

// Positive test case for a method
static void UtcDaliCustomActorDownCast()
{
  TestApplication application;

  Handle handle = TestCustomObject::New();

  

  customActor = NULL;
  DALI_TEST_CHECK( !customActor );

  customActor = DownCast< CustomActor >( child );
  DALI_TEST_CHECK( customActor );
}

// Negative test case for a method
static void UtcDaliCustomActorDownCastNegative()
{
  TestApplication application;

  Actor actor1 = Actor::New();
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  CustomActor customActor = CustomActor::DownCast( child );
  DALI_TEST_CHECK( !customActor );

  Actor unInitialzedActor;
  customActor = CustomActor::DownCast( unInitialzedActor );
  DALI_TEST_CHECK( !customActor );

  customActor = DownCast< CustomActor >( unInitialzedActor );
  DALI_TEST_CHECK( !customActor );
}

static void UtcDaliCustomActorGetImplementation()
{
  TestApplication application;

  TestCustomObject custom = TestCustomObject::New();
  CustomActorImpl& impl = custom.GetImplementation();
  impl.GetOwner();  // Test

  const TestCustomObject constCustom = TestCustomObject::New();
  const CustomActorImpl& constImpl = constCustom.GetImplementation();
  constImpl.GetOwner();  // Test

  DALI_TEST_CHECK( true );
}

