#ifndef __DALI_INTEGRATION_PLATFORM_ABSTRACTION_H__
#define __DALI_INTEGRATION_PLATFORM_ABSTRACTION_H__

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

// EXTERNAL INCLUDES
#include <stdint.h>

// INTERNAL INCLUDES
#include <dali/integration-api/resource-cache.h>
#include <dali/integration-api/glyph-set.h>

namespace Dali
{

typedef std::vector<uint32_t> TextArray;

namespace Integration
{

class Bitmap;
class DynamicsFactory;

/**
 * PlatformAbstraction is an abstract interface, used by Dali to access platform specific services.
 * A concrete implementation must be created for each platform, and provided when creating the
 * Dali::Integration::Core object.
 */
class DALI_IMPORT_API PlatformAbstraction
{
public:

  // Dali Lifecycle

  /**
   * Get the monotonic time since an unspecified reference point.
   * This is used by Dali to calculate time intervals during animations; the interval is
   * recalculated when Dali is resumed with Dali::Integration::Core::Resume().
   * Multi-threading note: this method may be called from either the main or rendering thread.
   * @param[out] seconds The time in seconds since the reference point.
   * @param[out] microSeconds The remainder in microseconds.
   */
  virtual void GetTimeMicroseconds(unsigned int &seconds, unsigned int &microSeconds) = 0;

  /**
   * Tell the platform abstraction that Dali is ready to pause, such as when the
   * application enters a background state.
   * Allows background threads to pause their work until Resume() is called.
   * This is a good time to release recreatable data such as memory caches
   * to cooperate with other apps and reduce the chance of this one being
   * force-killed in a low memory situation.
   */
  virtual void Suspend() {} ///!ToDo: Make pure virtual once dali-adaptor patch is in place = 0;

  /**
   * Tell the platform abstraction that Dali is resuming from a pause, such as
   * when it has transitioned from a background state to a foreground one.
   * It is time to wake up sleeping background threads and recreate memory
   * caches and other temporary data.
   */
  virtual void Resume() {} ///!ToDo: Make pure virtual once dali-adaptor patch is in place = 0;

  // Resource Loading

  /**
   * Load metadata from an image on the native filesystem. This is a synchronous request.
   * This function is used when the size of an image needs to be determined before it has loaded.
   * @param[in] filename name of the string to load.
   * @param[out] size Size of the image.
   */
  virtual void LoadImageMetadata(const std::string filename, Vector2 &size) = 0;

  /**
   * Request a resource from the native filesystem. This is an asynchronous request.
   * After this method returns, FillResourceCache() will be called to retrieve the result(s) of the
   * resource loading operation. Loading resources in separate worker thread is recommended.
   * Multi-threading note: this method will be called from the main thread only i.e. not
   * from within the Core::Render() method.
   * @param[in] request A unique resource request. This is not guaranteed to survive after LoadResource
   * returns; the loading process should take a copy.
   */
  virtual void LoadResource(const ResourceRequest& request) = 0;

  /**
   * Request that a resource be saved to the native filesystem.
   * This is an asynchronous request.
   */
  virtual void SaveResource(const ResourceRequest& request) = 0;
  /**
   * Cancel an ongoing LoadResource() request.
   * Multi-threading note: this method will be called from the main thread only i.e. not
   * from within the Core::Render() method.
   * @param[in] id The ID of the resource to cancel.
   * @param[in] typeId The ID type of the resource to cancel.
   */
  virtual void CancelLoad(ResourceId id, ResourceTypeId typeId) = 0;

  /**
   * Query whether any asynchronous LoadResource() requests are ongoing.
   * Multi-threading note: this method may be called from either the main or rendering thread.
   * @return True if resources are being loaded.
   */
  virtual bool IsLoading() = 0;

  /**
   * Retrieve newly loaded resources.
   * If no resources have finished loading, then this method returns immediately.
   * Multi-threading note: this method will be called from the update thread, from within
   * the UpdateManager::Update() method.
   * @param[in] cache The resource cache to fill.
   */
  virtual void GetResources(ResourceCache& cache) = 0;

  /**
   * Waits for the asynchronous loader threads (if any) to finish.
   * This will be only be called before Core destruction; no resource loading requests will be
   * made following this method.
   */
  virtual void JoinLoaderThreads() = 0;

  // Font Queries

  /**
   * Called by Dali to retrieve the default font family for the platform.
   * Multi-threading note: this method will be called from the main thread only i.e. not
   * from within the Core::Render() method.
   * @return The default font family.
   */
  virtual std::string GetDefaultFontFamily() const = 0;

  /**
   * Called by Dali to retrieve the default font size for the platform in points.
   * Multi-threading note: this method will be called from the main thread only i.e. not
   * from within the Core::Render() method.
   * @return The default font size.
   */
  virtual const float GetDefaultFontSize() const = 0;

  /**
   * Gets a font line height to match a given caps-height
   * @param[in] fontFamily The name of the font's family
   * @param[in] fontStyle  The style of the font
   * @param[in] capsHeight The caps-height in pixels
   */
  virtual const PixelSize GetFontLineHeightFromCapsHeight(const std::string fontFamily, const std::string& fontStyle, const CapsHeight& capsHeight) const = 0;

  /**
   * Called by Font objects to synchronously query glyph data.
   * @note Font's style goes inside the textRequest parameter
   * @param[in] textRequest  Resource request. Includes font's style.
   * @param[in] fontFamily   The name of the font's family
   * @param[in] getBitmap    Whether to load bitmaps for the symbols as well
   * @return A GlyphSet pointer with a list of the requested glyph metrics.
   */
  virtual Integration::GlyphSet* GetGlyphData ( const Integration::TextResourceType& textRequest,
                                                const std::string& fontFamily,
                                                bool getBitmap) const = 0;

  /**
   * Called by GlyphResourceManager to synchronously load glyph data.
   * @param[in] textRequest     resource request
   * @param[in] fontFamily      name of the font's family
   * @return A GlyphSet pointer containing the requested glyph bitmaps.
   */
  virtual Integration::GlyphSet* GetCachedGlyphData( const TextResourceType& textRequest,
                                                     const std::string& fontFamily ) const = 0;

  /**
   * Called by Font objects to synchronously query global font metrics.
   * @param[in] fontFamily     The name of the font's family
   * @param[in] fontStyle      The style of the font
   * @param[out] globalMetrics font requested global metrics.
   */
  virtual void GetGlobalMetrics( const std::string& fontFamily,
                                 const std::string& fontStyle,
                                 Integration::GlobalMetrics& globalMetrics ) const = 0;

  /**
   * Sets horizontal and vertical pixels per inch value that is used by the display
   * @param[in] dpiHorizontal horizontal dpi value
   * @param[in] dpiVertical   vertical dpi value
   */
  virtual void SetDpi (unsigned int dpiHorizontal, unsigned int dpiVertical) = 0;

  /**
   * Returns the name of the font's family for displayed text.
   * If possible, the returned font name should be able to display all characters in text.
   * Otherwise returns closest match.
   * @param[in] charsRequested displayed text
   */
  virtual std::string GetFontFamilyForChars(const TextArray& charsRequested) const = 0;

  /**
   * Checks whether all characters of text could be displayed with specified font family.
   * @param[in] fontFamily     The name of the font's family
   * @param[in] fontStyle      The style of the font
   * @param[in] text     displayed text
   */
  virtual bool AllGlyphsSupported(const std::string& fontFamily, const std::string& fontStyle, const TextArray& text) const = 0;

  /**
   * Checks whether fontName is a valid font family name.
   * closestMatch is always set to the best matching font or the system default font if no near match is detected.
   * @param[in] fontFamily     The name of the font's family
   * @param[in] fontStyle      The style of the font
   * @param[out] isDefaultSystemFont Whether this font has been created with a default system font.
   * @param[out] closestFontFamilyMatch Name of the font's family found based on the user input family's name
   * @param[out] closestFontStyleMatch  Name of the font's style found based on the user input font's style
   * @return     true if fontName is valid, false otherwise
   */
  virtual bool ValidateFontFamilyName(const std::string& fontFamily, const std::string& fontStyle, bool& isDefaultSystemFont, std::string& closestFontFamilyMatch, std::string& closestFontStyleMatch) const = 0;

  /**
   * The mode for GetFontList()
   */
  enum FontListMode
  {
    LIST_SYSTEM_FONTS,
    LIST_APPLICATION_FONTS,
    LIST_ALL_FONTS
  };

  /**
   * Gets a list of fonts installed on the system.
   * @param fontListMode which fonts to include in the list
   * @return a list of font family names
   */
  virtual std::vector<std::string> GetFontList( FontListMode mode ) const = 0;

  /**
   * Load a file into a buffer
   * @param[in] filename The filename to load
   * @param[out] buffer  A buffer to receive the file.
   *                     The buffer is implemeneted with a std::vector which is resized to fit the file.
   * @result             true if the file is loaded.
   */
  virtual bool LoadFile( const std::string& filename, std::vector< unsigned char >& buffer ) const = 0;

  /**
   * Load a file into a buffer
   * @param[in] filename The filename to save
   * @param[out] buffer  A buffer containing some data
   *                     The buffer is implemeneted with a std::vector. The size() member specifies the buffer length.
   * @result             true if the file is saved.
   */
  virtual bool SaveFile(const std::string& filename, std::vector< unsigned char >& buffer) const = 0;

  /**
   * This method re-loads the device defaults that Dali uses. Adaptor will call this
   * when devices settings change.
   */
  virtual void UpdateDefaultsFromDevice() = 0;

  /**
   * Get a pointer to the DynamicsFactory.
   */
  virtual DynamicsFactory* GetDynamicsFactory() = 0;

  /**
   * Read from the metrics cache into the global metrics parameter
   * @param[in] fontFamily The name of the font family
   * @param[in] fontStyle The name of the font style
   * @param[out] globalMetrics The data store to write into
   * @return \e true if the operation succeeded
   */
  virtual bool ReadGlobalMetricsFromCache( const std::string& fontFamily,
                                           const std::string& fontStyle,
                                           Integration::GlobalMetrics& globalMetrics ) = 0;

  /**
   * Write the global metrics parameter to the metrics cache
   * @param[in] fontFamily The name of the font family
   * @param[in] fontStyle The name of the font style
   * @param[out] globalMetrics The data store to write
   */
  virtual void WriteGlobalMetricsToCache( const std::string& fontFamily,
                                          const std::string& fontStyle,
                                          const Integration::GlobalMetrics& globalMetrics ) = 0;

  /**
   * Read the metrics from the cache into the supplied vector
   * @param[in] fontFamily The name of the font family
   * @param[in] fontStyle The name of the font style
   * @param[out] glyphMetricsContainer The vector of metrics to write
   * @return true if the operation succeeded
   */
  virtual bool ReadMetricsFromCache( const std::string& fontFamily,
                                     const std::string& fontStyle,
                                     std::vector<Integration::GlyphMetrics>& glyphMetricsContainer ) = 0;

  /**
   * Write the metrics to the cache
   * @param[in] fontFamily The name of the font family
   * @param[in] fontStyle The name of the font style
   * @param[in] glyphSet The set of metrics to write
   */
  virtual void WriteMetricsToCache( const std::string& fontFamily,
                                    const std::string& fontStyle,
                                    const Integration::GlyphSet& glyphSet ) = 0;

}; // class PlatformAbstraction

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_PLATFORM_ABSTRACTION_H__
