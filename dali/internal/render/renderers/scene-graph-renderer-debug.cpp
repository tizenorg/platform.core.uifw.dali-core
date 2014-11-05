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

#include "scene-graph-renderer-debug.h"
#include <dali/integration-api/shader-data.h>
#include <dali/integration-api/resource-declarations.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/renderers/render-data-provider.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

namespace
{
#if defined ( DEBUG_ENABLED )

// Create shader for debug drawing
const std::string DEBUG_DRAW_VERTEX_SHADER(
  "attribute mediump vec3 aPosition;\n"
  "uniform mediump mat4 uMvpMatrix;\n"
  "void main()\n"
  "{\n"
  "  gl_Position = uMvpMatrix * vec4(aPosition, 1.0);\n"
  "}\n" );

const std::string DEBUG_DRAW_FRAGMENT_SHADER(
  "uniform lowp vec4 uColor;\n"
  "void main()\n"
  "{\n"
  "  gl_FragColor = uColor;\n"
  "}\n" );

static GLuint gDebugProgram(0u); ///< a simple debug shader

inline void DebugCompileShader(Integration::GlAbstraction& gl, const std::string& source, GLenum shaderType)
{
  GLuint shaderId = gl.CreateShader(shaderType);
  gl.AttachShader(gDebugProgram, shaderId);
  const char* sourceStr = source.c_str();
  gl.ShaderSource(shaderId, 1, &sourceStr, NULL);
  gl.CompileShader(shaderId);
  GLint compiled;
  gl.GetShaderiv( shaderId, GL_COMPILE_STATUS, &compiled);
  if( !compiled )
  {
    DALI_LOG_ERROR("Bounding box shader failed to compile.\n");
  }
}

#endif

} // anonymous namespace


void DebugBoundingBox(Context& context, Rect<float> boundingBox, const Matrix& mvp)
{
#if defined ( DEBUG_ENABLED )
  Integration::GlAbstraction& gl = context.GetAbstraction();

  if( gDebugProgram == 0u )
  {
    // Don't use internal Programs - use GL directly.
    gDebugProgram = gl.CreateProgram();
    DebugCompileShader(gl, DEBUG_DRAW_VERTEX_SHADER, GL_VERTEX_SHADER);
    DebugCompileShader(gl, DEBUG_DRAW_FRAGMENT_SHADER, GL_FRAGMENT_SHADER);
    gl.LinkProgram( gDebugProgram );
    GLint linked;
    gl.GetProgramiv( gDebugProgram, GL_LINK_STATUS, &linked);
    if( !linked )
    {
      DALI_LOG_ERROR("Bounding box shader failed to link.\n");
    }
  }

  // Save current blend and cull settings ( to prevent breaking context's cache )
  bool blendEnabled = gl.IsEnabled(GL_BLEND);
  bool cullFaceEnabled = gl.IsEnabled(GL_CULL_FACE);

  // Disable blending and culling
  gl.Disable(GL_BLEND);
  gl.Disable(GL_CULL_FACE);

  const unsigned int numPoints=8;
  GLfloat vertices[numPoints*3] = {
    boundingBox.x,                      boundingBox.y, 0.0f,
    boundingBox.x,                      boundingBox.y + boundingBox.height, 0.0f,

    boundingBox.x,                      boundingBox.y + boundingBox.height, 0.0f,
    boundingBox.x + boundingBox.width,  boundingBox.y + boundingBox.height, 0.0f,

    boundingBox.x + boundingBox.width,  boundingBox.y + boundingBox.height, 0.0f,
    boundingBox.x + boundingBox.width,  boundingBox.y, 0.0f,

    boundingBox.x + boundingBox.width,  boundingBox.y, 0.0f,
    boundingBox.x,                      boundingBox.y, 0.0f,
  };

  gl.UseProgram( gDebugProgram );

  GpuBuffer vertexBuffer(context,GpuBuffer::ARRAY_BUFFER, GpuBuffer::STATIC_DRAW);
  vertexBuffer.UpdateDataBuffer(numPoints * 3 * sizeof(float), &vertices[0]);
  vertexBuffer.Bind();

  GLint positionLoc    = gl.GetAttribLocation(gDebugProgram, "aPosition");
  gl.VertexAttribPointer( positionLoc, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0 );

  // Can't easily bypass context's caching of attribute arrays.
  // Also means we have to use context for performing DrawArrays().
  context.EnableVertexAttributeArray( positionLoc );

  GLint mvpLoc = gl.GetUniformLocation(gDebugProgram, "uMvpMatrix");
  if( mvpLoc != -1 )
  {
    gl.UniformMatrix4fv( mvpLoc, 1, false, mvp.AsFloat() );
  }
  GLint colorLoc = gl.GetUniformLocation(gDebugProgram, "uColor");
  if( colorLoc != -1 )
  {
    gl.Uniform4f( colorLoc, 0.0f, 1.0f, 1.0f, 1.0f );
  }

  context.DrawArrays(GL_LINES, 0, numPoints);
  context.DisableVertexAttributeArray( positionLoc );

  // Re-enable blending and culling to match Context's cache.
  if( blendEnabled )
  {
    gl.Enable(GL_BLEND);
  }
  if( cullFaceEnabled )
  {
    gl.Enable(GL_CULL_FACE);
  }

#endif
}


} // SceneGraph
} // Internal
} // Dali
