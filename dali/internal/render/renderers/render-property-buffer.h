#ifndef DALI_INTERNAL_RENDER_PROPERTY_BUFFER_H
#define DALI_INTERNAL_RENDER_PROPERTY_BUFFER_H

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/internal/common/owner-pointer.h>
#include <dali/public-api/actors/sampling.h>
#include <dali/devel-api/rendering/sampler.h>
#include <dali/internal/render/renderers/render-sampler.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>

namespace Dali
{
namespace Internal
{
namespace Render
{

class PropertyBuffer
{
public:

  struct Component
  {
    std::string     name;
    unsigned int    offset;
    unsigned int    size;
    Property::Type  type;
  };

  /**
   * Structure that holds the meta-data of the format of PropertyBuffer.
   */
  struct Format
  {
    std::vector<Component> components;
    unsigned int           size;
  };

  PropertyBuffer()
  :mFormat(0)
  ,mData(0)
  ,mSize(0)
  {}

  /**
   * @brief Set the format of the buffer
   *
   * This function takes ownership of the pointer
   *
   * @param[in] format The format for the PropertyBuffer
   */
  void SetFormat( PropertyBuffer::Format* format )
  {
    mFormat = format;
    mDataChanged = true;
  }

  /**
   * @brief Set the data of the PropertyBuffer
   *
   * This function takes ownership of the pointer
   * @param[in] data The new data of the PropertyBuffer
   */
  void SetData( Dali::Vector<char>* data )
  {
    mData = data;
    mDataChanged = true;
  }

  /**
   * @brief Set the number of elements
   *
   * @param[in] size The number of elements
   */
  void SetSize( unsigned int size )
  {
    mSize = size;
  }

  void BindBuffer();
  void Update( Context& context );
  unsigned int EnableVertexAttributes( Context& context, Vector<GLint>& vAttributeLocation, unsigned int locationBase );

private:
  OwnerPointer< PropertyBuffer::Format >  mFormat; ///< Format of the buffer
  OwnerPointer< Dali::Vector< char > >    mData; ///< Data
  OwnerPointer<GpuBuffer> mGpuBuffer;               ///< Pointer to the GpuBuffer associated with this RenderPropertyBuffer
  GpuBuffer::Target mGpuBufferTarget;               ///< The type of GPU buffer to create
  GpuBuffer::Usage mGpuBufferUsage;                 ///< The type of usage the  GPU buffer will have

  size_t  mSize; ///< Number of Elements in the buffer
  bool mDataChanged; ///< Flag to know if data has changed in a frame

};

} // namespace Render



} // namespace Internal
} // namespace Dali


#endif //  DALI_INTERNAL_RENDER_PROPERTY_BUFFER_H
