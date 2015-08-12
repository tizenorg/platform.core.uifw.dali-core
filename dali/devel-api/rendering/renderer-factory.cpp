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
#include "renderer-factory.h" // Dali::Internal::Renderer

#include <map>

// INTERNAL INCLUDES
#include <dali/public-api/images/resource-image.h>

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
  //@todo replace with triangle strip when Geometry supports it
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

//////////////////////////IMAGE RENDERER//////////////////////////
namespace ImageRenderer
{

const char* VERTEX_SHADER = MAKE_SHADER(
  attribute mediump vec2 aPosition;
  varying mediump vec2 vTexCoord;
  uniform mediump mat4 uMvpMatrix;
  uniform mediump vec3 uSize;

  void main()
  {
    mediump vec4 vertexPosition = vec4( aPosition * uSize.xy, 0.0, 1.0 );
    vertexPosition = uMvpMatrix * vertexPosition;

    vTexCoord = aPosition + 0.5;
    gl_Position = vertexPosition;
  }
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
  varying mediump vec2 vTexCoord;
  uniform sampler2D sTexture;
  uniform lowp vec4 uColor;

  void main()
  {
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;
  }
);

} //namespace ImageRenderer

Renderer RendererFactory::GetImageRenderer( Image image )
{
  using namespace ImageRenderer;

  //@todo remove when RendererFactory geometry/material/shader cache is implemented
  static std::map< Image, Material > generatedImageMaterials;
  static std::map< std::string, Material > generatedResourceMaterials;

  ResourceImage resourceImage = ResourceImage::DownCast( image );
  Material& material = resourceImage ? generatedResourceMaterials[ resourceImage.GetUrl() ] :
                                       generatedImageMaterials[ image ];

  if( !material )
  {
    //@todo remove when RendererFactory geometry/material/shader cache is implemented
    static Shader shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );

    material = Material::New( shader );
    Sampler sampler = Sampler::New( image, "sTexture" );
    material.AddSampler( sampler );
  }

  Geometry geometry = GetUnitSquareGeometry();

  return Renderer::New( geometry, material );
}

} // namespace Dali
