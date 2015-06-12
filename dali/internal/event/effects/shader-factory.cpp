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

std::string shaderBinaryFilename( const std::string& vertexSource, const std::string& fragmentSource, size_t& shaderHash )
{
  shaderHash = CalculateHash( vertexSource, fragmentSource );
  std::stringstream binaryShaderFilenameBuilder;
  binaryShaderFilenameBuilder << CORE_MAJOR_VERSION << VERSION_SEPARATOR << CORE_MINOR_VERSION << VERSION_SEPARATOR << CORE_MICRO_VERSION << VERSION_SEPARATOR
                              << shaderHash
                              << SHADER_SUFFIX;
  return binaryShaderFilenameBuilder.str();
}

}

ShaderFactory::ShaderFactory(ResourceClient& resourceClient)
: mResourceClient(resourceClient)
{
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

  ShaderResourceType resourceType(shaderHash, vertexSource, fragmentSource); /// @todo Remove line [new_mesh-feature-033-synchronous-load-save-of-shader-binaries]
  ResourceTypePath typePath(resourceType, binaryShaderFilename);

  // Search for a matching resource already loaded or loading:
  ResourceTicketPtr ticket = GetShaderTicket( typePath );
  if ( !ticket )
  {
    // Load the shader (loaded synchronously in Update thread so it is ready by the time the set shader message is processed)
    ticket = mResourceClient.LoadShader(resourceType, binaryShaderFilename);
    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "ShaderFactory::Load Ticket ID:%d, path: \"%s\"\n", ticket->GetId(), binaryShaderFilename.c_str());

    mResourceTypePathIdMap.insert( ResourceTypePathIdPair( typePath, ticket->GetId() ) );
  }

  return ticket;
}

void ShaderFactory::LoadDefaultShaders()
{
  mDefaultShader = ShaderEffect::New();

  mDefaultShader->SendProgramMessage( ImageVertex, ImageFragment, false );
}

} // namespace Internal

} // namespace Dali
