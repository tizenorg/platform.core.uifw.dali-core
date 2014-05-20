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
#include <dali/dali.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_styled_text_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_styled_text_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

static const std::string TEXT( "Hello world" );
static const std::string TEXT2( "Lorem ipsum dolor sit amet" );
static const std::string FONT_FAMILY( "Arial" );
static const Vector4 TEXT_COLOR( Color::RED );
static const bool ITALICS( true );
static const bool UNDERLINE( true );

static const Text VOID_TEXT;
static const TextStyle DEFAULT_STYLE;

void CheckStyleInRange( const TextStyle& style, const StyledText& styledText, std::size_t firstCharacter, std::size_t lastCharacter, const char* location )
{
  for( std::size_t index = firstCharacter; index <= lastCharacter; ++index )
  {
    TextStyle characterStyle;

    styledText.GetStyleAt( index, characterStyle );

    DALI_TEST_EQUALS( style, characterStyle, location );
  }
}

} // anon namespace

int UtcDaliStyledTextDefaultConstructor(void)
{
  tet_infoline(" UtcDaliTextStyleDefaultConstructor ");

  TestApplication application;

  StyledText styledText;

  DALI_TEST_EQUALS( VOID_TEXT.GetText(), styledText.GetText().GetText(), TEST_LOCATION );

  Vector<std::size_t> positions;
  styledText.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 0u, positions.Count(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliStyledTextCopyConstructor(void)
{
  tet_infoline(" UtcDaliStyledTextCopyConstructor ");

  TestApplication application;

  StyledText styledText;

  styledText.SetText( Text( TEXT ) );

  TextStyle style;
  style.SetFontName( FONT_FAMILY );
  style.SetTextColor( TEXT_COLOR );

  styledText.SetStyle( style, 3u, 3u );

  StyledText styledText2( styledText );

  const Text& text = styledText2.GetText();

  DALI_TEST_EQUALS( TEXT, text.GetText(), TEST_LOCATION );

  for( std::size_t index = 0u; index < text.GetLength(); ++index )
  {
    TextStyle style1;
    TextStyle style2;

    styledText.GetStyleAt( index, style1 );
    styledText2.GetStyleAt( index, style2 );

    DALI_TEST_EQUALS( style1, style2, TEST_LOCATION );
  }

  Vector<std::size_t> positions1;
  Vector<std::size_t> positions2;
  styledText.GetStyleRanges( positions1 );
  styledText2.GetStyleRanges( positions2 );

  DALI_TEST_CHECK( positions1.Count() == positions2.Count() );

  for( std::size_t index = 0u, length = std::min( positions1.Count(), positions2.Count() );
       index < length;
       ++index )
  {
    DALI_TEST_CHECK( positions1[index] == positions2[index] );
  }

  CheckStyleInRange( DEFAULT_STYLE, styledText2, 0u, 2u, TEST_LOCATION );
  CheckStyleInRange( style, styledText2, 3u, 5u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText2, 6u, 10u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliStyledTextAssignementOperator(void)
{
  tet_infoline(" UtcDaliStyledTextAssignementOperator ");

  TestApplication application;

  // Test the assignement to a newly created styled text.

  StyledText styledText;

  styledText.SetText( Text( TEXT ) );

  TextStyle style;
  style.SetFontName( FONT_FAMILY );
  style.SetTextColor( TEXT_COLOR );

  styledText.SetStyle( style, 1u, 4u );
  styledText.SetStyle( style, 6u, 3u );

  StyledText styledText2;

  styledText2 = styledText;

  DALI_TEST_EQUALS( TEXT, styledText2.GetText().GetText(), TEST_LOCATION );

  for( std::size_t index = 0u; index < styledText2.GetText().GetLength(); ++index )
  {
    TextStyle style1;
    TextStyle style2;

    styledText.GetStyleAt( index, style1 );
    styledText2.GetStyleAt( index, style2 );

    DALI_TEST_EQUALS( style1, style2, TEST_LOCATION );
  }

  Vector<std::size_t> positions1;
  Vector<std::size_t> positions2;
  styledText.GetStyleRanges( positions1 );
  styledText2.GetStyleRanges( positions2 );

  DALI_TEST_CHECK( positions1.Count() == positions2.Count() );

  for( std::size_t index = 0u, length = std::min( positions1.Count(), positions2.Count() );
       index < length;
       ++index )
  {
    DALI_TEST_CHECK( positions1[index] == positions2[index] );
  }

  CheckStyleInRange( DEFAULT_STYLE, styledText2, 0u, 0u, TEST_LOCATION );
  CheckStyleInRange( style, styledText2, 1u, 4u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText2, 5u, 5u, TEST_LOCATION );
  CheckStyleInRange( style, styledText2, 6u, 8u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText2, 9u, 10u, TEST_LOCATION );

  // Test the assignement of the same object.

  StyledText& styledText3 = styledText2;

  styledText3 = styledText2;

  DALI_TEST_EQUALS( TEXT, styledText3.GetText().GetText(), TEST_LOCATION );

  for( std::size_t index = 0u; index < styledText3.GetText().GetLength(); ++index )
  {
    TextStyle style1;
    TextStyle style2;

    styledText2.GetStyleAt( index, style1 );
    styledText3.GetStyleAt( index, style2 );

    DALI_TEST_EQUALS( style1, style2, TEST_LOCATION );
  }

  positions1.Clear();
  positions2.Clear();
  styledText2.GetStyleRanges( positions1 );
  styledText3.GetStyleRanges( positions2 );

  DALI_TEST_CHECK( positions1.Count() == positions2.Count() );

  for( std::size_t index = 0u, length = std::min( positions1.Count(), positions2.Count() );
       index < length;
       ++index )
  {
    DALI_TEST_CHECK( positions1[index] == positions2[index] );
  }

  CheckStyleInRange( DEFAULT_STYLE, styledText3, 0u, 0u, TEST_LOCATION );
  CheckStyleInRange( style, styledText3, 1u, 4u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText3, 5u, 5u, TEST_LOCATION );
  CheckStyleInRange( style, styledText3, 6u, 8u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText3, 9u, 10u, TEST_LOCATION );

  // Test the assignement to a previously created styled text.
  StyledText styledText4;

  styledText4.SetText( TEXT2 );

  TextStyle style2;
  style2.SetItalics( ITALICS );
  style2.SetUnderline( UNDERLINE );

  styledText4.SetStyle( style2, 1u, 11u );
  styledText4.SetStyle( style2, 19u, 5u );

  styledText3 = styledText4;

  DALI_TEST_EQUALS( TEXT2, styledText3.GetText().GetText(), TEST_LOCATION );

  for( std::size_t index = 0u; index < styledText3.GetText().GetLength(); ++index )
  {
    TextStyle style1;
    TextStyle style2;

    styledText3.GetStyleAt( index, style1 );
    styledText4.GetStyleAt( index, style2 );

    DALI_TEST_EQUALS( style1, style2, TEST_LOCATION );
  }

  positions1.Clear();
  positions2.Clear();
  styledText3.GetStyleRanges( positions1 );
  styledText4.GetStyleRanges( positions2 );

  DALI_TEST_CHECK( positions1.Count() == positions2.Count() );

  for( std::size_t index = 0u, length = std::min( positions1.Count(), positions2.Count() );
       index < length;
       ++index )
  {
    DALI_TEST_CHECK( positions1[index] == positions2[index] );
  }

  CheckStyleInRange( DEFAULT_STYLE, styledText3, 0u, 0u, TEST_LOCATION );
  CheckStyleInRange( style2, styledText3, 1u, 11u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText3, 12u, 18u, TEST_LOCATION );
  CheckStyleInRange( style2, styledText3, 19u, 23u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText3, 24u, 25u, TEST_LOCATION );

  // Test the assignement of a default styled text.

  styledText3 = StyledText();

  styledText4 = styledText3;

  DALI_TEST_EQUALS( VOID_TEXT.GetText(), styledText4.GetText().GetText(), TEST_LOCATION );

  Vector<std::size_t> positions;
  styledText4.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 0u, positions.Count(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliStyledTextConstructor(void)
{
  tet_infoline(" UtcDaliStyledTextConstructor ");

  TestApplication application;

  Text text( TEXT );
  StyledText styledText( text );

  const Text& text2 = styledText.GetText();

  DALI_TEST_EQUALS( TEXT, text2.GetText(), TEST_LOCATION );

  Vector<std::size_t> positions;
  styledText.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 0u, positions.Count(), TEST_LOCATION );

  // Test with an empty text.

  Text text3;

  StyledText styledText2( text3 );

  DALI_TEST_CHECK( styledText2.GetText().IsEmpty() );

  positions.Clear();
  styledText.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 0u, positions.Count(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliStyledTextClear(void)
{
  tet_infoline(" UtcDaliStyledTextClear ");

  TestApplication application;

  Text text( TEXT );
  StyledText styledText( text );

  TextStyle style;
  style.SetItalics( ITALICS );
  style.SetUnderline( UNDERLINE );

  styledText.SetStyle( style, 1u, 3u );

  styledText.Clear();

  DALI_TEST_CHECK( styledText.GetText().IsEmpty() );

  Vector<std::size_t> positions;
  styledText.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 0u, positions.Count(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliStyledTextSetGetText(void)
{
  tet_infoline(" UtcDaliStyledTextSetGetText ");

  TestApplication application;

  Vector<std::size_t> positions;

  // Test set an empty text to an empty styled text

  StyledText styledText1;
  styledText1.SetText( Text() );

  DALI_TEST_CHECK( styledText1.GetText().IsEmpty() );

  styledText1.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 0u, positions.Count(), TEST_LOCATION );

  // Test set an empty text to a styled text with styles.
  StyledText styledText2( TEXT );

  TextStyle style;
  style.SetItalics( ITALICS );
  style.SetUnderline( UNDERLINE );

  styledText2.SetStyle( style, 1u, 3u );

  styledText2.SetText( Text() );

  DALI_TEST_CHECK( styledText2.GetText().IsEmpty() );

  positions.Clear();
  styledText2.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 2u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, positions[1u], TEST_LOCATION );

  CheckStyleInRange( DEFAULT_STYLE, styledText2, 0u, 0u, TEST_LOCATION );
  CheckStyleInRange( style, styledText2, 1u, 3u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText2, 4u, 10u, TEST_LOCATION );

  // Test set an empty text to an empty styled text.
  StyledText styledText3;
  styledText3.SetText( Text() );

  DALI_TEST_CHECK( styledText3.GetText().IsEmpty() );

  positions.Clear();
  styledText3.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 0u, positions.Count(), TEST_LOCATION );
 
  // Test set an empty text to a styled text with text.
  StyledText styledText4( TEXT );

  styledText4.SetText( Text() );

  DALI_TEST_CHECK( styledText4.GetText().IsEmpty() );
 
  // Test set text to a styled text with text.
  StyledText styledText5;
  styledText5.SetText( Text( TEXT ) );

  DALI_TEST_EQUALS( TEXT, styledText5.GetText().GetText(), TEST_LOCATION );

  // Test set text to a styled text.
  styledText5.SetText( Text( TEXT2 ) );

  DALI_TEST_EQUALS( TEXT2, styledText5.GetText().GetText(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliStyledTextSetGetStyle(void)
{
  tet_infoline(" UtcDaliStyledTextSetGetStyle ");

  TestApplication application;

  StyledText styledText;
  styledText.SetText( Text( TEXT ) );

  {
    TextStyle style;
    style.SetFontName( FONT_FAMILY );
    styledText.SetStyle( style, 1u, 5u );
  }

  {
    TextStyle style;
    style.SetTextColor( TEXT_COLOR );
    styledText.SetStyle( style, 2u, 5u );
  }

  {
    TextStyle style;
    style.SetItalics( ITALICS );
    styledText.SetStyle( style, 3u, 5u );
  }

  {
    TextStyle style;
    style.SetUnderline( UNDERLINE );
    styledText.SetStyle( style, 4u, 5u );
  }

  TextStyle style0;
  TextStyle style1;
  TextStyle style2;
  TextStyle style3;
  TextStyle style4;
  TextStyle style5;
  TextStyle style6;
  TextStyle style7;
  TextStyle style8;
  TextStyle style9;
  TextStyle style10;

  styledText.GetStyleAt( 0u, style0 );
  styledText.GetStyleAt( 1u, style1 );
  styledText.GetStyleAt( 2u, style2 );
  styledText.GetStyleAt( 3u, style3 );
  styledText.GetStyleAt( 4u, style4 );
  styledText.GetStyleAt( 5u, style5 );
  styledText.GetStyleAt( 6u, style6 );
  styledText.GetStyleAt( 7u, style7 );
  styledText.GetStyleAt( 8u, style8 );
  styledText.GetStyleAt( 9u, style9 );
  styledText.GetStyleAt( 10u, style10 );

  TextStyle STYLE1;
  STYLE1.SetFontName( FONT_FAMILY );

  TextStyle STYLE2;
  STYLE2.SetFontName( FONT_FAMILY );
  STYLE2.SetTextColor( TEXT_COLOR );

  TextStyle STYLE3;
  STYLE3.SetFontName( FONT_FAMILY );
  STYLE3.SetTextColor( TEXT_COLOR );
  STYLE3.SetItalics( ITALICS );

  TextStyle STYLE4;
  STYLE4.SetFontName( FONT_FAMILY );
  STYLE4.SetTextColor( TEXT_COLOR );
  STYLE4.SetItalics( ITALICS );
  STYLE4.SetUnderline( UNDERLINE );

  TextStyle STYLE5;
  STYLE5.SetTextColor( TEXT_COLOR );
  STYLE5.SetItalics( ITALICS );
  STYLE5.SetUnderline( UNDERLINE );

  TextStyle STYLE6;
  STYLE6.SetItalics( ITALICS );
  STYLE6.SetUnderline( UNDERLINE );

  TextStyle STYLE7;
  STYLE7.SetUnderline( UNDERLINE );

  DALI_TEST_EQUALS( DEFAULT_STYLE, style0, TEST_LOCATION );
  DALI_TEST_EQUALS( STYLE1, style1, TEST_LOCATION );
  DALI_TEST_EQUALS( STYLE2, style2, TEST_LOCATION );
  DALI_TEST_EQUALS( STYLE3, style3, TEST_LOCATION );
  DALI_TEST_EQUALS( STYLE4, style4, TEST_LOCATION );
  DALI_TEST_EQUALS( STYLE4, style5, TEST_LOCATION );
  DALI_TEST_EQUALS( STYLE5, style6, TEST_LOCATION );
  DALI_TEST_EQUALS( STYLE6, style7, TEST_LOCATION );
  DALI_TEST_EQUALS( STYLE7, style8, TEST_LOCATION );
  DALI_TEST_EQUALS( DEFAULT_STYLE, style9, TEST_LOCATION );
  DALI_TEST_EQUALS( DEFAULT_STYLE, style10, TEST_LOCATION );

  // Set a style to an empty styled text.
  StyledText styledText2;

  TextStyle style11;
  style11.SetFontName( FONT_FAMILY );
  styledText2.SetStyle( style11, 1u, 5u );

  CheckStyleInRange( DEFAULT_STYLE, styledText2, 0u, 0u, TEST_LOCATION );
  CheckStyleInRange( style11, styledText2, 1u, 5u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText2, 6u, 10u, TEST_LOCATION );

  // Get a style from an empty styled text.

  StyledText styledText3;

  TextStyle style12;
  styledText3.GetStyleAt( 0u, style12 );

  DALI_TEST_EQUALS( DEFAULT_STYLE, style12, TEST_LOCATION );

  END_TEST;
}

int UtcDaliStyledTextGetStyleRanges(void)
{
  tet_infoline(" UtcDaliStyledTextGetStyleRanges ");

  TestApplication application;

  StyledText styledText;
  styledText.SetText( Text( TEXT ) );

  {
    TextStyle style1;
    style1.SetFontName( FONT_FAMILY );
    styledText.SetStyle( style1, 1u, 5u );
  }

  {
    TextStyle style2;
    style2.SetTextColor( TEXT_COLOR );
    styledText.SetStyle( style2, 2u, 5u );
  }

  {
    TextStyle style3;
    style3.SetItalics( ITALICS );
    styledText.SetStyle( style3, 3u, 5u );
  }

  {
    TextStyle style4;
    style4.SetUnderline( UNDERLINE );
    styledText.SetStyle( style4, 4u, 7u );
  }

  {
    TextStyle style5;
    style5.SetItalics( ITALICS );
    styledText.SetStyle( style5, 8u, 1u );
  }

  Vector<std::size_t> positions;
  styledText.GetStyleRanges( positions );

  Vector<std::size_t> styleChangePositions;
  styleChangePositions.PushBack( 1u );
  styleChangePositions.PushBack( 2u );
  styleChangePositions.PushBack( 3u );
  styleChangePositions.PushBack( 4u );
  styleChangePositions.PushBack( 5u );
  styleChangePositions.PushBack( 6u );
  styleChangePositions.PushBack( 7u );
  styleChangePositions.PushBack( 8u );
  styleChangePositions.PushBack( 10u );

  DALI_TEST_CHECK( styleChangePositions.Count() == positions.Count() );

  for( std::size_t index = 0u, length = std::min( styleChangePositions.Count(), positions.Count() );
       index < length;
       ++index )
  {
    DALI_TEST_CHECK( styleChangePositions[index] == positions[index] );
  }

  END_TEST;
}

int UtcDaliStyledTextInsert(void)
{
  tet_infoline(" UtcDaliStyledTextInsert ");

  TestApplication application;

  // Insert an empty styled text.

  StyledText styledText( TEXT );

  TextStyle style;
  style.SetFontName( FONT_FAMILY );
  style.SetTextColor( TEXT_COLOR );

  styledText.SetStyle( style, 3u, 5u );

  StyledText styledText2;

  styledText.Insert( 5u, styledText2 );

  DALI_TEST_EQUALS( TEXT, styledText.GetText().GetText(), TEST_LOCATION );

  CheckStyleInRange( DEFAULT_STYLE, styledText, 0u, 2u, TEST_LOCATION );
  CheckStyleInRange( style, styledText, 3u, 7u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText, 8u, 10u, TEST_LOCATION );

  // Insert a styled text into an empty one.

  StyledText styledText3;

  styledText3.Insert( 0u, styledText );

  DALI_TEST_EQUALS( TEXT, styledText3.GetText().GetText(), TEST_LOCATION );

  CheckStyleInRange( DEFAULT_STYLE, styledText3, 0u, 2u, TEST_LOCATION );
  CheckStyleInRange( style, styledText3, 3u, 7u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText3, 8u, 10u, TEST_LOCATION );

  // Insert a styled text.

  // Before a existing style.
  StyledText styledText4( "Insert" );

  TextStyle style1;
  style1.SetItalics( ITALICS );
  style1.SetUnderline( UNDERLINE );

  styledText4.SetStyle( style1, 2u, 3u );

  styledText.Insert( 3u, styledText4 );

  DALI_TEST_EQUALS( "HelInsertlo world", styledText.GetText().GetText(), TEST_LOCATION );

  Vector<std::size_t> positions;
  styledText.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 4u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 5u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 7u, positions[1u], TEST_LOCATION );
  DALI_TEST_EQUALS( 9u, positions[2u], TEST_LOCATION );
  DALI_TEST_EQUALS( 13u, positions[3u], TEST_LOCATION );

  CheckStyleInRange( DEFAULT_STYLE, styledText, 0u, 4u, TEST_LOCATION );
  CheckStyleInRange( style1, styledText, 5u, 7u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText, 8u, 8u, TEST_LOCATION );
  CheckStyleInRange( style, styledText, 9u, 13u, TEST_LOCATION );

  // In the middle of an existing style.

  styledText = styledText3;

  styledText.Insert( 4u, styledText4 );

  DALI_TEST_EQUALS( "HellInserto world", styledText.GetText().GetText(), TEST_LOCATION );

  positions.Clear();
  styledText.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 4u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 6u, positions[1u], TEST_LOCATION );
  DALI_TEST_EQUALS( 8u, positions[2u], TEST_LOCATION );
  DALI_TEST_EQUALS( 13u, positions[3u], TEST_LOCATION );

  TextStyle style2;
  style2.SetFontName( FONT_FAMILY );
  style2.SetTextColor( TEXT_COLOR );
  style2.SetItalics( ITALICS );
  style2.SetUnderline( UNDERLINE );

  CheckStyleInRange( DEFAULT_STYLE, styledText, 0u, 2u, TEST_LOCATION );
  CheckStyleInRange( style, styledText, 3u, 3u, TEST_LOCATION );
  CheckStyleInRange( style2, styledText, 6u, 8u, TEST_LOCATION );
  CheckStyleInRange( style, styledText, 9u, 13u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText, 14u, 16u, TEST_LOCATION );

  // After an existing style.

  styledText = styledText3;

  styledText.Insert( 8u, styledText4 );

  DALI_TEST_EQUALS( "Hello woInsertrld", styledText.GetText().GetText(), TEST_LOCATION );

  positions.Clear();
  styledText.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 4u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 7u, positions[1u], TEST_LOCATION );
  DALI_TEST_EQUALS( 10u, positions[2u], TEST_LOCATION );
  DALI_TEST_EQUALS( 12u, positions[3u], TEST_LOCATION );

  CheckStyleInRange( DEFAULT_STYLE, styledText, 0u, 2u, TEST_LOCATION );
  CheckStyleInRange( style, styledText, 3u, 7u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText, 8u, 9u, TEST_LOCATION );
  CheckStyleInRange( style1, styledText, 10u, 12u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText, 13u, 16u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliStyledTextRemove(void)
{
  tet_infoline(" UtcDaliStyledTextRemove ");

  TestApplication application;

  Vector<std::size_t> positions;

  // Remove from an empty styled text.

  StyledText styledText1;

  styledText1.Remove( 2u, 3u );

  DALI_TEST_CHECK( styledText1.GetText().IsEmpty() );

  styledText1.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 0u, positions.Count(), TEST_LOCATION );

  // Remove zero characters.

  StyledText styledText2( TEXT );

  TextStyle style1;
  style1.SetFontName( FONT_FAMILY );
  style1.SetTextColor( TEXT_COLOR );

  styledText2.SetStyle( style1, 3u, 5u );

  styledText2.Remove( 3u, 0u );

  DALI_TEST_EQUALS( TEXT, styledText2.GetText().GetText(), TEST_LOCATION );

  positions.Clear();
  styledText2.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 2u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 7u, positions[1u], TEST_LOCATION );

  CheckStyleInRange( DEFAULT_STYLE, styledText2, 0u, 2u, TEST_LOCATION );
  CheckStyleInRange( style1, styledText2, 3u, 7u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText2, 8u, 10u, TEST_LOCATION );

  // Delete all characters from a styled text with text and styles.

  StyledText styledText3( TEXT );

  TextStyle style2;
  style2.SetUnderline( UNDERLINE );
  style2.SetItalics( ITALICS );

  styledText3.SetStyle( style2, 3u, 5u );

  styledText3.Remove( 0u, 11u );

  DALI_TEST_CHECK( styledText3.GetText().IsEmpty() );

  positions.Clear();
  styledText3.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 0u, positions.Count(), TEST_LOCATION );

  // Delete all characters from a styled text with no text but styles.
  StyledText styledText4;

  styledText4.SetStyle( style2, 3u, 5u );

  styledText4.Remove( 0u, 11u );

  DALI_TEST_CHECK( styledText4.GetText().IsEmpty() );

  positions.Clear();
  styledText4.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 0u, positions.Count(), TEST_LOCATION );

  // Delete some characters testing all branches.

  StyledText styledText5( TEXT );
  
  styledText5.SetStyle( style2, 3u, 5u );

  styledText5.Remove( 1u, 10u );

  DALI_TEST_EQUALS( "H", styledText5.GetText().GetText(), TEST_LOCATION );

  positions.Clear();
  styledText5.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 0u, positions.Count(), TEST_LOCATION );

  StyledText styledText6( TEXT );
  
  styledText6.SetStyle( style2, 3u, 5u );

  styledText6.Remove( 0u, 10u );

  DALI_TEST_EQUALS( "d", styledText6.GetText().GetText(), TEST_LOCATION );

  positions.Clear();
  styledText6.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 0u, positions.Count(), TEST_LOCATION );

  StyledText styledText7;

  styledText7.SetStyle( style2, 3u, 5u );

  styledText7.Remove( 0u, 2u );

  DALI_TEST_CHECK( styledText7.GetText().IsEmpty() );

  positions.Clear();
  styledText7.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 2u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 5u, positions[1u], TEST_LOCATION );

  CheckStyleInRange( DEFAULT_STYLE, styledText7, 0u, 0u, TEST_LOCATION );
  CheckStyleInRange( style2, styledText7, 1u, 5u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText7, 6u, 8u, TEST_LOCATION );

  StyledText styledText8;

  styledText8.SetStyle( style2, 3u, 5u );

  styledText8.Remove( 3u, 1u );

  DALI_TEST_CHECK( styledText8.GetText().IsEmpty() );

  positions.Clear();
  styledText8.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 2u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 6u, positions[1u], TEST_LOCATION );

  CheckStyleInRange( DEFAULT_STYLE, styledText8, 0u, 2u, TEST_LOCATION );
  CheckStyleInRange( style2, styledText8, 3u, 6u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText8, 7u, 9u, TEST_LOCATION );

  StyledText styledText9;

  styledText9.SetStyle( style2, 3u, 5u );

  styledText9.Remove( 6u, 1u );

  DALI_TEST_CHECK( styledText9.GetText().IsEmpty() );

  positions.Clear();
  styledText9.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 2u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 6u, positions[1u], TEST_LOCATION );

  CheckStyleInRange( DEFAULT_STYLE, styledText9, 0u, 2u, TEST_LOCATION );
  CheckStyleInRange( style2, styledText9, 3u, 6u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText9, 7u, 9u, TEST_LOCATION );

  StyledText styledText10;

  styledText10.SetStyle( style2, 3u, 5u );

  styledText10.Remove( 9u, 1u );

  DALI_TEST_CHECK( styledText10.GetText().IsEmpty() );

  positions.Clear();
  styledText10.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 2u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 7u, positions[1u], TEST_LOCATION );

  CheckStyleInRange( DEFAULT_STYLE, styledText10, 0u, 2u, TEST_LOCATION );
  CheckStyleInRange( style2, styledText10, 3u, 7u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText10, 8u, 9u, TEST_LOCATION );

  StyledText styledText11;

  styledText11.SetStyle( style2, 3u, 5u );

  styledText11.Remove( 6u, 3u );

  DALI_TEST_CHECK( styledText11.GetText().IsEmpty() );

  positions.Clear();
  styledText11.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 2u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 5u, positions[1u], TEST_LOCATION );

  CheckStyleInRange( DEFAULT_STYLE, styledText11, 0u, 2u, TEST_LOCATION );
  CheckStyleInRange( style2, styledText11, 3u, 5u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText11, 6u, 7u, TEST_LOCATION );

  StyledText styledText12( TEXT );

  styledText12.SetStyle( style1, 2u, 5u );
  styledText12.SetStyle( style2, 4u, 5u );

  styledText12.Remove( 3u, 3u );

  DALI_TEST_EQUALS( "Helworld", styledText12.GetText().GetText(), TEST_LOCATION );

  positions.Clear();
  styledText12.GetStyleRanges( positions );

  DALI_TEST_EQUALS( 3u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 2u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, positions[1u], TEST_LOCATION );
  DALI_TEST_EQUALS( 5u, positions[2u], TEST_LOCATION );

  TextStyle style3;
  style3.SetFontName( FONT_FAMILY );
  style3.SetTextColor( TEXT_COLOR );
  style3.SetUnderline( UNDERLINE );
  style3.SetItalics( ITALICS );

  CheckStyleInRange( DEFAULT_STYLE, styledText12, 0u, 1u, TEST_LOCATION );
  CheckStyleInRange( style1, styledText12, 2u, 2u, TEST_LOCATION );
  CheckStyleInRange( style3, styledText12, 3u, 3u, TEST_LOCATION );
  CheckStyleInRange( style2, styledText12, 4u, 5u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, styledText12, 6u, 7u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliStyledGetSubText(void)
{
  tet_infoline(" UtcDaliStyledGetSubText ");

  TestApplication application;

  StyledText* styledText = NULL;
  Vector<std::size_t> positions;

  // Test get sub-text from an empty styled text.
  StyledText styledText1;

  styledText = styledText1.GetSubText( 0u, 2u );

  DALI_TEST_CHECK( NULL == styledText );

  // Test get a zero characters sub-text.
  StyledText styledText2( TEXT );

  styledText = styledText2.GetSubText( 2u, 0u );

  DALI_TEST_CHECK( NULL == styledText );

  // Test get a sub-text oout of range.
  StyledText styledText3( TEXT );

  TextStyle style1;
  style1.SetFontName( FONT_FAMILY );
  style1.SetTextColor( TEXT_COLOR );

  styledText3.SetStyle( style1, 3u, 5u );

  styledText = styledText3.GetSubText( 22u, 4u );
  
  DALI_TEST_CHECK( NULL == styledText );

  // Test get a sub-text from a styled text with no text but styles.
  StyledText styledText4;

  styledText4.SetStyle( style1, 3u, 5u );

  styledText = styledText4.GetSubText( 2u, 4u );

  DALI_TEST_CHECK( styledText->GetText().IsEmpty() );

  positions.Clear();
  styledText->GetStyleRanges( positions );

  DALI_TEST_EQUALS( 2u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, positions[1u], TEST_LOCATION );

  CheckStyleInRange( DEFAULT_STYLE, *styledText, 0u, 0u, TEST_LOCATION );
  CheckStyleInRange( style1, *styledText, 1u, 3u, TEST_LOCATION );

  delete styledText;
  styledText = NULL;

  // Test get a sub-text testing all branches.

  StyledText styledText5( TEXT );

  styledText = styledText5.GetSubText( 2u, 4u );

  DALI_TEST_EQUALS( "llo ", styledText->GetText().GetText(), TEST_LOCATION );

  delete styledText;
  styledText = NULL;

  StyledText styledText6( "H" );

  styledText6.SetStyle( style1, 3u, 5u );

  styledText = styledText6.GetSubText( 1u, 1u );

  DALI_TEST_CHECK( NULL == styledText );

  styledText = styledText6.GetSubText( 10u, 1u );

  DALI_TEST_CHECK( NULL == styledText );

  delete styledText;
  styledText = NULL;

  StyledText styledText7( TEXT );

  styledText7.SetStyle( style1, 3u, 5u );

  styledText = styledText7.GetSubText( 1u, 1u );

  DALI_TEST_EQUALS( "e", styledText->GetText().GetText(), TEST_LOCATION );

  positions.Clear();
  styledText->GetStyleRanges( positions );

  DALI_TEST_EQUALS( 0u, positions.Count(), TEST_LOCATION );

  delete styledText;
  styledText = NULL;

  styledText = styledText7.GetSubText( 8u, 1u );

  DALI_TEST_EQUALS( "r", styledText->GetText().GetText(), TEST_LOCATION );

  positions.Clear();
  styledText->GetStyleRanges( positions );

  DALI_TEST_EQUALS( 0u, positions.Count(), TEST_LOCATION );

  delete styledText;
  styledText = NULL;

  styledText = styledText7.GetSubText( 1u, 4u );

  DALI_TEST_EQUALS( "ello", styledText->GetText().GetText(), TEST_LOCATION );

  positions.Clear();
  styledText->GetStyleRanges( positions );

  DALI_TEST_EQUALS( 2u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 2u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, positions[1u], TEST_LOCATION );

  CheckStyleInRange( DEFAULT_STYLE, *styledText, 0u, 1u, TEST_LOCATION );
  CheckStyleInRange( style1, *styledText, 2u, 3u, TEST_LOCATION );

  delete styledText;
  styledText = NULL;

  styledText = styledText7.GetSubText( 4u, 2u );

  DALI_TEST_EQUALS( "o ", styledText->GetText().GetText(), TEST_LOCATION );

  positions.Clear();
  styledText->GetStyleRanges( positions );

  DALI_TEST_EQUALS( 2u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 0u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, positions[1u], TEST_LOCATION );

  CheckStyleInRange( style1, *styledText, 0u, 1u, TEST_LOCATION );

  delete styledText;
  styledText = NULL;

  styledText = styledText7.GetSubText( 6u, 3u );

  DALI_TEST_EQUALS( "wor", styledText->GetText().GetText(), TEST_LOCATION );

  positions.Clear();
  styledText->GetStyleRanges( positions );

  DALI_TEST_EQUALS( 2u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 0u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, positions[1u], TEST_LOCATION );

  CheckStyleInRange( style1, *styledText, 0u, 1u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, *styledText, 2u, 2u, TEST_LOCATION );

  delete styledText;
  styledText = NULL;

  styledText = styledText7.GetSubText( 1u, 9u );

  DALI_TEST_EQUALS( "ello worl", styledText->GetText().GetText(), TEST_LOCATION );

  positions.Clear();
  styledText->GetStyleRanges( positions );

  DALI_TEST_EQUALS( 2u, positions.Count(), TEST_LOCATION );
  DALI_TEST_EQUALS( 2u, positions[0u], TEST_LOCATION );
  DALI_TEST_EQUALS( 6u, positions[1u], TEST_LOCATION );

  CheckStyleInRange( DEFAULT_STYLE, *styledText, 0u, 1u, TEST_LOCATION );
  CheckStyleInRange( style1, *styledText, 2u, 6u, TEST_LOCATION );
  CheckStyleInRange( DEFAULT_STYLE, *styledText, 7u, 9u, TEST_LOCATION );

  delete styledText;
  styledText = NULL;

  END_TEST;
}
