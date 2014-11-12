#ifndef __DALI_PATH_H__
#define __DALI_PATH_H__

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
#include <dali/public-api/object/base-handle.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Path;
}
/**
 * @brief
 *
 *
 */
class DALI_IMPORT_API Path : public BaseHandle
{
public:
  /**
   * @brief Create an initialized Path handle.
   *
   * @return a handle to a newly allocated Dali resource.
   */
  static Path New();

  /**
   * @brief Downcast an Object handle to Path handle.
   *
   * If handle points to a KeyFrames object the downcast produces
   * valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a Path object or an uninitialized handle
   */
  static Path DownCast( BaseHandle handle );

  /**
   * @brief Create an uninitialized Path handle.
   *
   * This can be initialized with Path::New(). Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  Path();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Path();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  Path(const Path& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  Path& operator=(const Path& rhs);

  /**
   * @brief Add an interpolation point.
   *
   * For each Interpolation point added there will be two new control points.
   * @param[in] point
   */
  void AddPoint(const Vector3& point );

  /**
   * @brief Automatic generation of control points
   *
   *
   */
  void GenerateControlPoints( float curvature );

  /**
   * @brief Sample path at point t.
   *
   *
   */
  void Sample( float t, Vector3& position, Vector3& tangent );


  Vector3& GetPoint( size_t index );
  Vector3& GetControlPoint( size_t index );


public: // Not intended for application developers
  /**
   * @brief This constructor is used by Dali::New() methods.
   *
   * @param[in] Path A pointer to an internal path resource
   */
  explicit DALI_INTERNAL Path(Internal::Path* path);
};

} // namespace Dali

#endif // __DALI_KEY_FRAMES_H__
