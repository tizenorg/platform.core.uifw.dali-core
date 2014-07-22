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
#include <dali/public-api/actors/text-actor.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/text-actor-impl.h>
#include <dali/internal/event/text/font-impl.h>

namespace Dali
{

using std::string;

const char* const TextActor::SIGNAL_TEXT_LOADING_FINISHED = "text-loading-finished";

TextActor::TextActor()
{
}

TextActor TextActor::New()
{
  Internal::TextActorPtr internal( Internal::TextActor::New( Text( std::string( "" ) ), true /*fontDetection*/ ) );
  return TextActor( internal.Get() );
}

TextActor TextActor::New(const string& text)
{
  Internal::TextActorPtr internal( Internal::TextActor::New( Text( text ), true /*fontDetection*/ ) );
  return TextActor( internal.Get() );
}

TextActor TextActor::New(const Text& text)
{
  Internal::TextActorPtr internal( Internal::TextActor::New( text, true /*fontDetection*/ ) );
  return TextActor( internal.Get() );
}

TextActor TextActor::New(const string& text, bool fontDetection)
{
  Internal::TextActorPtr internal( Internal::TextActor::New( Text( text ), fontDetection ) );
  return TextActor( internal.Get() );
}

TextActor TextActor::New(const Text& text, bool fontDetection)
{
  Internal::TextActorPtr internal( Internal::TextActor::New( text, fontDetection ) );
  return TextActor( internal.Get() );
}

TextActor TextActor::New(const string& text, bool fontDetection, bool /*isLeftToRight*/ )
{
  Internal::TextActorPtr internal( Internal::TextActor::New( Text( text ), fontDetection ) );
  return TextActor( internal.Get() );
}

TextActor TextActor::New(const Text& text, bool fontDetection, bool /*isLeftToRight*/ )
{
  Internal::TextActorPtr internal( Internal::TextActor::New( text, fontDetection ) );
  return TextActor( internal.Get() );
}

TextActor TextActor::New(const string& text, Font font)
{
  Internal::TextActorPtr internal( Internal::TextActor::New( Text( text ), true /*fontDetection*/, GetImplementation(font) ) );
  return TextActor( internal.Get() );
}

TextActor TextActor::New(const Text& text, Font font)
{
  Internal::TextActorPtr internal( Internal::TextActor::New( text, true /*fontDetection*/, GetImplementation(font) ) );
  return TextActor( internal.Get() );
}

TextActor TextActor::New(const string& text, Font font, bool fontDetection)
{
  Internal::TextActorPtr internal( Internal::TextActor::New( Text( text ), fontDetection, GetImplementation(font) ) );
  return TextActor( internal.Get() );
}

TextActor TextActor::New(const Text& text, Font font, bool fontDetection)
{
  Internal::TextActorPtr internal( Internal::TextActor::New( text, fontDetection, GetImplementation(font) ) );
  return TextActor( internal.Get() );
}

TextActor TextActor::New(const string& text, Font font, bool fontDetection, bool /*isLeftToRight*/ )
{
  Internal::TextActorPtr internal( Internal::TextActor::New( Text( text ), fontDetection, GetImplementation(font) ) );
  return TextActor( internal.Get() );
}

TextActor TextActor::New(const Text& text, Font font, bool fontDetection, bool /*isLeftToRight*/ )
{
  Internal::TextActorPtr internal( Internal::TextActor::New( text, fontDetection, GetImplementation(font) ) );
  return TextActor( internal.Get() );
}

TextActor TextActor::New(const Text& text, const TextStyle& style, bool fontDetection)
{
  Internal::TextActorPtr internal = Internal::TextActor::New( text, fontDetection, style );

  return TextActor(internal.Get());
}

TextActor TextActor::New(const Text& text, const TextStyle& style, bool fontDetection, bool /*isLeftToRight*/)
{
  Internal::TextActorPtr internal = Internal::TextActor::New( text, fontDetection, style );

  return TextActor(internal.Get());
}

TextActor TextActor::DownCast( BaseHandle handle )
{
  return TextActor( dynamic_cast<Dali::Internal::TextActor*>(handle.GetObjectPtr()) );
}

TextActor::~TextActor()
{
}

string TextActor::GetText() const
{
  return GetImplementation(*this).GetText();
}

void TextActor::SetText(const string& text)
{
  GetImplementation(*this).SetText(text);
}

void TextActor::SetText(const Text& text)
{
  GetImplementation(*this).SetText(text);
}

void TextActor::SetToNaturalSize()
{
  GetImplementation(*this).SetToNaturalSize();
}

Font TextActor::GetFont() const
{
  Internal::Font* font = GetImplementation(*this).GetFont();

  return Font(font);
}

void TextActor::SetFont(Font& font)
{
  GetImplementation(*this).SetFont(GetImplementation(font));
}

void TextActor::SetGradientColor( const Vector4& color )
{
  GetImplementation(*this).SetGradientColor( color );
}

Vector4 TextActor::GetGradientColor() const
{
  return GetImplementation(*this).GetGradientColor();
}

void TextActor::SetGradientStartPoint( const Vector2& position )
{
  GetImplementation(*this).SetGradientStartPoint( position );
}

Vector2 TextActor::GetGradientStartPoint() const
{
  return GetImplementation(*this).GetGradientStartPoint();
}

void TextActor::SetGradientEndPoint( const Vector2& position )
{
  GetImplementation(*this).SetGradientEndPoint( position );
}

Vector2 TextActor::GetGradientEndPoint() const
{
  return GetImplementation(*this).GetGradientEndPoint();
}

void TextActor::SetTextStyle( const TextStyle& style )
{
  GetImplementation(*this).SetTextStyle( style );
}

TextStyle TextActor::GetTextStyle() const
{
  return GetImplementation(*this).GetTextStyle();
}

void TextActor::SetTextColor( const Vector4& color )
{
  GetImplementation(*this).SetTextColor(color);
}

Vector4 TextActor::GetTextColor() const
{
  return GetImplementation(*this).GetTextColor();
}

void TextActor::SetSmoothEdge( float smoothEdge )
{
  GetImplementation(*this).SetSmoothEdge(smoothEdge);
}

void TextActor::SetOutline( bool enable, const Vector4& color, const Vector2& thickness )
{
  GetImplementation(*this).SetOutline(enable, color, thickness);
}

void TextActor::SetGlow( bool enable, const Vector4& color, float intensity )
{
  GetImplementation(*this).SetGlow(enable, color, intensity);
}

void TextActor::SetShadow( bool enable, const Vector4& color, const Vector2& offset, float size )
{
  GetImplementation(*this).SetShadow(enable, color, offset, size);
}

void TextActor::SetItalics( bool enabled, Degree angle )
{
  GetImplementation(*this).SetItalics( enabled ? Radian(angle) : Radian( 0.0f ) );
}

void TextActor::SetItalics( bool enabled, Radian angle )
{
  GetImplementation(*this).SetItalics( enabled ? angle : Radian( 0.0f ) );
}

bool TextActor::GetItalics() const
{
  return GetImplementation(*this).GetItalics();
}

Radian TextActor::GetItalicsAngle() const
{
  return GetImplementation(*this).GetItalicsAngle();
}

void TextActor::SetUnderline( bool enable )
{
  GetImplementation(*this).SetUnderline( enable, 0.f, 0.f );
}

bool TextActor::GetUnderline() const
{
  return GetImplementation(*this).GetUnderline();
}

void TextActor::SetWeight( TextStyle::Weight weight )
{
  GetImplementation(*this).SetWeight( weight );
}

TextStyle::Weight TextActor::GetWeight() const
{
  return GetImplementation(*this).GetWeight();
}

void TextActor::SetFontDetectionAutomatic(bool value)
{
  GetImplementation(*this).SetFontDetectionAutomatic(value);
}

bool TextActor::IsFontDetectionAutomatic() const
{
  return GetImplementation(*this).IsFontDetectionAutomatic();
}

LoadingState TextActor::GetLoadingState() const
{
  return GetImplementation(*this).GetLoadingState();
}

TextActor::TextActor(Internal::TextActor* internal)
: RenderableActor(internal)
{
}

} // namespace Dali
