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
#include <dali/internal/render/gl-resources/gpu-buffer.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Internal
{

namespace
{
/**
 * Helper to get our type enum as GL enum
 * @param type to convert
 * @return the corresponding GL enum or -1
 */
inline GLenum TypeAsGlEnum( GpuBuffer::Target type )
{
  GLenum retval( -1 );
  switch( type )
  {
    case GpuBuffer::ARRAY_BUFFER :
    {
      retval = GL_ARRAY_BUFFER;
      break;
    }
    case GpuBuffer::ELEMENT_ARRAY_BUFFER :
    {
      retval = GL_ELEMENT_ARRAY_BUFFER;
      break;
    }
    case GpuBuffer::TRANSFORM_FEEDBACK_BUFFER :
    {
      retval = GL_TRANSFORM_FEEDBACK_BUFFER;
      break;
    }
  }
  return retval;
}

/**
 * Helper to get our drawmode enum as GL enum
 * @param type to convert
 * @return the corresponding GL enum or -1
 */
inline GLenum ModeAsGlEnum( GpuBuffer::Usage type )
{
  GLenum retval( -1 );
  switch( type )
  {
    case GpuBuffer::STREAM_DRAW :
    {
      retval = GL_STREAM_DRAW;
      break;
    }
    case GpuBuffer::STATIC_DRAW :
    {
      retval = GL_STATIC_DRAW;
      break;
    }
    case GpuBuffer::DYNAMIC_DRAW :
    {
      retval = GL_DYNAMIC_DRAW;
      break;
    }
  }
  return retval;
}

}

GpuBuffer::GpuBuffer( Context& context )
: mContext( context ),
  mCapacity( 0 ),
  mSize( 0 ),
  mBufferId( 0 ),
  mBufferCreated( false )
{
}

GpuBuffer::~GpuBuffer()
{
  // If we have a buffer then delete it.
  if( mBufferId )
  {
    // If a buffer object that is currently bound is deleted, the binding reverts to 0
    // (the absence of any buffer object, which reverts to client memory usage)
    mContext.DeleteBuffers(1,&mBufferId);
  }
}

/*
 * Creates or updates the buffer data depending on whether it
 * already exists or not.
 */
void GpuBuffer::UpdateDataBuffer(GLsizeiptr size,const GLvoid *data, Usage usage)
{
  DALI_ASSERT_DEBUG( size > 0 );
  mSize = size;
  // make sure we have a buffer name/id before uploading
  if (mBufferId == 0)
  {
    mContext.GenBuffers(1,&mBufferId);
    DALI_ASSERT_DEBUG(mBufferId);
  }

  // make sure the buffer is bound, don't perform any checks because size may be zero
  mContext.BindArrayBuffer( mBufferId );

  // if the buffer has already been created, just update the data providing it fits
  if( mBufferCreated )
  {
    // if the data will fit in the existing buffer, just update it
    if (size <= mCapacity )
    {
      mContext.BufferSubData( GL_ARRAY_BUFFER, 0, size, data );
    }
    else
    {
      // create a new buffer of the larger size,
      // gl should automatically deallocate the old buffer
      mContext.BufferData( GL_ARRAY_BUFFER, size, data, ModeAsGlEnum( usage ) );
      mCapacity = size;
    }
  }
  else
  {
    // create the buffer
    mContext.BufferData( GL_ARRAY_BUFFER, size, data, ModeAsGlEnum( usage ) );
    mBufferCreated = true;
    mCapacity = size;
  }

  mContext.BindArrayBuffer(0);
}

void GpuBuffer::Bind(Target target) const
{
  DALI_ASSERT_DEBUG(mCapacity);

  if( target == ARRAY_BUFFER )
  {
    mContext.BindArrayBuffer(mBufferId);
  }
  else
  {
    mContext.BindElementArrayBuffer(mBufferId);
  }
}

bool GpuBuffer::BufferIsValid() const
{
  return mBufferCreated && (0 != mCapacity );
}

void GpuBuffer::GlContextDestroyed()
{
  // If the context is destroyed, GL would have released the buffer.
  mCapacity = 0;
  mSize = 0;
  mBufferId = 0;
  mBufferCreated = false;
}

} // namespace Internal

} //namespace Dali
