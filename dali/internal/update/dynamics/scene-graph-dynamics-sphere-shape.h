#ifndef __SCENE_GRAPH_DYNAMICS_SPHERE_SHAPE_H__
#define __SCENE_GRAPH_DYNAMICS_SPHERE_SHAPE_H__

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

// BASE CLASS HEADERS
#include <dali/internal/update/dynamics/scene-graph-dynamics-shape.h>

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/event-to-update.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class DynamicsWorld;

class DynamicsSphereShape : public DynamicsShape
{
public:
  /**
   * Constructor.
   * @param[in] world   The dynamics world object
   */
  DynamicsSphereShape(DynamicsWorld& world);

  /**
   * Destructor.
   */
  virtual ~DynamicsSphereShape();

  /**
   * Initialize the sphere.
   * @param[in] radius  The radius of the sphere.
   */
  void Initialize( float radius );

private:
  // unimplemented copy constructor and assignment operator
  DynamicsSphereShape(const DynamicsSphereShape&);
  DynamicsSphereShape& operator=(const DynamicsSphereShape&);

}; // class DynamicsSphereShape

// Messages for DynamicsSphereShape

inline void InitializeDynamicsSphereShapeMessage( EventToUpdate& eventToUpdate, const DynamicsSphereShape& shape, const float radius )
{
  typedef MessageValue1< DynamicsSphereShape, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shape, &DynamicsSphereShape::Initialize, radius );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __SCENE_GRAPH_DYNAMICS_SPHERE_SHAPE_H__
