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

// CLASS HEADER
#include "renderer-factory.h"

namespace Dali
{

namespace
{

#define MAKE_SHADER(A)#A

/**
 * @brief Returns the geometry for a unit square centred at the origin
 *
 * @return The unit square geometry
 */
Geometry GetUnitSquareGeometry()
{
  //@todo remove when RendererFactory geometry/material/shader cache is implemented
  static Geometry geometry;
  if( geometry )
  {
    return geometry;
  }

  // Create vertices
  Vector2 vertices[4] = { Vector2( -0.5f, -0.5f ),
                          Vector2(  0.5f, -0.5f ),
                          Vector2( -0.5f,  0.5f ),
                          Vector2(  0.5f,  0.5f ) };

  Property::Map vertexFormat;
  vertexFormat[ "aPosition" ] = Property::VECTOR2;
  Dali::PropertyBuffer vertexPropertyBuffer = Dali::PropertyBuffer::New( vertexFormat, 4 );
  vertexPropertyBuffer.SetData( vertices );

  // Create indices
  //TODO: replace with triangle strip when Geometry supports it
  unsigned int indices[ 6 ] = { 0, 3, 1, 0, 2, 3 };
  Property::Map indexFormat;
  indexFormat[ "indices" ] = Property::INTEGER;
  Dali::PropertyBuffer indexPropertyBuffer = Dali::PropertyBuffer::New( indexFormat, 6 );
  indexPropertyBuffer.SetData( indices );

  // Create the geometry object
  geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexPropertyBuffer );
  geometry.SetIndexBuffer( indexPropertyBuffer );

  return geometry;
}

} // unnamed namespace

//////////////////////////COLOR RENDERER///////////////////////
namespace ColorRenderer
{

const char* VERTEX_SHADER = MAKE_SHADER(
  attribute mediump vec2 aPosition;
  uniform mediump mat4 uMvpMatrix;
  uniform mediump vec3 uSize;

  void main()
  {
    mediump vec4 vertexPosition = vec4( aPosition * uSize.xy, 0.0, 1.0 );
    vertexPosition = uMvpMatrix * vertexPosition;

    gl_Position = vertexPosition;
  }
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
  uniform lowp vec4 uColor;
  uniform lowp vec4 uRendererColor;

  void main()
  {
    gl_FragColor = uRendererColor * uColor;
  }
);

} // namespace ColorRenderer

Renderer RendererFactory::GetColorRenderer( const Vector4& rendererColor )
{
  using namespace ColorRenderer;

  Renderer renderer;

  //@todo remove when RendererFactory geometry/material/shader cache is implemented
  static Material material;
  if( !material )
  {
    Shader shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
    material = Material::New( shader );
  }

  Geometry geometry = GetUnitSquareGeometry();

  renderer = Renderer::New( geometry, material );
  renderer.RegisterProperty( "uRendererColor", rendererColor );

  return renderer;
}

} // namespace Dali
