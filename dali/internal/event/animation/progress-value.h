#ifndef __DALI_INTERNAL_PROGRESS_VALUE_H__
#define __DALI_INTERNAL_PROGRESS_VALUE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/degree.h>

namespace Dali
{

namespace Internal
{

/**
 * Progress / value pair for animating channels (properties) with keyframes
 */
template <typename T>
class ProgressValue
{
public:
  ProgressValue (float progress, T value)
  : mProgress(progress),
    mValue (value)
  {
  }

  ~ProgressValue ()
  {
  }

  float GetProgress () const
  {
    return mProgress;
  }

  const T& GetValue () const
  {
    return mValue;
  }

public:
  float mProgress;   ///< Progress this value applies to animation channel
  T mValue;          ///< value this animation channel should take
};

typedef ProgressValue<Quaternion>                       ProgressQuaternion;
typedef std::vector<ProgressQuaternion>                 ProgressQuaternionContainer;

typedef ProgressValue<AngleAxis>                        ProgressAngleAxis;
typedef std::vector<AngleAxis>                          ProgressAngleAxisContainer;

typedef ProgressValue<bool>                             ProgressBoolean;
typedef std::vector<ProgressBoolean>                    ProgressBooleanContainer;

typedef ProgressValue<float>                            ProgressNumber;
typedef std::vector<ProgressNumber>                     ProgressNumberContainer;

typedef ProgressValue<int>                              ProgressInteger;
typedef std::vector<ProgressInteger>                    ProgressIntegerContainer;

typedef ProgressValue<Vector2>                          ProgressVector2;
typedef std::vector<ProgressVector2>                    ProgressVector2Container;

typedef ProgressValue<Vector3>                          ProgressVector3;
typedef std::vector<ProgressVector3>                    ProgressVector3Container;

typedef ProgressValue<Vector4>                          ProgressVector4;
typedef std::vector<ProgressVector4>                    ProgressVector4Container;


template <typename T>
inline void Interpolate( T& result, const T& a, const T&b, float progress )
{
  result = (a + (b - a) * progress);
}

//Specialization for bool
template <>
inline void Interpolate (bool& result, const bool& a, const bool& b, float progress)
{
  result = progress < 0.5f ? a : b;
}

//Specialization for int
template <>
inline void Interpolate(int& result, const int& a, const int& b, float progress)
{
  result = static_cast<int>(a + (b - a) * progress + 0.5f);
}

//Specialization for Quaternion
template <>
inline void Interpolate (Quaternion& result, const Quaternion& a, const Quaternion& b, float progress)
{
  result = Quaternion::Slerp(a, b, progress);
}

//Specialization for AngleAxis
template <>
inline void Interpolate ( AngleAxis& result, const AngleAxis& a, const AngleAxis& b, float progress)
{
  Quaternion q1(Radian(a.angle), a.axis);
  Quaternion q2(Radian(b.angle), b.axis);

  Quaternion iq = Quaternion::Slerp(q1, q2, progress);
  iq.ToAxisAngle(result.axis, result.angle);
}

/* Cubic Interpolation (Catmull-Rom spline) between values p1 and p2. p0 and p3 are prev and next values
 * and are used as control points to calculate tangent of the curve at interpolation points.
 *
 * f(t) = a3*t^3 + a2*t^2 + a1*t + a0
 * Restrictions: f(0)=p1   f(1)=p2   f'(0)=(p2-p0)*0.5   f'(1)=(p3-p1)*0.5
 */
template <typename T>
inline void CubicInterpolate( T& result, const T& p0, const T& p1, const T&  p2, const T&  p3, float progress )
{
  T a3 = p3*0.5f - p2*1.5f + p1*1.5f - p0*0.5f;
  T a2 = p0 - p1*2.5f + p2*2.0f - p3*0.5f;
  T a1 = (p2-p0)*0.5f;

  result = a3*progress*progress*progress + a2*progress*progress + a1*progress + p1;
}

//Specialization for int
template <>
inline void CubicInterpolate( int& result, const int& p0, const int& p1, const int&  p2, const int&  p3, float progress )
{
  float a3 = p3*0.5f - p2*1.5f + p1*1.5f - p0*0.5f;
  float a2 = p0 - p1*2.5f + p2*2.0f - p3*0.5f;
  float a1 = (p2-p0)*0.5f;

  result = static_cast<int>( a3*progress*progress*progress + a2*progress*progress + a1*progress + p1 );
}

//Specialization for bool
template <>
inline void CubicInterpolate( bool& result, const bool& p0, const bool& p1, const bool&  p2, const bool&  p3, float progress )
{
  Interpolate( result, p1, p2, progress );
}

} // namespace Internal

} // namespace Dali

#endif //__DALI_PROGRESS_VALUE_H__
