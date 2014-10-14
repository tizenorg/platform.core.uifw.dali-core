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
#include <dali/internal/common/dali-hash.h>


namespace Dali
{

namespace //un-named namespace
{
/*
 * djb2 (http://www.cse.yorku.ca/~oz/hash.html)
 */
const unsigned long INITIAL_HASH_VALUE = 5381;

inline void HashShader( const char* string, unsigned long& hash )
{
  // performs normal hash but ignores white spaces, carriage returns and tabs
  while( int c = *string++ )
  {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }
}
} // un-named namespace

unsigned long StringHash::operator()(const std::string& toHash)
{
  unsigned long hash = INITIAL_HASH_VALUE;

  HashShader( toHash.c_str(), hash );

  return hash;
}

unsigned long StringHashShaderCode::operator()(const std::string& fragment, const std::string& vertex)
{
  unsigned long hash = INITIAL_HASH_VALUE;

  HashShader( fragment.c_str() , hash);
  HashShader( vertex.c_str(), hash );

  return hash;
}




} // namespace Dali
