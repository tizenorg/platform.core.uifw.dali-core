#ifndef __DALI_INTERNAL_FRUSTRUM_H__
#define __DALI_INTERNAL_FRUSTRUM_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/public-api/math/vector3.h>

namespace Dali
{

namespace Internal
{

/**
 * This encapsulates the information required to store a plane equation
 * (a point on the plane and it's normal), and methods on the plane.
 */
struct Plane
{
  /**
   * Note that points should be counter-clockwise
   */
  void Set( const Vector3& point1, const Vector3& point2, const Vector3& point3 )
  {
    mNormal = Vector3(point1 - point2).Cross( Vector3(point3 - point2) );
    mNormal.Normalize();
    mAbsoluteNormal.x = fabsf(mNormal.x);
    mAbsoluteNormal.y = fabsf(mNormal.y);
    mAbsoluteNormal.z = fabsf(mNormal.z);
    mNormalDotPoint = -mNormal.Dot( point2 );
  }

  float DistanceFromPoint( const Vector3& point ) const
  {
    return mNormal.Dot( point ) + mNormalDotPoint;
  }

  Vector3 mNormal;         // Normalized normal vector
  Vector3 mAbsoluteNormal; // Componentwise abs of normal vector
  float mNormalDotPoint;   // Distance of plane from origin

  // Plane equation:
  // n . (P-Vo) = 0   where n = plane normal, P = a point on the plane, Vo = origin of plane
  // => ax + by + cz + d = 0        a,b,c,d are plane coefficients

  // Distance of arbitrary point to plane is given by
  // d(Po,P) = abs(Po-Vo)cos theta == ( n dot (Po-Vo) ) / abs(n) == a*x0 + b*y0 + c*z0 + d / sqrt(a*a+b*b+c*c)
  // Given unit normal |n| = 1, this simplifies to
  // d(Po, Plane) = a*x0 + b*y0 + c*z0 + d
  // i.e. mNormal.Dot(point) + mNormalDotPoint;
};

enum FrustumIndices
{
  LEFT_PLANE,
  RIGHT_PLANE,
  TOP_PLANE,
  BOTTOM_PLANE,
  FRUSTUM_PLANE_COUNT
};

/**
 * The frustum is a set of 4 planes used for culling.
 */
typedef Plane Frustum[ FRUSTUM_PLANE_COUNT ];

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_FRUSTRUM_H__
