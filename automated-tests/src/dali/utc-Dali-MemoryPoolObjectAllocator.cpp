/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_memorypoolobjectallocator_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_memorypoolobjectallocator_cleanup(void)
{
  test_return_value = TET_PASS;
}

struct MemoryPoolObjectAllocatorTestObjectTracking
{
  int testObjectDestructed;
  int testObjectMethod;
  int testObjectDataAccess;

  MemoryPoolObjectAllocatorTestObjectTracking()
  : testObjectDestructed( 0 ),
    testObjectMethod( 0 ),
    testObjectDataAccess( 0 )
  {
  }
};

class MemoryPoolObjectAllocatorTestObject
{
public:

  MemoryPoolObjectAllocatorTestObject()
  : mData1( 0 ),
    mData2( false ),
    mTracking( NULL )
  {
  }

  ~MemoryPoolObjectAllocatorTestObject()
  {
    if( mTracking )
    {
      mTracking->testObjectDestructed++;
    }
  }

  void Method()
  {
    if( mTracking )
    {
      mTracking->testObjectMethod++;
    }
  }

  void DataAccess()
  {
    mData1++;
    mData2 = true;

    if( mTracking )
    {
      mTracking->testObjectDataAccess++;
    }
  }

  void SetTracking( MemoryPoolObjectAllocatorTestObjectTracking* tracking )
  {
    mTracking = tracking;
  }

private:

  int mData1;
  bool mData2;

  MemoryPoolObjectAllocatorTestObjectTracking* mTracking;
};

int UtcDaliMemoryPoolObjectAllocatorObjectAllocation(void)
{

  MemoryPoolObjectAllocator< MemoryPoolObjectAllocatorTestObject > allocator;

  // Allocate an object
  MemoryPoolObjectAllocatorTestObject* testObject1 = allocator.Allocate();
  DALI_TEST_CHECK( testObject1 );

  MemoryPoolObjectAllocatorTestObjectTracking tracking1;
  testObject1->SetTracking( &tracking1 );

  testObject1->Method();
  DALI_TEST_EQUALS( tracking1.testObjectMethod, 1, TEST_LOCATION );

  testObject1->DataAccess();
  DALI_TEST_EQUALS( tracking1.testObjectDataAccess, 1, TEST_LOCATION );

  allocator.Free( testObject1 );
  DALI_TEST_EQUALS( tracking1.testObjectDestructed, 1, TEST_LOCATION );

  // Reset and allocate another object
  allocator.ResetMemoryPool();

  MemoryPoolObjectAllocatorTestObject* testObject2 = allocator.Allocate();
  DALI_TEST_CHECK( testObject2 );

  MemoryPoolObjectAllocatorTestObjectTracking tracking2;
  testObject2->SetTracking( &tracking2 );

  testObject2->Method();
  DALI_TEST_EQUALS( tracking2.testObjectMethod, 1, TEST_LOCATION );

  testObject2->DataAccess();
  DALI_TEST_EQUALS( tracking2.testObjectDataAccess, 1, TEST_LOCATION );

  allocator.Free( testObject2 );
  DALI_TEST_EQUALS( tracking2.testObjectDestructed, 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMemoryPoolObjectAllocatorObjectRawAllocation(void)
{
  MemoryPoolObjectAllocator< MemoryPoolObjectAllocatorTestObject > allocator;

  MemoryPoolObjectAllocatorTestObject* testObject = new ( allocator.AllocateRaw() ) MemoryPoolObjectAllocatorTestObject();
  DALI_TEST_CHECK( testObject );

  MemoryPoolObjectAllocatorTestObjectTracking tracking;
  testObject->SetTracking( &tracking );

  testObject->Method();
  DALI_TEST_EQUALS( tracking.testObjectMethod, 1, TEST_LOCATION );

  testObject->DataAccess();
  DALI_TEST_EQUALS( tracking.testObjectDataAccess, 1, TEST_LOCATION );

  allocator.Free( testObject );
  DALI_TEST_EQUALS( tracking.testObjectDestructed, 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMemoryPoolObjectAllocatorObjectAllocationPOD(void)
{
  MemoryPoolObjectAllocator< bool > allocator;

  bool* testObject1 = allocator.Allocate();
  DALI_TEST_CHECK( testObject1 );

  allocator.Free( testObject1 );

  allocator.ResetMemoryPool();

  bool* testObject2 = allocator.Allocate();
  DALI_TEST_CHECK( testObject2 );

  allocator.Free( testObject2 );

  END_TEST;
}
