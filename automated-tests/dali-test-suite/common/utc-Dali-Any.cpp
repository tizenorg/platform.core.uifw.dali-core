/*
Copyright (c) 2000-2014 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

namespace
{

struct MyStruct
{
  MyStruct()
  : mUiValue( 10u ),
    mFlValue( 1.f ),
    mStrValue( "Hello" )
  {
    ++mNumCallsToDefaultConstructor;
  }

  ~MyStruct()
  {
  }

  MyStruct( const MyStruct& rhs )
  : mUiValue( rhs.mUiValue ),
    mFlValue( rhs.mFlValue ),
    mStrValue( rhs.mStrValue )
  {
    ++mNumCallsToCopyConstructor;
  }

  MyStruct& operator=( const MyStruct& rhs )
  {
    mUiValue = rhs.mUiValue;
    mFlValue = rhs.mFlValue;
    mStrValue = rhs.mStrValue;

    ++mNumCallsToAssignmentOperator;

    return *this;
  }

  bool operator==( const MyStruct& rhs ) const
  {
    return ( ( mUiValue == rhs.mUiValue ) &&
             ( fabsf( mFlValue - rhs.mFlValue ) < Math::MACHINE_EPSILON_1000 ) &&
             ( mStrValue == rhs.mStrValue ) );
  }

  unsigned int mUiValue;
  float mFlValue;
  std::string mStrValue;

  static size_t mNumCallsToDefaultConstructor;
  static size_t mNumCallsToCopyConstructor;
  static size_t mNumCallsToAssignmentOperator;
};

size_t MyStruct::mNumCallsToDefaultConstructor = 0u;
size_t MyStruct::mNumCallsToCopyConstructor = 0u;
size_t MyStruct::mNumCallsToAssignmentOperator = 0u;

} // namespace

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

TEST_FUNCTION( UtcDaliAnyConstructors, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnyAssignmentOperators, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnyNegativeAssignmentOperators, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnyGetType, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnyGet, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnyNegativeGet, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAnyReferences, POSITIVE_TC_IDX);

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliAnyConstructors()
{
  TestApplication application;

  tet_infoline("Test Any constructors.");

  // Test default constructor.
  Any value;

  DALI_TEST_CHECK( typeid( void ) == value.GetType() );

  // Test constructor Any( const Type& )
  Any value1 = 4u;

  // Test constructor Any( const Any& )
  Any value2 = value1;

  // Test constructor Any( const Any& ) with a non initialized Any
  Any value3 = value;

  DALI_TEST_CHECK( typeid( unsigned int ) == value1.GetType() );
  DALI_TEST_CHECK( typeid( unsigned int ) == value2.GetType() );
  DALI_TEST_CHECK( typeid( void ) == value3.GetType() );

  unsigned int uiValue1 = 0u;
  unsigned int uiValue2 = 0u;
  value1.Get( uiValue1 );
  value2.Get( uiValue2 );

  DALI_TEST_EQUALS( uiValue1, uiValue2, TEST_LOCATION );
}

static void UtcDaliAnyAssignmentOperators()
{
  TestApplication application;

  tet_infoline("Test assignment operators.");

  float fValue = 0.f;

  Any value1;

  value1 = 4.f; // Test operator=( const Type& ) when current object is not initialized.

  value1.Get( fValue );

  DALI_TEST_EQUALS( fValue, 4.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  Any value2 = 0.f;

  value2 = 9.f; // Test operator=( const Type& ).

  value2.Get( fValue );

  DALI_TEST_EQUALS( fValue, 9.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  Any value3 = 5.f;

  value1 = value3; // Test operator=( const Any& ).

  value1.Get( fValue );

  DALI_TEST_EQUALS( fValue, 5.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  Any value4;

  value4 = value3; // Test operator=( const Any& ) when current object is not initialized.

  value4.Get( fValue );

  DALI_TEST_EQUALS( fValue, 5.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Test assign a value to value3 doesn't modify value1.
  value3 = 3.f;

  value1.Get( fValue );

  DALI_TEST_EQUALS( fValue, 5.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  value3.Get( fValue );

  DALI_TEST_EQUALS( fValue, 3.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Test the branch when copying the same object.
  Any value5 = 3.f;
  Any& value6( value5 );

  value6 = value5;

  value6.Get( fValue );
  DALI_TEST_EQUALS( fValue, 3.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
}

static void UtcDaliAnyNegativeAssignmentOperators()
{
  TestApplication application;

  tet_infoline("Test assignment operators.");

  Any value1 = 4.f;
  Any value2 = 5u;

  bool assert1 = false;
  bool assert2 = false;
  try
  {
    value1 = value2; // Test operator=( const Any& );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"Any::operator=( const Any& Any ). Trying to assign two values with different types.\"", TEST_LOCATION );

    assert1 = true;
  }

  try
  {
    value1 = 0u; // Test operator=( const Type& );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"Any::operator=( const Type& value ). Trying to assign two values with different types.\"", TEST_LOCATION );

    assert2 = true;
  }

  if( assert1 && assert2 )
  {
    tet_result( TET_PASS );
  }
  else
  {
    tet_result( TET_FAIL );
  }
}

static void UtcDaliAnyGetType()
{
  TestApplication application;

  tet_infoline("Test GetType().");

  Any value;

  DALI_TEST_CHECK( typeid( void ) == value.GetType() );

  value = 5.f;

  DALI_TEST_CHECK( typeid( float ) == value.GetType() );
}

static void UtcDaliAnyGet()
{
  TestApplication application;

  tet_infoline("Test Get().");

  Any value1( 5.f );

  float fValue = value1.Get<float>();

  DALI_TEST_EQUALS( fValue, 5.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  fValue = 0.f;
  value1.Get( fValue );
  DALI_TEST_EQUALS( fValue, 5.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
}

static void UtcDaliAnyNegativeGet()
{
  TestApplication application;

  tet_infoline("Test Get().");

  Any value1;
  Any value2( 5.f );

  unsigned int uiValue = 0u;

  bool assert1 = false;
  bool assert2 = false;

  try
  {
    uiValue = value1.Get<unsigned int>();
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"Any::Get(). Trying to retrieve a value of a non initialized Any.\"", TEST_LOCATION );

    assert1 = true;
  }

  try
  {
    uiValue = value2.Get<unsigned int>();
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"Any::Get(). Trying to retrieve a value of a different type than the template one.\"", TEST_LOCATION );

    assert2 = true;
  }

  if( assert1 && assert2 )
  {
    tet_result( TET_PASS );
  }
  else
  {
    tet_result( TET_FAIL );
  }
}

static void UtcDaliAnyReferences()
{
  MyStruct::mNumCallsToDefaultConstructor = 0u;
  MyStruct::mNumCallsToCopyConstructor = 0u;
  MyStruct::mNumCallsToAssignmentOperator = 0u;

  std::cout << "UtcDaliAnyReferences" << std::endl;
  TestApplication application;

  std::vector<Any> data;

  data.push_back( Any( std::string( "Hello world!" ) ) );

  const std::string& helloWorld = ( *data.begin() ).Get<std::string>();

  DALI_TEST_EQUALS( helloWorld, std::string( "Hello world!" ), TEST_LOCATION );

  MyStruct myStruct; // Calls MyStruct default constructor.
  Any vt( myStruct ); // Calls MyStruct copy constructor.
  data.push_back( vt ); // Calls MyStruct copy constructor.

  MyStruct myStruct2; // Calls MyStruct default constructor.
  ( *( data.begin() + 1 ) ).Get( myStruct2 ); // Calls MyStruct assignment operator.

  const MyStruct& myStruct3( ( *( data.begin() + 1 ) ).Get<MyStruct>() );

  DALI_TEST_CHECK( myStruct == myStruct2 );
  DALI_TEST_CHECK( myStruct2 == myStruct3 );

  DALI_TEST_CHECK( MyStruct::mNumCallsToDefaultConstructor == 2u );
  DALI_TEST_CHECK( MyStruct::mNumCallsToCopyConstructor == 2u );
  DALI_TEST_CHECK( MyStruct::mNumCallsToAssignmentOperator == 1u );
}
