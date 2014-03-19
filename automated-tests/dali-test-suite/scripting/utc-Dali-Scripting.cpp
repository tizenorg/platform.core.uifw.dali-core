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
using namespace Dali::Scripting;

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

TEST_FUNCTION( UtcDaliScriptingGetColorMode, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliScriptingGetPositionInheritanceMode, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliScriptingGetDrawMode, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliScriptingGetAnchorConstant, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliScriptingNewImageNegative, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliScriptingNewImage, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliScriptingNewShaderEffect, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliScriptingNewActorNegative, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliScriptingNewActorProperties, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliScriptingNewActorChildren, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

void UtcDaliScriptingGetColorMode()
{
  TestApplication application;

  DALI_TEST_EQUALS( Dali::USE_OWN_COLOR, GetColorMode( "USE_OWN_COLOR" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::USE_PARENT_COLOR, GetColorMode( "USE_PARENT_COLOR" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::USE_OWN_MULTIPLY_PARENT_COLOR, GetColorMode( "USE_OWN_MULTIPLY_PARENT_COLOR" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::USE_OWN_MULTIPLY_PARENT_ALPHA, GetColorMode( "USE_OWN_MULTIPLY_PARENT_ALPHA" ), TEST_LOCATION );

  try
  {
    (void)GetColorMode("INVALID_ARG");
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
  }
}

void UtcDaliScriptingGetPositionInheritanceMode()
{
  TestApplication application;

  DALI_TEST_EQUALS( Dali::INHERIT_PARENT_POSITION, GetPositionInheritanceMode( "INHERIT_PARENT_POSITION" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::USE_PARENT_POSITION, GetPositionInheritanceMode( "USE_PARENT_POSITION" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::USE_PARENT_POSITION_PLUS_LOCAL_POSITION, GetPositionInheritanceMode( "USE_PARENT_POSITION_PLUS_LOCAL_POSITION" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::DONT_INHERIT_POSITION, GetPositionInheritanceMode( "DONT_INHERIT_POSITION" ), TEST_LOCATION );

  try
  {
    (void)GetPositionInheritanceMode("INVALID_ARG");
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
  }
}

void UtcDaliScriptingGetDrawMode()
{
  TestApplication application;

  DALI_TEST_EQUALS( Dali::DrawMode::NORMAL, GetDrawMode( "NORMAL" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::DrawMode::OVERLAY, GetDrawMode( "OVERLAY" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::DrawMode::STENCIL, GetDrawMode( "STENCIL" ), TEST_LOCATION );

  try
  {
    (void)GetDrawMode("INVALID_ARG");
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
  }
}

void UtcDaliScriptingGetAnchorConstant()
{
  TestApplication application;

  DALI_TEST_EQUALS( Dali::ParentOrigin::BACK_TOP_LEFT, GetAnchorConstant( "BACK_TOP_LEFT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BACK_TOP_CENTER, GetAnchorConstant( "BACK_TOP_CENTER" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BACK_TOP_RIGHT, GetAnchorConstant( "BACK_TOP_RIGHT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BACK_CENTER_LEFT, GetAnchorConstant( "BACK_CENTER_LEFT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BACK_CENTER, GetAnchorConstant( "BACK_CENTER" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BACK_CENTER_RIGHT, GetAnchorConstant( "BACK_CENTER_RIGHT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BACK_BOTTOM_LEFT, GetAnchorConstant( "BACK_BOTTOM_LEFT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BACK_BOTTOM_CENTER, GetAnchorConstant( "BACK_BOTTOM_CENTER" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BACK_BOTTOM_RIGHT, GetAnchorConstant( "BACK_BOTTOM_RIGHT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::TOP_LEFT, GetAnchorConstant( "TOP_LEFT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::TOP_CENTER, GetAnchorConstant( "TOP_CENTER" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::TOP_RIGHT, GetAnchorConstant( "TOP_RIGHT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::CENTER_LEFT, GetAnchorConstant( "CENTER_LEFT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::CENTER, GetAnchorConstant( "CENTER" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::CENTER_RIGHT, GetAnchorConstant( "CENTER_RIGHT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BOTTOM_LEFT, GetAnchorConstant( "BOTTOM_LEFT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BOTTOM_CENTER, GetAnchorConstant( "BOTTOM_CENTER" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::BOTTOM_RIGHT, GetAnchorConstant( "BOTTOM_RIGHT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::FRONT_TOP_LEFT, GetAnchorConstant( "FRONT_TOP_LEFT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::FRONT_TOP_CENTER, GetAnchorConstant( "FRONT_TOP_CENTER" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::FRONT_TOP_RIGHT, GetAnchorConstant( "FRONT_TOP_RIGHT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::FRONT_CENTER_LEFT, GetAnchorConstant( "FRONT_CENTER_LEFT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::FRONT_CENTER, GetAnchorConstant( "FRONT_CENTER" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::FRONT_CENTER_RIGHT, GetAnchorConstant( "FRONT_CENTER_RIGHT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::FRONT_BOTTOM_LEFT, GetAnchorConstant( "FRONT_BOTTOM_LEFT" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::FRONT_BOTTOM_CENTER, GetAnchorConstant( "FRONT_BOTTOM_CENTER" ), TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::ParentOrigin::FRONT_BOTTOM_RIGHT, GetAnchorConstant( "FRONT_BOTTOM_RIGHT" ), TEST_LOCATION );

  try
  {
    (void)GetAnchorConstant("INVALID_ARG");
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
  }
}

void UtcDaliScriptingNewImageNegative()
{
  TestApplication application;

  // Invalid filename
  try
  {
    Property::Map map;
    map.push_back( Property::StringValuePair( "filename", Vector3::ZERO ) );
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "map.GetValue(field).GetType()", TEST_LOCATION );
  }

  // Invalid load-policy
  try
  {
    Property::Map map;
    map.push_back( Property::StringValuePair( "load-policy", Vector3::ZERO ) );
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "map.GetValue(field).GetType()", TEST_LOCATION );

    // Invalid value
    try
    {
      Property::Map map;
      map.push_back( Property::StringValuePair( "load-policy", "INVALID" ) );
      Image image = NewImage( map );
      tet_result( TET_FAIL );
    }
    catch ( DaliException& e )
    {
      DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
    }
  }

  // Invalid release-policy
  try
  {
    Property::Map map;
    map.push_back( Property::StringValuePair( "release-policy", Vector3::ZERO ) );
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "map.GetValue(field).GetType()", TEST_LOCATION );

    // Invalid value
    try
    {
      Property::Map map;
      map.push_back( Property::StringValuePair( "release-policy", "INVALID" ) );
      Image image = NewImage( map );
      tet_result( TET_FAIL );
    }
    catch ( DaliException& e )
    {
      DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
    }
  }

  // Invalid width
  try
  {
    Property::Map map;
    map.push_back( Property::StringValuePair( "width", "Invalid" ) );
    map.push_back( Property::StringValuePair( "height", "Invalid" ) );
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "value.GetType()", TEST_LOCATION );
  }

  // Invalid height
  try
  {
    Property::Map map;
    map.push_back( Property::StringValuePair( "width", 10 ) );
    map.push_back( Property::StringValuePair( "height", "Invalid" ) );
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "value.GetType()", TEST_LOCATION );
  }

  // Invalid pixel-format
  try
  {
    Property::Map map;
    map.push_back( Property::StringValuePair( "pixel-format", Vector3::ZERO ) );
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "map.GetValue(field).GetType()", TEST_LOCATION );

    // Invalid value
    try
    {
      Property::Map map;
      map.push_back( Property::StringValuePair( "pixel-format", "INVALID" ) );
      Image image = NewImage( map );
      tet_result( TET_FAIL );
    }
    catch ( DaliException& e )
    {
      DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
    }
  }

  // Invalid scaling-mode
  try
  {
    Property::Map map;
    map.push_back( Property::StringValuePair( "scaling-mode", Vector3::ZERO ) );
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "map.GetValue(field).GetType()", TEST_LOCATION );

    // Invalid value
    try
    {
      Property::Map map;
      map.push_back( Property::StringValuePair( "scaling-mode", "INVALID" ) );
      Image image = NewImage( map );
      tet_result( TET_FAIL );
    }
    catch ( DaliException& e )
    {
      DALI_TEST_ASSERT( e, "!\"Unknown", TEST_LOCATION );
    }
  }

  // Invalid crop
  try
  {
    Property::Map map;
    map.push_back( Property::StringValuePair( "crop", "Invalid" ) );
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "map.GetValue(field).GetType()", TEST_LOCATION );
  }

  // Invalid type
  try
  {
    Property::Map map;
    map.push_back( Property::StringValuePair( "type", Vector3::ZERO ) );
    Image image = NewImage( map );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "map.GetValue(\"type\").GetType()", TEST_LOCATION );
  }
}

//////////////////////////////////////////////////////////////////////////////
// Helpers for string to enum comparisons for Image and ImageAttributes
//////////////////////////////////////////////////////////////////////////////

struct StringEnum
{
  const char * string;
  int value;
};

/**
 * Template to check enumerations of type T, with a class of type X
 */
template< typename T, typename X >
void TestEnumStrings(
  Property::Map& map,                       // The map used to create instance of type X
  const StringEnum* values,                 // An array of string values
  unsigned int num,                         // Number of items in the array
  T ( X::*method )() const,                 // The member method of X to call to get the enum
  X ( *creator ) ( const Property::Value& ) // The method which creates an instance of type X
)
{
  Property::Map::iterator iter = map.end() - 1;
  for ( unsigned int i = 0; i < num; ++i )
  {
    iter->second = values[i].string;
    tet_printf("Checking: %s: %s\n", iter->first.c_str(), values[i].string );
    X instance = creator( map );
    DALI_TEST_EQUALS( values[i].value, ( instance.*method )(), TEST_LOCATION );
  }
}

/// Helper method to create ImageAttributes using an Image
ImageAttributes NewImageAttributes( const Property::Value& map )
{
  Image image = NewImage( map );
  return image.GetAttributes();
}

//////////////////////////////////////////////////////////////////////////////

void UtcDaliScriptingNewImage()
{
  TestApplication application;

  Property::Map map;
  map.push_back( Property::StringValuePair( "filename", "TEST_FILE" ) );

  // Filename only
  {
    Image image = NewImage( map );
    DALI_TEST_EQUALS( "TEST_FILE", image.GetFilename(), TEST_LOCATION );
  }

  // load-policy
  map.push_back( Property::StringValuePair( "load-policy", "" ) );
  {
    const StringEnum values[] =
    {
        { "IMMEDIATE", Image::Immediate },
        { "ON_DEMAND", Image::OnDemand }
    };
    TestEnumStrings< Image::LoadPolicy, Image >( map, values, ( sizeof( values ) / sizeof ( values[0] ) ), &Image::GetLoadPolicy, &NewImage );
  }

  // release-policy
  map.push_back( Property::StringValuePair( "release-policy", "" ) );
  {
    const StringEnum values[] =
    {
        { "UNUSED", Image::Unused },
        { "NEVER", Image::Never }
    };
    TestEnumStrings< Image::ReleasePolicy, Image >( map, values, ( sizeof( values ) / sizeof ( values[0] ) ), &Image::GetReleasePolicy, &NewImage );
  }

  // float width and height
  map.push_back( Property::StringValuePair( "width", (float) 10.0f ) );
  map.push_back( Property::StringValuePair( "height", (float) 20.0f ) );
  {
    Image image = NewImage( map );
    DALI_TEST_EQUALS( image.GetWidth(), 10.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( image.GetHeight(), 20.0f, TEST_LOCATION );
  }

  // int width and height
  map.erase( map.end() - 2, map.end() );
  map.push_back( Property::StringValuePair( "width", 50 ) );
  map.push_back( Property::StringValuePair( "height", 70 ) );
  {
    Image image = NewImage( map );
    DALI_TEST_EQUALS( image.GetWidth(), 50u, TEST_LOCATION );
    DALI_TEST_EQUALS( image.GetHeight(), 70u, TEST_LOCATION );
  }

  //map.erase( map.end() - 2, map.end() );

  // pixel-format
  map.push_back( Property::StringValuePair( "pixel-format", "" ) );
  {
    const StringEnum values[] =
    {
        { "A8", Pixel::A8 },
        { "L8", Pixel::L8 },
        { "LA88", Pixel::LA88 },
        { "RGB565", Pixel::RGB565 },
        { "BGR565", Pixel::BGR565 },
        { "RGBA4444", Pixel::RGBA4444 },
        { "BGRA4444", Pixel::BGRA4444 },
        { "RGBA5551", Pixel::RGBA5551 },
        { "BGRA5551", Pixel::BGRA5551 },
        { "RGB888", Pixel::RGB888 },
        { "RGB8888", Pixel::RGB8888 },
        { "BGR8888", Pixel::BGR8888 },
        { "RGBA8888", Pixel::RGBA8888 },
        { "BGRA8888", Pixel::BGRA8888 },
        { "COMPRESSED_R11_EAC", Pixel::COMPRESSED_R11_EAC },
        { "COMPRESSED_SIGNED_R11_EAC", Pixel::COMPRESSED_SIGNED_R11_EAC },
        { "COMPRESSED_RG11_EAC", Pixel::COMPRESSED_RG11_EAC },
        { "COMPRESSED_SIGNED_RG11_EAC", Pixel::COMPRESSED_SIGNED_RG11_EAC },
        { "COMPRESSED_RGB8_ETC2", Pixel::COMPRESSED_RGB8_ETC2 },
        { "COMPRESSED_SRGB8_ETC2", Pixel::COMPRESSED_SRGB8_ETC2 },
        { "COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2", Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 },
        { "COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2", Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 },
        { "COMPRESSED_RGBA8_ETC2_EAC", Pixel::COMPRESSED_RGBA8_ETC2_EAC },
        { "COMPRESSED_SRGB8_ALPHA8_ETC2_EAC", Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC },
        { "COMPRESSED_RGB8_ETC1", Pixel::COMPRESSED_RGB8_ETC1 },
        { "COMPRESSED_RGB_PVRTC_4BPPV1", Pixel::COMPRESSED_RGB_PVRTC_4BPPV1 },
        { "A8", Pixel::A8 }, // Checked already but reset so that BitmapImage works
    };
    TestEnumStrings< Pixel::Format, ImageAttributes >( map, values, ( sizeof( values ) / sizeof ( values[0] ) ), &ImageAttributes::GetPixelFormat, &NewImageAttributes );
  }

  // scaling-mode
  map.push_back( Property::StringValuePair( "scaling-mode", "" ) );
  {
    const StringEnum values[] =
    {
        { "SHRINK_TO_FIT", ImageAttributes::ShrinkToFit },
        { "SCALE_TO_FILL", ImageAttributes::ScaleToFill },
        { "FIT_WIDTH", ImageAttributes::FitWidth },
        { "FIT_HEIGHT", ImageAttributes::FitHeight },
    };
    TestEnumStrings< ImageAttributes::ScalingMode, ImageAttributes >( map, values, ( sizeof( values ) / sizeof ( values[0] ) ), &ImageAttributes::GetScalingMode, &NewImageAttributes );
  }

  // crop
  map.push_back( Property::StringValuePair( "crop", Vector4( 50, 60, 70, 80 ) ) );
  {
    Image image = NewImage( map );
    ImageAttributes attributes = image.GetAttributes();
    Rect<float> crop = attributes.GetCrop();
    DALI_TEST_EQUALS( crop.x, 50, TEST_LOCATION );
    DALI_TEST_EQUALS( crop.y, 60, TEST_LOCATION );
    DALI_TEST_EQUALS( crop.width, 70, TEST_LOCATION );
    DALI_TEST_EQUALS( crop.height, 80, TEST_LOCATION );
  }

  // type FrameBufferImage
  map.push_back( Property::StringValuePair( "type", "FrameBufferImage" ) );
  {
    Image image = NewImage( map );
    DALI_TEST_CHECK( FrameBufferImage::DownCast( image ) );
  }
  // type BitMapImage
  (map.end() - 1)->second = "BitmapImage";
  {
    Image image = NewImage( map );
    DALI_TEST_CHECK( BitmapImage::DownCast( image ) );
  }
  // type Image
  (map.end() - 1)->second = "Image";
  {
    Image image = NewImage( map );
    DALI_TEST_CHECK( Image::DownCast( image ) );
    DALI_TEST_CHECK( !FrameBufferImage::DownCast( image ) );
    DALI_TEST_CHECK( !BitmapImage::DownCast( image ) );
  }
}

void UtcDaliScriptingNewShaderEffect()
{
  TestApplication application;

  Property::Map programMap;
  programMap.push_back( Property::StringValuePair( "vertex-filename", "bump.vert" ) );
  programMap.push_back( Property::StringValuePair( "fragment-filename", "bump.frag" ) );

  Property::Map imageMap;
  imageMap.push_back( Property::StringValuePair( "filename", "image.png" ) );

  Property::Map map;
  map.push_back( Property::StringValuePair( "image", imageMap ) );
  map.push_back( Property::StringValuePair( "program", programMap ) );
  map.push_back( Property::StringValuePair( "uLightPosition", Vector3( 0.0, 0.0, -1.5) ) );
  map.push_back( Property::StringValuePair( "uAmbientLight", (int)10 ) );

  ShaderEffect shader = NewShaderEffect( map );

  DALI_TEST_CHECK( shader );
}

void UtcDaliScriptingNewActorNegative()
{
  TestApplication application;

  // Empty map
  {
    Actor handle = NewActor( Property::Map() );
    DALI_TEST_CHECK( !handle );
  }

  // Map with only properties
  {
    Property::Map map;
    map.push_back( Property::StringValuePair( "parent-origin", ParentOrigin::TOP_CENTER ) );
    map.push_back( Property::StringValuePair( "anchor-point", AnchorPoint::TOP_CENTER ) );
    Actor handle = NewActor( map );
    DALI_TEST_CHECK( !handle );
  }

  // Add some signals to the map, we should have no signal connections as its not yet supported
  {
    Property::Map map;
    map.push_back( Property::StringValuePair( "type", "Actor" ) );
    map.push_back( Property::StringValuePair( "signals", Property::MAP ) );
    Actor handle = NewActor( map );
    DALI_TEST_CHECK( handle );
    DALI_TEST_CHECK( !handle.MouseWheelEventSignal().GetConnectionCount() );
    DALI_TEST_CHECK( !handle.OffStageSignal().GetConnectionCount() );
    DALI_TEST_CHECK( !handle.OnStageSignal().GetConnectionCount() );
    DALI_TEST_CHECK( !handle.SetSizeSignal().GetConnectionCount() );
    DALI_TEST_CHECK( !handle.TouchedSignal().GetConnectionCount() );
  }
}

void UtcDaliScriptingNewActorProperties()
{
  TestApplication application;

  Property::Map map;
  map.push_back( Property::StringValuePair( "type", "Actor" ) );
  map.push_back( Property::StringValuePair( "size", Vector3::ONE ) );
  map.push_back( Property::StringValuePair( "position", Vector3::XAXIS ) );
  map.push_back( Property::StringValuePair( "scale", Vector3::ONE ) );
  map.push_back( Property::StringValuePair( "visible", false ) );
  map.push_back( Property::StringValuePair( "color", Color::MAGENTA ) );
  map.push_back( Property::StringValuePair( "name", "MyActor" ) );

  // Default properties
  {
    Actor handle = NewActor( map );
    DALI_TEST_CHECK( handle );

    Stage::GetCurrent().Add( handle );
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( handle.GetCurrentSize(), Vector3::ONE, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetCurrentPosition(), Vector3::XAXIS, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.IsVisible(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetCurrentColor(), Color::MAGENTA, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetName(), "MyActor", TEST_LOCATION );

    Stage::GetCurrent().Remove( handle );
  }

  // Check Anchor point and parent origin vector3s
  map.push_back( Property::StringValuePair( "parent-origin", ParentOrigin::TOP_CENTER ) );
  map.push_back( Property::StringValuePair( "anchor-point", AnchorPoint::TOP_LEFT ) );
  {
    Actor handle = NewActor( map );
    DALI_TEST_CHECK( handle );

    Stage::GetCurrent().Add( handle );
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( handle.GetCurrentParentOrigin(), ParentOrigin::TOP_CENTER, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetCurrentAnchorPoint(), AnchorPoint::TOP_LEFT, TEST_LOCATION );

    Stage::GetCurrent().Remove( handle );
  }

  // Check Anchor point and parent origin STRINGS
  map.erase( map.end() - 2, map.end() ); // delete previously added parent origin and anchor point
  map.push_back( Property::StringValuePair( "parent-origin", "BACK_TOP_LEFT" ) );
  map.push_back( Property::StringValuePair( "anchor-point", "FRONT_CENTER_LEFT" ) );
  {
    Actor handle = NewActor( map );
    DALI_TEST_CHECK( handle );

    Stage::GetCurrent().Add( handle );
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( handle.GetCurrentParentOrigin(), ParentOrigin::BACK_TOP_LEFT, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetCurrentAnchorPoint(), AnchorPoint::FRONT_CENTER_LEFT, TEST_LOCATION );

    Stage::GetCurrent().Remove( handle );
  }

  // Set Non-properties
  map.push_back( Property::StringValuePair( "opacity", 0.5f ) );
  map.push_back( Property::StringValuePair( "color-mode", "USE_PARENT_COLOR" ) );
  map.push_back( Property::StringValuePair( "inherit-shader-effect", false ) );
  map.push_back( Property::StringValuePair( "sensitive", false ) );
  map.push_back( Property::StringValuePair( "leave-required", true ) );
  map.push_back( Property::StringValuePair( "position-inheritance", "DONT_INHERIT_POSITION" ) );
  map.push_back( Property::StringValuePair( "draw-mode", "STENCIL" ) );
  map.push_back( Property::StringValuePair( "inherit-rotation", false ) );
  map.push_back( Property::StringValuePair( "inherit-scale", false ) );
  {
    Actor handle = NewActor( map );
    DALI_TEST_CHECK( handle );

    Stage::GetCurrent().Add( handle );
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( handle.GetCurrentOpacity(), 0.5f, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetColorMode(), USE_PARENT_COLOR, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetInheritShaderEffect(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.IsSensitive(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetLeaveRequired(), true, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetPositionInheritanceMode(), DONT_INHERIT_POSITION, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.GetDrawMode(), DrawMode::STENCIL, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.IsRotationInherited(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handle.IsScaleInherited(), false, TEST_LOCATION );

    Stage::GetCurrent().Remove( handle );
  }
}

void UtcDaliScriptingNewActorChildren()
{
  TestApplication application;

  Property::Map map;
  map.push_back( Property::StringValuePair( "type", "Actor" ) );
  map.push_back( Property::StringValuePair( "position", Vector3::XAXIS ) );

  Property::Map child1Map;
  child1Map.push_back( Property::StringValuePair( "type", "ImageActor" ) );
  child1Map.push_back( Property::StringValuePair( "position", Vector3::YAXIS ) );

  Property::Map child2Map;
  child2Map.push_back( Property::StringValuePair( "type", "TextActor" ) );
  child2Map.push_back( Property::StringValuePair( "position", Vector3::ZAXIS ) );

  Property::Map grandChildMap;
  grandChildMap.push_back( Property::StringValuePair( "type", "LightActor" ) );
  grandChildMap.push_back( Property::StringValuePair( "position", Vector3::ONE ) );

  // Add arrays to appropriate maps
  Property::Array grandChildArray;
  grandChildArray.push_back( grandChildMap );
  Property::Array childArray;
  child1Map.push_back( Property::StringValuePair( "actors", grandChildArray ) );
  childArray.push_back( child1Map );
  childArray.push_back( child2Map );
  map.push_back( Property::StringValuePair( "actors", childArray ) );

  // Create
  Actor handle = NewActor( map );
  DALI_TEST_CHECK( handle );

  Stage::GetCurrent().Add( handle );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( handle.GetCurrentPosition(), Vector3::XAXIS, TEST_LOCATION );
  DALI_TEST_EQUALS( handle.GetChildCount(), 2u, TEST_LOCATION );

  Actor child1 = handle.GetChildAt(0);
  DALI_TEST_CHECK( child1 );
  DALI_TEST_CHECK( ImageActor::DownCast( child1 ) );
  DALI_TEST_EQUALS( child1.GetCurrentPosition(), Vector3::YAXIS, TEST_LOCATION );
  DALI_TEST_EQUALS( child1.GetChildCount(), 1u, TEST_LOCATION );

  Actor child2 = handle.GetChildAt(1);
  DALI_TEST_CHECK( child2 );
  DALI_TEST_CHECK( TextActor::DownCast( child2 ) );
  DALI_TEST_EQUALS( child2.GetCurrentPosition(), Vector3::ZAXIS, TEST_LOCATION );
  DALI_TEST_EQUALS( child2.GetChildCount(), 0u, TEST_LOCATION );

  Actor grandChild = child1.GetChildAt( 0 );
  DALI_TEST_CHECK( grandChild );
  DALI_TEST_CHECK( LightActor::DownCast( grandChild ) );
  DALI_TEST_EQUALS( grandChild.GetCurrentPosition(), Vector3::ONE, TEST_LOCATION );
  DALI_TEST_EQUALS( grandChild.GetChildCount(), 0u, TEST_LOCATION );

  Stage::GetCurrent().Remove( handle );
}
