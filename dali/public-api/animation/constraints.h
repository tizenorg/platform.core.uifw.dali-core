#ifndef __DALI_CONSTRAINTS_H__
#define __DALI_CONSTRAINTS_H__

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

#include <iostream>
#include <stdio.h>
// INTERNAL INCLUDES
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/animation/path.h>

namespace Dali
{

/**
 * @brief Scale To Fit constraint.
 *
 * Scales an Actor, such that it fits within it's Parent's Size.
 * f(current, size, parentSize) = parentSize / size
 */
struct ScaleToFitConstraint
{
  /**
   * @brief Constructor.
   */
  ScaleToFitConstraint()
  { }

  /**
   * @brief Functor operator
   *
   * @param[in] current The actor's current scale
   * @param[in] sizeProperty The actor's current size
   * @param[in] parentSizeProperty The parent's current size.
   * @return The actor's new scale
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& sizeProperty,
                     const PropertyInput& parentSizeProperty)
  {
    const Vector3 size = sizeProperty.GetVector3();
    const Vector3 parentSize = parentSizeProperty.GetVector3();

    return Vector3( fabsf(size.x) > Math::MACHINE_EPSILON_1 ? (parentSize.x / size.x) : 0.0f,
                    fabsf(size.y) > Math::MACHINE_EPSILON_1 ? (parentSize.y / size.y) : 0.0f,
                    fabsf(size.z) > Math::MACHINE_EPSILON_1 ? (parentSize.z / size.z) : 0.0f );
  }
};

/**
 * @brief Scale To Fit Keep Aspect Ratio constraint.
 *
 * Scales an Actor, such that it fits within its Parent's Size Keeping the aspect ratio.
 * f(current, size, parentSize) = Vector3( min( parentSizeX / sizeX, min( parentSizeY / sizeY, parentSizeZ / sizeZ ) )
 */
struct ScaleToFitKeepAspectRatioConstraint
{
  /**
   * @brief Constructor.
   */
  ScaleToFitKeepAspectRatioConstraint()
  { }

  /**
   * @brief Functor operator
   *
   * @param[in] current The actor's current scale
   * @param[in] sizeProperty The actor's current size
   * @param[in] parentSizeProperty The parent's current size.
   * @return The actor's new scale
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& sizeProperty,
                     const PropertyInput& parentSizeProperty)
  {
    return FitKeepAspectRatio( parentSizeProperty.GetVector3(), sizeProperty.GetVector3() );
  }
};

/**
 * @brief Scale To Fill XY Keep Aspect Ratio constraint.
 *
 * Scales an Actor, such that it fill its Parent's Size in the X and Y coordinates Keeping the aspect ratio.
 * f(current, size, parentSize) = Vector3( max( parentSizeX / sizeX, max( parentSizeY / sizeY, parentSizeZ / sizeZ ) )
 */
struct ScaleToFillXYKeepAspectRatioConstraint
{
  /**
   * @brief Constructor.
   */
  ScaleToFillXYKeepAspectRatioConstraint()
  { }

  /**
   * @param[in] current The actor's current scale
   * @param[in] sizeProperty The actor's current size
   * @param[in] parentSizeProperty The parent's current size.
   * @return The actor's new scale
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& sizeProperty,
                     const PropertyInput& parentSizeProperty)
  {
    return FillXYKeepAspectRatio( parentSizeProperty.GetVector3(), sizeProperty.GetVector3() );
  }
};

/**
 * @brief EqualToConstraint
 *
 * f(current, property) = property
 */
struct EqualToConstraint
{
  /**
   * @brief Constructor.
   */
  EqualToConstraint()
  { }

  /**
   * @brief override functor for float properties
   *
   * @param[in] current The current property value
   * @param[in] property The property to copy
   * @return The copy of the input property
   */
  float operator()(const float current, const PropertyInput& property)
  {
    return property.GetFloat();
  }

  /**
   * @brief override functor for float properties
   *
   * @param[in] current The current property value
   * @param[in] property The property to copy
   * @return The copy of the input property
   */
  Vector3 operator()(const Vector3& current, const PropertyInput& property)
  {
    return property.GetVector3();
  }

  /**
   * @brief override functor for float properties
   *
   * @param[in] current The current property value
   * @param[in] property The property to copy
   * @return The copy of the input property
   */
  Vector4 operator()(const Vector4& current, const PropertyInput& property)
  {
    return property.GetVector4();
  }

  /**
   * @brief override functor for float properties
   *
   * @param[in] current The current property value
   * @param[in] property The property to copy
   * @return The copy of the input property
   */
  Quaternion operator()(const Quaternion& current, const PropertyInput& property)
  {
    return property.GetQuaternion();
  }

  /**
   * @brief override functor for float properties
   *
   * @param[in] current The current property value
   * @param[in] property The property to copy
   * @return The copy of the input property
   */
  Matrix3 operator()(const Matrix3& current, const PropertyInput& property)
  {
    return property.GetMatrix3();
  }

  /**
   * @brief override functor for float properties
   *
   * @param[in] current The current property value
   * @param[in] property The property to copy
   * @return The copy of the input property
   */
  Matrix operator()(const Matrix& current, const PropertyInput& property)
  {
    return property.GetMatrix();
  }

};

/**
 * @brief RelativeToConstraint for Vector3 properties
 *
 * f(current, property, scale) = property * scale
 */
struct RelativeToConstraint
{
  /**
   * @brief Constructor.
   */
  RelativeToConstraint( float scale )
  : mScale( scale, scale, scale ) { }

  /**
   * @brief Constructor.
   */
  RelativeToConstraint( const Vector3& scale )
  : mScale( scale ) { }

  /**
   * @brief Functor.
   */
  Vector3 operator()(const Vector3& current, const PropertyInput& property)
  {
    return property.GetVector3() * mScale;
  }

  Vector3 mScale; ///< Component-wise scale factor
};

/**
 * @brief RelativeToConstraint for float properties
 */
struct RelativeToConstraintFloat
{
  /**
   * @brief Constructor.
   */
  RelativeToConstraintFloat( float scale )
  : mScale( scale ) { }

  /**
   * @brief Functor.
   */
  float operator()(const float current, const PropertyInput& property)
  {
    return property.GetFloat() * mScale;
  }

  float mScale; ///< Scale factor
};

/**
 * @brief Constraint which sets width to be another actor's width,
 * and the height to a fixed height.
 */
struct SourceWidthFixedHeight
{
  /**
   * @brief Constructor.
   */
  SourceWidthFixedHeight( float height )
  : mFixedHeight( height ) { }

  /**
   * @brief Functor.
   */
  Vector3 operator()(const Vector3& current,
                     const PropertyInput& sourceSize)
  {
    return Vector3( sourceSize.GetVector3().width, mFixedHeight, current.depth );
  }

  float mFixedHeight; ///< the fixed height
};

/**
 * @brief Constraint function to aim a camera at a target.
 *
 * Constraint which sets camera's rotation given camera world position
 * and a target world position.  Uses target's up vector to orient the
 * constrained actor along the vector between camera position and
 * target position.
 *
 * @param[in] current The current rotation property value
 * @param[in] targetPosition World position of target
 * @param[in] cameraPosition World position of camera
 * @param[in] targetRotation World rotation of the target
 * @return The orientation of the camera
 */
inline Quaternion LookAt( const Quaternion& current,
                          const PropertyInput& targetPosition,
                          const PropertyInput& cameraPosition,
                          const PropertyInput& targetRotation )
{
  Vector3 vForward = targetPosition.GetVector3() - cameraPosition.GetVector3();
  vForward.Normalize();

  const Quaternion& targetRotationQ = targetRotation.GetQuaternion();

  Vector3 targetY(targetRotationQ.Rotate(Vector3::YAXIS));
  targetY.Normalize();

  // Camera Right vector is perpendicular to forward & target up
  Vector3 vX = targetY.Cross(vForward);
  vX.Normalize();

  // Camera Up vector is perpendicular to forward and right
  Vector3 vY = vForward.Cross(vX);
  vY.Normalize();

  return Quaternion( vX, vY, vForward );
}

/**
 * @brief Constraint functor to constraint porperties to paths.
 *
 * Vector3 properties will be constrained to the position of the path and
 * Rotation properties will be constrained to follow the tangent of the path
 * given a forward vector in object's local space.
 */
struct PathConstraintFunctor
{
  /**
   * @brief Constructor.
   *
   * @param[in] path The path used in the constraint
   * @param[in] The range of values in the input property which will be mapped to 0..1
   */

  PathConstraintFunctor(Dali::Path path, const Vector2& range ):mPath(path),mRange(range){}

  /**
   * @brief Constructor.
   *
   * @param[in] path The path used in the constraint
   * @param[in] The range of values in the input property which will be mapped to 0..1
   * @param[in] forward Vector in object space which will be aligned with the tangent of the path
   */
  PathConstraintFunctor(Dali::Path path, const Vector2& range,const Vector3& forward ):mPath(path),mForward(forward),mRange(range){}

  /**
   * @brief Functor operator for Vector3 properties
   *
   * @param[in] current Current value of the property
   * @param[in] property The input property used as the parameter for the path
   *
   * @return The position of the path at the given parameter.
   */
  Vector3 operator()(const Vector3& current,
                     const PropertyInput& property)
  {
    float t = ( property.GetFloat() - mRange.x ) / (mRange.y-mRange.x);
    Vector3 position, tangent;
    mPath.Sample( t, position, tangent );
    return position;
  }

  /**
   * @brief Functor operator for Quaternion properties
   *
   * @param[in] current Current value of the property
   * @param[in] property The input property used as the parameter for the path
   *
   * @return The rotation which will align the forward vector and the tangent of the path at the given parameter.
   */
  Quaternion operator()( const Quaternion& current,
                         const PropertyInput& property)
  {
    float t = ( property.GetFloat() - mRange.x ) / (mRange.y-mRange.x);
    Vector3 position, tangent;
    mPath.Sample( t, position, tangent );
    return Quaternion( mForward, tangent );
  }

  Dali::Path  mPath;      ///< The path used
  Vector3     mForward;   ///< Vector in object space which will be aligned with the tangent of the path
  Vector2     mRange;     ///< The range of values in the input property which will be mapped to 0..1
};


} // namespace Dali

#endif // __DALI_CONSTRAINTS_H__
