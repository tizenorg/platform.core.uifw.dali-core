
#ifndef TRANSFORM_MANAGER_H_
#define TRANSFORM_MANAGER_H_

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

// INTERNAL INCLUDES
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/internal/update/manager/free-list.h>

// EXTERNAL INCLUDES
#include <vector>


namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

typedef unsigned int TransformId;
static const TransformId INVALID_TRANSFORM_ID = -1;

enum TransformManagerProperty
{
  TRANSFORM_PROPERTY_POSITION = 0,
  TRANSFORM_PROPERTY_SCALE,
  TRANSFORM_PROPERTY_ANCHOR_POINT,
  TRANSFORM_PROPERTY_PARENT_ORIGIN,
  TRANSFORM_PROPERTY_SIZE,
  TRANSFORM_PROPERTY_COUNT,
};

class TransformManager
{
public:

  struct TransformComponent
  {
    Vector3 mPosition;
    Vector3 mParentOrigin;
    Vector3 mAnchorPoint;
    Vector3 mScale;
    Quaternion mOrientation;
  };

  /**
   * Default constructor
   */
  TransformManager();

  /**
   * Class destructor
   */
  ~TransformManager();

  /**
   * Add a new transform component to the manager
   */
  TransformId CreateTransform();

  /**
   * Removes an existing transform component
   */
  void RemoveTransform(TransformId tx);

  /**
   * Sets the parent transform of an existing component
   */
  void SetParent( TransformId tx, TransformId txParent );

  /**
   * Get an existing transform component
   */
  TransformComponent GetTransformComponent( TransformId tx );

  /**
   * Updates a transform component
   */
  void SetTransform( TransformId tx, const Vector3& position, const Vector3& scale,  const Vector3& size, const Quaternion& orientation, const Vector3& parentOrigin, const Vector3& anchorPoint );

  /**
   * Gets the world transform matrix of an exisiting transform component
   */
  const Matrix& GetWorldMatrix( TransformId id );


  /**
   * Recomputes all world transform matrices
   */
  void Update();

  Vector3& GetVector3PropertyValue( TransformId id, TransformManagerProperty property );
  float& GetVector3PropertyComponentValue(TransformId id, TransformManagerProperty property,unsigned int component );
  void SetVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value );
  void SetVector3PropertyComponentValue( TransformId id, TransformManagerProperty property, float value, unsigned int component );
  void BakeVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value );
  void BakeVector3PropertyComponentValue( TransformId id, TransformManagerProperty property, float value, unsigned int component );
  void BakeXVector3PropertyValue( TransformId id, TransformManagerProperty property, float value );
  void BakeYVector3PropertyValue( TransformId id, TransformManagerProperty property, float value );
  void BakeZVector3PropertyValue( TransformId id, TransformManagerProperty property, float value );
  Quaternion& GetQuaternionPropertyValue( TransformId id );
  void SetQuaternionPropertyValue( TransformId id, const Quaternion& q );
  void BakeQuaternionPropertyValue( TransformId id, const Quaternion& q );


private:

  //Helper struct to order components
  struct SOrderItem
  {
    SOrderItem():id(INVALID_TRANSFORM_ID),level(0){}
    SOrderItem( TransformId i , int l ):id(i),level(l){}
    bool operator<(const SOrderItem& item) const {return level < item.level;}

    TransformId  id;
    unsigned int level;
  };

  /**
   * Swaps two components in the vectors
   */
  void SwapComponents( unsigned int i, unsigned int j );

  /**
   * Reorder components in hierarchic order
   */
  void ReorderComponents();

  FreeList mId;
  std::vector<TransformComponent> mTxComponent;
  std::vector<TransformId> mComponentId;
  std::vector<Vector3> mSize;
  std::vector<TransformId> mParent;
  std::vector<Matrix> mWorld;
  std::vector<Matrix> mLocal;

  std::vector<Vector3> mPositionBase;
  std::vector<Vector3> mScaleBase;
  std::vector<Vector3> mSizeBase;
  std::vector<Quaternion> mOrientationBase;
  std::vector<unsigned int> mComponentDirty;

  bool mReorder;
  unsigned int mComponentCount;
};


} //namespace SceneGraph
} //namespace Internal
} //namespace Dali


#endif /* TRANSFORM_MANAGER_H_ */
