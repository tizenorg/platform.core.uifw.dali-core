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
#include <dali/internal/update/resources/complete-status-manager.h>

// INTERNAL INCLUDES
#include <dali/integration-api/gl-sync-abstraction.h>
#include <dali/integration-api/resource-declarations.h>
#include <dali/internal/update/resources/resource-manager.h>

namespace Dali
{
namespace Internal
{

CompleteStatusManager::CompleteStatusManager( ResourceManager& resourceManager )
: mResourceManager(resourceManager)
{
}

CompleteStatusManager::~CompleteStatusManager()
{
}

bool CompleteStatusManager::HasFrameBufferBeenRenderedTo( Integration::ResourceId id ) const
{
  const std::size_t count = mFboRenderedIds.Count();
  for( std::size_t index = 0; index < count; ++index )
  {
    if( mFboRenderedIds[ index ] == id )
    {
      return true;
    }
  }
  return false;
}

void CompleteStatusManager::SetFrameBufferBeenRenderedTo( Integration::ResourceId id )
{
  const std::size_t count = mFboRenderedIds.Count();
  for( std::size_t index = 0; index < count; ++index )
  {
    if( mFboRenderedIds[ index ] == id )
    {
      // already there, dont add duplicate
      return;
    }
  }
  mFboRenderedIds.PushBack( id );
}

CompleteStatusManager::CompleteState CompleteStatusManager::GetStatus( Integration::ResourceId id ) const
{
  CompleteState readiness = CompleteStatusManager::NOT_READY;

  if( 0 != id )
  {
    if( mResourceManager.IsResourceLoaded( id ) )
    {
      readiness = CompleteStatusManager::COMPLETE;
    }
    else if( mResourceManager.IsResourceLoadFailed( id ) )
    {
      readiness = CompleteStatusManager::FAILED;
    }
    else if( HasFrameBufferBeenRenderedTo( id ) )
    {
      readiness = CompleteStatusManager::COMPLETE;
    }
  }
  else
  {
    // Loading is essentially finished if we don't have a resource ID
    readiness = CompleteStatusManager::COMPLETE;
  }

  return readiness;
}

} // Internal

} // Dali
