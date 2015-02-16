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
#include <dali/internal/event/actors/text-actor-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/text/text-actor-parameters.h>
#include <dali/internal/event/actor-attachments/text-attachment-impl.h>
#include <dali/internal/event/common/property-index-ranges.h>
#include <dali/internal/event/text/font-impl.h>
#include <dali/internal/event/text/utf8-impl.h>
#include <dali/internal/event/text/text-impl.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/common/core-impl.h>

namespace Dali
{

const Property::Index TextActor::TEXT                       = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT;
const Property::Index TextActor::FONT                       = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 1;
const Property::Index TextActor::FONT_STYLE                 = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 2;
const Property::Index TextActor::OUTLINE_ENABLE             = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 3;
const Property::Index TextActor::OUTLINE_COLOR              = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 4;
const Property::Index TextActor::OUTLINE_THICKNESS_WIDTH    = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 5;
const Property::Index TextActor::SMOOTH_EDGE                = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 6;
const Property::Index TextActor::GLOW_ENABLE                = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 7;
const Property::Index TextActor::GLOW_COLOR                 = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 8;
const Property::Index TextActor::GLOW_INTENSITY             = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 9;
const Property::Index TextActor::SHADOW_ENABLE              = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 10;
const Property::Index TextActor::SHADOW_COLOR               = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 11;
const Property::Index TextActor::SHADOW_OFFSET              = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 12;
const Property::Index TextActor::ITALICS_ANGLE              = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 13;
const Property::Index TextActor::UNDERLINE                  = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 14;
const Property::Index TextActor::WEIGHT                     = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 15;
const Property::Index TextActor::FONT_DETECTION_AUTOMATIC   = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 16;
const Property::Index TextActor::GRADIENT_COLOR             = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 17;
const Property::Index TextActor::GRADIENT_START_POINT       = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 18;
const Property::Index TextActor::GRADIENT_END_POINT         = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 19;
const Property::Index TextActor::SHADOW_SIZE                = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 20;
const Property::Index TextActor::TEXT_COLOR                 = Internal::DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT + 21;

namespace
{

const char* DEFAULT_TEXT_ACTOR_PROPERTY_NAMES[] =
{
  "text",
  "font",
  "font-style",
  "outline-enable",
  "outline-color",
  "outline-thickness-width",
  "smooth-edge",
  "glow-enable",
  "glow-color",
  "glow-intensity",
  "shadow-enable",
  "shadow-color",
  "shadow-offset",
  "italics-angle",
  "underline",
  "weight",
  "font-detection-automatic",
  "gradient-color",
  "gradient-start-point",
  "gradient-end-point",
  "shadow-size",
  "text-color"
};
const int DEFAULT_TEXT_ACTOR_PROPERTY_COUNT = sizeof( DEFAULT_TEXT_ACTOR_PROPERTY_NAMES ) / sizeof( DEFAULT_TEXT_ACTOR_PROPERTY_NAMES[0] );

const Property::Type DEFAULT_TEXT_ACTOR_PROPERTY_TYPES[DEFAULT_TEXT_ACTOR_PROPERTY_COUNT] =
{
  Property::STRING,   // "text"
  Property::STRING,   // "font"
  Property::STRING,   // "font-style"
  Property::BOOLEAN,  // "outline-enable"
  Property::VECTOR4,  // "outline-color"
  Property::VECTOR2,  // "outline-thickness-width"
  Property::FLOAT,    // "smooth-edge"
  Property::BOOLEAN,  // "glow-enable"
  Property::VECTOR4,  // "glow-color"
  Property::FLOAT,    // "glow-intensity"
  Property::BOOLEAN,  // "shadow-enable"
  Property::VECTOR4,  // "shadow-color"
  Property::VECTOR2,  // "shadow-offset"
  Property::FLOAT,    // "italics-angle"
  Property::BOOLEAN,  // "underline"
  Property::INTEGER,  // "weight"
  Property::BOOLEAN,  // "font-detection-automatic"
  Property::VECTOR4,  // "gradient-color",
  Property::VECTOR2,  // "gradient-start-point",
  Property::VECTOR2,  // "gradient-end-point"
  Property::FLOAT,    // "shadow-size"
  Property::VECTOR4,  // "text-color",
};

}

namespace Internal
{

namespace
{

BaseHandle Create()
{
  return Dali::TextActor::New();
}

TypeRegistration mType( typeid(Dali::TextActor), typeid(Dali::RenderableActor), Create );

SignalConnectorType s1( mType, Dali::TextActor::SIGNAL_TEXT_LOADING_FINISHED, &TextActor::DoConnectSignal );

}

TextActorPtr TextActor::New( const Integration::TextArray& utfCodes, const TextActorParameters& parameters )
{
  // first stage construction
  TextActorPtr actor ( new TextActor( parameters.IsAutomaticFontDetectionEnabled() ) );

  const TextStyle& style = parameters.GetTextStyle();

  FontPointer fontPtr( Font::New(style.GetFontName(), style.GetFontStyle(), style.GetFontPointSize() ) );

  // Second-phase construction
  actor->Initialize();

  //create the attachment
  actor->mTextAttachment = TextAttachment::New( *actor->mStage, *actor->mNode, Integration::TextArray(), fontPtr );
  actor->Attach(*actor->mTextAttachment);

  // Note: SetTextStyle() MUST be called before SetText(), to ensure
  //       that a single ResourceRequest for the glyphs is made. Calling
  //       them in the wrong order will issue two requests.
  actor->SetTextStyle( style, DONT_REQUEST_NEW_TEXT );

  actor->SetText( utfCodes );

  return actor;
}

TextActor::TextActor(bool fontDetection)
: RenderableActor(),
  mLoadingState(Dali::ResourceLoading),
  mUsingNaturalSize(true),
  mInternalSetSize(false),
  mFontDetection(fontDetection),
  mObserving(false)
{
}

void TextActor::OnInitialize()
{
}

TextActor::~TextActor()
{
  StopObservingTextLoads();
}

const std::string TextActor::GetText() const
{
  const Integration::TextArray& utfCodes = mTextAttachment->GetText();

  std::string text;

  const std::size_t length = utfCodes.Count();
  // minimize allocations for ascii strings
  text.reserve( length );

  for (unsigned int i = 0; i < length; ++i)
  {
    unsigned char utf8Data[4];
    unsigned int utf8Length;

    utf8Length = UTF8Write(utfCodes[i], utf8Data);

    text.append(reinterpret_cast<const char*>(utf8Data), utf8Length);
  }

  return text;
}

Font* TextActor::GetFont() const
{
  return &mTextAttachment->GetFont();
}

void TextActor::SetToNaturalSize()
{
  // ignore size set by application
  mUsingNaturalSize = true;
  TextChanged(); // this will calculate natural size
}

void TextActor::StopObservingTextLoads()
{
  if( mObserving )
  {
    mTextAttachment->GetFont().RemoveObserver( *this );
    mObserving = false;
  }
}

void TextActor::StartObservingTextLoads()
{
  if( !mObserving )
  {
    mTextAttachment->GetFont().AddObserver( *this );
    mObserving = true;
  }
}

void TextActor::SetText(const Integration::TextArray& utfCodes)
{
  StopObservingTextLoads();

  // assign the new text
  mTextAttachment->SetText(utfCodes);

  if( mFontDetection )
  {
    // first check if the provided font supports the text
    //
    if( !mTextAttachment->GetFont().AllGlyphsSupported(utfCodes) )
    {
      // auto-detect font
      // @todo GetFamilyForText should return font name and style
      const std::string fontName = Font::GetFamilyForText(utfCodes);

      // use previous formatting
      Internal::Font& font = mTextAttachment->GetFont();

      Dali::Font fontNew = Dali::Font::New( Dali::FontParameters( fontName, font.GetStyle(), PointSize(font.GetPointSize() ) ) );

      SetFont( GetImplementation(fontNew), DONT_REQUEST_NEW_TEXT );
    }
  }

  TextChanged();
}

void TextActor::SetFont(Font& font, TextRequestMode mode )
{
  StopObservingTextLoads();

  if( mode == REQUEST_NEW_TEXT )
  {
    // set the new font
    mTextAttachment->SetFont( font );

    // request text for new font
    TextChanged();
  }
  else
  {
    // just set the font
    mTextAttachment->SetFont( font );
  }
}

Vector3 TextActor::GetNaturalSize() const
{
  Vector2 naturalSize( mTextAttachment->GetNaturalTextSize() );
  return Vector3( naturalSize.width, naturalSize.height, CalculateSizeZ( naturalSize ) );
}

void TextActor::OnSizeSet(const Vector3& targetSize)
{
  if( !mInternalSetSize )
  {
    // after size is once set by application we no longer use the natural size
    mUsingNaturalSize = false;
  }
}

void TextActor::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  // after size has been animated by application we no longer use the natural size
  mUsingNaturalSize = false;
}

RenderableAttachment& TextActor::GetRenderableAttachment() const
{
  DALI_ASSERT_DEBUG( mTextAttachment );
  return *mTextAttachment;
}

void TextActor::SetGradientColor( const Vector4& color )
{
  mTextAttachment->SetGradient( color, mTextAttachment->GetGradientStartPoint(), mTextAttachment->GetGradientEndPoint() );
}

const Vector4& TextActor::GetGradientColor() const
{
  return mTextAttachment->GetGradientColor();
}

void TextActor::SetGradientStartPoint( const Vector2& position )
{
  mTextAttachment->SetGradient( mTextAttachment->GetGradientColor(), position, mTextAttachment->GetGradientEndPoint() );
}

const Vector2& TextActor::GetGradientStartPoint() const
{
  return mTextAttachment->GetGradientStartPoint();
}

void TextActor::SetGradientEndPoint( const Vector2& position )
{
  mTextAttachment->SetGradient( mTextAttachment->GetGradientColor(), mTextAttachment->GetGradientStartPoint(), position );
}

const Vector2& TextActor::GetGradientEndPoint() const
{
  return mTextAttachment->GetGradientEndPoint();
}

void TextActor::SetGradient( const Vector4& color, const Vector2& startPoint, const Vector2& endPoint )
{
  mTextAttachment->SetGradient( color, startPoint, endPoint );
}

void TextActor::SetTextStyle( const TextStyle& style, TextRequestMode mode )
{
  // Set font.
  const Font& font = mTextAttachment->GetFont();

  // Determine the font name/style/size that Font would create.
  // Then compare this to the existing font (which has been validated by Font).

  std::string resolvedFontName = style.GetFontName();
  std::string resolvedFontStyle = style.GetFontStyle();
  float resolvedFontPointSize = style.GetFontPointSize();
  bool resolvedFontFamilyDefault(false);
  bool resolvedFontPointSizeDefault(false);

  Font::ValidateFontRequest( resolvedFontName,
                             resolvedFontStyle,
                             resolvedFontPointSize,
                             resolvedFontFamilyDefault,
                             resolvedFontPointSizeDefault );

  // Now compare to existing font used to see if a font change is necessary.
  if( ( font.GetName() != resolvedFontName ) ||
      ( font.GetStyle() != resolvedFontStyle ) ||
      ( fabsf(font.GetPointSize() - resolvedFontPointSize) >= GetRangedEpsilon(font.GetPointSize(), resolvedFontPointSize) ) )
  {
    // Create font with original request (so font can determine if family and/or point size is default)
    SetFont( *(Font::New( style.GetFontName(), style.GetFontStyle(), style.GetFontPointSize() ) ), mode );
  }

  // Set color.
  if( !style.IsTextColorDefault() )
  {
    SetTextColor( style.GetTextColor() );
  }
  else
  {
    mTextAttachment->ResetTextColor();
  }

  // Italics
  if( !style.IsItalicsDefault() )
  {
    SetItalics( style.IsItalicsEnabled() ? Radian( style.GetItalicsAngle() ) : Radian( 0.0f ) );
  }
  else
  {
    mTextAttachment->ResetItalics();
  }

  // Underline
  if( !style.IsUnderlineDefault() )
  {
    SetUnderline( style.IsUnderlineEnabled(), style.GetUnderlineThickness(), style.GetUnderlinePosition() );
  }
  else
  {
    mTextAttachment->ResetUnderline();
  }

  // Shadow
  if( !style.IsShadowDefault() )
  {
    SetShadow( style.IsShadowEnabled(), style.GetShadowColor(), style.GetShadowOffset(), style.GetShadowSize() );
  }
  else
  {
    mTextAttachment->ResetShadow();
  }

  // Glow
  if( !style.IsGlowDefault() )
  {
    SetGlow( style.IsGlowEnabled(), style.GetGlowColor(), style.GetGlowIntensity() );
  }
  else
  {
    mTextAttachment->ResetGlow();
  }

  // Soft Smooth edge.
  if( !style.IsSmoothEdgeDefault() )
  {
    SetSmoothEdge( style.GetSmoothEdge() );
  }
  else
  {
    mTextAttachment->ResetSmoothEdge();
  }

  // Outline
  if( !style.IsOutlineDefault() )
  {
    SetOutline( style.IsOutlineEnabled(), style.GetOutlineColor(), style.GetOutlineThickness() );
  }
  else
  {
    mTextAttachment->ResetOutline();
  }

  // Weight
  if( !style.IsFontWeightDefault() )
  {
    SetWeight( style.GetWeight() );
  }
  else
  {
    mTextAttachment->ResetWeight();
  }

  //Gradient
  if( !style.IsGradientDefault() )
  {
    if( style.IsGradientEnabled() )
    {
      SetGradient( style.GetGradientColor(), style.GetGradientStartPoint(), style.GetGradientEndPoint() );
    }
    else
    {
      SetGradient( TextStyle::DEFAULT_GRADIENT_COLOR, TextStyle::DEFAULT_GRADIENT_START_POINT, TextStyle::DEFAULT_GRADIENT_END_POINT );
    }
  }
  else
  {
    mTextAttachment->ResetGradient();
  }
  TextChanged();
}

TextStyle TextActor::GetTextStyle() const
{
  TextStyle textStyle;
  mTextAttachment->GetTextStyle( textStyle );

  return textStyle;
}

void TextActor::SetTextColor(const Vector4& color)
{
  mTextAttachment->SetTextColor( color );
}

Vector4 TextActor::GetTextColor() const
{
  return mTextAttachment->GetTextColor();
}

void TextActor::SetSmoothEdge( float smoothEdge )
{
  mTextAttachment->SetSmoothEdge(smoothEdge);
}

void TextActor::SetOutline( bool enable, const Vector4& color, const Vector2& offset )
{
  mTextAttachment->SetOutline(enable, color, offset);
}

void TextActor::SetGlow( bool enable, const Vector4& color, float intensity )
{
  mTextAttachment->SetGlow(enable, color, intensity);
}

void TextActor::SetShadow( bool enable, const Vector4& color, const Vector2& offset, float size )
{
  mTextAttachment->SetShadow(enable, color, offset, size);
}

void TextActor::SetItalics( Radian angle )
{
  mTextAttachment->SetItalics( angle );

  TextChanged();
}

bool TextActor::GetItalics() const
{
  return mTextAttachment->GetItalics();
}

Radian TextActor::GetItalicsAngle() const
{
  return mTextAttachment->GetItalicsAngle();
}

void TextActor::SetUnderline( bool enable, float thickness, float position )
{
  mTextAttachment->SetUnderline( enable, thickness, position );

  TextChanged();
}

bool TextActor::GetUnderline() const
{
  return mTextAttachment->GetUnderline();
}

float TextActor::GetUnderlineThickness() const
{
  return mTextAttachment->GetUnderlineThickness();
}

float TextActor::GetUnderlinePosition() const
{
  return mTextAttachment->GetUnderlinePosition();
}

void TextActor::SetWeight( TextStyle::Weight weight )
{
  mTextAttachment->SetWeight( weight );
}

TextStyle::Weight TextActor::GetWeight() const
{
  return mTextAttachment->GetWeight();
}

void TextActor::SetFontDetectionAutomatic(bool value)
{
  mFontDetection = value;
}

bool TextActor::IsFontDetectionAutomatic() const
{
  return mFontDetection;
}

bool TextActor::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  TextActor* textActor = dynamic_cast<TextActor*>(object);

  if( Dali::TextActor::SIGNAL_TEXT_LOADING_FINISHED == signalName )
  {
    textActor->TextAvailableSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void TextActor::TextLoaded()
{
  // if the text is loaded, trigger the loaded finished signal
  CheckTextLoadState();
}

void TextActor::TextChanged()
{
  // this will tell the text attachment to act on any text or font changes
  mTextAttachment->TextChanged();

  // check the loading state
  bool loaded = CheckTextLoadState();
  if( ! loaded)
  {
    mLoadingState = Dali::ResourceLoading;

    StartObservingTextLoads();
  }
  // the text natural size is calculated synchronously above, when TextChanged() is called
  if (mUsingNaturalSize)
  {
    mInternalSetSize = true; // to know we're internally setting size
    SetSize( mTextAttachment->GetNaturalTextSize() );
    mInternalSetSize = false;
  }
}

bool TextActor::CheckTextLoadState()
{
  if( mTextAttachment->IsTextLoaded() )
  {
    mLoadingState = Dali::ResourceLoadingSucceeded;

    StopObservingTextLoads();

    // emit text available signal

    mLoadingFinished.Emit( Dali::TextActor( this ) );

    return true;
  }

  // text not loaded
  return false;
}

unsigned int TextActor::GetDefaultPropertyCount() const
{
  return RenderableActor::GetDefaultPropertyCount() + DEFAULT_TEXT_ACTOR_PROPERTY_COUNT;
}

void TextActor::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  RenderableActor::GetDefaultPropertyIndices( indices ); // RenderableActor class properties

  indices.reserve( indices.size() + DEFAULT_TEXT_ACTOR_PROPERTY_COUNT );

  int index = DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT;
  for ( int i = 0; i < DEFAULT_TEXT_ACTOR_PROPERTY_COUNT; ++i, ++index )
  {
    indices.push_back( index );
  }
}

const char* TextActor::GetDefaultPropertyName( Property::Index index ) const
{
  if(index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT)
  {
    return RenderableActor::GetDefaultPropertyName(index) ;
  }
  else
  {
    index -= DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT;

    if ( ( index >= 0 ) && ( index < DEFAULT_TEXT_ACTOR_PROPERTY_COUNT ) )
    {
      return DEFAULT_TEXT_ACTOR_PROPERTY_NAMES[index];
    }
    else
    {
      return NULL;
    }
  }
}

Property::Index TextActor::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Look for name in default properties
  for( int i = 0; i < DEFAULT_TEXT_ACTOR_PROPERTY_COUNT; ++i )
  {
    if( 0 == strcmp( name.c_str(), DEFAULT_TEXT_ACTOR_PROPERTY_NAMES[ i ] ) ) // dont want to convert rhs to string
    {
      index = i + DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT;
      break;
    }
  }

  // If not found, check in base class
  if( Property::INVALID_INDEX == index )
  {
    index = RenderableActor::GetDefaultPropertyIndex( name );
  }

  return index;
}

bool TextActor::IsDefaultPropertyWritable( Property::Index index ) const
{
  if(index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT)
  {
    return RenderableActor::IsDefaultPropertyWritable(index) ;
  }
  else
  {
    return true;
  }
}

bool TextActor::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  if(index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT)
  {
    return RenderableActor::IsDefaultPropertyAnimatable(index) ;
  }
  else
  {
    return false;
  }
}

bool TextActor::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if( index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT )
  {
    return RenderableActor::IsDefaultPropertyAConstraintInput(index);
  }
  return true; // Our properties can be used as input to constraints.
}

Property::Type TextActor::GetDefaultPropertyType( Property::Index index ) const
{
  if(index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT)
  {
    return RenderableActor::GetDefaultPropertyType(index) ;
  }
  else
  {
    index -= DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT;

    if ( ( index >= 0 ) && ( index < DEFAULT_TEXT_ACTOR_PROPERTY_COUNT ) )
    {
      return DEFAULT_TEXT_ACTOR_PROPERTY_TYPES[index];
    }
    else
    {
      // index out-of-bounds
      return Property::NONE;
    }
  }
}

void TextActor::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  if(index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT)
  {
    RenderableActor::SetDefaultProperty(index, propertyValue) ;
  }
  else
  {
    switch(index)
    {
      case Dali::TextActor::TEXT:
      {
        SetText( GetTextArray( Dali::Text( propertyValue.Get<std::string>() ) ) );
        break;
      }
      case Dali::TextActor::FONT:
      {
        SetFont(*Font::New(propertyValue.Get<std::string>(),
                           mTextAttachment->GetFont().GetStyle(),
                           PointSize(mTextAttachment->GetFont().GetPointSize())));
        break;
      }
      case Dali::TextActor::FONT_STYLE:
      {
        SetFont(*Font::New(mTextAttachment->GetFont().GetName(),
                           propertyValue.Get<std::string>(),
                           PointSize(mTextAttachment->GetFont().GetPointSize())));
        break;
      }
      case Dali::TextActor::OUTLINE_ENABLE:
      {
        Vector4 color;
        Vector2 thickness;
        mTextAttachment->GetOutlineParams( color, thickness );
        mTextAttachment->SetOutline(propertyValue.Get<bool>(), color, thickness);
        break;
      }
      case Dali::TextActor::OUTLINE_COLOR:
      {
        Vector4 color;
        Vector2 thickness;
        mTextAttachment->GetOutlineParams( color, thickness );
        mTextAttachment->SetOutline(mTextAttachment->GetOutline(), propertyValue.Get<Vector4>(), thickness);
        break;
      }
      case Dali::TextActor::OUTLINE_THICKNESS_WIDTH:
      {
        Vector4 color;
        Vector2 thickness;
        mTextAttachment->GetOutlineParams( color, thickness );
        mTextAttachment->SetOutline(mTextAttachment->GetOutline(), color, propertyValue.Get<Vector2>());
        break;
      }
      case Dali::TextActor::SMOOTH_EDGE:
      {
        mTextAttachment->SetSmoothEdge( propertyValue.Get<float>());
        break;
      }
      case Dali::TextActor::GLOW_ENABLE:
      {
        Vector4 color;
        float intensity;
        mTextAttachment->GetGlowParams( color, intensity );
        mTextAttachment->SetGlow(propertyValue.Get<bool>(), color, intensity);
        break;
      }
      case Dali::TextActor::GLOW_COLOR:
      {
        Vector4 color;
        float intensity;
        mTextAttachment->GetGlowParams( color, intensity );
        mTextAttachment->SetGlow(mTextAttachment->GetGlow(), propertyValue.Get<Vector4>(), intensity);
        break;
      }
      case Dali::TextActor::GLOW_INTENSITY:
      {
        Vector4 color;
        float intensity;
        mTextAttachment->GetGlowParams( color, intensity );
        mTextAttachment->SetGlow(mTextAttachment->GetGlow(), color, propertyValue.Get<float>());
        break;
      }
      case Dali::TextActor::SHADOW_ENABLE:
      {
        Vector4 color;
        Vector2 offset;
        float size;
        mTextAttachment->GetShadowParams( color, offset, size );
        mTextAttachment->SetShadow(propertyValue.Get<bool>(), color, offset, size );
        break;
      }
      case Dali::TextActor::SHADOW_COLOR:
      {
        Vector4 color;
        Vector2 offset;
        float size;
        mTextAttachment->GetShadowParams( color, offset, size );
        mTextAttachment->SetShadow(mTextAttachment->GetShadow(), propertyValue.Get<Vector4>(), offset, size);
        break;
      }
      case Dali::TextActor::SHADOW_OFFSET:
      {
        Vector4 color;
        Vector2 offset;
        float size;
        mTextAttachment->GetShadowParams( color, offset, size );
        mTextAttachment->SetShadow(mTextAttachment->GetShadow(), color, propertyValue.Get<Vector2>(), size );
        break;
      }
      case Dali::TextActor::SHADOW_SIZE:
      {
        Vector4 color;
        Vector2 offset;
        float size;
        mTextAttachment->GetShadowParams( color, offset, size );
        mTextAttachment->SetShadow(mTextAttachment->GetShadow(), color, offset, propertyValue.Get<float>());
        break;
      }
      case Dali::TextActor::ITALICS_ANGLE:
      {
        SetItalics(Radian(propertyValue.Get<float>())) ;
        break;
      }
      case Dali::TextActor::UNDERLINE:
      {
        SetUnderline(propertyValue.Get<bool>(), 0.f, 0.f ) ;
        break;
      }
      case Dali::TextActor::WEIGHT:
      {
        mTextAttachment->SetWeight(static_cast<TextStyle::Weight>(propertyValue.Get<int>())) ;
        break;
      }
      case Dali::TextActor::FONT_DETECTION_AUTOMATIC:
      {
        mFontDetection = propertyValue.Get<bool>()  ;
        break;
      }
      case Dali::TextActor::GRADIENT_COLOR:
      {
        mTextAttachment->SetGradient( propertyValue.Get<Vector4>(), mTextAttachment->GetGradientStartPoint(), mTextAttachment->GetGradientEndPoint() );
        break;
      }
      case Dali::TextActor::GRADIENT_START_POINT:
      {
        mTextAttachment->SetGradient( mTextAttachment->GetGradientColor(), propertyValue.Get<Vector2>(), mTextAttachment->GetGradientEndPoint() );
        break;
      }
      case Dali::TextActor::GRADIENT_END_POINT:
      {
        mTextAttachment->SetGradient( mTextAttachment->GetGradientColor(), mTextAttachment->GetGradientStartPoint(), propertyValue.Get<Vector2>() );
        break;
      }
      case Dali::TextActor::TEXT_COLOR:
      {
        mTextAttachment->SetTextColor( propertyValue.Get<Vector4>() );
        break;
      }
      default:
      {
        DALI_LOG_WARNING("Unknown text set property (%d)\n", index);
        break;
      }
    } // switch(index)

  } // else
}

Property::Value TextActor::GetDefaultProperty( Property::Index index ) const
{
  Property::Value ret ;
  if(index < DEFAULT_RENDERABLE_ACTOR_PROPERTY_MAX_COUNT)
  {
    ret = RenderableActor::GetDefaultProperty(index) ;
  }
  else
  {
    switch(index)
    {
      case Dali::TextActor::TEXT:
      {
        ret = GetText();
        break;
      }
      case Dali::TextActor::FONT:
      {
        ret = mTextAttachment->GetFont().GetName();
        break;
      }
      case Dali::TextActor::FONT_STYLE:
      {
        ret = mTextAttachment->GetFont().GetStyle();
        break;
      }
      case Dali::TextActor::OUTLINE_ENABLE:
      {
        ret = mTextAttachment->GetOutline();
        break;
      }
      case Dali::TextActor::OUTLINE_COLOR:
      {
        Vector4 color;
        Vector2 thickness;
        mTextAttachment->GetOutlineParams( color, thickness );
        ret = color;
        break;
      }
      case Dali::TextActor::OUTLINE_THICKNESS_WIDTH:
      {
        Vector4 color;
        Vector2 thickness;
        mTextAttachment->GetOutlineParams( color, thickness );
        ret = thickness;
        break;
      }
      case Dali::TextActor::SMOOTH_EDGE:
      {
        ret = mTextAttachment->GetSmoothEdge();
        break;
      }
      case Dali::TextActor::GLOW_ENABLE:
      {
        ret = mTextAttachment->GetGlow();
        break;
      }
      case Dali::TextActor::GLOW_COLOR:
      {
        Vector4 color;
        float intensity(0.0f);
        mTextAttachment->GetGlowParams( color, intensity );
        ret  = color;
        break;
      }
      case Dali::TextActor::GLOW_INTENSITY:
      {
        Vector4 color;
        float intensity(0.0f);
        mTextAttachment->GetGlowParams( color, intensity );
        ret = intensity;
        break;
      }
      case Dali::TextActor::SHADOW_ENABLE:
      {
        ret = mTextAttachment->GetShadow();
        break;
      }
      case Dali::TextActor::SHADOW_COLOR:
      {
        Vector4 color;
        Vector2 offset;
        float size;
        mTextAttachment->GetShadowParams( color, offset, size );
        ret = color;
        break;
      }
      case Dali::TextActor::SHADOW_OFFSET:
      {
        Vector4 color;
        Vector2 offset;
        float size;
        mTextAttachment->GetShadowParams( color, offset, size );
        ret = offset;
        break;
      }
      case Dali::TextActor::SHADOW_SIZE:
      {
        Vector4 color;
        Vector2 offset;
        float size;
        mTextAttachment->GetShadowParams( color, offset, size );
        ret = size;
        break;
      }
      case Dali::TextActor::ITALICS_ANGLE:
      {
        ret = static_cast<float>(mTextAttachment->GetItalics()) ;
        break;
      }
      case Dali::TextActor::UNDERLINE:
      {
        ret = mTextAttachment->GetUnderline() ;
        break;
      }
      case Dali::TextActor::WEIGHT:
      {
        ret = static_cast<int>(mTextAttachment->GetWeight());
        break;
      }
      case Dali::TextActor::FONT_DETECTION_AUTOMATIC:
      {
        ret = mFontDetection;
        break;
      }
      case Dali::TextActor::GRADIENT_COLOR:
      {
        ret = mTextAttachment->GetGradientColor();
        break;
      }
      case Dali::TextActor::GRADIENT_START_POINT:
      {
        ret = mTextAttachment->GetGradientStartPoint();
        break;
      }
      case Dali::TextActor::GRADIENT_END_POINT:
      {
        ret = mTextAttachment->GetGradientEndPoint();
        break;
      }
      case Dali::TextActor::TEXT_COLOR:
      {
        ret = mTextAttachment->GetTextColor();
        break;
      }
      default:
      {
        DALI_LOG_WARNING("Unknown text set property (%d)\n", index);
        break;
      }
    } // switch(index)
  } // if from base class

  return ret ;
}

} // namespace Internal

} // namespace Dali
