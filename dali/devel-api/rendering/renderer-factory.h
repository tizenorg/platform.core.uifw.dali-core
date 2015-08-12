#ifndef DALI_INTERNAL_RENDERER_FACTORY_H
#define DALI_INTERNAL_RENDERER_FACTORY_H

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
 *
 */

// INTERNAL INCLUDES
#include <dali/devel-api/rendering/renderer.h>
#include <dali/public-api/images/image.h>

namespace Dali
{

/**
 * RendererFactory class.
 * A factory for creating different types of renderers
 */
class RendererFactory
{
public:

  /**
   * Gets the renderer for rendering an image
   * @param[in] image   The image to render
   * @return The renderer that will render the image
   */
  static Renderer GetImageRenderer( Image image );
};

} // namespace Dali

#endif // DALI_INTERNAL_IMAGE_RENDERER_FACTORY_H
