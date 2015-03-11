#ifndef DALI_INTERNAL_SCENE_GRAPH_MATERIAL_DATA_PROVIDER_H
#define DALI_INTERNAL_SCENE_GRAPH_MATERIAL_DATA_PROVIDER_H
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

#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class SamplerDataProvider;


class MaterialDataProvider
{
public:
  typedef Dali::Vector< const SamplerDataProvider* > Samplers;

  /**
   * Construtor
   */
  MaterialDataProvider()
  {
  }

  /**
   * Returns the list of samplers that this material provides
   * @return The list of samplers
   */
  virtual const Samplers& GetSamplers() const = 0;

protected:
  /**
   * Destructor. No deletion through this interface
   */
  virtual ~MaterialDataProvider()
  {
  }
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_MATERIAL_DATA_PROVIDER_H
