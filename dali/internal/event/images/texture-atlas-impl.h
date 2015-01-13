#ifndef __DALI_INTERNAL_TEXTURE_ATLAS_H__
#define __DALI_INTERNAL_TEXTURE_ATLAS_H__

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
#include <dali/public-api/object/ref-object.h>
#include <dali/internal/event/images/bitmap-image-impl.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/texture-atlas.h>

namespace Dali
{

namespace Internal
{

class TextureAtlas;
typedef IntrusivePtr<TextureAtlas> TextureAtlasPtr;

class ResourceClient;
class ResourceManager;

class TextureAtlas : public Image
{
public:

  static TextureAtlasPtr New( unsigned int width,
                              unsigned int height,
                              unsigned int spacing,
                              Pixel::Format pixelformat);

  TextureAtlas( unsigned int width, unsigned int height, unsigned int spacing, Pixel::Format pixelformat );

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextureAtlas();

public:

  void Update ();

  unsigned int GetAtlasBufferSize() const;

  const Vector2 GetAtlasSize() const;

  const unsigned int GetSpacing() const;

  Dali::TextureAtlas::AddResult Add( unsigned int width, unsigned int height );

  void Remove( unsigned int id );

  void ShowBorders( const bool show );

  /**
   * @brief Install the Packing Algorithm to use for this Atlas ( note this will remove all current textures )
   *
   * @param packingAlgorithm Implementation from PackingAlgorithm class
   */
  void InstallPackingAlgorithm( Dali::TextureAtlas::PackingAlgorithm& packingAlgorithm );

  const unsigned int Count() const;

  Dali::TextureAtlas::Result ForceSort();

  Dali::UVBlock GetUVCoordinates( unsigned int id );


protected: // From Resource
  /**
   * @copydoc Dali::Internal::Image::Connect
   */
  virtual void Connect();

  /**
   * @copydoc Dali::Internal::Image::Disconnect
   */
  virtual void Disconnect();

  /**
   * Get the bitmap from local cache or ticket.
   **/
  Integration::Bitmap* GetBitmap() const;

private:

  enum AtlasState
  {
    ATLAS_CREATING,
    ATLAS_RESIZING
  };

  void CopyInPlace( unsigned int width, unsigned int height, const Dali::TextureAtlas::SelectedBlock& block );
  void Resize();

  Vector2                                   mSize;
  PixelBuffer*                              mBuffer;
  ResourceClient*                           mResourceClient;
  unsigned int                              mBufferSize;
  unsigned int                              mSpacing;      // gap to leave between individual textures
  Dali::TextureAtlas::ResizePolicy          mResizePolicy;
  bool                                      mShowBorders:1;
  std::vector< Dali::TextureAtlas::Block >  mTextures;
  Dali::TextureAtlas::PackingAlgorithm*     mPackingAlgorithm;

protected:
  Integration::BitmapPtr     mBitmapCached;
};

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::TextureAtlas& GetImplementation(Dali::TextureAtlas& image)
{
  DALI_ASSERT_ALWAYS( image && "TextureAtlas handle is empty" );

  BaseObject& handle = image.GetBaseObject();

  return static_cast<Internal::TextureAtlas&>(handle);
}

inline const Internal::TextureAtlas& GetImplementation(const Dali::TextureAtlas& image)
{
  DALI_ASSERT_ALWAYS( image && "TextureAtlas handle is empty" );

  const BaseObject& handle = image.GetBaseObject();

  return static_cast<const Internal::TextureAtlas&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_TEXTURE_ATLAS_H__
