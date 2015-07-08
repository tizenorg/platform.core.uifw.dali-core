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
#include <dali/internal/render/gl-resources/frame-buffer-texture.h>

// INTERNAL INCLUDES
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/texture-units.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

FrameBufferTexture::FrameBufferTexture(unsigned int width, unsigned int height, Context& context)
: Texture( context,
           width, height,
           width, height ),
  mFrameBufferName(0),
  mRenderBufferName(0),
  mStencilBufferName(0),
  mPixelFormat(Pixel::RGBA8888),
  mBufferFormat(RenderBuffer::COLOR)
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);

}

FrameBufferTexture::FrameBufferTexture(unsigned int width, unsigned int height, Pixel::Format pixelFormat, Context& context)
: Texture( context,
           width, height,
           width, height ),
  mFrameBufferName(0),
  mRenderBufferName(0),
  mStencilBufferName(0),
  mPixelFormat( pixelFormat ),
  mBufferFormat(RenderBuffer::COLOR)
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);

}

FrameBufferTexture::FrameBufferTexture(unsigned int width, unsigned int height, Pixel::Format pixelFormat, RenderBuffer::Format bufferFormat, Context& context)
: Texture( context,
           width, height,
           width, height ),
  mFrameBufferName(0),
  mRenderBufferName(0),
  mStencilBufferName(0),
  mPixelFormat( pixelFormat ),
  mBufferFormat( bufferFormat )
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
}

FrameBufferTexture::~FrameBufferTexture()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
  // GlCleanup() should already have been called by TextureCache ensuring the resource is destroyed
  // on the render thread. (And avoiding a potentially problematic virtual call in the destructor)
}

bool FrameBufferTexture::IsFullyOpaque() const
{
  return true;
}

bool FrameBufferTexture::HasAlphaChannel() const
{
  return false;
}

bool FrameBufferTexture::Init()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
  return true;
}

bool FrameBufferTexture::Prepare()
{
  // bind texture
  Bind( GL_TEXTURE_2D, TEXTURE_UNIT_FRAMEBUFFER );

  if( 0 != mId )
  {
    // bind frame buffer
    mContext.BindFramebuffer(GL_FRAMEBUFFER, mFrameBufferName);

    return true;
  }

  // Texture could not be bound
  return false;
}

bool FrameBufferTexture::CreateGlTexture()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);

  mContext.GenTextures(1, &mId);
  mContext.ActiveTexture( TEXTURE_UNIT_UPLOAD );  // bind in unused unit so rebind works the first time
  mContext.Bind2dTexture(mId);

  // set texture parameters
  mContext.PixelStorei(GL_UNPACK_ALIGNMENT, 1);
  mContext.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  mContext.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Assign memory for texture in GL memory space
  GLenum pixelFormat = GL_RGBA;
  GLenum pixelDataType = GL_UNSIGNED_BYTE;
  Integration::ConvertToGlFormat(mPixelFormat, pixelDataType, pixelFormat);

  mContext.TexImage2D(GL_TEXTURE_2D, 0, pixelFormat, mWidth, mHeight, 0, pixelFormat, pixelDataType, NULL);

  // generate frame and render buffer names
  mContext.GenFramebuffers(1, &mFrameBufferName);

  /* WE ALWAYS HAVE COLOR COMPONENT */

  // bind frame buffer
  mContext.BindFramebuffer(GL_FRAMEBUFFER, mFrameBufferName);
  // attach texture to the color attachment point
  mContext.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mId, 0);

  if (mBufferFormat == RenderBuffer::COLOR_DEPTH_STENCIL)
  {
    mContext.GenRenderbuffers(1, &mRenderBufferName);

    // Bind render buffer and create 16 depth buffer
    mContext.BindRenderbuffer(GL_RENDERBUFFER, mRenderBufferName);
    mContext.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);

    // attach render buffer to the depth buffer attachment point
    mContext.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderBufferName);
    // attach render buffer to the stencil buffer attachment point
    mContext.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderBufferName);
  }
  else if (mBufferFormat == RenderBuffer::COLOR_DEPTH)
  {
    mContext.GenRenderbuffers(1, &mRenderBufferName);

    // Bind render buffer and create 8 stencil buffer
    mContext.BindRenderbuffer(GL_RENDERBUFFER, mRenderBufferName);
    mContext.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mWidth, mHeight);

    // attach render buffer to the depth buffer attachment point
    mContext.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderBufferName);
  }
  else if (mBufferFormat == RenderBuffer::COLOR_STENCIL)
  {
    mContext.GenRenderbuffers(1, &mStencilBufferName);

    // Bind render buffer and create 8 stencil buffer
    mContext.BindRenderbuffer(GL_RENDERBUFFER, mStencilBufferName);
    mContext.RenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, mWidth, mHeight);

    // attach render buffer to the depth buffer attachment point
    mContext.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mStencilBufferName);
  }

  int status = mContext.CheckFramebufferStatus(GL_FRAMEBUFFER);
  if ( GL_FRAMEBUFFER_COMPLETE != status )
  {
    DALI_LOG_ERROR( "status (0x%x), glError (0x%x)\n", status, mContext.GetError() );
    DALI_ASSERT_ALWAYS( false && "Frame buffer is not complete!" );
  }

  return mId != 0;
}

void FrameBufferTexture::GlCleanup()
{
  Texture::GlCleanup();

  if (mFrameBufferName != 0)
  {
    mContext.DeleteFramebuffers(1, &mFrameBufferName );
    mFrameBufferName = 0;
  }

  if (mRenderBufferName != 0)
  {
    mContext.DeleteRenderbuffers(1, &mRenderBufferName );
    mRenderBufferName = 0;
  }

  if (mStencilBufferName != 0)
  {
    mContext.DeleteRenderbuffers(1, &mStencilBufferName );
    mStencilBufferName = 0;
  }
}

} //namespace Internal

} //namespace Dali
