#ifndef __DALI_TEXTURE_SET_IMAGE_H__
#define __DALI_TEXTURE_SET_IMAGE_H__

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
 *
 */

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/texture-set-impl.h>

namespace Dali
{


/**
 * @brief Set the image at position "index"
 * @param[in] textureSet The TextureSet to use
 * @param[in] index The position in the TextureSet that the image will be set
 * @param[in] image The image to set
 */
void TextureSetImage( TextureSet textureSet, size_t index, Image image );


} //namespace Dali

#endif // __DALI_TEXTURE_SET_IMAGE_H__
