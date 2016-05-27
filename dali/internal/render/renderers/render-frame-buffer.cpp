/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 */

// CLASS HEADER
#include <dali/internal/render/renderers/render-frame-buffer.h>

//INTERNAL INCLUDES
#include <dali/internal/render/renderers/render-texture.h>

namespace Dali
{
namespace Internal
{
namespace Render
{

FrameBuffer::FrameBuffer()
:mId( 0 ),
 mWidth(0u),
 mHeight(0u)
{
}

FrameBuffer::~FrameBuffer()
{}

void FrameBuffer::Destroy( Context& context )
{
  if( mId )
  {
    context.DeleteFramebuffers( 1, &mId );
  }
}

void FrameBuffer::Initialize(Context& context)
{
  context.GenFramebuffers( 1, &mId );
}

void FrameBuffer::AttachColorTexture( Context& context, Render::NewTexture* texture )
{
  context.BindFramebuffer( GL_FRAMEBUFFER, mId );
  context.FramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->GetId(), 0 );
  context.BindFramebuffer( GL_FRAMEBUFFER, 0 );

  mWidth = texture->GetWidth();
  mHeight = texture->GetHeight();
}

void FrameBuffer::AttachDepthStencilTexture( Context& context, Render::NewTexture* texture )
{
  context.BindFramebuffer( GL_FRAMEBUFFER, mId );

  //Attach the texture to both depth and stencil attachment points
  GLuint textureId = texture->GetId();
  context.FramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureId, 0 );
  context.FramebufferTexture2D( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureId, 0 );

  context.BindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void FrameBuffer::Bind( Context& context )
{
  context.BindFramebuffer( GL_FRAMEBUFFER, mId );
}

unsigned int FrameBuffer::GetWidth() const
{
  return mWidth;
}

unsigned int FrameBuffer::GetHeight() const
{
  return mHeight;
}


} //Render

} //Internal

} //Dali
