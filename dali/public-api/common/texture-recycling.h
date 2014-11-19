#ifndef DALI_COMMON_TEXTURE_RECYCLING_H
#define DALI_COMMON_TEXTURE_RECYCLING_H
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
 */

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/images/pixel.h>

namespace Dali
{
namespace Config
{

void DALI_IMPORT_API SetRecycledTextureWidth(unsigned int width);
unsigned int DALI_IMPORT_API GetRecycledTextureWidth();

void DALI_IMPORT_API SetRecycledTextureHeight(unsigned int height);
unsigned int DALI_IMPORT_API GetRecycledTextureHeight();

void DALI_IMPORT_API SetRecycledTexturePixelFormat(Pixel::Format pixelFormat);
Pixel::Format DALI_IMPORT_API GetRecycledTexturePixelFormat();

void DALI_IMPORT_API SetNumberOfRecycledTextures(unsigned int numberOfRecycledTextures);
unsigned int DALI_IMPORT_API GetNumberOfRecycledTextures();

} // namespace Config
} // namespace Dali

#endif // DALI_COMMON_TEXTURE_RECYCLING_H
