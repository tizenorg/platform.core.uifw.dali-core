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

/*
 * djb2 (http://www.cse.yorku.ca/~oz/hash.html)
 */
unsigned long StringHash::operator()(const std::string& toHash)
{
  unsigned long hash = 5381;

  const char *str = toHash.c_str();

  while( int c = *str++ )
  {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  return hash;
}

unsigned long StringHashShaderCode::operator()(const std::string& fragment, const std::string& vertex)
{
  unsigned long hash = 5381;

  const char *fragStr = fragment.c_str();

  // hash fragment code
  while( int c = *fragStr++ )
  {
    if((  c == ' ') || ( c == '\n') || ( c== '\t') )
    {
      continue;
    }
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  // hash vertex code
  const char *vertStr = vertex.c_str();
  while( int c = *vertStr++ )
  {
    if((  c == ' ') || ( c == '\n') || ( c== '\t') )
    {
      continue;
    }
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  return hash;
}




} // namespace Dali
