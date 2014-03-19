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

// CLASS HEADER
#include <dali/public-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/image-attributes.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/event/images/frame-buffer-image-impl.h>
#include <dali/internal/event/images/bitmap-image-impl.h>
#include <dali/internal/event/effects/shader-effect-impl.h>

namespace Dali
{

namespace Scripting
{

bool CompareEnums(const std::string& a, const std::string& b)
{
  std::string::const_iterator ia = a.begin();
  std::string::const_iterator ib = b.begin();

  while( (ia != a.end()) && (ib != b.end()) )
  {
    char ca = *ia;
    char cb = *ib;

    if(ca == '-' || ca == '_')
    {
      ++ia;
      continue;
    }

    if(cb == '-' || cb == '_')
    {
      ++ib;
      continue;
    }

    if( 'A' <= ca && ca <= 'Z')
    {
      ca = ca + ('a' - 'A');
    }

    if( 'A' <= cb && cb <= 'Z')
    {
      cb = cb + ('a' - 'A');
    }

    if( ca != cb )
    {
      return false;
    }

    ++ia;
    ++ib;
  }

  if( (ia == a.end() && ib == b.end() ) )
  {
    return true;
  }
  else
  {
    return false;
  }

}


ColorMode GetColorMode( const std::string& value )
{
  ColorMode v( USE_OWN_COLOR );

  bool set = \
    SetIfEqual(value, "USE_OWN_COLOR"                , v, USE_OWN_COLOR                 ) || \
    SetIfEqual(value, "USE_PARENT_COLOR"             , v, USE_PARENT_COLOR              ) || \
    SetIfEqual(value, "USE_OWN_MULTIPLY_PARENT_COLOR", v, USE_OWN_MULTIPLY_PARENT_COLOR);

  if( !set )
  {
    DALI_ASSERT_ALWAYS( !"Unknown Color mode" );
  }

  return v;
}


PositionInheritanceMode GetPositionInheritanceMode( const std::string& value )
{
  PositionInheritanceMode v(INHERIT_PARENT_POSITION);

  bool set = \
    SetIfEqual(value, "INHERIT_PARENT_POSITION", v, INHERIT_PARENT_POSITION                                    ) || \
    SetIfEqual(value, "USE_PARENT_POSITION"    , v, USE_PARENT_POSITION                                        ) || \
    SetIfEqual(value, "USE_PARENT_POSITION_PLUS_LOCAL_POSITION", v, USE_PARENT_POSITION_PLUS_LOCAL_POSITION    ) || \
    SetIfEqual(value, "DONT_INHERIT_POSITION"  , v, DONT_INHERIT_POSITION);

  if( !set )
  {
    DALI_ASSERT_ALWAYS( !"Unknown position inheritance mode" );
  }

  return v;
}


DrawMode::Type GetDrawMode( const std::string& value )
{
  DrawMode::Type e(DrawMode::NORMAL);

  bool set = \
    SetIfEqual(value, "NORMAL", e, DrawMode::NORMAL  ) || \
    SetIfEqual(value, "OVERLAY",e, DrawMode::OVERLAY ) || \
    SetIfEqual(value, "STENCIL",e, DrawMode::STENCIL);

  if(!set)
  {
    DALI_ASSERT_ALWAYS( !"Unknown DrawMode type" );
  }

  return e;
}


Vector3 GetAnchorConstant( const std::string& value )
{
  Vector3 v = ParentOrigin::CENTER; // a Vector3x

  bool set =
    SetIfEqual( value, "BACK_TOP_LEFT",        v, ParentOrigin::BACK_TOP_LEFT      ) ||
    SetIfEqual( value, "BACK_TOP_CENTER",      v, ParentOrigin::BACK_TOP_CENTER    ) ||
    SetIfEqual( value, "BACK_TOP_RIGHT",       v, ParentOrigin::BACK_TOP_RIGHT     ) ||
    SetIfEqual( value, "BACK_CENTER_LEFT",     v, ParentOrigin::BACK_CENTER_LEFT   ) ||
    SetIfEqual( value, "BACK_CENTER",          v, ParentOrigin::BACK_CENTER        ) ||
    SetIfEqual( value, "BACK_CENTER_RIGHT",    v, ParentOrigin::BACK_CENTER_RIGHT  ) ||
    SetIfEqual( value, "BACK_BOTTOM_LEFT",     v, ParentOrigin::BACK_BOTTOM_LEFT   ) ||
    SetIfEqual( value, "BACK_BOTTOM_CENTER",   v, ParentOrigin::BACK_BOTTOM_CENTER ) ||
    SetIfEqual( value, "BACK_BOTTOM_RIGHT",    v, ParentOrigin::BACK_BOTTOM_RIGHT  ) ||
    SetIfEqual( value, "TOP_LEFT",             v, ParentOrigin::TOP_LEFT           ) ||
    SetIfEqual( value, "TOP_CENTER",           v, ParentOrigin::TOP_CENTER         ) ||
    SetIfEqual( value, "TOP_RIGHT",            v, ParentOrigin::TOP_RIGHT          ) ||
    SetIfEqual( value, "CENTER_LEFT",          v, ParentOrigin::CENTER_LEFT        ) ||
    SetIfEqual( value, "CENTER",               v, ParentOrigin::CENTER             ) ||
    SetIfEqual( value, "CENTER_RIGHT",         v, ParentOrigin::CENTER_RIGHT       ) ||
    SetIfEqual( value, "BOTTOM_LEFT",          v, ParentOrigin::BOTTOM_LEFT        ) ||
    SetIfEqual( value, "BOTTOM_CENTER",        v, ParentOrigin::BOTTOM_CENTER      ) ||
    SetIfEqual( value, "BOTTOM_RIGHT",         v, ParentOrigin::BOTTOM_RIGHT       ) ||
    SetIfEqual( value, "FRONT_TOP_LEFT",       v, ParentOrigin::FRONT_TOP_LEFT     ) ||
    SetIfEqual( value, "FRONT_TOP_CENTER",     v, ParentOrigin::FRONT_TOP_CENTER   ) ||
    SetIfEqual( value, "FRONT_TOP_RIGHT",      v, ParentOrigin::FRONT_TOP_RIGHT    ) ||
    SetIfEqual( value, "FRONT_CENTER_LEFT",    v, ParentOrigin::FRONT_CENTER_LEFT  ) ||
    SetIfEqual( value, "FRONT_CENTER",         v, ParentOrigin::FRONT_CENTER       ) ||
    SetIfEqual( value, "FRONT_CENTER_RIGHT",   v, ParentOrigin::FRONT_CENTER_RIGHT ) ||
    SetIfEqual( value, "FRONT_BOTTOM_LEFT",    v, ParentOrigin::FRONT_BOTTOM_LEFT  ) ||
    SetIfEqual( value, "FRONT_BOTTOM_CENTER",  v, ParentOrigin::FRONT_BOTTOM_CENTER) ||
    SetIfEqual( value, "FRONT_BOTTOM_RIGHT",   v, ParentOrigin::FRONT_BOTTOM_RIGHT );

  if(!set)
  {
    DALI_ASSERT_ALWAYS(!"Unknown Parent origin Constant" );
  }

  return v;
}


Image NewImage( const Property::Value& map )
{
  Image ret;

  std::string filename;
  Image::LoadPolicy loadPolicy       = Dali::Internal::ImageLoadPolicyDefault;
  Image::ReleasePolicy releasePolicy = Dali::Internal::ImageReleasePolicyDefault;
  ImageAttributes attributes         = ImageAttributes::New();

  if( Property::MAP == map.GetType() )
  {
    std::string field = "filename";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::STRING && "Image filename property is not a string" );
      filename = map.GetValue(field).Get<std::string>();
    }

    field = "load-policy";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::STRING && "Image load-policy property is not a string" );
      std::string v(map.GetValue(field).Get<std::string>());
      if(v == "IMMEDIATE")
      {
        loadPolicy = Dali::Image::Immediate;
      }
      else if(v == "ON_DEMAND")
      {
        loadPolicy = Dali::Image::OnDemand;
      }
      else
      {
        DALI_ASSERT_ALWAYS(!"Unknown image load-policy");
      }
    }

    field = "release-policy";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::STRING && "Image release-policy property is not a string" );
      std::string v(map.GetValue(field).Get<std::string>());
      if(v == "UNUSED")
      {
        releasePolicy = Dali::Image::Unused;
      }
      else if(v == "NEVER")
      {
        releasePolicy = Dali::Image::Never;
      }
      else
      {
        DALI_ASSERT_ALWAYS(!"Unknown image release-policy");
      }
    }

    if( map.HasKey("width") && map.HasKey("height") )
    {
      Property::Value &value = map.GetValue("width");
      unsigned int w = 0, h = 0;
      // handle floats and integer the same for json script
      if( value.GetType() == Property::FLOAT)
      {
        w = static_cast<unsigned int>( value.Get<float>() );
      }
      else
      {
        DALI_ASSERT_ALWAYS(value.GetType() == Property::INTEGER && "Image width property is not a number" );
        w = value.Get<int>();
      }

      value = map.GetValue("height");
      if( value.GetType() == Property::FLOAT)
      {
        h = static_cast<unsigned int>( value.Get<float>() );
      }
      else
      {
        DALI_ASSERT_ALWAYS(value.GetType() == Property::INTEGER && "Image width property is not a number" );
        h = value.Get<int>();
      }

      attributes.SetSize( w, h );
    }

    field = "pixel-format";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::STRING && "Image release-policy property is not a string" );

      std::string s(map.GetValue(field).Get<std::string>());

      if(s == "A8")
      {
        attributes.SetPixelFormat(Pixel::A8);
      }
      else if(s == "L8")
      {
        attributes.SetPixelFormat(Pixel::L8);
      }
      else if(s == "LA88")
      {
        attributes.SetPixelFormat(Pixel::LA88);
      }
      else if(s == "RGB565")
      {
        attributes.SetPixelFormat(Pixel::RGB565);
      }
      else if(s == "BGR565")
      {
        attributes.SetPixelFormat(Pixel::BGR565);
      }
      else if(s == "RGBA4444")
      {
        attributes.SetPixelFormat(Pixel::RGBA4444);
      }
      else if(s == "BGRA4444")
      {
        attributes.SetPixelFormat(Pixel::BGRA4444);
      }
      else if(s == "RGBA5551")
      {
        attributes.SetPixelFormat(Pixel::RGBA5551);
      }
      else if(s == "BGRA5551")
      {
        attributes.SetPixelFormat(Pixel::BGRA5551);
      }
      else if(s == "RGB888")
      {
        attributes.SetPixelFormat(Pixel::RGB888);
      }
      else if(s == "RGB8888")
      {
        attributes.SetPixelFormat(Pixel::RGB8888);
      }
      else if(s == "BGR8888")
      {
        attributes.SetPixelFormat(Pixel::BGR8888);
      }
      else if(s == "RGBA8888")
      {
        attributes.SetPixelFormat(Pixel::RGBA8888);
      }
      else if(s == "BGRA8888")
      {
        attributes.SetPixelFormat(Pixel::BGRA8888);
      }
      else
      {
        DALI_ASSERT_ALWAYS(!"Unknown image pixel format");
      }
    }

    field = "scaling-mode";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::STRING && "Image release-policy property is not a string" );

      std::string s(map.GetValue(field).Get<std::string>());

      if(s == "SHRINK_TO_FIT")
      {
        attributes.SetScalingMode(ImageAttributes::ShrinkToFit);
      }
      else if(s == "SCALE_TO_FILL")
      {
        attributes.SetScalingMode(ImageAttributes::ScaleToFill);
      }
      else if(s == "FIT_WIDTH")
      {
        attributes.SetScalingMode(ImageAttributes::FitWidth);
      }
      else if(s == "FIT_HEIGHT")
      {
        attributes.SetScalingMode(ImageAttributes::FitHeight);
      }
      else
      {
        DALI_ASSERT_ALWAYS(!"Unknown image scaling mode");
      }
    }

    field = "crop";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::VECTOR4 && "Image release-policy property is not a string" );
      Vector4 v(map.GetValue(field).Get<Vector4>());
      attributes.SetCrop( Rect<float>(v.x, v.y, v.z, v.w) );
    }

    if( map.HasKey("type") )
    {
      DALI_ASSERT_ALWAYS( map.GetValue("type").GetType() == Property::STRING );
      std::string s(map.GetValue("type").Get<std::string>());
      if("FrameBufferImage" == s)
      {
        ret = new Internal::FrameBufferImage(attributes.GetWidth(),
                                             attributes.GetHeight(),
                                             attributes.GetPixelFormat(),
                                             releasePolicy );
      }
      else if("BitMapImage" == s)
      {
        ret = new Internal::BitmapImage(attributes.GetWidth(),
                                        attributes.GetHeight(),
                                        attributes.GetPixelFormat(),
                                        loadPolicy,
                                        releasePolicy);
      }
      else if("Image" == s)
      {
        ret = Image::New(filename, attributes, loadPolicy, releasePolicy);
      }
    }
    else
    {
      ret = Image::New(filename, attributes, loadPolicy, releasePolicy);
    }
  }

  return ret;

} // Image NewImage( Property::Value map )


ShaderEffect NewShaderEffect( const Property::Value& map )
{
  Internal::ShaderEffectPtr ret;

  if( map.GetType() == Property::MAP )
  {
    ret = Internal::ShaderEffect::New(Dali::ShaderEffect::HINT_NONE); // hint can be reset if in map

    if( map.HasKey("program") )
    {
      Property::Index index = ret->GetPropertyIndex("program");
      DALI_ASSERT_DEBUG( map.GetValue("program").GetType() == Property::MAP );
      ret->SetProperty(index, map.GetValue("program"));
    }

    for(int i = 0; i < map.GetSize(); ++i)
    {
      const std::string& key = map.GetKey(i);
      if(key != "program")
      {
        Property::Index index = ret->GetPropertyIndex( key );

        if( Property::INVALID_INDEX != index )
        {
          ret->SetProperty(index, map.GetItem(i));
        }
        else
        {
          // if its not a property then register it as a uniform (making a custom property)

          if(map.GetItem(i).GetType() == Property::INTEGER)
          {
            // valid uniforms are floats, vec3's etc so we recast if the user accidentally
            // set as integer. Note the map could have come from json script.
            Property::Value asFloat( static_cast<float>( map.GetItem(i).Get<int>() ) );
            ret->SetUniform( key, asFloat, Dali::ShaderEffect::COORDINATE_TYPE_DEFAULT );
          }
          else
          {
            ret->SetUniform( key, map.GetItem(i), Dali::ShaderEffect::COORDINATE_TYPE_DEFAULT );
          }
        }
      }
    }
  }

  return Dali::ShaderEffect(ret.Get());
}


Actor NewActor( const Property::Map& map )
{
  BaseHandle handle;

  const Property::Map::const_iterator endIter = map.end();

  // First find type and create Actor
  Property::Map::const_iterator typeIter = map.begin();
  for (; typeIter != endIter; ++typeIter )
  {
    if ( typeIter->first == "type" )
    {
      TypeInfo type = TypeRegistry::Get().GetTypeInfo( typeIter->second.Get< std::string >() );
      if ( type )
      {
        handle = type.CreateInstance();
      }
      break;
    }
  }

  if ( !handle )
  {
    DALI_LOG_ERROR( "Actor type not provided, returning empty handle" );
    return Actor();
  }

  Actor actor( Actor::DownCast( handle ) );

  if ( actor )
  {
    // Now set the properties, or create children
    for ( Property::Map::const_iterator iter = map.begin(); iter != endIter; ++iter )
    {
      if ( iter == typeIter )
      {
        continue;
      }

      if ( iter->first == "actors" )
      {
        // Create children

        Property::Array actorArray = iter->second.Get< Property::Array >();
        for ( Property::Array::iterator arrayIter = actorArray.begin(), arrayEndIter = actorArray.end(); arrayIter != arrayEndIter; ++arrayIter )
        {
          actor.Add( NewActor( arrayIter->Get< Property::Map >() ) );
        }
      }
      else if ( iter->first == "signals" )
      {
        DALI_LOG_ERROR( "signals not supported" );
      }
      else if( iter->first ==  "parent-origin" )
      {
        // Parent Origin can be a string constant as well as a Vector3

        const Property::Type type( iter->second.GetType() );
        if ( type == Property::VECTOR3 )
        {
          actor.SetParentOrigin( iter->second.Get< Vector3 >() );
        }
        else if( type == Property::STRING )
        {
          actor.SetParentOrigin( GetAnchorConstant( iter->second.Get< std::string >() ) );
        }
      }
      else if( iter->first ==  "anchor-point" )
      {
        // Anchor Point can be a string constant as well as a Vector3

        const Property::Type type( iter->second.GetType() );
        if ( type == Property::VECTOR3 )
        {
          actor.SetAnchorPoint( iter->second.Get< Vector3 >() );
        }
        else if( type == Property::STRING )
        {
          actor.SetAnchorPoint( GetAnchorConstant( iter->second.Get< std::string >() ) );
        }
      }
      else if( iter->first == "opacity" )
      {
        actor.SetOpacity( iter->second.Get< float >() );
      }
      else if( iter->first == "color-mode" )
      {
        actor.SetColorMode( GetColorMode( iter->second.Get< std::string >() ) );
      }
      else if( iter->first == "inherit-shader-effect")
      {
        actor.SetInheritShaderEffect( iter->second.Get< bool >() );
      }
      else if( iter->first == "sensitive" )
      {
        actor.SetSensitive( iter->second.Get< bool >() );
      }
      else if( iter->first == "leave-required" )
      {
        actor.SetLeaveRequired( iter->second.Get< bool >() );
      }
      else if( iter->first == "position-inheritance" )
      {
        actor.SetPositionInheritanceMode( GetPositionInheritanceMode( iter->second.Get< std::string >() ) );
      }
      else if( iter->first == "draw-mode" )
      {
        actor.SetDrawMode( GetDrawMode( iter->second.Get< std::string >() ) );
      }
      else if( iter->first == "inherit-rotation" )
      {
        actor.SetInheritRotation( iter->second.Get< bool >() );
      }
      else if( iter->first == "inherit-scale" )
      {
        actor.SetInheritScale( iter->second.Get< bool >() );
      }
      else
      {
        Property::Index index( actor.GetPropertyIndex( iter->first ) );

        if ( index != Property::INVALID_INDEX )
        {
          actor.SetProperty( index, iter->second );
        }
      }
    }
  }

  return actor;
}


} // namespace scripting

} // namespace Dali




