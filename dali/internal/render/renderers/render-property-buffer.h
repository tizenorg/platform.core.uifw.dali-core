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

  PropertyBuffer();

  ~PropertyBuffer();
  /**
   * @brief Set the format of the buffer
   *
   * This function takes ownership of the pointer
   *
   * @param[in] format The format for the PropertyBuffer
   */
  void SetFormat( PropertyBuffer::Format* format );

  /**
   * @brief Set the data of the PropertyBuffer
   *
   * This function takes ownership of the pointer
   * @param[in] data The new data of the PropertyBuffer
   */
  void SetData( Dali::Vector<char>* data );

  /**
   * @brief Set the number of elements
   *
   * @param[in] size The number of elements
   */
  void SetSize( unsigned int size );

  void BindBuffer(GpuBuffer::Target target);
  bool Update( Context& context, bool isIndexBuffer );
  unsigned int EnableVertexAttributes( Context& context, Vector<GLint>& vAttributeLocation, unsigned int locationBase );


  unsigned int GetAttributeCount() const
  {
    DALI_ASSERT_DEBUG( mFormat && "Format should be set ");
    return mFormat->components.size();
  }

  const std::string& GetAttributeName(unsigned int index ) const
  {
    DALI_ASSERT_DEBUG( mFormat && "Format should be set ");
    return mFormat->components[index].name;
  }

  std::size_t GetDataSize() const
  {
    DALI_ASSERT_DEBUG( mFormat && "Format should be set ");

    return mFormat->size * mSize;
  }

  std::size_t GetElementSize() const
  {
    return mFormat->size;
  }

  unsigned int GetElementCount() const
  {
    return mSize;
  }

private:
  OwnerPointer< PropertyBuffer::Format >  mFormat; ///< Format of the buffer
  OwnerPointer< Dali::Vector< char > >    mData; ///< Data
  OwnerPointer<GpuBuffer> mGpuBuffer;               ///< Pointer to the GpuBuffer associated with this RenderPropertyBuffer

  size_t  mSize; ///< Number of Elements in the buffer
  bool mDataChanged; ///< Flag to know if data has changed in a frame

};

} // namespace Render



} // namespace Internal
} // namespace Dali


#endif //  DALI_INTERNAL_RENDER_PROPERTY_BUFFER_H
