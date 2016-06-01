#ifndef DALI_INTERNAL_RENDER_FRAME_BUFFER_H
#define DALI_INTERNAL_RENDER_FRAME_BUFFER_H

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


// INTERNAL INCLUDES
#include <dali/devel-api/rendering/texture.h>
#include <dali/integration-api/resource-declarations.h>

#include <dali/internal/render/gl-resources/context.h>
#include <dali/integration-api/gl-defines.h>
#include <dali/internal/render/renderers/render-sampler.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
class NewTexture;

class FrameBuffer
{
public:

  /**
   * Constructor
   */
  FrameBuffer();

  /**
   * Destructor
   */
  ~FrameBuffer();

  /**
   * Creates a FrameBuffer object in the GPU.
   * @param[in] context The GL context
   */
  void Initialize(Context& context);

  /**
   * Deletes the framebuffer object from the GPU
   * @param[in] context The GL context
   */
  void Destroy( Context& context );

  /**
   * @brief Attach a texture for color rendering
   * param[in] context The GL context
   * @param[in] texture The texture that will be used as output when rendering
   */
  void AttachColorTexture( Context& context, Render::NewTexture* texture );

  /**
   * @brief Attach a texture to use as depth-stencil output
   * param[in] context The GL context
   * @param[in] texture The texture that will be used as depth-stencil buffer when rendering
   */
  void AttachDepthStencilTexture( Context& context, Render::NewTexture* texture );

  /**
   * @brief Bind the framebuffer
   * @param[in] context The GL context
   */
  void Bind( Context& context );

  /**
   * @brief Get the width of the FrameBuffer
   * @return The width of the framebuffer
   */
  unsigned int GetWidth() const;

  /**
   * @brief Get the height of the FrameBuffer
   * @return The height of the framebuffer
   */
  unsigned int GetHeight() const;

private:

  GLuint mId;
  unsigned int mWidth;
  unsigned int mHeight;
};


} // namespace Render

} // namespace Internal

} // namespace Dali


#endif //  DALI_INTERNAL_RENDER_FRAME_BUFFER_H
