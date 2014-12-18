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
                              Pixel::Format pixelformat );

  TextureAtlas( unsigned int width, unsigned int height, Pixel::Format pixelformat );

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextureAtlas();

public:

  void Update ();

  PixelBuffer* GetAtlasBuffer();

  unsigned int GetAtlasBufferSize() const;

  const Vector2 GetAtlasSize() const;

  Dali::TextureAtlas::TextureAtlasResult Add( const unsigned int width, const unsigned int height );

  Dali::TextureAtlas::TextureAtlasResult Remove( const Dali::BitmapImage& bitmap );

  void SetSpacing( unsigned int pixels );

  void ShowBorders( const bool show );

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

  struct Block
  {
    unsigned int x;
    unsigned int y;
    unsigned int w;
    unsigned int h;
  };

  struct SelectedBlock
  {
    Block block;
    bool isRotated;
  };

  struct FreeBlock
  {
    Block block;
    unsigned int alternates[ 2 ];
  };

  struct FittingBlock
  {
      unsigned int index;
      bool isRotated;
  };

  std::vector< SelectedBlock > mTextures;
  std::vector< FreeBlock > mFreeBlocks;

  SelectedBlock FindSmallestBlock( const unsigned int width, const unsigned int height );
  void CopyInPlace( const unsigned int width, const unsigned int height, const SelectedBlock& block );
  void DiscardBlock( const unsigned int block );
  void DisplayFreeBlocks();

  Vector2                          mSize;
  PixelBuffer*                     mBuffer;
  ResourceClient*                  mResourceClient;
  unsigned int                     mBufferSize;
  unsigned int                     mSpacing;      // gap to leave between individual textures
  unsigned int                     mFreeBlockCount;
  Dali::TextureAtlas::ResizePolicy mResizePolicy;
  bool                             mShowBorders:1;

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
