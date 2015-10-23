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

// CLASS HEADER
#include <dali/internal/render/data-providers/uniform-cache.h>

namespace Dali
{
namespace Internal
{
namespace Render
{

UniformCache::UniformCache()
{}

UniformCache::~UniformCache()
{ // OwnerContainer cleans up
}

int32_t UniformCache::GetSamplerUniformUniqueIndex( const std::string& uniformName )
{
  OwnerContainer< UniformEntry* >::SizeType index = 0;
  const OwnerContainer< UniformEntry* >::SizeType end = mSamplerUniformCache.Size();

  for( ;index < end; ++index )
  {
    if( mSamplerUniformCache[ index ]->uniformName == uniformName )
    {
      // match, return the index
      return index;
    }
  }
  // no match found
  mSamplerUniformCache.PushBack( new UniformEntry( uniformName ) );

  return end;
}

} // namespace Render

} // namespace Internal

} // namespace Dali
