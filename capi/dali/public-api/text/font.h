#ifndef __DALI_FONT_H__
#define __DALI_FONT_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/text/font-parameters.h>
#include <dali/public-api/text/text.h>

namespace Dali DALI_IMPORT_API
{

struct Vector3;

namespace Internal DALI_INTERNAL
{
class Font;
}

/**
 * Class to encapsulate a font.
 * Fonts are managed by the font manager, which loads any new fonts requested by applications. The font
 * manager keeps a cache of the most recently used fonts, and if a new font is requested when the cache
 * is full it will delete an old one (if there is one not in use).
 * This font class will request a font from the font manager in a manner which is not visible to the
 * application.
 *
 * Fonts will be created from a font name (like courier or comic) and font size (specified in points).
 */

class Font : public BaseHandle
{
public:
  /**
   * Stores glyph's metrics.
   * <ul>
   *   <li>\e Advance. The distance between the glyph's current pen position and the pen's position of the next glyph.
   *   <li>\e Bearing. The horizontal top side bearing. Is the distance between the baseline and the top of the glyph.
   *   <li>\e Width.   The glyph's width.
   *   <li>\e Height.  The glyph's height.
   * </ul>
   */
  class Metrics
  {
  public:
    /**
     * Default constructor.
     * Creates the implentation instance.
     */
    Metrics();

    /**
     * Destructor.
     * Destroyes the implementaiton instance.
     */
    virtual ~Metrics();

    /**
     * Copy constructor.
     * @param [in] metrics Metrics to be copied.
     */
    Metrics( const Metrics& metrics );

    /**
     * Assignment operator.
     * @param [in] metrics Metrics to be assigned.
     */
    Metrics& operator=( const Metrics& metrics );

    /**
     * Retrieves the advance metric.
     * @return the advance metric.
     */
    float GetAdvance() const;

    /**
     * Retrieves the bearing metric.
     * @return the bearing metric.
     */
    float GetBearing() const;

    /**
     * Retrieves the width metric.
     * @return the width metric.
     */
    float GetWidth() const;

    /**
     * Retrieves the height metric.
     * @return the height metric.
     */
    float GetHeight() const;

  public: // Not intended for application developers
    struct Impl;

    /**
     * Constructor. Initialization with metrics data.
     * @param implementation Glyph's metrics.
     */
    Metrics( const Impl& implementation );

  private:
    Impl* mImpl; ///< Implementation.
  };

public:
  /**
   * Create an empty Font. This can be initialised with Font::New(...)
   */
  Font();

  /**
   * Create an initialised Font with the given parameters. If no parameters are given, system defaults are used.
   * @param [in] fontParameters The font parameters.
   * @return A handle to a newly allocated font.
   */
  static Font New( const FontParameters& fontParameters = DEFAULT_FONT_PARAMETERS );

  /**
   * Downcast an Object handle to Font handle. If handle points to a Font object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a Font object or an uninitialized handle
   */
  static Font DownCast( BaseHandle handle );

  /**
   * Try to detect font for text.
   * @param [in] text displayed text
   */
  static const std::string GetFamilyForText(const std::string& text);

  /**
   * @copydoc GetFamilyForText(const std::string& text)
   */
  static const std::string GetFamilyForText(const Text& text);

  /**
   * Try to detect font for character.
   * @param [in] character displayed character
   */
  static const std::string GetFamilyForText(const Character& character);

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~Font();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * Convert a PixelSize from CapsHeight to it's equivalent LineHeight.
   * @param [in] fontFamily   The family's name of the font requested
   * @param [in] fontStyle The style of the font requested.
   * @param [in] capsHeight The size of the font ascenders required in pixels
   * @return The equivalent LineHeight (baseline to baseline) for the font
   */
  static PixelSize GetLineHeightFromCapsHeight(const std::string& fontFamily, const std::string& fontStyle, const CapsHeight& capsHeight);

  /**
   * The mode for GetInstalledFonts()
   */
  enum FontListMode
  {
    LIST_SYSTEM_FONTS,                   ///< List system fonts
    LIST_APPLICATION_FONTS,              ///< List application fonts
    LIST_ALL_FONTS                       ///< List all fonts
  };

  /**
   * Gets the list of available fonts.
   * @param mode which fonts to include in the list, default is LIST_SYSTEM_FONTS
   * @return a list of font family names
   */
  static std::vector<std::string> GetInstalledFonts( FontListMode mode = LIST_SYSTEM_FONTS );

  /**
   * Returns the width of the area needed to display some text if the text is textHeightPx pixels high.
   * Note that the text is not processed in any way before this calculation is performed (no stretching/scaling)
   * @param [in] text           The text to measure
   * @param [in] textHeightPx   The text height required
   * @return                    The displayed width in pixels
   */
  float MeasureTextWidth(const std::string& text, float textHeightPx) const;

  /**
   * @copydoc MeasureTextWidth(const std::string& text, float textHeightPx) const
   */
  float MeasureTextWidth(const Text& text, float textHeightPx) const;

  /**
   * Returns the width of the area needed to display the character if the text is textHeightPx pixels high.
   * Note that the character is not processed in any way before this calculation is performed (no stretching/scaling)
   * @param [in] character      The character to measure
   * @param [in] textHeightPx   The text height required
   * @return                    The displayed width in pixels
   */
  float MeasureTextWidth(const Character& character, float textHeightPx) const;

  /**
   * Returns the height of the area needed to display the text if the text is textWidthPx pixels wide.
   * Note that the text is not processed in any way before this calculation is performed (no stretching/scaling)
   * @param [in] text           The text to measure
   * @param [in] textWidthPx    The text width required
   * @return                    The displayed height in pixels
   */
  float MeasureTextHeight(const std::string& text, float textWidthPx) const;

  /**
   * @copydoc MeasureTextHeight(const std::string& text, float textWidthPx) const
   */
  float MeasureTextHeight(const Text& text, float textWidthPx) const;

  /**
   * Returns the height of the area needed to display the character if the text is textWidthPx pixels wide.
   * Note that the character is not processed in any way before this calculation is performed (no stretching/scaling)
   * @param [in] character      The character to measure
   * @param [in] textWidthPx    The text width required
   * @return                    The displayed height in pixels
   */
  float MeasureTextHeight(const Character& character, float textWidthPx) const;

  /**
   * Measure the natural size of a text string, as displayed in this font.
   * @param[in] text The text string to measure.
   * @return The natural size of the text.
   */
  Vector3 MeasureText(const std::string& text) const;

  /**
   * @copydoc MeasureText(const std::string& text) const
   */
  Vector3 MeasureText(const Text& text) const;

  /**
   * Measure the natural size of a character, as displayed in this font.
   * @param[in] character The character to measure.
   * @return The natural size of the character.
   */
  Vector3 MeasureText(const Character& character) const;

  /**
   * Tells whether text is supported with font.
   * @param [in] text displayed text
   */
  bool AllGlyphsSupported(const std::string& text) const;

  /**
   * @copydoc AllGlyphsSupported(const std::string& text) const
   */
  bool AllGlyphsSupported(const Text& text) const;

  /**
   * Tells whether character is supported with font.
   * @param [in] character displayed character.
   */
  bool AllGlyphsSupported(const Character& character) const;

  /**
   * Retrieves the line height.
   * The line height is the distance between two consecutive base lines.
   * @return The line height.
   */
  float GetLineHeight() const;

  /**
   * Retrieves the ascender metric.
   * The ascender metric is the distance between the base line and the top of the highest character in the font.
   * @return The ascender metric.
   */
  float GetAscender() const;

  /**
   * Retrieves the underline's thickness.
   *
   * It includes the vertical pad adjust used to add effects like glow or shadow.
   *
   * @return The underline's thickness.
   */
  float GetUnderlineThickness() const;

  /**
   * Retrieves the underline's position.
   *
   * It includes the vertical pad adjust used to add effects like glow or shadow.
   *
   * @return The underline's position.
   */
  float GetUnderlinePosition() const;

  /**
   * Retrieves glyph metrics.
   * @see Font::Metrics.
   * @param [in] character The character which its metrics are going to be retrieved.
   * @return The glyph metrics.
   */
  Metrics GetMetrics(const Character& character) const;

  /**
   * Retrieves whether this font was created with a default system font.
   * @return \e true if this font was created as a default system font.
   */
  bool IsDefaultSystemFont() const;

  /**
   * Retrieves whether this font was created with a default system size.
   * @return \e true if this font was created as a default system size.
   */
  bool IsDefaultSystemSize() const;

  /**
   * Gets the name of the font's family.
   * @return The name of the font's family.
   */
  const std::string& GetName() const;

  /**
   * Gets the font's style.
   * @return The font's style.
   */
  const std::string& GetStyle() const;

  /**
   * Return font size in points.
   * @return size in points
   */
  float GetPointSize() const;

  /**
   * Return font size in pixels.
   * @return size in pixels
   */
  unsigned int GetPixelSize() const;

  /**
   * Retrieves the size of the font in pixels from a given size in points.
   * @param[in] pointSize Size of the font in points.
   * @return size of the font in pixels.
   */
  static unsigned int PointsToPixels( float pointSize );

  /**
   * Retrieves the size of the font in points from a given size in pixels
   * @param[in] pixelsSize Size of the font in pixels.
   * @return size of the font in points.
   */
  static float PixelsToPoints( unsigned int pixelsSize );

public: // Not intended for application developers

  /**
   * This constructor is used by Dali New() methods
   * @param [in] font A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Font(Internal::Font* font);
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_FONT_H__
