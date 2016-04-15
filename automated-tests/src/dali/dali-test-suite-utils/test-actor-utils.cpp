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

// HEADER
#include "test-actor-utils.h"
#include "mesh-builder.h"

// EXTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
#include <dali/devel-api/rendering/renderer.h>

namespace Dali
{

namespace
{
const char * const TEXTURE_UNIFORM_NAME( "sTexture" );

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  \n
  void main()\n
  {\n
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
    vertexPosition.xyz *= uSize;\n
    gl_Position = uMvpMatrix * vertexPosition;\n
  }\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  uniform lowp vec4 uColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = uColor;\n
  }\n
);

} // unnamed namespace

Actor CreateRenderableActor()
{
  return CreateRenderableActor( Image(), VERTEX_SHADER, FRAGMENT_SHADER );
}

Actor CreateRenderableActor( Image texture )
{
  return CreateRenderableActor( texture, VERTEX_SHADER, FRAGMENT_SHADER );
}

Actor CreateRenderableActor( Image texture, const std::string& vertexShader, const std::string& fragmentShader )
{
  // Create the geometry
  Geometry geometry = CreateQuadGeometry();

  // Create Shader
  Shader shader = Shader::New( vertexShader, fragmentShader );

  // Create material from shader
  Material material = Material::New( shader );
  if( texture )
  {
    material.AddTexture( texture, TEXTURE_UNIFORM_NAME );
  }

  // Create renderer from geometry and material
  Renderer renderer = Renderer::New( geometry, material );

  // Create actor and set renderer
  Actor actor = Actor::New();
  actor.AddRenderer( renderer );

  // Set actor to the size of the texture if set
  if( texture )
  {
    actor.SetSize( texture.GetWidth(), texture.GetHeight() );
  }

  return actor;
}

Image GetTexture( Actor actor )
{
  Image image;
  if( actor && actor.GetRendererCount() )
  {
    Renderer renderer = actor.GetRendererAt( 0u );
    if( renderer )
    {
      Material material = renderer.GetMaterial();
      if( material && material.GetNumberOfTextures() )
      {
        image = material.GetTexture( 0u );
      }
    }
  }

  return image;
}

} // namespace Dali
