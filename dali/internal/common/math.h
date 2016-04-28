#ifndef __DALI_INTERNAL_MATH_H__
#define __DALI_INTERNAL_MATH_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

namespace Dali
{

namespace Internal
{

typedef float vec3[3];
typedef float vec4[4];
typedef float mat4[16];

/**
 * @brief Applies a transformation matrix to a vector
 *
 * @param[out] result The transformed vector
 * @param[in] m The transformation matrix
 * @param[in] v The vector to transform
 */
inline void TransformVector3( vec3 result, const mat4 m, const vec3 v )
{
#ifndef  __ARM_NEON__

  result[0] = v[0] * m[0] + v[1] * m[4] + v[2] * m[8];
  result[1] = v[0] * m[1] + v[1] * m[5] + v[2] * m[9];
  result[2] = v[0] * m[2] + v[1] * m[6] + v[2] * m[10];

#else

  vec4 temp = { v[0], v[1], v[2], 0.0f };
  vec4 tempResult;

  asm volatile ( "VLD1.F32     {q0}, [%1]        \n\t"  //Load "temp" from memory
                 "VLD1.F32     {q9}, [%0]!       \n\t"  //Load first row of the matrix from memory
                 "VMUL.F32     q10,  q9,   d0[0] \n\t"  //q10 = (m[0..3] * v.x)
                 "VLD1.F32     {q9}, [%0]!       \n\t"  //Load second row of the matrix from memory
                 "VMLA.F32     q10,  q9,   d0[1] \n\t"  //q10 = (m[0..3] * v.x) + (m[4..7] * v.y)
                 "VLD1.F32     {q9}, [%0]!       \n\t"  //Load third row of the matrix from memory
                 "VMLA.F32     q10,  q9,   d1[0] \n\t"  //q10 = (m[0..3] * v.x) + (m[4..7] * v.y) + (m[8...11] * v.z)
                 "VST1.F32     {q10},[%2]        \n\t"  //Write te result back to memory
                 :
                 : "r"(m), "r"(temp), "r"(tempResult)
                 : "q0", "q9", "q10", "q11", "memory" );

  result[0] = tempResult[0];
  result[1] = tempResult[1];
  result[2] = tempResult[2];

#endif
}

/**
 * @brief Computes the length of a vector3
 *
 * @param[in] v The vector
 * @return The lenght of the vector
 */
inline float Length( const vec3 v )
{
  return sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

} // namespace Internal

} // namespace Dali

#endif  //__DALI_INTERNAL_MATH_H__
