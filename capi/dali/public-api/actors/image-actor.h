#ifndef __DALI_IMAGE_ACTOR_H__
#define __DALI_IMAGE_ACTOR_H__

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
#include <dali/public-api/actors/renderable-actor.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/images/image.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class ImageActor;
}

/**
 * An actor for displaying images.
 *
 * Allows the developer to add an actor to stage which displays the content of an Image object.
 *
 * By default CullFaceMode is set to CullNone to enable the ImageActor to be viewed from all angles.
 */
class DALI_IMPORT_API ImageActor : public RenderableActor
{
public:

  // Default Properties additional to RenderableActor
  static const Property::Index PIXEL_AREA;           ///< name "pixel-area",          type RECTANGLE
  static const Property::Index FADE_IN;              ///< name "fade-in",             type BOOLEAN
  static const Property::Index FADE_IN_DURATION;     ///< name "fade-in-duration",    type FLOAT
  static const Property::Index STYLE;                ///< name "style",               type STRING
  static const Property::Index BORDER;               ///< name "border",              type VECTOR4
  static const Property::Index IMAGE;                ///< name "image",               type MAP {"filename":"", "load-policy":...}

  /**
  * Style determines how the Image is rendered.
  *
  * @code
  * STYLE_QUAD:
  *
  *  0---------2           0-----------------2
  *  |        /|           |                /|
  *  |  A    / |           |      A       /  |
  *  |      /  |           |            /    |
  *  |     /   | SCALE (X) |          /      |
  *  |    /    | --------> |        /        |
  *  |   /     |           |      /          |
  *  |  /      |           |    /            |
  *  | /    B  |           |  /        B     |
  *  |/        |           |/                |
  *  1---------3           1-----------------3
  *
  * Image is rendered as a textured rectangle. The texture
  * is scaled uniformly as the quad is resized.
  *
  * STYLE_NINE_PATCH:
  *
  *  |---|---------------|---|       |---|-----------------------------|---|
  *  | 1 |       2       | 3 |       | 1 |              2              | 3 |
  *  |---|---------------|---|       |---|-----------------------------|---|
  *  |   |               |   |       |   |                             |   |
  *  |   |               |   |       |   |                             |   |
  *  | 4 |       5       | 6 | SCALE |   |                             |   |
  *  |   |               |   | ----> |   |                             |   |
  *  |   |               |   |       | 4 |              5              | 6 |
  *  |-------------------|---|       |   |                             |   |
  *  | 7 |       8       | 9 |       |   |                             |   |
  *  |---|---------------|---|       |   |                             |   |
  *                                  |---------------------------------|---|
  *                                  | 7 |              8              | 9 |
  *                                  |---|-----------------------------|---|
  *
  * @endcode
  * Image is rendered as a textured rectangle. The texture
  * is scaled differently over each of the 9 sections.
  *
  * Visualise a Picture Frame:
  *
  * - Corner sections (1,3,7,9) are not scaled, regardless
  * of how big the Image is.
  * - Horizontal edge sections (2,8) are scaled only in the
  * X axis as the image increases in width.
  * - Vertical edge sections (4,6) are scaled only in the
  * Y axis as the image increases in height.
  * - Center section (5) is scaled in both X and Y axes as
  * the image increases in width and/or height.
  *
  * Note: If GRID hints are enabled (via a Shader that requires it),
  * the above geometry will be further subdivided into rectangles of
  * approx. 40x40 in size.
  *
  */
  enum Style
  {
    STYLE_QUAD,       ///< As a simple quad.
    STYLE_NINE_PATCH  ///< As a nine-patch.
  };

  /**
   * Pixel area is relative to the top-left (0,0) of the image.
   */
  typedef Rect<int> PixelArea;

  /**
   * Create an uninitialized ImageActor handle; this can be initialized with ImageActor::New(...)
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  ImageActor();

  /**
   * Create an empty image actor object.
   * @return A handle to a newly allocated actor.
   */
  static ImageActor New();

  /**
   * Create a image actor object.
   * When the image is loaded the actors size will reset to the image size,
   * unless a custom size chosen via Actor:SetSize().
   * @pre image must be initialized.
   * @param[in] image The image to display.
   * @return A handle to a newly allocated actor.
   */
  static ImageActor New(Image image);

  /**
   * Create a image actor object.
   * When the image is loaded the actors size will reset to the pixelArea,
   * unless a custom size was chosen via Actor:SetSize().
   * @pre image must be initialized.
   * @param [in] image The image to display.
   * @param [in] pixelArea The area of the image to display.
   * This in pixels, relative to the top-left (0,0) of the image.
   * @return A handle to a newly allocated actor.
   */
  static ImageActor New(Image image, PixelArea pixelArea);

  /**
   * Downcast an Object handle to ImageActor. If handle points to a ImageActor the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a ImageActor or an uninitialized handle
   */
  static ImageActor DownCast( BaseHandle handle );

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~ImageActor();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * Set the image rendered by the actor.
   * When the image is loaded the actors size will be reset to the image size,
   * unless a custom size was chosen via Actor:SetSize().
   * @note The old image will continue to be displayed until the given image has loaded.
   * @pre image must be initialized.
   * @param [in] image The image to display.
   */
  void SetImage(Image image);

  /**
   * Retrieve the image rendered by the actor's attachment.
   * @return The image.
   */
  Image GetImage();

  /**
   * Set a region of the image to display, in pixels.
   * When the image is loaded the actors size will be reset to the pixelArea,
   * unless a custom size chosen via Actor:SetSize().
   * Note! PixelArea should be inside the image data size. It gets clamped by GL
   * @pre image must be initialized.
   * @param [in] pixelArea The area of the image to display.
   * This in pixels, relative to the top-left (0,0) of the image.
   */
  void SetPixelArea(const PixelArea& pixelArea);

  /**
   * Retrieve the region of the image to display, in pixels.
   * @pre image must be initialized.
   * @return The pixel area, or a default-constructed area if none was set.
   */
  PixelArea GetPixelArea() const;

  /**
   * Query whether a pixel area has been set.
   * @pre image must be initialized.
   * @return True if a pixel area has been set.
   */
  bool IsPixelAreaSet() const;

  /**
   * Remove any pixel areas specified with SetPixelArea; the entire image will be displayed.
   * @pre image must be initialized.
   */
  void ClearPixelArea();

  /**
   * Set how the image is rendered; the default is STYLE_QUAD.
   * @pre image must be initialized.
   * @param [in] style The new style.
   */
  void SetStyle(Style style);

  /**
   * Query how the image is rendered.
   * @pre image must be initialized.
   * @return The rendering style.
   */
  Style GetStyle() const;

  /**
   * Set the border used with STYLE_NINE_PATCH.
   * The values are in pixels from the left, top, right, and bottom of the image respectively.
   * i.e. SetNinePatchBorder( Vector4(1,2,3,4) ) sets the left-border to 1, top-border to 2, right-border to 3, and bottom-border to 4 pixels.
   * @param [in] border The new nine-patch border.
   */
  void SetNinePatchBorder(const Vector4& border);

  /**
   * Retrieve the border used with STYLE_NINE_PATCH.
   * @return The nine-patch border.
   */
  Vector4 GetNinePatchBorder() const;

  /**
   * Set whether the image should gradually fade in when first rendered.
   * @pre image must be initialized.
   * @param [in] enableFade True if the image should fade in.
   */
  void SetFadeIn(bool enableFade);

  /**
   * Query whether the image will gradually fade in when first rendered.
   * @pre image must be initialized.
   * @return True if the image will fade in.
   */
  bool GetFadeIn() const;

  /**
   * Set the duration of the fade-in effect; the default is 1 second.
   * @pre image must be initialized.
   * @param [in] durationSeconds The duration in seconds.
   */
  void SetFadeInDuration(float durationSeconds);

  /**
   * Retrieve the duration of the fade-in effect.
   * @pre image must be initialized.
   * @return The duration in seconds.
   */
  float GetFadeInDuration() const;

  /**
   * Retrieve the size of the displayed image within the image actor.  The size of the image may be
   * different to that of the image actor size depending on the geometry scaling used.
   * @pre image must be initialized.
   * @return The actual size of the image shown.
   * @note If a pixel area is set then this returns the size of the pixel area shown.
   */
  Vector2 GetCurrentImageSize() const;

public: // Not intended for application developers

  explicit DALI_INTERNAL ImageActor(Internal::ImageActor*);
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_IMAGE_ACTOR_H__
