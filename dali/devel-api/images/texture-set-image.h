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

// EXTERNAL INCLUDES
#include <stdlib.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

class Image;
class TextureSet;

/**
 * @brief Set the image at the specified position index.
 * @param[in] textureSet The TextureSet to use
 * @param[in] index The position in the TextureSet that the image will be set
 * @param[in] image The image to set
 */
DALI_IMPORT_API void TextureSetImage( TextureSet textureSet, size_t index, Image image );

/**
 * @brief Get the image at the specified position index.
 * @param[in] textureSet The TextureSet to use
 * @param[in] index The position in the TextureSet that the image will be set
 * @return A handle to the image at the the specified position
 */
DALI_IMPORT_API Image TextureGetImage( TextureSet textureSet, size_t index );


} //namespace Dali

#endif // __DALI_TEXTURE_SET_IMAGE_H__
