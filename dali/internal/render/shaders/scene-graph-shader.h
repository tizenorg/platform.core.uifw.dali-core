#ifndef __DALI_INTERNAL_SCENE_GRAPH_SHADER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_SHADER_H__

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
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/internal/common/shader-data.h>
#include <dali/internal/update/common/property-owner.h>

namespace Dali
{
namespace Internal
{

class Program;
class ProgramCache;

namespace SceneGraph
{

/**
 * A holder class for Program; also enables sharing of uniform properties
 */
class Shader : public PropertyOwner
{
public:

  /**
   * Constructor
   * @param hints Geometry hints
   */
  Shader( Dali::ShaderEffect::GeometryHints& hints );

  /**
   * Virtual destructor
   */
  virtual ~Shader();

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // The following methods are called during UpdateManager::Update()
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Query whether a shader geometry hint is set.
   * @pre The shader has been initialized.
   * @param[in] hint The geometry hint to check.
   * @return True if the given geometry hint is set.
   */
  bool GeometryHintEnabled( Dali::ShaderEffect::GeometryHints hint ) const
  {
    return mGeometryHints & hint;
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex )
  {
    // no default properties
  }


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // The following methods are called in Render thread
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  /**
   * @brief Set the program for this shader.
   * @param[in] shaderData        The program's vertex/fragment source and optionally precompiled shader binary.
   * @param[in] programCache      Owner of the Programs.
   * @param[in] modifiesGeometry  True if the vertex shader changes the positions of vertexes such that
   * they might exceed the bounding box of vertexes passing through the default transformation.
   */
  void SetProgram( Internal::ShaderDataPtr shaderData,
                   ProgramCache* programCache,
                   bool modifiesGeometry );

  /**
   * Get the program built for this shader
   * @return The program built from the shader sources.
   */
  Program* GetProgram();

private: // Data

  Dali::ShaderEffect::GeometryHints mGeometryHints;    ///< shader geometry hints for building the geometry

  Program*                       mProgram;
};


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_SHADER_H__
