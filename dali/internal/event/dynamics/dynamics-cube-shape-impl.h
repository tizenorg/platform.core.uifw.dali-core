#ifndef __DYNAMICS_CUBE_SHAPE_IMPL_H__
#define __DYNAMICS_CUBE_SHAPE_IMPL_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// BASE CLASS HEADERS
#include <dali/internal/event/dynamics/dynamics-shape-impl.h>

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-declarations.h>

namespace Dali
{

namespace Internal
{

/**
 * A cube
 */
class DynamicsCubeShape : public DynamicsShape
{
public:
  /**
   * Constructor.
   * @copydoc Dali::DynamicsShape::NewCube
   */
  DynamicsCubeShape(const Vector3& dimensions);

protected:
  /**
   * Destructor.
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~DynamicsCubeShape();

private:
  // unimplemented copy constructor and assignment operator
  DynamicsCubeShape(const DynamicsCubeShape&);
  DynamicsCubeShape& operator=(const DynamicsCubeShape&);

public:
  /**
   * Get an axis aligned bounding box for this shape
   * @return An axis aligned bounding box for this shape
   */
  virtual Vector3 GetAABB() const;
}; // class DynamicsCubeShape

} // namespace Internal

} // namespace Dali

#endif // __DYNAMICS_CUBE_SHAPE_IMPL_H__
