/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/glyph-set.h>
#include <dali-test-suite-utils.h>

// Internal headers are allowed here

#include <dali/internal/event/text/text-impl.h>

using namespace Dali;

void utc_dali_internal_text_startup()
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_text_cleanup()
{
  test_return_value = TET_PASS;
}

int UtcDaliTextGetImplementation01(void)
{
  TestApplication application;

  Text text( std::string( "Hello world" ) );

  const Internal::Text& textImpl1 = text.GetImplementation();

  DALI_TEST_CHECK( NULL != &textImpl1 );

  Internal::Text& textImpl2 = text.GetImplementation();

  DALI_TEST_CHECK( NULL != &textImpl2 );

  END_TEST;
}

int UtcDaliTextGetImplementation02(void)
{
  TestApplication application;

  Text text;

  bool assert1 = false;
  bool assert2 = false;

  try  // const GetImpl
  {
    const Internal::Text& impl = text.GetImplementation();
    (void)impl; // Avoid unused variable warning
  }
  catch( DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "NULL != mImpl && \"Text::GetImplementation: Text is uninitialized\"", TEST_LOCATION );

    assert1 = true;
  }

  try   // non const getImp
  {
    Internal::Text& impl = text.GetImplementation();
    (void)impl; // Avoid unused variable warning
  }
  catch( DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "NULL != mImpl && \"Text::GetImplementation: Text is uninitialized\"", TEST_LOCATION );

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
  END_TEST;
}

int UtcDaliTextGetTextArray(void)
{
  TestApplication application;

  Text text( std::string( "Hello world" ) );

  const TextArray& textArray1 = text.GetImplementation().GetTextArray();

  DALI_TEST_EQUALS( textArray1.size(), 11u, TEST_LOCATION );

  DALI_TEST_EQUALS( textArray1[0u], 72u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray1[1u], 101u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray1[2u], 108u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray1[3u], 108u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray1[4u], 111u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray1[5u], 32u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray1[6u], 119u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray1[7u], 111u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray1[8u], 114u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray1[9u], 108u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray1[10u], 100u, TEST_LOCATION );

  const TextArray& textArray2 = Internal::GetTextArray( text );

  DALI_TEST_EQUALS( textArray2.size(), 11u, TEST_LOCATION );

  DALI_TEST_EQUALS( textArray2[0u], 72u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray2[1u], 101u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray2[2u], 108u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray2[3u], 108u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray2[4u], 111u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray2[5u], 32u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray2[6u], 119u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray2[7u], 111u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray2[8u], 114u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray2[9u], 108u, TEST_LOCATION );
  DALI_TEST_EQUALS( textArray2[10u], 100u, TEST_LOCATION );

  const TextArray& textArray3 = Internal::GetTextArray( Text() );

  DALI_TEST_CHECK( textArray3.empty() );

  END_TEST;
}
