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
#include <dali/internal/render/gl-resources/bitmap-texture.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/render/common/vertex.h>
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/texture-units.h>

namespace Dali
{

namespace Internal
{

BitmapTexture::BitmapTexture(
  Integration::Bitmap* const bitmap,
  const Integration::Bitmap::PackedPixelsProfile * const bitmapPackedPixelsProfile,
  Context& context,
  ResourcePolicy::Discardable policy)
: Texture( context,
           bitmapPackedPixelsProfile->GetBufferWidth(),
           bitmapPackedPixelsProfile->GetBufferHeight(),
           bitmap->GetImageWidth(),
           bitmap->GetImageHeight(),
           bitmap->GetPixelFormat()),
  mBitmap(bitmap),
  mClearPixels(false),
  mDiscardPolicy(policy)
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
  DALI_LOG_SET_OBJECT_STRING(this, DALI_LOG_GET_OBJECT_STRING(bitmap));
}

BitmapTexture::BitmapTexture(
  unsigned int width,
  unsigned int height,
  Pixel::Format pixelFormat,
  bool clearPixels,
  Context& context,
  ResourcePolicy::Discardable policy)
: Texture( context,
           width, height,
           width, height,
           pixelFormat),
  mBitmap(NULL),
  mClearPixels(clearPixels),
  mDiscardPolicy(policy)
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
}

BitmapTexture::~BitmapTexture()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);

  // GlCleanup() should already have been called by TextureCache ensuring the resource is destroyed
  // on the render thread. (And avoiding a potentially problematic virtual call in the destructor)
}

bool BitmapTexture::HasAlphaChannel() const
{
  return Pixel::HasAlpha(mPixelFormat);
}

bool BitmapTexture::IsFullyOpaque() const
{
  if( mBitmap )
  {
    return mBitmap->IsFullyOpaque();
  }
  else
  {
    return ! HasAlphaChannel(); // Todo: amalgamate updated bitmap's IsFullyOpaque()
  }
}

// Bitmap buffer has been changed. Upload changes to GPU.
void BitmapTexture::AreaUpdated( const RectArea& updateArea, const unsigned char* pixels )
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::Verbose, "BitmapTexture::AreaUpdated()\n");

  GLenum pixelFormat   = GL_RGBA;
  GLenum pixelDataType = GL_UNSIGNED_BYTE;
  Integration::ConvertToGlFormat(mPixelFormat, pixelDataType, pixelFormat);

  mContext.ActiveTexture( TEXTURE_UNIT_UPLOAD );

  mContext.Bind2dTexture(mId);

  if( ! updateArea.IsEmpty() )
  {
    mContext.PixelStorei( GL_UNPACK_ALIGNMENT, 1 ); // We always use tightly packed data
    DALI_LOG_INFO( Debug::Filter::gImage, Debug::General, "Update x:%d y:%d w:%d h:%d\n",
                   updateArea.x, updateArea.y, updateArea.width ,updateArea.height );

    // TODO: obtain pitch of source image, obtain pixel depth of source image.
    const unsigned int pitchPixels = mImageWidth;
    const unsigned int pixelDepth = sizeof(unsigned int);

    // If the width of the source update area is the same as the pitch, then can
    // copy the contents in a single contiguous TexSubImage call.
    if(updateArea.x == 0 && updateArea.width == pitchPixels)
    {
      pixels += updateArea.y * pitchPixels * pixelDepth;
      mContext.TexSubImage2D( GL_TEXTURE_2D,0, updateArea.x, updateArea.y,
                              updateArea.width, updateArea.height,
                              pixelFormat, pixelDataType, pixels );
    }
    else
    {
      // Otherwise the source buffer needs to be copied line at a time, as OpenGL ES
      // does not support source strides. (no GL_UNPACK_ROW_LENGTH supported)
      unsigned int yBottom = updateArea.y + updateArea.height;
      pixels += (updateArea.y * pitchPixels + updateArea.x) * pixelDepth;

      for(unsigned int y = updateArea.y; y < yBottom; y++)
      {
        mContext.TexSubImage2D( GL_TEXTURE_2D,0, updateArea.x, y,
                                updateArea.width, 1,
                                pixelFormat, pixelDataType, pixels );
        pixels += pitchPixels * pixelDepth;
      }
    }

    INCREASE_BY( PerformanceMonitor::TEXTURE_DATA_UPLOADED,
                 updateArea.Area()* GetBytesPerPixel( mPixelFormat ));
  }
}

void BitmapTexture::AssignBitmap( bool generateTexture, const unsigned char* pixels )
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::Verbose, "BitmapTexture::AssignBitmap()\n");

  GLenum pixelFormat = GL_RGBA;
  GLenum pixelDataType = GL_UNSIGNED_BYTE;

  if( generateTexture )
  {
    mContext.GenTextures(1, &mId);
  }
  DALI_ASSERT_DEBUG( mId != 0 );

  mContext.ActiveTexture( TEXTURE_UNIT_UPLOAD );
  mContext.Bind2dTexture(mId);
  Integration::ConvertToGlFormat(mPixelFormat, pixelDataType, pixelFormat);

  mContext.PixelStorei(GL_UNPACK_ALIGNMENT, 1); // We always use tightly packed data
  mContext.TexImage2D(GL_TEXTURE_2D, 0, pixelFormat, mWidth, mHeight, 0, pixelFormat, pixelDataType, pixels);
  mContext.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  mContext.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  INCREASE_BY( PerformanceMonitor::TEXTURE_DATA_UPLOADED, GetBytesPerPixel(mPixelFormat) * mWidth * mHeight );
}

void BitmapTexture::Update( Integration::Bitmap* bitmap )
{
  DALI_LOG_INFO( Debug::Filter::gGLResource, Debug::General, "BitmapTexture::Update(bitmap:%p )\n", bitmap );
  DALI_ASSERT_DEBUG( bitmap != 0 );
  if( !bitmap )
  {
    DALI_LOG_ERROR( "Passed a null bitmap to update this bitmap texture." );
    return;
  }

  // Only Packed-pixel bitmaps are ever associated with BitmapTextures, so we should never be passed any other kind:
  const Integration::Bitmap::PackedPixelsProfile * const bitmapPackedPixels = bitmap->GetPackedPixelsProfile();
  DALI_ASSERT_DEBUG(bitmapPackedPixels);
  if( !bitmapPackedPixels )
  {
    ///! This should never happen.
    DALI_LOG_ERROR("Passed an incompatible bitmap type to update this bitmap texture.");
    return;
  }
  mBitmap = bitmap;

  const unsigned char* pixels = mBitmap->GetBuffer();

  // We should never have null pixel data here - resource manager has deliberately loaded/reloaded data

  DALI_ASSERT_DEBUG( pixels != NULL );

  if( NULL == pixels )
  {
    DALI_LOG_ERROR("BitmapTexture::Upload() - Bitmap has no pixel data.\n");
  }
  else if( mId != 0 )
  {
    if( mImageWidth == mBitmap->GetImageWidth() &&
        mImageHeight == mBitmap->GetImageHeight() &&
        mWidth  == bitmapPackedPixels->GetBufferWidth() &&
        mHeight == bitmapPackedPixels->GetBufferHeight() &&
        mPixelFormat == mBitmap->GetPixelFormat() ) // and size hasn't changed
    {
      RectArea area(0, 0, mImageWidth, mImageHeight);  // just update whole texture
      AreaUpdated( area, pixels );
      DiscardBitmapBuffer();
    }
    else // Otherwise, reload the pixel data
    {
      mImageWidth  = mBitmap->GetImageWidth();
      mImageHeight = mBitmap->GetImageHeight();
      mWidth       = bitmapPackedPixels->GetBufferWidth();
      mHeight      = bitmapPackedPixels->GetBufferHeight();
      mPixelFormat = mBitmap->GetPixelFormat();

      AssignBitmap( false, pixels );
    }
  }
}

void BitmapTexture::Update( Integration::Bitmap* srcBitmap, std::size_t xOffset, std::size_t yOffset )
{
  if( NULL != srcBitmap )
  {
    GLenum pixelFormat   = GL_RGBA;
    GLenum pixelDataType = GL_UNSIGNED_BYTE;
    Integration::ConvertToGlFormat( mPixelFormat, pixelDataType, pixelFormat );

    if( !mId )
    {
      mContext.GenTextures( 1, &mId );

      mContext.ActiveTexture( TEXTURE_UNIT_UPLOAD );
      mContext.Bind2dTexture( mId );
      mContext.PixelStorei( GL_UNPACK_ALIGNMENT, 1 );

      mContext.TexImage2D( GL_TEXTURE_2D, 0, pixelFormat, mWidth, mHeight, 0, pixelFormat, pixelDataType, NULL );
      mContext.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
      mContext.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    }
    else
    {
      mContext.ActiveTexture( TEXTURE_UNIT_UPLOAD );
      mContext.Bind2dTexture( mId );
      mContext.PixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    }

    //const unsigned char* pixels = DEBUG_PIXELS;
    const unsigned char* pixels = srcBitmap->GetBuffer();
    unsigned int srcWidth = srcBitmap->GetImageWidth();
    const unsigned int pixelDepth = Pixel::GetBytesPerPixel( mPixelFormat );

    unsigned int yBottom = yOffset + srcBitmap->GetImageHeight();

    for( unsigned int y = yOffset; y < yBottom; y++ )
    {
      mContext.TexSubImage2D( GL_TEXTURE_2D, 0,
                              xOffset, y,
                              srcWidth, 1,
                              pixelFormat, pixelDataType, pixels );

      pixels += srcWidth * pixelDepth;
    }
  }
}

void BitmapTexture::UpdateArea( const RectArea& updateArea )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "BitmapTexture::UpdateArea()\n");

  if( mBitmap != 0 )
  {
    const unsigned char* pixels = mBitmap->GetBuffer();

    // Pixel data could be null if we've uploaded to GL and discarded the data.

    if( NULL != pixels )
    {
      if( mId ) // If the texture is already bound
      {
        if( updateArea.IsEmpty() )
        {
          RectArea area;
          area.x = 0;
          area.y = 0;
          area.width = mImageWidth;
          area.height = mImageHeight;
          AreaUpdated( area, pixels );
        }
        else
        {
          AreaUpdated( updateArea, pixels );
        }
      }
    }
  }
}

bool BitmapTexture::UpdateOnCreate()
{
  return true;
}

bool BitmapTexture::CreateGlTexture()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
  DALI_LOG_INFO(Debug::Filter::gImage, Debug::Verbose, "BitmapTexture::CreateGlTexture() Bitmap: %s\n", DALI_LOG_GET_OBJECT_C_STR(this));

  if( mBitmap )
  {
    const unsigned char* pixels = mBitmap->GetBuffer();

    // pixel data could be NULL here if we've had a context loss and we previously discarded
    // the pixel data on the previous upload. If it is null, then we shouldn't generate a
    // new GL Texture; leaving mId as zero. Eventually, the bitmap will get reloaded,
    // and pixels will become non-null.

    if( NULL != pixels )
    {
      AssignBitmap( true, pixels );
      DiscardBitmapBuffer();
    }
  }
  else
  {
    const unsigned char* pixels = NULL;
    std::vector<unsigned char> pixelData;
    if( ( NULL == pixels ) && ( true == mClearPixels ) )
    {
      unsigned int size = mWidth * mHeight * Pixel::GetBytesPerPixel(mPixelFormat);
      pixelData.resize(size, 0);
      pixels = &pixelData[0];
    }
    AssignBitmap( true, pixels );
  }

  return mId != 0;
}

bool BitmapTexture::Init()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
  // mBitmap should be initialized by now
  return (mBitmap != 0);
}

unsigned int BitmapTexture::GetWidth() const
{
  unsigned int width = mWidth;
  if( mBitmap )
  {
    width = mBitmap->GetImageWidth();
  }
  return width;
}

unsigned int BitmapTexture::GetHeight() const
{
  unsigned int height = mHeight;
  if( mBitmap )
  {
    height = mBitmap->GetImageHeight();
  }
  return height;
}

void BitmapTexture::DiscardBitmapBuffer()
{
  DALI_LOG_INFO(Debug::Filter::gImage, Debug::General, "BitmapTexture::DiscardBitmapBuffer() DiscardPolicy: %s\n", mDiscardPolicy == ResourcePolicy::DISCARD?"DISCARD":"RETAIN");

  if( ResourcePolicy::DISCARD == mDiscardPolicy )
  {
    DALI_LOG_INFO(Debug::Filter::gImage, Debug::General, "  Discarding bitmap\n");
    mBitmap->DiscardBuffer();
  }
}


} //namespace Internal

} //namespace Dali
