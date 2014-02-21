#ifndef __DALI_TEXT_STYLE_H__
#define __DALI_TEXT_STYLE_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/text/font.h>

namespace Dali DALI_IMPORT_API
{

/**
 * Class to encapsulate a text style.
 */
class TextStyle
{
public:
  /**
   * Mask used to set text styles.
   */
  enum Mask
  {
    FONT =      1 << 0, ///< Sets the given font family.
    STYLE =     1 << 1, ///< Sets the given font style.
    SIZE =      1 << 2, ///< Sets the given font point size.
    WEIGHT =    1 << 3, ///< Sets the given font weight to bold.
    COLOR =     1 << 4, ///< Sets the given text color.
    ITALICS =   1 << 5, ///< Sets italics.
    UNDERLINE = 1 << 6, ///< Sets underline.
    SHADOW =    1 << 7, ///< Sets shadow.
    GLOW =      1 << 8, ///< Sets glow.
    OUTLINE =   1 << 9, ///< Sets outline
    ALL =      -1       ///< Sets all given style parameters.
  };

  enum Weight
  {
    THIN = 0,
    EXTRALIGHT,
    LIGHT,
    BOOK,
    REGULAR,
    MEDIUM,
    DEMIBOLD,
    BOLD,
    EXTRABOLD,
    BLACK,
    EXTRABLACK
  };

public:
  static const Degree  DEFAULT_ITALICS_ANGLE;              ///< Default angle used for text italics. @see SetItalics()
  static const float   DEFAULT_UNDERLINE_THICKNESS;        ///< Default underline's thickness.
  static const float   DEFAULT_UNDERLINE_POSITION;         ///< Default underline's position.
  static const Vector4 DEFAULT_TEXT_COLOR;                 ///< Default color for the text (white)
  static const Vector4 DEFAULT_SHADOW_COLOR;               ///< Default color for the shadow (black)
  static const Vector2 DEFAULT_SHADOW_OFFSET;              ///< Default offset used for text shadow. @see SetShadow()
  static const float   DEFAULT_SHADOW_SIZE;                ///< Default size of text shadow. @see SetShadow()
  static const Vector4 DEFAULT_GLOW_COLOR;                 ///< Default color for the glow (white)
  static const float   DEFAULT_GLOW_INTENSITY;             ///< Default intensity used for text glow. @see SetGlow()
  static const float   DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD; ///< Default distance field used for edge smooth. @see SetSmoothEdge()
  static const Vector4 DEFAULT_OUTLINE_COLOR;              ///< Default color for the outline (white)
  static const Vector2 DEFAULT_OUTLINE_THICKNESS;          ///< Default thickness used for text outline. @see SetOutline()
  static const Vector4 DEFAULT_GRADIENT_COLOR;             ///< Default gradient color
  static const Vector2 DEFAULT_GRADIENT_START_POINT;       ///< Default gradient start point
  static const Vector2 DEFAULT_GRADIENT_END_POINT;         ///< Default gradient end point

public:

  /**
   * Default constructor.
   * The style created used a default font @see Dali::Font, color is white, and is neither underlined nor italics.
   */
  TextStyle();

  /**
   * Copy constructor.
   * @param[in] textStyle The text style to be copied.
   */
  TextStyle( const TextStyle& textStyle );

  /**
   * Destructor.
   */
  virtual ~TextStyle();

  /**
   * Assignment operator.
   * @param[in] textStyle The text style to be assigned.
   */
  TextStyle& operator=( const TextStyle& textStyle );

  /**
   * Equality operator.
   * @param [in] textStyle The text style to be compared.
   */
  bool operator==( const TextStyle& textStyle ) const;

  /**
   * Inequality operator.
   * @param [in] textStyle The text style to be compared.
   */
  bool operator!=( const TextStyle& textStyle ) const;

  /**
   * Copies from the given text style those parameters specified in the given mask.
   *
   * @param[in] textStyle The given text style.
   * @param[in] mask Specifies which text style parameters are going to be copied. By default all parateres are copied.
   */
  void Copy( const TextStyle& textStyle, const Mask mask = ALL );

  /**
   * Retrieves the font name.
   * @return The font name.
   */
  const std::string& GetFontName() const;

  /**
   * Sets the font name.
   * @param[in] fontName
   */
  void SetFontName( const std::string& fontName );

  /**
   * Retrieves the font style.
   * @return The font style.
   */
  const std::string& GetFontStyle() const;

  /**
   * Sets the font style.
   * @param[in] fontStyle
   */
  void SetFontStyle( const std::string& fontStyle );

  /**
   * Retrieves the font point size.
   * @return the point size.
   */
  PointSize GetFontPointSize() const;

  /**
   * Sets the font point size.
   * @param[in] fontPointSize The font point size.
   */
  void SetFontPointSize( PointSize fontPointSize );

  /**
   * Retrieves the style weight.
   * @return The style weight.
   */
  Weight GetWeight() const;

  /**
   * Sets the style weight.
   * @param[in] weight The style weight
   */
  void SetWeight( Weight weight );

  /**
   * Retrieves the text color.
   * @return The text color.
   */
  const Vector4& GetTextColor() const;

  /**
   * Sets the text color.
   * @param[in] textColor The text color
   */
  void SetTextColor( const Vector4& textColor );

  /**
   * Whether the style italics option is enabled or not.
   * @return \e true if italics is enabled.
   */
  bool GetItalics() const;

  /**
   * Sets the italics option.
   * @param[in] italics The italics option.
   */
  void SetItalics( bool italics );

  /**
   * Retrieves the italics angle.
   * @return \e The italics angle in degree.
   */
  Degree GetItalicsAngle() const;

  /**
   * Sets the italics angle.
   * @param[in] angle The italics angle in degree.
   */
  void SetItalicsAngle( Degree angle );

  /**
   * Whether the style underline option is enabled or not.
   * @return \e true if underline is enabled.
   */
  bool GetUnderline() const;

  /**
   * Sets the underline option.
   * @param[in] underline The underline option.
   */
  void SetUnderline( bool underline );

  /**
   * Retrieves the underline thickness.
   * @return The underline thickness.
   */
  float GetUnderlineThickness() const;

  /**
   * Sets the underline thickness.
   *
   * @param[in] thickness The underline thickness.
   */
  void SetUnderlineThickness( float thickness );

  /**
   * Retrieves the underline position.
   * @return The underline position.
   */
  float GetUnderlinePosition() const;

  /**
   * Sets the underline position.
   *
   * @param[in] position The underline position.
   */
  void SetUnderlinePosition( float position );

  /**
   * Whether the style shadow option is enabled or not.
   * @return \e true if shadow is enabled.
   */
  bool GetShadow() const;

  /**
   * Retrieves the shadow color.
   * @return The shadow color.
   */
  const Vector4& GetShadowColor() const;

  /**
   * Retrieves the shadow offset
   * @return The shadow offset.
   */
  const Vector2& GetShadowOffset() const;

  /**
   * Retrieves the shadow size
   * @return The shadow size.
   */
  float GetShadowSize() const;

  /**
   * Sets the shadow option.
   * @param[in] shadow        The shadow option.
   * @param[in] shadowColor   The color of the shadow
   * @param[in] shadowOffset  Offset in pixels.
   * @param[in] shadowSize    Size of shadow in pixels. 0 means the shadow is the same size as the text.
   */
  void SetShadow( bool shadow,
                  const Vector4& shadowColor = DEFAULT_SHADOW_COLOR,
                  const Vector2& shadowOffset = DEFAULT_SHADOW_OFFSET,
                  const float shadowSize = DEFAULT_SHADOW_SIZE );

  /**
   * Whether the glow option is enabled or not.
   * @return \e true if glow is enabled.
   */
  bool GetGlow() const;

  /**
   * Retrieves the glow color.
   * @return The glow color.
   */
  const Vector4& GetGlowColor() const;

  /**
   * Retrieve the glow intensity.
   * @return The glow intensity.
   */
  float GetGlowIntensity() const;

  /**
   * Sets the glow option and color.
   * @param[in] glow The glow option.
   * @param[in] glowColor The color of the glow.
   * @param[in] glowIntensity Determines the amount of glow around text.
   *                          The edge of the text is at the value set with smoothEdge.
   *                          SetSmoothEdge() The distance field value at which the glow becomes fully transparent.
   */
  void SetGlow( bool glow, const Vector4& glowColor = DEFAULT_GLOW_COLOR, float glowIntensity = DEFAULT_GLOW_INTENSITY );

  /**
   * Retrieves the soft smooth edge.
   * @return The soft smooth edge.
   */
  float GetSmoothEdge() const;

  /**
   * Set soft edge smoothing
   * @param[in] smoothEdge Specify the distance field value for the center of the text edge.
   *            0 <= smoothEdge <= 1
   */
  void SetSmoothEdge( float smoothEdge = DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD );

  /**
   * Whether the outline option is enabled or not.
   * @return \e true if outline is enabled.
   */
  bool GetOutline() const;

  /**
   * Retrieves the outline color.
   * @return The outline color.
   */
  const Vector4& GetOutlineColor() const;

  /**
   * Retrieves the outline thickness.
   * @return The outline thickness.
   */
  const Vector2& GetOutlineThickness() const;

  /**
   * Sets the outline option and color.
   * @param[in] outline The outline option.
   * @param[in] outlineColor The color of the outline.
   * @param[in] outlineThickness Thickness of outline. The outline thickness is determined by two parameters.
   *                             outlineThickness[0] Specifies the distance field value for the center of the outline.
   *                             outlineThickness[1] Specifies the softness/width/anti-aliasing of the outlines inner edge.
   *                             SetSmoothEdge() specifies the smoothness/anti-aliasing of the text outer edge.
   *                             0 <= smoothEdge[0] <= 1.
   *                             0 <= smoothEdge[1] <= 1.
   */
  void SetOutline( bool outline, const Vector4& outlineColor = DEFAULT_OUTLINE_COLOR, const Vector2& outlineThickness = DEFAULT_OUTLINE_THICKNESS );

private:
  struct Impl;
  Impl* mImpl;
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TEXT_STYLE_H__
