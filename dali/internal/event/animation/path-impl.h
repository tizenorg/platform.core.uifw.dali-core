#ifndef __DALI_INTERNAL_PATH_H__
#define __DALI_INTERNAL_PATH_H__

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
#include <dali/public-api/animation/path.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/math/matrix.h>

namespace Dali
{
namespace Internal
{
typedef IntrusivePtr<Path> PathPtr;

/**
 * A 3D path
 */
class Path : public BaseObject
{
public:

  /**
   * Construct a new path
   */
  static Path* New();

  /**
   * Constructor
   */
  Path();

protected:
  /**
   * virtual destructor
   */
  virtual ~Path();

  /**
   * Copy constructor
   * Allow cloning of this object
   */
  Path(const Path& p)
  : mPoint(p.mPoint),
    mControlPoint(p.mControlPoint )
  {
  }

private:

  /**
   * Don't allow copy operator
   */
  Path& operator=(const Path& rhs);

public:

  /**
   * Returns a clone to the given path
   */
  static Path* Clone(const Path& path)
  {
    return new Path(path);
  }

  /**
   * @copydoc Dali::Path::AddPoint
   */
  void AddPoint(const Vector3& point );

  /**
   * @copydoc Dali::Path::GenerateControlPoints
   */
  void GenerateControlPoints( float curvature );

  /**
   * @copydoc Dali::Path::Sample
   */
  void Sample( float t, Vector3& position, Vector3& tangent ) const;

  /**
   * Sample position at point t.
   *
   * @param[in] progress  A floating point value between 0.0 and 1.0.
   * @param[out] position The interpolated position at that progress.
   */
  Vector3 SamplePosition( float t ) const;

  /**
   * @brief Sample tangent at point t.
   *
   * @param[in] progress  A floating point value between 0.0 and 1.0.
   * @param[out] tangent The interpolated tangent at that progress.
   */
  Vector3 SampleTangent( float t ) const;

  /**
   * @copydoc Dali::Path::GetPoint
   */
  Vector3& GetPoint( size_t index );

  /**
   * @copydoc Dali::Path::GetControlPoint
   */
  Vector3& GetControlPoint( size_t index );


private:

  /**
   * Helper function to calculate the segment and local progress in that segment
   * given a progress
   *
   * @param[in] t Progress
   * @param[out] segment Segment for t
   * @param[out] tLocal Local progress in the segment
   *
   * @return false if there is no segments in the path. True otherwise.
   */
  bool FindSegmentAndProgress( float t, unsigned int& segment, float& tLocal ) const;

  /**
   * Helper function to calculate to number of segments in the path
   */
  unsigned int GetNumberOfSegments() const;

  std::vector<Vector3> mPoint;            ///< Interpolation points
  std::vector<Vector3> mControlPoint;     ///< Control points

  static const Matrix  mBasis;            ///< Basis matrix for the curve
};

} // Internal

// Get impl of handle
inline Internal::Path& GetImplementation(Dali::Path& path)
{
  DALI_ASSERT_ALWAYS( path && "Path handle is empty" );
  Dali::RefObject& object = path.GetBaseObject();
  return static_cast<Internal::Path&>(object);
}

inline const Internal::Path& GetImplementation(const Dali::Path& path)
{
  DALI_ASSERT_ALWAYS( path && "Path handle is empty" );
  const Dali::RefObject& object = path.GetBaseObject();
  return static_cast<const Internal::Path&>(object);
}


} // Dali

#endif //__DALI_INTERNAL_KEY_FRAMES_H__
