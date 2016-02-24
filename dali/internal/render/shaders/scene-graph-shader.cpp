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
#include <dali/internal/render/shaders/scene-graph-shader.h>

// INTERNAL INCLUDES
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/render/gl-resources/texture-units.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/uniform-meta.h>
#include <dali/internal/common/image-sampler.h>

// See render-debug.h
#ifdef DALI_PRINT_RENDER_INFO

#include <sstream>
#define DALI_DEBUG_OSTREAM(streamName) std::stringstream streamName;

#define DALI_PRINT_UNIFORM(streamName,bufferIndex,name,value) \
  { \
    streamName << " " << name << ": " << value; \
  }

#define DALI_PRINT_CUSTOM_UNIFORM(streamName,bufferIndex,name,property) \
  { \
    streamName << " " << name << ": "; \
    property.DebugPrint( streamName, bufferIndex ); \
  }

#define DALI_PRINT_SHADER_UNIFORMS(streamName) \
  { \
    std::string debugString( streamName.str() ); \
    DALI_LOG_RENDER_INFO( "           %s\n", debugString.c_str() ); \
  }

#else // DALI_PRINT_RENDER_INFO

#define DALI_DEBUG_OSTREAM(streamName)
#define DALI_PRINT_UNIFORM(streamName,bufferIndex,name,value)
#define DALI_PRINT_CUSTOM_UNIFORM(streamName,bufferIndex,name,property)
#define DALI_PRINT_SHADER_UNIFORMS(streamName)

#endif // DALI_PRINT_RENDER_INFO

namespace Dali
{

namespace Internal
{

template <> struct ParameterType< Dali::ShaderEffect::GeometryHints> : public BasicType< Dali::ShaderEffect::GeometryHints > {};
template <> struct ParameterType< Dali::ShaderEffect::UniformCoordinateType > : public BasicType< Dali::ShaderEffect::UniformCoordinateType > {};

namespace SceneGraph
{

Shader::Shader( Dali::ShaderEffect::GeometryHints& hints )
: mGeometryHints( hints ),
  mGridDensity( Dali::ShaderEffect::DEFAULT_GRID_DENSITY ),
  mTexture( NULL ),
  mRenderTextureId( 0 ),
  mUpdateTextureId( 0 ),
  mProgram( NULL ),
  mRenderQueue( NULL )
{
}

Shader::~Shader()
{
}

void Shader::Initialize( RenderQueue& renderQueue )
{
  mRenderQueue = &renderQueue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following methods are called during UpdateManager::Update()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Shader::ForwardTextureId( BufferIndex updateBufferIndex, ResourceId textureId )
{
  mUpdateTextureId = textureId;

  typedef MessageValue1< Shader, Integration::ResourceId > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue->ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( this, &Shader::SetTextureId, textureId );
}

Integration::ResourceId Shader::GetEffectTextureResourceId()
{
  return mUpdateTextureId;
}

void Shader::ForwardUniformMeta( BufferIndex updateBufferIndex, UniformMeta* meta )
{
  // Defer setting uniform metadata until the next Render

  typedef MessageValue1< Shader, UniformMeta* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue->ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( this, &Shader::InstallUniformMetaInRender, meta );
}

void Shader::ForwardCoordinateType( BufferIndex updateBufferIndex, unsigned int index, Dali::ShaderEffect::UniformCoordinateType type )
{
  // Defer setting uniform coordinate type until the next Render
  typedef MessageValue2< Shader, unsigned int, Dali::ShaderEffect::UniformCoordinateType > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue->ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( this, &Shader::SetCoordinateTypeInRender, index, type );
}

void Shader::ForwardGridDensity( BufferIndex updateBufferIndex, float density )
{
  typedef MessageValue1< Shader, float > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue->ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( this, &Shader::SetGridDensity, density );
}

void Shader::ForwardHints( BufferIndex updateBufferIndex, Dali::ShaderEffect::GeometryHints hint )
{
  typedef MessageValue1< Shader, Dali::ShaderEffect::GeometryHints > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue->ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( this, &Shader::SetGeometryHints, hint );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following methods are called during RenderManager::Render()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Shader::SetTextureId( Integration::ResourceId textureId )
{
  if ( mRenderTextureId != textureId )
  {
    mRenderTextureId = textureId;
    mTexture = NULL;
  }
}

Integration::ResourceId Shader::GetTextureIdToRender()
{
  return mRenderTextureId;
}

void Shader::SetGridDensity( float density )
{
  mGridDensity = density;
}

float Shader::GetGridDensity()
{
  return mGridDensity;
}

void Shader::InstallUniformMetaInRender( UniformMeta* meta )
{
  mUniformMetadata.PushBack( meta );
}

void Shader::SetCoordinateTypeInRender( unsigned int index, Dali::ShaderEffect::UniformCoordinateType type )
{
  DALI_ASSERT_DEBUG( index < mUniformMetadata.Count() );
  mUniformMetadata[ index ]->SetCoordinateType( type );
}

void Shader::SetProgram( Internal::ShaderDataPtr shaderData,
                         ProgramCache* programCache,
                         bool modifiesGeometry )
{
  DALI_LOG_TRACE_METHOD_FMT( Debug::Filter::gShader, "%d\n", shaderData->GetHashValue() );

  mProgram = Program::New( *programCache, shaderData, modifiesGeometry );
  // The program cache owns the Program object so we don't need to worry about this raw allocation here.
}

Program* Shader::GetProgram()
{
  return mProgram;
}


// Messages

void SetTextureIdMessage( EventThreadServices& eventThreadServices, const Shader& shader, Integration::ResourceId textureId )
{
  typedef MessageDoubleBuffered1< Shader, Integration::ResourceId > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shader, &Shader::ForwardTextureId, textureId );
}

void SetGridDensityMessage( EventThreadServices& eventThreadServices, const Shader& shader, float density )
{
  typedef MessageDoubleBuffered1< Shader, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shader, &Shader::ForwardGridDensity, density );
}

void SetHintsMessage( EventThreadServices& eventThreadServices, const Shader& shader, Dali::ShaderEffect::GeometryHints hint )
{
  typedef MessageDoubleBuffered1< Shader, Dali::ShaderEffect::GeometryHints > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shader, &Shader::ForwardHints, hint );
}

void InstallUniformMetaMessage( EventThreadServices& eventThreadServices, const Shader& shader, UniformMeta& meta )
{
  typedef MessageDoubleBuffered1< Shader, UniformMeta* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shader, &Shader::ForwardUniformMeta, &meta );
}

void SetCoordinateTypeMessage( EventThreadServices& eventThreadServices, const Shader& shader, unsigned int index, Dali::ShaderEffect::UniformCoordinateType type )
{
  typedef MessageDoubleBuffered2< Shader, unsigned int, Dali::ShaderEffect::UniformCoordinateType > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shader, &Shader::ForwardCoordinateType, index, type );
}


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
