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

// CLASS HEADER
#include <dali/internal/event/effects/shader-factory.h>

// EXTERNAL INCLUDES
#include <sstream>

// INTERNAL INCLUDES
#include <dali/public-api/dali-core-version.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/devel-api/common/hash.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/event/effects/shader-effect-impl.h>
#include <dali/internal/event/effects/shader-declarations.h>

// compile time generated shader strings
#include "dali-shaders.h"

namespace
{
const char* VERSION_SEPARATOR = "-";
const char* SHADER_SUFFIX = ".dali-bin";
}

// Use pre-compiler constants in order to utilize string concatenation
#define SHADER_DEF_USE_BONES    "#define USE_BONES\n"
#define SHADER_DEF_USE_COLOR    "#define USE_COLOR\n"
#define SHADER_DEF_USE_GRADIENT "#define USE_GRADIENT\n"

using namespace Dali::Integration;

namespace Dali
{

namespace Internal
{

namespace
{

/**
 * @brief Generates a filename for a shader binary based on the hash value passed in.
 */
std::string shaderBinaryFilename( size_t shaderHash )
{
  std::stringstream binaryShaderFilenameBuilder;
  binaryShaderFilenameBuilder << CORE_MAJOR_VERSION << VERSION_SEPARATOR << CORE_MINOR_VERSION << VERSION_SEPARATOR << CORE_MICRO_VERSION << VERSION_SEPARATOR
                              << shaderHash
                              << SHADER_SUFFIX;
  return binaryShaderFilenameBuilder.str();
}

/**
 * @brief Generates a filename for a shader binary based on hashing the source code passed in and saves the hash value.
 */
std::string shaderBinaryFilename( const std::string& vertexSource, const std::string& fragmentSource, size_t& shaderHash )
{
  shaderHash = CalculateHash( vertexSource.c_str(), fragmentSource.c_str() );
  return shaderBinaryFilename( shaderHash );
}

}

ShaderFactory::ShaderFactory(ResourceClient& resourceClient)
: mResourceClient(resourceClient)
{
   for( int i = 0; i < DALI_SHADER_CACHE_SIZE; ++i )
   {
     mShaderBinaryCache[i] = std::pair< Integration::ShaderDataPtr, size_t >( ShaderDataPtr(0), Integration::ShaderData::UNINITIALISED_HASH_VALUE );
   }
}

ShaderFactory::~ShaderFactory()
{
}

ResourceTicketPtr ShaderFactory::GetShaderTicket( const ResourceTypePath& typePath )
{
  ResourceTicketPtr ticket;

  // Search for a matching resource
  ResourceTypePathIdIter iter = mResourceTypePathIdMap.find( typePath );

  if( mResourceTypePathIdMap.end() != iter )
  {
    // The resource was previously requested
    unsigned int resourceId = iter->second;

    // The ticket may still be alive, request another copy
    ticket = mResourceClient.RequestResourceTicket( resourceId );

    // Clean-up the map of resource IDs, if the ticket has been discarded
    if( !ticket )
    {
      mResourceTypePathIdMap.erase( iter );
    }
    else
    {
      DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "ShaderFactory::Load filename= %s already requested to Load\n", typePath.path.c_str());
    }
  }

  return ticket;
}

ResourceTicketPtr ShaderFactory::Load( const std::string& vertexSource, const std::string& fragmentSource, size_t& shaderHash )
{

  std::string binaryShaderFilename( shaderBinaryFilename( vertexSource, fragmentSource, shaderHash ) );

  ShaderResourceType resourceType( shaderHash, vertexSource, fragmentSource );
  ResourceTypePath typePath( resourceType, binaryShaderFilename );

  // Search for a matching resource already loaded or loading:
  ResourceTicketPtr ticket = GetShaderTicket( typePath );
  if( !ticket )
  {
    // Load the shader (loaded synchronously in Update thread so it is ready by the time the set shader message is processed)
    ticket = mResourceClient.LoadShader( resourceType, binaryShaderFilename );
    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "ShaderFactory::Load Ticket ID:%d, path: \"%s\"\n", ticket->GetId(), binaryShaderFilename.c_str());

    mResourceTypePathIdMap.insert( ResourceTypePathIdPair( typePath, ticket->GetId() ) );
  }

  return ticket;
}

ShaderDataPtr ShaderFactory::AttemptCachedLoadImmediate( const std::string& vertexSource, const std::string& fragmentSource, size_t& shaderHash )
{
  // Work out the filename for the binary that the glsl source will be compiled and linked to:
  std::string binaryShaderFilename( shaderBinaryFilename( vertexSource, fragmentSource, shaderHash ) );

  ShaderDataPtr shaderData;

  /// Check a cache of preloaded shaders:
  for( int i = 0; i < DALI_SHADER_CACHE_SIZE; ++i )
  {
    if( mShaderBinaryCache[i].second == shaderHash )
    {
      shaderData = mShaderBinaryCache[i].first;

      // Promote the entry since it is a hit:
      const int newLocation = i / 2;
      ShaderDataPtr tempP = mShaderBinaryCache[newLocation].first;
      const size_t  tempH = mShaderBinaryCache[newLocation].second;
      mShaderBinaryCache[newLocation] = mShaderBinaryCache[i];
      mShaderBinaryCache[i].first = tempP;
      mShaderBinaryCache[i].second = tempH;

      DALI_LOG_INFO( Debug::Filter::gShader, Debug::General, "Mem cache hit on path: \"%s\"\n", binaryShaderFilename.c_str() );
      break;
    }
  }

  // If memory cache failed check the file system for a binary or return a source-only ShaderData:
  if( shaderData.Get() == NULL )
  {
    // Allocate the structure that returns the loaded shader:
    shaderData = new ShaderData( vertexSource, fragmentSource );
    shaderData->SetHashValue( shaderHash );
    shaderData->GetBuffer().Clear();
    shaderData->SetResourceId( 0 );

    // Try to load the binary (this will fail if the shader source has never been compiled before):
    ThreadLocalStorage& tls = ThreadLocalStorage::Get();
    Integration::PlatformAbstraction& platformAbstraction = tls.GetPlatformAbstraction();
    const bool loaded = platformAbstraction.LoadShaderBinaryFile( binaryShaderFilename, shaderData->GetBuffer() );

    if( loaded )
    {
      MemoryCacheInsert( shaderData );
    }

    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, loaded ?
        "loaded on path: \"%s\"\n" :
        "failed to load on path: \"%s\"\n",
        binaryShaderFilename.c_str());
  }

  return shaderData;
}

void ShaderFactory::Dispatch( Integration::ShaderDataPtr shaderData )
{
  // Save the binary to the file system:
  std::string binaryShaderFilename( shaderBinaryFilename( shaderData->GetHashValue() ) );

  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  Integration::PlatformAbstraction& platformAbstraction = tls.GetPlatformAbstraction();
  const bool saved = platformAbstraction.SaveShaderBinaryFile( binaryShaderFilename, &shaderData->GetBuffer()[0], shaderData->GetBufferSize() );

  // Save the binary into to memory cache:
  MemoryCacheInsert( shaderData );

  DALI_LOG_INFO( Debug::Filter::gShader, Debug::General, saved ? "Saved to file: %s\n" : "Save to file failed: %s\n", binaryShaderFilename.c_str() );
}

void ShaderFactory::LoadDefaultShaders()
{
  mDefaultShader = ShaderEffect::New();

  mDefaultShader->SendProgramMessage( ImageVertex, ImageFragment, false );
}

void ShaderFactory::MemoryCacheInsert( ShaderDataPtr shaderData )
{
  // Save the binary into to memory cache:
  // Cache insertions happen in the bottom half of the cache using a random replacement policy to avoid keeping any state around.
  // Cache hits promote entries out of the bottom half so they avoid this replacement.
  if( shaderData->GetBufferSize() > 0 )
  {
    const int insertion = DALI_SHADER_CACHE_MAX - rand() % (DALI_SHADER_CACHE_SIZE / 2);
    mShaderBinaryCache[insertion].first = shaderData;
    mShaderBinaryCache[insertion].second = shaderData->GetHashValue();

    DALI_LOG_INFO( Debug::Filter::gShader, Debug::General, "CACHED BINARY FOR HASH: %u\n", shaderData->GetHashValue() );
  }
}

} // namespace Internal

} // namespace Dali
