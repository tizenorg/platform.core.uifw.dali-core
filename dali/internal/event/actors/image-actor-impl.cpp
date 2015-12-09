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

// CLASS HEADER
#include <dali/internal/event/actors/image-actor-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/public-api/animation/constraints.h> // for EqualToConstraint
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/effects/shader-effect-impl.h>
#include <dali/internal/event/images/image-connector.h>
#include <dali/internal/event/images/nine-patch-image-impl.h>

namespace Dali
{

namespace Internal
{

namespace
{

// Properties

//              Name           Type   writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "pixelArea",    RECTANGLE, true,    false,   true,    Dali::ImageActor::Property::PIXEL_AREA )
DALI_PROPERTY( "style",        STRING,    true,    false,   true,    Dali::ImageActor::Property::STYLE      )
DALI_PROPERTY( "border",       VECTOR4,   true,    false,   true,    Dali::ImageActor::Property::BORDER     )
DALI_PROPERTY( "image",        MAP,       true,    false,   false,   Dali::ImageActor::Property::IMAGE      )
DALI_PROPERTY_TABLE_END( DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX )

BaseHandle Create()
{
  return Dali::ImageActor::New();
}

TypeRegistration mType( typeid( Dali::ImageActor ), typeid( Dali::Actor ), Create );

struct GridVertex
{
  Vector3 mPosition;
  Vector2 mTextureCoord;
};

GeometryPtr CreateGeometry( unsigned int gridWidth, unsigned int gridHeight )
{
  // Create vertices
  std::vector< Vector2 > vertices;
  vertices.reserve( ( gridWidth + 1 ) * ( gridHeight + 1 ) );

  for( unsigned int y = 0u; y < gridHeight + 1; ++y )
  {
    float yPos = (float)y / gridHeight;
    for( unsigned int x = 0u; x < gridWidth + 1; ++x )
    {
      float xPos = (float)x / gridWidth;
      vertices.push_back( Vector2( xPos - 0.5f, yPos - 0.5f ) );
    }
  }

  // Create indices
  Vector< unsigned int > indices;
  indices.Reserve( ( gridWidth + 2 ) * gridHeight * 2 - 2);

  for( unsigned int row = 0u; row < gridHeight; ++row )
  {
    unsigned int rowStartIndex = row*(gridWidth+1u);
    unsigned int nextRowStartIndex = rowStartIndex + gridWidth +1u;

    if( row != 0u ) // degenerate index on non-first row
    {
      indices.PushBack( rowStartIndex );
    }

    for( unsigned int column = 0u; column < gridWidth+1u; column++) // main strip
    {
      indices.PushBack( rowStartIndex + column);
      indices.PushBack( nextRowStartIndex + column);
    }

    if( row != gridHeight-1u ) // degenerate index on non-last row
    {
      indices.PushBack( nextRowStartIndex + gridWidth );
    }
  }


  Property::Map vertexFormat;
  vertexFormat[ "aPosition" ] = Property::VECTOR2;
  PropertyBufferPtr vertexPropertyBuffer = PropertyBuffer::New();
  vertexPropertyBuffer->SetFormat( vertexFormat );
  vertexPropertyBuffer->SetSize( vertices.size() );
  if( vertices.size() > 0 )
  {
    vertexPropertyBuffer->SetData( &vertices[ 0 ] );
  }

  Property::Map indexFormat;
  indexFormat[ "indices" ] = Property::INTEGER;
  PropertyBufferPtr indexPropertyBuffer = PropertyBuffer::New();
  indexPropertyBuffer->SetFormat( indexFormat );
  indexPropertyBuffer->SetSize( indices.Size() );
  if( indices.Size() > 0 )
  {
    indexPropertyBuffer->SetData( &indices[ 0 ] );
  }

  // Create the geometry object
  GeometryPtr geometry = Geometry::New();
  geometry->AddVertexBuffer( *vertexPropertyBuffer );
  geometry->SetIndexBuffer( *indexPropertyBuffer );
  geometry->SetGeometryType( Dali::Geometry::TRIANGLE_STRIP );

  return geometry;

}

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  varying mediump vec2 vTexCoord;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  uniform mediump vec4 sTextureRect;\n
  \n
  void main()\n
  {\n
    gl_Position = uMvpMatrix * vec4(aPosition*uSize.xy, 0.0, 1.0);\n
    vTexCoord = mix( sTextureRect.xy, sTextureRect.zw, aPosition + vec2(0.5));\n
  }\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
  }\n
);

const size_t INVALID_TEXTURE_ID = (size_t)-1;
const int INVALID_RENDERER_ID = -1;
const uint16_t MAXIMUM_GRID_SIZE = 2048;
}

ImageActorPtr ImageActor::New()
{
  ImageActorPtr actor( new ImageActor );

  // Second-phase construction of base class
  actor->Initialize();

  //Create the renderer
  actor->mRenderer = Renderer::New();

  GeometryPtr quad  = CreateGeometry( 1u, 1u );
  actor->mRenderer->SetGeometry( *quad );

  ShaderPtr shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER, Dali::Shader::HINT_NONE );
  MaterialPtr material = Material::New();
  material->SetShader( *shader );
  actor->mRenderer->SetMaterial( *material );

  return actor;
}

void ImageActor::OnInitialize()
{
  // TODO: Remove this, at the moment its needed for size negotiation to work
  SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
}

void ImageActor::SetImage( ImagePtr& image )
{
  if( !image )
  {
    if( mRendererIndex != INVALID_RENDERER_ID )
    {
      RemoveRenderer( mRendererIndex );
      mRendererIndex = INVALID_RENDERER_ID;
    }
  }
  else
  {
    SamplerPtr sampler = Sampler::New();
    sampler->SetFilterMode( mMinFilter, mMagFilter );

    mTextureIndex = mRenderer->GetMaterial()->AddTexture( image, "sTexture", sampler );

    if( mRendererIndex == INVALID_RENDERER_ID )
    {
      mRendererIndex = AddRenderer( *mRenderer );
    }

    if( !mIsPixelAreaSet )
    {
      mPixelArea = PixelArea( 0, 0, image->GetWidth(), image->GetHeight() );
    }

    RelayoutRequest();
    UpdateTexureRect();
  }
}

ImagePtr ImageActor::GetImage() const
{
  return mRenderer->GetMaterial()->GetTexture( mTextureIndex );
}

void ImageActor::SetPixelArea( const PixelArea& pixelArea )
{
  mPixelArea = pixelArea;
  mIsPixelAreaSet = true;

  RelayoutRequest();
  UpdateTexureRect();
}

const ImageActor::PixelArea& ImageActor::GetPixelArea() const
{
  return mPixelArea;
}

bool ImageActor::IsPixelAreaSet() const
{
  return mIsPixelAreaSet;
}

void ImageActor::ClearPixelArea()
{
  mIsPixelAreaSet = false;

  int imageWidth = 0;
  int imageHeight = 0;
  ImagePtr image = GetImage();
  if( image )
  {
    imageWidth = image->GetWidth();
    imageHeight = image->GetHeight();
  }

  mPixelArea = PixelArea( 0, 0, imageWidth, imageHeight );

  RelayoutRequest();
  UpdateTexureRect();
}

ImageActor::ImageActor()
: Actor( Actor::BASIC ),
  mGridSize( 1u, 1u ),
  mRendererIndex( INVALID_RENDERER_ID ),
  mTextureIndex( INVALID_TEXTURE_ID ),
  mEffectTextureIndex( INVALID_TEXTURE_ID ),
  mMinFilter( FilterMode::DEFAULT ),
  mMagFilter( FilterMode::DEFAULT ),
  mIsPixelAreaSet( false )
{
}

ImageActor::~ImageActor()
{
}

Vector3 ImageActor::GetNaturalSize() const
{
  Vector2 naturalSize( CalculateNaturalSize() );
  return Vector3( naturalSize.width, naturalSize.height, 0.f );
}

Vector2 ImageActor::CalculateNaturalSize() const
{
  // if no image then natural size is 0
  Vector2 size( 0.0f, 0.0f );

  ImagePtr image = GetImage();
  if( image )
  {
    if( IsPixelAreaSet() )
    {
      PixelArea area(GetPixelArea());
      size.width = area.width;
      size.height = area.height;
    }
    else
    {
      size = image->GetNaturalSize();
    }
  }

  return size;
}

void ImageActor::UpdateGeometry()
{
  uint16_t gridWidth = 1u;
  uint16_t gridHeight = 1u;

  if( mShaderEffect )
  {
    Vector2 gridSize = mShaderEffect->GetGridSize( Vector2(mPixelArea.width, mPixelArea.height) );

    //limit the grid size
    gridWidth = std::min( MAXIMUM_GRID_SIZE, static_cast<uint16_t>(gridSize.width) );
    gridHeight = std::min( MAXIMUM_GRID_SIZE, static_cast<uint16_t>(gridSize.height) );
  }

  if( gridWidth != mGridSize.GetWidth() || gridHeight != mGridSize.GetHeight() )
  {
    mGridSize.SetWidth( gridWidth );
    mGridSize.SetHeight( gridHeight );

    GeometryPtr geometry = CreateGeometry( gridWidth, gridHeight );
    mRenderer->SetGeometry( *geometry );
  }
}
void ImageActor::UpdateTexureRect()
{
  Vector4 textureRect( 0.f, 0.f, 1.f, 1.f );

  ImagePtr image = GetImage();
  if( mIsPixelAreaSet && image )
  {
    const float uScale = 1.0f / float(image->GetWidth());
    const float vScale = 1.0f / float(image->GetHeight());
    // bottom left
    textureRect.x = uScale * float(mPixelArea.x);
    textureRect.y = vScale * float(mPixelArea.y);
    // top right
    textureRect.z  = uScale * float(mPixelArea.x + mPixelArea.width);
    textureRect.w = vScale * float(mPixelArea.y + mPixelArea.height);
  }

  Material* material = mRenderer->GetMaterial();
  material->RegisterProperty( "sTextureRect", textureRect );
}

unsigned int ImageActor::GetDefaultPropertyCount() const
{
  return Actor::GetDefaultPropertyCount() + DEFAULT_PROPERTY_COUNT;
}

void ImageActor::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  Actor::GetDefaultPropertyIndices( indices ); // Actor class properties

  indices.Reserve( indices.Size() + DEFAULT_PROPERTY_COUNT );

  int index = DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i, ++index )
  {
    indices.PushBack( index );
  }
}

bool ImageActor::IsDefaultPropertyWritable( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyWritable(index);
  }

  index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[ index ].writable;
  }

  return false;
}

bool ImageActor::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyAnimatable( index );
  }

  index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[ index ].animatable;
  }

  return false;
}

bool ImageActor::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyAConstraintInput( index );
  }

  index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[ index ].constraintInput;
  }

  return false;
}

Property::Type ImageActor::GetDefaultPropertyType( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::GetDefaultPropertyType( index );
  }

  index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[index].type;
  }

  // index out-of-bounds
  return Property::NONE;
}

const char* ImageActor::GetDefaultPropertyName( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::GetDefaultPropertyName(index);
  }

  index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[index].name;
  }

  // index out-of-bounds
  return NULL;
}

Property::Index ImageActor::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Look for name in default properties
  for( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    const Internal::PropertyDetails* property = &DEFAULT_PROPERTY_DETAILS[ i ];
    if( 0 == strcmp( name.c_str(), property->name ) ) // Don't want to convert rhs to string
    {
      index = i + DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
      break;
    }
  }

  // If not found, check in base class
  if( Property::INVALID_INDEX == index )
  {
    index = Actor::GetDefaultPropertyIndex( name );
  }
  return index;
}

void ImageActor::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    Actor::SetDefaultProperty( index, propertyValue );
  }
  else
  {
    switch(index)
    {
      case Dali::ImageActor::Property::PIXEL_AREA:
      {
        SetPixelArea(propertyValue.Get<Rect<int> >());
        break;
      }
      case Dali::ImageActor::Property::STYLE:
      {
        //not supported
        break;
      }
      case Dali::ImageActor::Property::BORDER:
      {
        //not supported
        break;
      }
      case Dali::ImageActor::Property::IMAGE:
      {
        Dali::Image img = Scripting::NewImage( propertyValue );
        if(img)
        {
          ImagePtr image( &GetImplementation(img) );
          SetImage( image );
        }
        else
        {
          DALI_LOG_WARNING("Cannot create image from property value\n");
        }
        break;
      }
      default:
      {
        DALI_LOG_WARNING("Unknown property (%d)\n", index);
        break;
      }
    } // switch(index)

  } // else
}

Property::Value ImageActor::GetDefaultProperty( Property::Index index ) const
{
  Property::Value ret;
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    ret = Actor::GetDefaultProperty( index );
  }
  else
  {
    switch( index )
    {
      case Dali::ImageActor::Property::PIXEL_AREA:
      {
        Rect<int> r = GetPixelArea();
        ret = r;
        break;
      }
      case Dali::ImageActor::Property::STYLE:
      {
        //not supported
        break;
      }
      case Dali::ImageActor::Property::BORDER:
      {
        //not supported
        break;
      }
      case Dali::ImageActor::Property::IMAGE:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( Dali::Image( GetImage().Get() ), map );
        ret = Property::Value( map );
        break;
      }
      default:
      {
        DALI_LOG_WARNING( "Unknown property (%d)\n", index );
        break;
      }
    } // switch(index)
  }

  return ret;
}

void ImageActor::SetSortModifier(float modifier)
{
  mRenderer->SetDepthIndex( modifier );
}

float ImageActor::GetSortModifier() const
{
  return mRenderer->GetDepthIndex();
}

void ImageActor::SetCullFace(CullFaceMode mode)
{
  mRenderer->GetMaterial()->SetFaceCullingMode( static_cast< Dali::Material::FaceCullingMode >( mode ) );
}

CullFaceMode ImageActor::GetCullFace() const
{
  return static_cast< CullFaceMode >( mRenderer->GetMaterial()->GetFaceCullingMode() );
}

void ImageActor::SetBlendMode( BlendingMode::Type mode )
{
  mRenderer->GetMaterial()->SetBlendMode( mode );
}

BlendingMode::Type ImageActor::GetBlendMode() const
{
  return mRenderer->GetMaterial()->GetBlendMode();
}

void ImageActor::SetBlendFunc( BlendingFactor::Type srcFactorRgba,   BlendingFactor::Type destFactorRgba )
{
  mRenderer->GetMaterial()->SetBlendFunc( srcFactorRgba, destFactorRgba, srcFactorRgba, destFactorRgba );
}

void ImageActor::SetBlendFunc( BlendingFactor::Type srcFactorRgb,   BlendingFactor::Type destFactorRgb,
                               BlendingFactor::Type srcFactorAlpha, BlendingFactor::Type destFactorAlpha )
{
  mRenderer->GetMaterial()->SetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
}

void ImageActor::GetBlendFunc( BlendingFactor::Type& srcFactorRgb,   BlendingFactor::Type& destFactorRgb,
                               BlendingFactor::Type& srcFactorAlpha, BlendingFactor::Type& destFactorAlpha ) const
{
  mRenderer->GetMaterial()->GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
}

void ImageActor::SetBlendEquation( BlendingEquation::Type equationRgba )
{
  mRenderer->GetMaterial()->SetBlendEquation( equationRgba, equationRgba );
}

void ImageActor::SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha )
{
  mRenderer->GetMaterial()->SetBlendEquation( equationRgb, equationAlpha );
}

void ImageActor::GetBlendEquation( BlendingEquation::Type& equationRgb, BlendingEquation::Type& equationAlpha ) const
{
  mRenderer->GetMaterial()->GetBlendEquation( equationRgb, equationAlpha );
}

void ImageActor::SetBlendColor( const Vector4& color )
{
  mBlendColor = color;
  mRenderer->GetMaterial()->SetBlendColor( mBlendColor );
}

const Vector4& ImageActor::GetBlendColor() const
{
  return mBlendColor;
}

void ImageActor::SetFilterMode( FilterMode::Type minFilter, FilterMode::Type magFilter )
{
  mMinFilter = minFilter;
  mMagFilter = magFilter;

  if( mTextureIndex != INVALID_TEXTURE_ID )
  {
    SamplerPtr sampler = Sampler::New();
    sampler->SetFilterMode( minFilter, magFilter );

    mRenderer->GetMaterial()->SetTextureSampler( mTextureIndex, sampler.Get() );
  }
}

void ImageActor::GetFilterMode( FilterMode::Type& minFilter, FilterMode::Type& magFilter ) const
{
  minFilter = mMinFilter;
  magFilter = mMagFilter;
}

void ImageActor::SetShaderEffect( ShaderEffect& effect )
{
  if( mShaderEffect )
  {
    mShaderEffect->Disconnect( this );
  }

  mShaderEffect = ShaderEffectPtr( &effect );
  effect.Connect( this );

  ShaderPtr shader = mShaderEffect->GetShader();
  mRenderer->GetMaterial()->SetShader( *shader );

  EffectImageUpdated();

  UpdateGeometry();
}

ShaderEffectPtr ImageActor::GetShaderEffect() const
{
  return mShaderEffect;
}

void ImageActor::RemoveShaderEffect()
{
  if( mShaderEffect )
  {
    mShaderEffect->Disconnect( this );
    // change to the standard shader and quad geometry
    ShaderPtr shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER, Dali::Shader::HINT_NONE );
    mRenderer->GetMaterial()->SetShader( *shader );
    mShaderEffect.Reset();

    UpdateGeometry();
  }
}

void ImageActor::EffectImageUpdated()
{
  if( mShaderEffect )
  {
    Dali::Image effectImage = mShaderEffect->GetEffectImage();
    if( effectImage )
    {
      Image& effectImageImpl = GetImplementation( effectImage );

      if( mEffectTextureIndex == INVALID_TEXTURE_ID )
      {
        mEffectTextureIndex = mRenderer->GetMaterial()->AddTexture( &effectImageImpl, "sEffect", NULL );
      }
      else
      {
        mRenderer->GetMaterial()->SetTextureImage( mEffectTextureIndex, &effectImageImpl );
      }
    }
    else
    {
      if( mEffectTextureIndex != INVALID_TEXTURE_ID )
      {
        mRenderer->GetMaterial()->RemoveTexture( mEffectTextureIndex );
      }
      mEffectTextureIndex = INVALID_TEXTURE_ID;
    }

  }
}

} // namespace Internal

} // namespace Dali
