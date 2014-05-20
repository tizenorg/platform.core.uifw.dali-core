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
static const std::string FONT_FAMILY( "Arial" );
static const Vector4 TEXT_COLOR( Color::RED );
static const bool ITALICS( true );
static const bool UNDERLINE( true );

} // anon namespace

int UtcDaliStyledTextDefaultConstructor(void)
{
  tet_infoline(" UtcDaliTextStyleDefaultConstructor ");

  TestApplication application;

  StyledText styledText;
  
  const Text& text = styledText.GetText();

  DALI_TEST_CHECK( text.IsEmpty() );

  std::vector<std::size_t> positions;
  styledText.GetStyleChangePositions( positions );

  DALI_TEST_CHECK( positions.empty() );

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

  StyledText styledText2( styledText );

  const Text& text = styledText2.GetText();

  DALI_TEST_EQUALS( TEXT, text.GetText(), TEST_LOCATION );

  for( std::size_t index = 0u; index < text.GetLength(); ++index )
  {
    TextStyle style1;
    TextStyle style2;

    styledText.GetStyleAt( index, style1 );
    styledText2.GetStyleAt( index, style2 );

    DALI_TEST_CHECK( style1 == style2 );
  }

  std::vector<std::size_t> positions1;
  std::vector<std::size_t> positions2;
  styledText.GetStyleChangePositions( positions1 );
  styledText2.GetStyleChangePositions( positions2 );

  DALI_TEST_CHECK( positions1 == positions2 );  

  END_TEST;
}

int UtcDaliStyledTextAssignementOperator(void)
{
  tet_infoline(" UtcDaliStyledTextAssignementOperator ");

  TestApplication application;

  StyledText styledText;

  styledText.SetText( Text( TEXT ) );

  TextStyle style;
  style.SetFontName( FONT_FAMILY );
  style.SetTextColor( TEXT_COLOR );

  styledText.SetStyle( style, 1u, 4u );
  styledText.SetStyle( style, 6u, 8u );

  StyledText styledText2;

  styledText2 = styledText;

  const Text& text = styledText2.GetText();

  DALI_TEST_EQUALS( TEXT, text.GetText(), TEST_LOCATION );

  for( std::size_t index = 0u; index < text.GetLength(); ++index )
  {
    TextStyle style1;
    TextStyle style2;

    styledText.GetStyleAt( index, style1 );
    styledText2.GetStyleAt( index, style2 );

    DALI_TEST_CHECK( style1 == style2 );
  }

  std::vector<std::size_t> positions1;
  std::vector<std::size_t> positions2;
  styledText.GetStyleChangePositions( positions1 );
  styledText2.GetStyleChangePositions( positions2 );

  DALI_TEST_CHECK( positions1 == positions2 );  

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

  END_TEST;
}

int UtcDaliStyledTextSetGetText(void)
{
  tet_infoline(" UtcDaliStyledTextSetGetText ");

  TestApplication application;

  StyledText styledText;

  styledText.SetText( Text( TEXT ) );

  DALI_TEST_EQUALS( TEXT, styledText.GetText().GetText(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliStyledTextSetGetStyle(void)
{
  tet_infoline(" UtcDaliStyledTextSetGetStyle ");

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
    styledText.SetStyle( style2, 2u, 6u );
  }

  {
    TextStyle style3;
    style3.SetItalics( ITALICS );
    styledText.SetStyle( style3, 3u, 7u );
  }

  {
    TextStyle style4;
    style4.SetUnderline( UNDERLINE );
    styledText.SetStyle( style4, 4u, 8u );
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

  std::cout << "   style0 " << style0 << std::endl; 
  std::cout << "   style1 " << style1 << std::endl; 
  std::cout << "   style2 " << style2 << std::endl; 
  std::cout << "   style3 " << style3 << std::endl; 
  std::cout << "   style4 " << style4 << std::endl; 
  std::cout << "   style5 " << style5 << std::endl; 
  std::cout << "   style6 " << style6 << std::endl; 
  std::cout << "   style7 " << style7 << std::endl; 
  std::cout << "   style8 " << style8 << std::endl; 
  std::cout << "   style9 " << style9 << std::endl; 
  std::cout << "  style10 " << style10 << std::endl; 

  TextStyle DEFAULT;

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

  DALI_TEST_CHECK( DEFAULT == style0 );
  DALI_TEST_CHECK( STYLE1 == style1 );
  DALI_TEST_CHECK( STYLE2 == style2 );
  DALI_TEST_CHECK( STYLE3 == style3 );
  DALI_TEST_CHECK( STYLE4 == style4 );
  DALI_TEST_CHECK( STYLE4 == style5 );
  DALI_TEST_CHECK( STYLE5 == style6 );
  DALI_TEST_CHECK( STYLE6 == style7 );
  DALI_TEST_CHECK( STYLE7 == style8 );
  DALI_TEST_CHECK( DEFAULT == style9 );
  DALI_TEST_CHECK( DEFAULT == style10 );

  END_TEST;
}

int UtcDaliStyledTextGetStyleChangePositions(void)
{
  tet_infoline(" UtcDaliStyledTextGetStyleChangePositions ");

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
    styledText.SetStyle( style2, 2u, 6u );
  }

  {
    TextStyle style3;
    style3.SetItalics( ITALICS );
    styledText.SetStyle( style3, 3u, 7u );
  }

  {
    TextStyle style4;
    style4.SetUnderline( UNDERLINE );
    styledText.SetStyle( style4, 4u, 10u );
  }

  {
    TextStyle style5;
    style5.SetItalics( ITALICS );
    styledText.SetStyle( style5, 8u, 8u );
  }

  std::vector<std::size_t> positions;
  styledText.GetStyleChangePositions( positions );

  std::vector<std::size_t> styleChangePositions;
  styleChangePositions.push_back( 1u );
  styleChangePositions.push_back( 2u );
  styleChangePositions.push_back( 3u );
  styleChangePositions.push_back( 4u );
  styleChangePositions.push_back( 5u );
  styleChangePositions.push_back( 6u );
  styleChangePositions.push_back( 7u );
  styleChangePositions.push_back( 8u );
  styleChangePositions.push_back( 9u );
  styleChangePositions.push_back( 10u );

  DALI_TEST_CHECK( styleChangePositions == positions );

  END_TEST;
}

int UtcDaliStyledTextSplit(void)
{
  tet_infoline(" UtcDaliStyledTextSplit ");

  TestApplication application;

  END_TEST;
}
