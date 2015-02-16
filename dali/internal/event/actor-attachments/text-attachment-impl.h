#ifndef __DALI_INTERNAL_TEXT_ATTACHMENT_H__
#define __DALI_INTERNAL_TEXT_ATTACHMENT_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/text/text-style.h>
#include <dali/internal/event/actor-attachments/renderable-attachment-impl.h>
#include <dali/internal/event/actor-attachments/actor-attachment-declarations.h>
#include <dali/internal/event/text/font-declarations.h>
#include <dali/internal/event/text/text-request-helper.h>
#include <dali/internal/event/text/resource/glyph-texture-observer.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/integration-api/text-array.h>

namespace Dali
{

namespace Internal
{

class TextParameters;

namespace SceneGraph
{
class TextAttachment;
class Node;
}

/**
 * An attachment for rendering text from a font atlas.
 */
class TextAttachment : public RenderableAttachment, public GlyphTextureObserver
{
public:

  /**
   * Create a new TextAttachment.
   * @param[in] stage The stage to use for messaging
   * @param[in] parentNode The node to attach a scene-object to.
   * @param [in] text The text which will be displayed.
   * @param [in] font The font which will be used for the text.
   * @return A smart-pointer to the newly allocated TextAttachment.
   */
  static TextAttachmentPtr New( Stage& stage, const SceneGraph::Node& parentNode, const Integration::TextArray& text, FontPointer font );

  /**
   * Set the text label displayed by the attachment
   * @param [in] text The new text label
   */
  void SetText(const Integration::TextArray& text);

  /**
   * Get the text label displayed by the attachment
   * @return The text label
   */
  const Integration::TextArray& GetText() const
  {
    // This is not animatable; the cached value is up-to-date.
    return mText;
  }

  /**
   * Get the natural text size
   * @return the natural size of the text.
   */
  const Vector2& GetNaturalTextSize() const
  {
    // This is not animatable; the cached value is up-to-date.
    return mTextSize;
  }

  /**
   * Set the font used to display the text label displayed by the attachment
   * @param [in] font The new font
   */
  void SetFont(Font& font);

  /**
   * Get the font used to display the text label displayed by the attachment
   * @return The font currently in use
   */
  Font& GetFont()
  {
    return *mFont;
  }

  /**
   * Set the text color
   * @param[in] color The text color
   */
  void SetTextColor(const Vector4& color);

  /**
   * Get the text color
   * @return The text color
   */
  Vector4 GetTextColor() const;

  /**
   * Resets to default the text color.
   */
  void ResetTextColor();

  /**
   * @copydoc Dali::TextActor::SetWeight()
   */
  void SetWeight( TextStyle::Weight weight );

  /**
   * @copydoc Dali::TextActor::GetWeight()
   */
  TextStyle::Weight GetWeight() const;

  /**
   * Resets to default the text weight.
   */
  void ResetWeight();

  /**
   * @copydoc Dali::TextActor::SetSmoothEdge(const float)
   */
  void SetSmoothEdge( float smoothEdge );

  /**
   * Retrieve the smooth edge value.
   * @return the smooth edge value.
   */
  float GetSmoothEdge() const;

  /**
   * Resets to default the smooth edge.
   */
  void ResetSmoothEdge();

  /**
   * Enable italics on the text actor, the text will be sheared by the given angle.
   * @param[in] angle Italics angle in radians.
   */
  void SetItalics( Radian angle );

  /**
   * @copydoc Dali::TextActor::GetItalics()
   */
  bool GetItalics() const;

  /**
   * @copydoc Dali::TextActor::GetItalicsAngle()
   */
  Radian GetItalicsAngle() const;

  /**
   * Resets to default the italics.
   */
  void ResetItalics();

  /**
   * @copydoc Dali::TextActor::SetUnderline()
   */
  void SetUnderline( bool enable, float thickness, float position );

  /**
   * @copydoc Dali::TextActor::GetUnderline()
   */
  bool GetUnderline() const;

  /**
   * @copydoc Internal::TextActor::GetUnderlineThickness()
   */
  float GetUnderlineThickness() const;

  /**
   * @copydoc Internal::TextActor::GetUnderlinePosition()
   */
  float GetUnderlinePosition() const;

  /**
   * Resets to default the text underline.
   */
  void ResetUnderline();

  /**
   * @copydoc Dali::TextActor::SetOutline(const bool,const Vector4&,const Vector2&)
   */
  void SetOutline( bool enable, const Vector4& color, const Vector2& thickness );

  /**
   * Get outline state.
   * @return \e true if outline is enabled, otherwise \e false.
   */
  bool GetOutline() const;

  /**
   * Retrieve outline parameters.
   * @param[out] color The outline color.
   * @param[out] thickness The outline parameters.
   */
  void GetOutlineParams( Vector4& color, Vector2& thickness ) const;

  /**
   * Resets to default the text outline.
   */
  void ResetOutline();

  /**
   * @copydoc Dali::TextActor::SetGlow(const bool,const Vector4&,const float)
   */
  void SetGlow( bool enable, const Vector4& color, float intensity );

  /**
   * Get glow state.
   * @return \e true if glow is enabled, otherwise \e false.
   */
  bool GetGlow() const;

  /**
   * Retrieve glow parameters.
   * @param[out] color The glow color.
   * @param[out] intensity The glow intensity.
   */
  void GetGlowParams( Vector4& color, float& intensity ) const;

  /**
   * Resets to default the text glow.
   */
  void ResetGlow();

  /**
   * @copydoc Dali::TextActor::SetShadow(const bool,const Vector4&,const Vector2&,const float)
   */
  void SetShadow( bool enable, const Vector4& color, const Vector2& offset, float size );

  /**
   * Retrieve the shadow state.
   * @return \e true if shadow is enabled, otherwise \e false.
   */
  bool GetShadow() const;

  /**
   * Retrieve shadow parameters.
   * @param[out] color The shadow color.
   * @param[out] offset The shadow offset.
   * @param[out] size The shadow size.
   */
  void GetShadowParams( Vector4& color, Vector2& offset, float& size ) const;

  /**
   * Resets to default the text shadow.
   */
  void ResetShadow();

  /**
   * @see Dali::TextActor::SetGradientColor()
   * @see Dali::TextActor::SetGradientStartPoint()
   * @see Dali::TextActor::SetGradientEndPoint()
   */
  void SetGradient( const Vector4& color, const Vector2& startPoint, const Vector2& endPoint );

  /**
   * @copydoc Dali::TextActor::GetGradientColor()
   */
  const Vector4& GetGradientColor() const;

  /**
   * @copydoc Dali::TextActor::GetGradientStartPoint()
   */
  const Vector2& GetGradientStartPoint() const;

  /**
   * @copydoc Dali::TextActor::GetGradientEndPoint()
   */
  const Vector2& GetGradientEndPoint() const;

  /**
   * Resets to default the text gradient.
   */
  void ResetGradient();

  /**
   * Retrieves the text style.
   *
   * @param[out] style The text style.
   */
  void GetTextStyle( TextStyle& style ) const;

  /**
   * Measure the natural size of a text string, as displayed in this font.
   * @return The natural size of the text.
   */
  Vector3 MeasureText() const;

  /**
   * Inform the text attachment the text / font it is using has changed.
   * The TextChanged() and TextNotRequired() functions are used to avoid duplicate
   * requests for text. For example if the text is changed, and the font is changed
   * then we want to avoid requesting the new text with the old font, then the new
   * text with the new font.
   */
  void TextChanged();

  /**
   * @return true if all text has been loaded and is ready to display
   */
  bool IsTextLoaded();

  /**
   * Calculates the weighted smoothing value.
   */
  void CalculateWeightedSmoothing( TextStyle::Weight weight, float smoothEdge );

public: // glyph texture observer

  /**
   *  @copydoc Dali::Internal::GlyphTextureObserver::TextureResized()
   */
  virtual void TextureResized( const TextureIdList& oldTextureIds, unsigned int newTextureId );

  /**
   * @copydoc Dali::Internal::GlyphTextureObserver::TextureSplit()
   */
  virtual void TextureSplit( FontId fontId, const TextureIdList& oldTextureIds, unsigned int newTextureId );

private:

  /**
   * Check if the text has been modified
   * @return true if the text or it's formating has been modified
   */
   bool IsTextModified();

  /**
   * First stage construction of a TextAttachment.
   * @param[in] stage Used to send messages to scene-graph.
   */
  TextAttachment( Stage& stage );

  /**
   * @copydoc Dali::Internal::RenderableAttachment::OnStageConnection2()
   */
  virtual void OnStageConnection2();

  /**
   * @copydoc Dali::Internal::RenderableAttachment::OnStageDisconnection2()
   */
  virtual void OnStageDisconnection2();

  /**
   * @copydoc Dali::Internal::RenderableAttachment::GetSceneObject()
   */
  virtual const SceneGraph::RenderableAttachment& GetSceneObject() const;

  /**
   * Update any text & font changes to the on stage scene graph text attachment
   */
  void SetTextChanges();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextAttachment();

private:

  const SceneGraph::TextAttachment* mSceneObject; ///< Not owned

  TextRequestHelper mTextRequestHelper;           ///< Text request helper
  FontPointer mFont;                              ///< The font resource
  Vector4* mTextColor;                            ///< on demand storage for text color
  TextStyle mStyle;                               ///< on demand storage for text effects parameters ( do not store font parameters and color )
                                                  ///< Font parameters are stored in the FontPointer member. Inside the TextStyle, font parameters and color are allocated in the same struct,
                                                  ///< so store the text color in the style will allocate space for the font parameters as well, wasting space.
  // Cached values for public getters
  Integration::TextArray mText;

  bool    mTextChanged:1;       ///< whether the text has changed
  bool    mFontChanged:1;       ///< whether the font has changed
  bool    mUnderlineChanged:1;  ///< whether the underline has changed
  bool    mItalicsChanged:1;    ///< whether the italics has changed
  bool    mTextureIdSet:1;      ///< flag to signify if the texture id has been set on the scene-graph text-attachment
  unsigned int mTextureId;      ///< the texture id of the glyph atlas being used
  Vector2 mTextSize;            ///< text natural size

  OwnerPointer< TextVertexBuffer > mVertexBuffer; ///< vertex data to display the text
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TEXT_ATTACHMENT_H__
