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

namespace
{

// Helpers for converting strings to the enumerations and vice versa

template< typename T >
struct StringEnum
{
  const std::string string;
  const T value;
};

const StringEnum< ColorMode > COLOR_MODE_TABLE[] =
{
  { "USE_OWN_COLOR",                    USE_OWN_COLOR                    },
  { "USE_PARENT_COLOR",                 USE_PARENT_COLOR                 },
  { "USE_OWN_MULTIPLY_PARENT_COLOR",    USE_OWN_MULTIPLY_PARENT_COLOR    },
  { "USE_OWN_MULTIPLY_PARENT_ALPHA",    USE_OWN_MULTIPLY_PARENT_ALPHA    },
};
const unsigned int COLOR_MODE_TABLE_COUNT = sizeof( COLOR_MODE_TABLE ) / sizeof( COLOR_MODE_TABLE[0] );

const StringEnum< PositionInheritanceMode > POSITION_INHERITANCE_MODE_TABLE[] =
{
  { "INHERIT_PARENT_POSITION",                    INHERIT_PARENT_POSITION                    },
  { "USE_PARENT_POSITION",                        USE_PARENT_POSITION                        },
  { "USE_PARENT_POSITION_PLUS_LOCAL_POSITION",    USE_PARENT_POSITION_PLUS_LOCAL_POSITION    },
  { "DONT_INHERIT_POSITION",                      DONT_INHERIT_POSITION                      },
};
const unsigned int POSITION_INHERITANCE_MODE_TABLE_COUNT = sizeof( POSITION_INHERITANCE_MODE_TABLE ) / sizeof( POSITION_INHERITANCE_MODE_TABLE[0] );

const StringEnum< DrawMode::Type > DRAW_MODE_TABLE[] =
{
  { "NORMAL",     DrawMode::NORMAL     },
  { "OVERLAY",    DrawMode::OVERLAY    },
  { "STENCIL",    DrawMode::STENCIL    },
};
const unsigned int DRAW_MODE_TABLE_COUNT = sizeof( DRAW_MODE_TABLE ) / sizeof( DRAW_MODE_TABLE[0] );

const StringEnum< Vector3 > ANCHOR_CONSTANT_TABLE[] =
{
  { "BACK_TOP_LEFT",          ParentOrigin::BACK_TOP_LEFT          },
  { "BACK_TOP_CENTER",        ParentOrigin::BACK_TOP_CENTER        },
  { "BACK_TOP_RIGHT",         ParentOrigin::BACK_TOP_RIGHT         },
  { "BACK_CENTER_LEFT",       ParentOrigin::BACK_CENTER_LEFT       },
  { "BACK_CENTER",            ParentOrigin::BACK_CENTER            },
  { "BACK_CENTER_RIGHT",      ParentOrigin::BACK_CENTER_RIGHT      },
  { "BACK_BOTTOM_LEFT",       ParentOrigin::BACK_BOTTOM_LEFT       },
  { "BACK_BOTTOM_CENTER",     ParentOrigin::BACK_BOTTOM_CENTER     },
  { "BACK_BOTTOM_RIGHT",      ParentOrigin::BACK_BOTTOM_RIGHT      },
  { "TOP_LEFT",               ParentOrigin::TOP_LEFT               },
  { "TOP_CENTER",             ParentOrigin::TOP_CENTER             },
  { "TOP_RIGHT",              ParentOrigin::TOP_RIGHT              },
  { "CENTER_LEFT",            ParentOrigin::CENTER_LEFT            },
  { "CENTER",                 ParentOrigin::CENTER                 },
  { "CENTER_RIGHT",           ParentOrigin::CENTER_RIGHT           },
  { "BOTTOM_LEFT",            ParentOrigin::BOTTOM_LEFT            },
  { "BOTTOM_CENTER",          ParentOrigin::BOTTOM_CENTER          },
  { "BOTTOM_RIGHT",           ParentOrigin::BOTTOM_RIGHT           },
  { "FRONT_TOP_LEFT",         ParentOrigin::FRONT_TOP_LEFT         },
  { "FRONT_TOP_CENTER",       ParentOrigin::FRONT_TOP_CENTER       },
  { "FRONT_TOP_RIGHT",        ParentOrigin::FRONT_TOP_RIGHT        },
  { "FRONT_CENTER_LEFT",      ParentOrigin::FRONT_CENTER_LEFT      },
  { "FRONT_CENTER",           ParentOrigin::FRONT_CENTER           },
  { "FRONT_CENTER_RIGHT",     ParentOrigin::FRONT_CENTER_RIGHT     },
  { "FRONT_BOTTOM_LEFT",      ParentOrigin::FRONT_BOTTOM_LEFT      },
  { "FRONT_BOTTOM_CENTER",    ParentOrigin::FRONT_BOTTOM_CENTER    },
  { "FRONT_BOTTOM_RIGHT",     ParentOrigin::FRONT_BOTTOM_RIGHT     },
};
const unsigned int ANCHOR_CONSTANT_TABLE_COUNT = sizeof( ANCHOR_CONSTANT_TABLE ) / sizeof( ANCHOR_CONSTANT_TABLE[0] );

const StringEnum< Image::LoadPolicy > IMAGE_LOAD_POLICY_TABLE[] =
{
  { "IMMEDIATE", Image::Immediate },
  { "ON_DEMAND", Image::OnDemand  },
};
const unsigned int IMAGE_LOAD_POLICY_TABLE_COUNT = sizeof( IMAGE_LOAD_POLICY_TABLE ) / sizeof( IMAGE_LOAD_POLICY_TABLE[0] );

const StringEnum< Image::ReleasePolicy > IMAGE_RELEASE_POLICY_TABLE[] =
{
  { "UNUSED", Image::Unused },
  { "NEVER",  Image::Never  },
};
const unsigned int IMAGE_RELEASE_POLICY_TABLE_COUNT = sizeof( IMAGE_RELEASE_POLICY_TABLE ) / sizeof( IMAGE_RELEASE_POLICY_TABLE[0] );

const StringEnum< Pixel::Format > PIXEL_FORMAT_TABLE[] =
{
  { "A8",                                           Pixel::A8                                           },
  { "L8",                                           Pixel::L8                                           },
  { "LA88",                                         Pixel::LA88                                         },
  { "RGB565",                                       Pixel::RGB565                                       },
  { "BGR565",                                       Pixel::BGR565                                       },
  { "RGBA4444",                                     Pixel::RGBA4444                                     },
  { "BGRA4444",                                     Pixel::BGRA4444                                     },
  { "RGBA5551",                                     Pixel::RGBA5551                                     },
  { "BGRA5551",                                     Pixel::BGRA5551                                     },
  { "RGB888",                                       Pixel::RGB888                                       },
  { "RGB8888",                                      Pixel::RGB8888                                      },
  { "BGR8888",                                      Pixel::BGR8888                                      },
  { "RGBA8888",                                     Pixel::RGBA8888                                     },
  { "BGRA8888",                                     Pixel::BGRA8888                                     },
  { "COMPRESSED_R11_EAC",                           Pixel::COMPRESSED_R11_EAC                           },
  { "COMPRESSED_SIGNED_R11_EAC",                    Pixel::COMPRESSED_SIGNED_R11_EAC                    },
  { "COMPRESSED_RG11_EAC",                          Pixel::COMPRESSED_RG11_EAC                          },
  { "COMPRESSED_SIGNED_RG11_EAC",                   Pixel::COMPRESSED_SIGNED_RG11_EAC                   },
  { "COMPRESSED_RGB8_ETC2",                         Pixel::COMPRESSED_RGB8_ETC2                         },
  { "COMPRESSED_SRGB8_ETC2",                        Pixel::COMPRESSED_SRGB8_ETC2                        },
  { "COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2",     Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2     },
  { "COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2",    Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2    },
  { "COMPRESSED_RGBA8_ETC2_EAC",                    Pixel::COMPRESSED_RGBA8_ETC2_EAC                    },
  { "COMPRESSED_SRGB8_ALPHA8_ETC2_EAC",             Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC             },
  { "COMPRESSED_RGB8_ETC1",                         Pixel::COMPRESSED_RGB8_ETC1                         },
  { "COMPRESSED_RGB_PVRTC_4BPPV1",                  Pixel::COMPRESSED_RGB_PVRTC_4BPPV1                  },
};
const unsigned int PIXEL_FORMAT_TABLE_COUNT = sizeof( PIXEL_FORMAT_TABLE ) / sizeof( PIXEL_FORMAT_TABLE[0] );

const StringEnum< ImageAttributes::ScalingMode > IMAGE_SCALING_MODE_TABLE[] =
{
  { "SHRINK_TO_FIT", ImageAttributes::ShrinkToFit },
  { "SCALE_TO_FILL", ImageAttributes::ScaleToFill },
  { "FIT_WIDTH",     ImageAttributes::FitWidth    },
  { "FIT_HEIGHT",    ImageAttributes::FitHeight   },
};
const unsigned int IMAGE_SCALING_MODE_TABLE_COUNT = sizeof( IMAGE_SCALING_MODE_TABLE ) / sizeof( IMAGE_SCALING_MODE_TABLE[0] );

template< typename T >
T GetEnumeration( const std::string& value, const StringEnum< T >* table, const unsigned int tableCount )
{
  T v;
  bool set( false );

  for ( unsigned int i = 0; ( i < tableCount ) && ( !set ); ++i )
  {
    set = SetIfEqual(value, table->string, v, table->value );
    ++table;
  }

  if ( !set )
  {
    DALI_ASSERT_ALWAYS( !"Unknown enumeration string" );
  }

  return v;
}

template< typename T >
std::string GetEnumerationName( const T& value, const StringEnum< T >* table, const unsigned int tableCount )
{
  std::string v;

  for ( unsigned int i = 0; i < tableCount; ++i )
  {
    if ( value == table[i].value )
    {
      v = table[i].string;
      break;
    }
  }

  return v;
}

} // unnamed namespace

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
  return GetEnumeration< ColorMode >( value, COLOR_MODE_TABLE, COLOR_MODE_TABLE_COUNT );
}


PositionInheritanceMode GetPositionInheritanceMode( const std::string& value )
{
  return GetEnumeration< PositionInheritanceMode >( value, POSITION_INHERITANCE_MODE_TABLE, POSITION_INHERITANCE_MODE_TABLE_COUNT );
}


DrawMode::Type GetDrawMode( const std::string& value )
{
  return GetEnumeration< DrawMode::Type >( value, DRAW_MODE_TABLE, DRAW_MODE_TABLE_COUNT );
}


Vector3 GetAnchorConstant( const std::string& value )
{
  return GetEnumeration< Vector3 >( value, ANCHOR_CONSTANT_TABLE, ANCHOR_CONSTANT_TABLE_COUNT );
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
      loadPolicy = GetEnumeration< Image::LoadPolicy >( v, IMAGE_LOAD_POLICY_TABLE, IMAGE_LOAD_POLICY_TABLE_COUNT );
    }

    field = "release-policy";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::STRING && "Image release-policy property is not a string" );
      std::string v(map.GetValue(field).Get<std::string>());
      releasePolicy = GetEnumeration< Image::ReleasePolicy >( v, IMAGE_RELEASE_POLICY_TABLE, IMAGE_RELEASE_POLICY_TABLE_COUNT );
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
      attributes.SetPixelFormat( GetEnumeration< Pixel::Format >( s, PIXEL_FORMAT_TABLE, PIXEL_FORMAT_TABLE_COUNT ));
    }

    field = "scaling-mode";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::STRING && "Image release-policy property is not a string" );
      std::string s(map.GetValue(field).Get<std::string>());
      attributes.SetScalingMode( GetEnumeration< ImageAttributes::ScalingMode >( s, IMAGE_SCALING_MODE_TABLE, IMAGE_SCALING_MODE_TABLE_COUNT ) );
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
      else if("BitmapImage" == s)
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
      else
      {
        DALI_ASSERT_ALWAYS( !"Unknown image type" );
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

void CreatePropertyMap( Actor actor, Property::Map& map )
{
  map.clear();

  if ( actor )
  {
    map.push_back( Property::StringValuePair( "type", Property::Value( actor.GetTypeName() ) ) );

    // Default properties
    Property::IndexContainer indices;
    actor.GetPropertyIndices( indices );
    const Property::IndexContainer::const_iterator endIter = indices.end();
    for ( Property::IndexContainer::iterator iter = indices.begin(); iter != endIter; ++iter )
    {
      map.push_back( Property::StringValuePair( actor.GetPropertyName( *iter ), actor.GetProperty( *iter ) ) );
    }

    // Non-properties
    map.push_back( Property::StringValuePair( "opacity", actor.GetCurrentOpacity() ) );
    map.push_back( Property::StringValuePair( "color-mode", GetEnumerationName< ColorMode >( actor.GetColorMode(), COLOR_MODE_TABLE, COLOR_MODE_TABLE_COUNT ) ) );
    map.push_back( Property::StringValuePair( "inherit-shader-effect", actor.GetInheritShaderEffect() ) );
    map.push_back( Property::StringValuePair( "sensitive", actor.IsSensitive() ) );
    map.push_back( Property::StringValuePair( "leave-required", actor.GetLeaveRequired() ) );
    map.push_back( Property::StringValuePair( "position-inheritance", GetEnumerationName< PositionInheritanceMode >( actor.GetPositionInheritanceMode(), POSITION_INHERITANCE_MODE_TABLE, POSITION_INHERITANCE_MODE_TABLE_COUNT ) ) );
    map.push_back( Property::StringValuePair( "draw-mode", GetEnumerationName< DrawMode::Type >( actor.GetDrawMode(), DRAW_MODE_TABLE, DRAW_MODE_TABLE_COUNT ) ) );
    map.push_back( Property::StringValuePair( "inherit-rotation", actor.IsRotationInherited() ) );
    map.push_back( Property::StringValuePair( "inherit-scale", actor.IsScaleInherited() ) );

    // Children
    unsigned int childCount( actor.GetChildCount() );
    if ( childCount )
    {
      Property::Array childArray;
      for ( unsigned int child = 0; child < childCount; ++child )
      {
        Property::Map childMap;
        CreatePropertyMap( actor.GetChildAt( child ), childMap );
        childArray.push_back( childMap );
      }
      map.push_back( Property::StringValuePair( "actors", childArray ) );
    }
  }
}

} // namespace scripting

} // namespace Dali




