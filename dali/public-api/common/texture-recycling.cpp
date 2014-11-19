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

#include "texture-recycling.h"
#include <dali/internal/event/common/thread-local-storage.h>

namespace Dali
{
namespace Config
{

void SetRecycledTextureWidth(unsigned int width)
{
  Internal::ThreadLocalStorage& tls = Internal::ThreadLocalStorage::Get();
  Internal::TextureRecyclingConfiguration& config = tls.GetTextureRecyclingConfiguration();

  config.width = width;
  UpdateTextureCacheConfigurationMessage( tls.GetUpdateManager(), config );
}

unsigned int GetRecycledTextureWidth()
{
  Internal::ThreadLocalStorage& tls = Internal::ThreadLocalStorage::Get();
  Internal::TextureRecyclingConfiguration& config = tls.GetTextureRecyclingConfiguration();
  return config.width;
}

void SetRecycledTextureHeight(unsigned int height)
{
  Internal::ThreadLocalStorage& tls = Internal::ThreadLocalStorage::Get();
  Internal::TextureRecyclingConfiguration& config = tls.GetTextureRecyclingConfiguration();

  config.height = height;
  UpdateTextureCacheConfigurationMessage( tls.GetUpdateManager(), config );
}

unsigned int GetRecycledTextureHeight()
{
  Internal::ThreadLocalStorage& tls = Internal::ThreadLocalStorage::Get();
  Internal::TextureRecyclingConfiguration& config = tls.GetTextureRecyclingConfiguration();
  return config.height;
}

void SetRecycledTexturePixelFormat(Pixel::Format pixelFormat)
{
  Internal::ThreadLocalStorage& tls = Internal::ThreadLocalStorage::Get();
  Internal::TextureRecyclingConfiguration& config = tls.GetTextureRecyclingConfiguration();

  config.pixelFormat = pixelFormat;
  UpdateTextureCacheConfigurationMessage( tls.GetUpdateManager(), config );
}

Pixel::Format GetRecycledTexturePixelFormat()
{
  Internal::ThreadLocalStorage& tls = Internal::ThreadLocalStorage::Get();
  Internal::TextureRecyclingConfiguration& config = tls.GetTextureRecyclingConfiguration();
  return config.pixelFormat;
}


void SetNumberOfRecycledTextures(unsigned int numberOfRecycledTextures)
{
  Internal::ThreadLocalStorage& tls = Internal::ThreadLocalStorage::Get();
  Internal::TextureRecyclingConfiguration& config = tls.GetTextureRecyclingConfiguration();

  config.maxNumberOfTextures = numberOfRecycledTextures;
  UpdateTextureCacheConfigurationMessage( tls.GetUpdateManager(), config );
}

unsigned int GetNumberOfRecycledTextures()
{
  Internal::ThreadLocalStorage& tls = Internal::ThreadLocalStorage::Get();
  Internal::TextureRecyclingConfiguration& config = tls.GetTextureRecyclingConfiguration();
  return config.maxNumberOfTextures;
}

} // Config
} // Dali
