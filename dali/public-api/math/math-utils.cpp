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
#include <dali/public-api/math/math-utils.h>

// EXTERNAL INCLUDES
#include <math.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>


namespace Dali
{

float GetRangedEpsilon(float a, float b)
{
  float abs_f = std::max(fabsf(a), fabsf(b));
  int abs_i = (int) abs_f;

  float epsilon = Math::MACHINE_EPSILON_10000;
  if (abs_f < 0.1f)
  {
    return Math::MACHINE_EPSILON_0;
  }
  else if (abs_i < 2)
  {
    return Math::MACHINE_EPSILON_1;
  }
  else if (abs_i < 20)
  {
    return Math::MACHINE_EPSILON_10;
  }
  else if (abs_i < 200)
  {
    return Math::MACHINE_EPSILON_100;
  }
  else if (abs_i < 2000)
  {
    return Math::MACHINE_EPSILON_1000;
  }
  return epsilon;
}

float WrapInDomain(float x, float start, float end)
{
  float domain = end - start;
  x -= start;

  if(fabsf(domain) > Math::MACHINE_EPSILON_1)
  {
    return start + (x - floorf(x / domain) * domain);
  }

  return start;
}

float ShortestDistanceInDomain(float a, float b, float start, float end)
{
  //  (a-start + end-b)
  float size = end-start;
  float vect = b-a;

  if(vect > 0)
  {
    // +ve vector, let's try perspective 1 domain to the right,
    // and see if closer.
    float aRight = a+size;
    if( aRight-b < vect )
    {
      return b-aRight;
    }
  }
  else
  {
    // -ve vector, let's try perspective 1 domain to the left,
    // and see if closer.
    float aLeft = a-size;
    if( aLeft-b > vect )
    {
      return b-aLeft;
    }
  }

  return vect;
}

} // namespace Dali
