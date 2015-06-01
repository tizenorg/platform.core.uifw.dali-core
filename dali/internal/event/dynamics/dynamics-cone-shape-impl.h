#ifndef __DYNAMICS_CONE_SHAPE_IMPL_H__
#define __DYNAMICS_CONE_SHAPE_IMPL_H__

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
 * A cone.
 */
class DynamicsConeShape : public DynamicsShape
{
public:
  /**
   * Constructor.
   * @copydoc Dali::DynamicsShape::NewCone
   */
  DynamicsConeShape(const float radius, const float length);

protected:
  /**
   * Destructor.
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~DynamicsConeShape();

private:
  // unimplemented copy constructor and assignment operator
  DynamicsConeShape(const DynamicsConeShape&);
  DynamicsConeShape& operator=(const DynamicsConeShape&);

public:
  /**
   * Get an axis aligned bounding box for this shape
   * @return An axis aligned bounding box for this shape
   */
  virtual Vector3 GetAABB() const;
}; // class DynamicsConeShape

} // namespace Internal

} // namespace Dali

#endif // __DYNAMICS_CONE_SHAPE_IMPL_H__
