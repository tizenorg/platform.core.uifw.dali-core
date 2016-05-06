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

// CLASS HEADER
#include <dali/internal/common/math.h>

//EXTERNAL INCLUDES
#include <cmath>

void Dali::Internal::TransformVector3( Vec3 result, const Mat4 m, const Vec3 v )
{
#ifndef __ARM_NEON__

  result[0] = v[0] * m[0] + v[1] * m[4] + v[2] * m[8];
  result[1] = v[0] * m[1] + v[1] * m[5] + v[2] * m[9];
  result[2] = v[0] * m[2] + v[1] * m[6] + v[2] * m[10];

#else

  Vec4 temp = { v[0], v[1], v[2], 0.0f };
  Vec4 tempResult;

  asm volatile ( "VLD1.F32   {q0}, [%1]     \n\t"  //Load "temp" from memory to register q0
                 "VLD1.F32   {q1}, [%0]!    \n\t"  //Load first row of the matrix from memory to register q1
                 "VMUL.F32   q2, q1, d0[0]  \n\t"  //q2 = (m[0..3] * v.x)
                 "VLD1.F32   {q1}, [%0]!    \n\t"  //Load second row of the matrix from memory
                 "VMLA.F32   q2, q1, d0[1]  \n\t"  //q2 = (m[0..3] * v.x) + (m[4..7] * v.y)
                 "VLD1.F32   {q1}, [%0]!    \n\t"  //Load third row of the matrix from memory
                 "VMLA.F32   q2, q1, d1[0]  \n\t"  //q2 = (m[0..3] * v.x) + (m[4..7] * v.y) + (m[8...11] * v.z)
                 "VST1.F32   {q2}, [%2]     \n\t"  //Write the result back to memory
                 :
                 : "r"(m), "r"(temp), "r"(tempResult)
                 : "q0", "q1", "q2", "memory" );

  result[0] = tempResult[0];
  result[1] = tempResult[1];
  result[2] = tempResult[2];

#endif
}

float Dali::Internal::Length( const Vec3 v )
{
  return sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

void Dali::Internal::MultiplyVectorBySize( Vec2& result, const Vec2 v, const Size3 s )
{
  result[0] = v[0] * s[0];
  result[1] = v[1] * s[1];
}

void Dali::Internal::MultiplyVectorBySize( Vec3& result, const Vec3 v, const Size3 s )
{
  result[0] = v[0] * s[0];
  result[1] = v[1] * s[1];
  result[2] = v[2] * s[2];
}

void Dali::Internal::MultiplyVectorBySize( Vec4& result, const Vec4 v, const Size3 s )
{
  result[0] = v[0] * s[0];
  result[1] = v[1] * s[1];
  result[2] = v[2] * s[2];
  result[3] = 1.0f;
}

void Dali::Internal::MultiplyVectorByMatrix4( Vec2& result, const Mat4 m, const Vec2 v )
{
  result[0] = v[0] * m[0] + v[1] * m[4] + m[12];
  result[1] = v[0] * m[1] + v[1] * m[5] + m[13];
}

void Dali::Internal::MultiplyVectorByMatrix4( Vec3& result, const Mat4 m, const Vec3 v )
{
  result[0] = v[0] * m[0] + v[1] * m[4] + v[2] * m[8]  +  m[12];
  result[1] = v[0] * m[1] + v[1] * m[5] + v[2] * m[9]  +  m[13];
  result[2] = v[0] * m[2] + v[1] * m[6] + v[2] * m[10] +  m[14];
}

void Dali::Internal::MultiplyVectorByMatrix4( Vec4& result, const Mat4 m, const Vec4 rhs )
{
  result[0] = rhs[0] * m[0] + rhs[1] * m[4] + rhs[2] * m[8]  +  rhs[3] * m[12];
  result[1] = rhs[0] * m[1] + rhs[1] * m[5] + rhs[2] * m[9]  +  rhs[3] * m[13];
  result[2] = rhs[0] * m[2] + rhs[1] * m[6] + rhs[2] * m[10] +  rhs[3] * m[14];
  result[3] = rhs[0] * m[3] + rhs[1] * m[7] + rhs[2] * m[11] +  rhs[3] * m[15];
}

void Dali::Internal::MultiplyMatrices( Mat4& result, const Mat4 lhs, const Mat4 rhs )
{
  float* temp = reinterpret_cast<float*>( result );

#ifndef  __ARM_NEON__

  for( int i=0; i < 4; i++ )
  {
    // i<<2 gives the first vector / column
    int loc = i<<2;
    int loc1 = loc + 1;
    int loc2 = loc + 2;
    int loc3 = loc + 3;
    float value0 = lhs[loc];
    float value1 = lhs[loc1];
    float value2 = lhs[loc2];
    float value3 = lhs[loc3];
    temp[loc]  = (value0 * rhs[0]) +
                   (value1 * rhs[4]) +
                   (value2 * rhs[8]) +
                   (value3 * rhs[12]);

    temp[loc1] = (value0 * rhs[1]) +
                   (value1 * rhs[5]) +
                   (value2 * rhs[9]) +
                   (value3 * rhs[13]);

    temp[loc2] = (value0 * rhs[2]) +
                   (value1 * rhs[6]) +
                   (value2 * rhs[10])+
                   (value3 * rhs[14]);

    temp[loc3] = (value0 * rhs[3]) +
                   (value1 * rhs[7]) +
                   (value2 * rhs[11])+
                   (value3 * rhs[15]);
  }

#else

  // 64 32bit registers,
  // aliased to
  // d = 64 bit double-word d0 -d31
  // q =128 bit quad-word   q0 -q15  (enough to handle a column of 4 floats in a matrix)
  // e.g. q0 = d0 and d1

  // load and stores interleaved as NEON can load and store while calculating
  asm volatile ( "VLDM         %1,  {q0-q3}        \n\t"   // load matrix 1 (lhsPtr) q[0..q3]
                 "VLDM         %0,  {q8-q11}       \n\t"   // load matrix 2 (rhsPtr) q[q8-q11]
                 "VMUL.F32     q12, q8, d0[0]      \n\t"   // column 0 = rhsPtr[0..3] * lhsPtr[0..3]
                 "VMUL.F32     q13, q8, d2[0]      \n\t"   // column 1 = rhsPtr[0..3] * lhsPtr[4..7]
                 "VMUL.F32     q14, q8, d4[0]      \n\t"   // column 2 = rhsPtr[0..3] * lhsPtr[8..11]
                 "VMUL.F32     q15, q8, d6[0]      \n\t"   // column 3 = rhsPtr[0..3] * lhsPtr[12..15]

                 "VMLA.F32     q12, q9, d0[1]      \n\t"   // column 0 += rhsPtr[4..7] * lhsPtr[0..3]
                 "VMLA.F32     q13, q9, d2[1]      \n\t"   // column 1 += rhsPtr[4..7] * lhsPtr[4..7]
                 "VMLA.F32     q14, q9, d4[1]      \n\t"   // column 2 += rhsPtr[4..7] * lhsPtr[8..11]
                 "VMLA.F32     q15, q9, d6[1]      \n\t"   // column 3 += rhsPtr[4..7] * lhsPtr[12..15]

                 "VMLA.F32     q12, q10, d1[0]     \n\t"   // column 0 += rhsPtr[8..11] * lhsPtr[0..3]
                 "VMLA.F32     q13, q10, d3[0]     \n\t"   // column 1 += rhsPtr[8..11] * lhsPtr[4..7]
                 "VMLA.F32     q14, q10, d5[0]     \n\t"   // column 2 += rhsPtr[8..11] * lhsPtr[8..11]
                 "VMLA.F32     q15, q10, d7[0]     \n\t"   // column 3 += rhsPtr[8..11] * lhsPtr[12..15]

                 "VMLA.F32     q12, q11, d1[1]     \n\t"   // column 0 += rhsPtr[12..15] * lhsPtr[0..3]
                 "VMLA.F32     q13, q11, d3[1]     \n\t"   // column 1 += rhsPtr[12..15] * lhsPtr[4..7]
                 "VMLA.F32     q14, q11, d5[1]     \n\t"   // column 2 += rhsPtr[12..15] * lhsPtr[8..11]
                 "VMLA.F32     q15, q11, d7[1]     \n\t"   // column 3 += rhsPtr[12..15] * lhsPtr[12..15]
                 "VSTM         %2,  {q12-q15}      \n\t"   // store entire output matrix.
                 : "+r"(rhs), "+r"(lhs), "+r"(temp)
                 :
                 : "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "memory" );

#endif
}
