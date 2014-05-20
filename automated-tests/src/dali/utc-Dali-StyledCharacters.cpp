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

#include <dali/dali.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_styled_characters_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_styled_characters_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
static const TextStyle DEFAULT_STYLE;
} // anon namespace

int UtcDaliStyledCharactersDefaultConstructor(void)
{
  tet_infoline(" UtcDaliStyledCharactersDefaultConstructor ");

  TestApplication application;

  StyledCharacters styledCharacters;

  std::size_t firstCharacter = 0u;
  std::size_t numberOfCharacters = 0u;

  styledCharacters.GetRange( firstCharacter, numberOfCharacters );

  DALI_TEST_EQUALS( 0u, firstCharacter, TEST_LOCATION );
  DALI_TEST_EQUALS( 0u, numberOfCharacters, TEST_LOCATION );
  DALI_TEST_EQUALS( DEFAULT_STYLE, styledCharacters.GetTextStyle(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliStyledCharactersCopyConstructor(void)
{
  tet_infoline(" UtcDaliStyledCharactersCopyConstructor ");

  TestApplication application;

  TextStyle style;
  style.SetTextColor( Color::GREEN );

  StyledCharacters styledCharacters( 1u, 3u, style );

  StyledCharacters styledCharacters2( styledCharacters );
  
  std::size_t firstCharacter = 0u;
  std::size_t numberOfCharacters = 0u;

  styledCharacters2.GetRange( firstCharacter, numberOfCharacters );
  
  DALI_TEST_EQUALS( 1u, firstCharacter, TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, numberOfCharacters, TEST_LOCATION );
  DALI_TEST_EQUALS( style, styledCharacters2.GetTextStyle(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliStyledCharactersAssignmentOperator(void)
{
  tet_infoline(" UtcDaliStyledCharactersAssignmentOperator ");

  TestApplication application;

  // A StyledCharacters object is assigned to a newly created object.

  TextStyle style;
  style.SetTextColor( Color::GREEN );

  StyledCharacters styledCharacters( 1u, 3u, style );

  StyledCharacters styledCharacters2;

  styledCharacters2 = styledCharacters;
  
  std::size_t firstCharacter = 0u;
  std::size_t numberOfCharacters = 0u;

  styledCharacters2.GetRange( firstCharacter, numberOfCharacters );
  
  DALI_TEST_EQUALS( 1u, firstCharacter, TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, numberOfCharacters, TEST_LOCATION );
  DALI_TEST_EQUALS( style, styledCharacters2.GetTextStyle(), TEST_LOCATION );

  // Test the asignement of the same object.

  StyledCharacters& styledCharacters3 = styledCharacters2;

  styledCharacters2 = styledCharacters3;

  firstCharacter = 0u;
  numberOfCharacters = 0u;
  styledCharacters2.GetRange( firstCharacter, numberOfCharacters );
  
  DALI_TEST_EQUALS( 1u, firstCharacter, TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, numberOfCharacters, TEST_LOCATION );
  DALI_TEST_EQUALS( style, styledCharacters2.GetTextStyle(), TEST_LOCATION );

  // A StyledCharacters object is assigned to a previously created object.

  style.SetItalics( true );
  StyledCharacters styledCharacters4( 4u, 7u, style );

  styledCharacters2 = styledCharacters4;

  firstCharacter = 0u;
  numberOfCharacters = 0u;
  styledCharacters2.GetRange( firstCharacter, numberOfCharacters );
  
  DALI_TEST_EQUALS( 4u, firstCharacter, TEST_LOCATION );
  DALI_TEST_EQUALS( 7u, numberOfCharacters, TEST_LOCATION );
  DALI_TEST_EQUALS( style, styledCharacters2.GetTextStyle(), TEST_LOCATION );

  // A newly created StyledCharacters object is assigned to a previously created object.
  StyledCharacters styledCharacters5;

  styledCharacters4 = styledCharacters5;

  firstCharacter = 0u;
  numberOfCharacters = 0u;
  styledCharacters4.GetRange( firstCharacter, numberOfCharacters );
  
  DALI_TEST_EQUALS( 0u, firstCharacter, TEST_LOCATION );
  DALI_TEST_EQUALS( 0u, numberOfCharacters, TEST_LOCATION );
  DALI_TEST_EQUALS( DEFAULT_STYLE, styledCharacters4.GetTextStyle(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliStyledCharactersConstructor(void)
{
  tet_infoline(" UtcDaliStyledCharactersConstructor ");

  TestApplication application;

  TextStyle style;
  style.SetTextColor( Color::GREEN );

  StyledCharacters styledCharacters( 1u, 3u, style );
  
  std::size_t firstCharacter = 0u;
  std::size_t numberOfCharacters = 0u;

  styledCharacters.GetRange( firstCharacter, numberOfCharacters );
  
  DALI_TEST_EQUALS( 1u, firstCharacter, TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, numberOfCharacters, TEST_LOCATION );
  DALI_TEST_EQUALS( style, styledCharacters.GetTextStyle(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliStyledCharactersGetRange(void)
{
  tet_infoline(" UtcDaliStyledCharactersGetRange ");

  TestApplication application;

  StyledCharacters styledCharacters;

  std::size_t firstCharacter = 0u;
  std::size_t numberOfCharacters = 0u;

  styledCharacters.GetRange( firstCharacter, numberOfCharacters );
 
  DALI_TEST_EQUALS( 0u, firstCharacter, TEST_LOCATION );
  DALI_TEST_EQUALS( 0u, numberOfCharacters, TEST_LOCATION );
  DALI_TEST_EQUALS( 0u, styledCharacters.GetFirstCharacterIndex(), TEST_LOCATION );
  DALI_TEST_EQUALS( 0u, styledCharacters.GetNumberOfCharacters(), TEST_LOCATION );

  TextStyle style;
  style.SetTextColor( Color::GREEN );

  styledCharacters = StyledCharacters( 1u, 3u, style );
  
  firstCharacter = 0u;
  numberOfCharacters = 0u;

  styledCharacters.GetRange( firstCharacter, numberOfCharacters );
  
  DALI_TEST_EQUALS( 1u, firstCharacter, TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, numberOfCharacters, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, styledCharacters.GetFirstCharacterIndex(), TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, styledCharacters.GetNumberOfCharacters(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliStyledCharactersUpdateRange(void)
{
  tet_infoline(" UtcDaliStyledCharactersUpdateRange ");

  TestApplication application;

  StyledCharacters styledCharacters;
  styledCharacters.UpdateRange( 6u, 7u);

  std::size_t firstCharacter = 0u;
  std::size_t numberOfCharacters = 0u;

  styledCharacters.GetRange( firstCharacter, numberOfCharacters );
 
  DALI_TEST_EQUALS( 0u, firstCharacter, TEST_LOCATION );
  DALI_TEST_EQUALS( 0u, numberOfCharacters, TEST_LOCATION );

  TextStyle style;
  style.SetTextColor( Color::GREEN );

  styledCharacters = StyledCharacters( 2u, 3u, style );
  styledCharacters.UpdateRange( 6u, 7u);

  styledCharacters.GetRange( firstCharacter, numberOfCharacters );
 
  DALI_TEST_EQUALS( 6u, firstCharacter, TEST_LOCATION );
  DALI_TEST_EQUALS( 7u, numberOfCharacters, TEST_LOCATION );
  
  END_TEST;
}

int UtcDaliStyledCharactersGetStyle(void)
{
  tet_infoline(" UtcDaliStyledCharactersGetStyle ");

  TestApplication application;

  TextStyle style;
  style.SetTextColor( Color::GREEN );

  StyledCharacters styledCharacters( 1u, 3u, style );

  DALI_TEST_EQUALS( style, styledCharacters.GetTextStyle(), TEST_LOCATION );

  END_TEST;
}
