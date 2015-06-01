#ifndef __DALI_INTERNAL_SHADER_DECLARATIONS_H__
#define __DALI_INTERNAL_SHADER_DECLARATIONS_H__

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

// INTERNAL HEADERS
#include <dali/public-api/object/ref-object.h>

namespace Dali
{

namespace Internal
{

/**
 * Shader sub types.
 */
enum ShaderSubTypes
{
  SHADER_DEFAULT = 0,  ///< Use default shader for geometry type
  SHADER_SUBTYPE_LAST,

  SHADER_SUBTYPE_ALL = 0xFF,
};

class ShaderEffect;
typedef IntrusivePtr<ShaderEffect> ShaderEffectPtr;

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SHADER_DECLARATIONS_H__
