
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


enum InheritanceMode
{
  DONT_INHERIT_TRANSFORM  = 0,
  INHERIT_POSITION        = 1,
  INHERIT_SCALE           = 4,
  INHERIT_ORIENTATION     = 8,
  INHERIT_ALL             = INHERIT_POSITION | INHERIT_SCALE | INHERIT_ORIENTATION,
};

typedef unsigned int TransformId;
static const TransformId INVALID_TRANSFORM_ID = -1;

enum TransformManagerProperty
{
  TRANSFORM_PROPERTY_POSITION = 0,
  TRANSFORM_PROPERTY_SCALE,
  TRANSFORM_PROPERTY_ANCHOR_POINT,
  TRANSFORM_PROPERTY_PARENT_ORIGIN,
  TRANSFORM_PROPERTY_SIZE,
  TRANSFORM_PROPERTY_WORLD_POSITION,
  TRANSFORM_PROPERTY_WORLD_SCALE,
  TRANSFORM_PROPERTY_WORLD_ORIENTATION,
  TRANSFORM_PROPERTY_WORLD_MATRIX,
  TRANSFORM_PROPERTY_COUNT,
};

class TransformManager
{
public:


  struct TransformComponentAnimatable
  {
    Vector3 mScale;
    Quaternion mOrientation;
    Vector3 mPosition;
  };

  struct TransformComponentStatic
  {
    Vector3 mParentOrigin;
    Vector3 mAnchorPoint;
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
   * Gets the world transform matrix of an exisiting transform component
   */
  const Matrix& GetWorldMatrix( TransformId id ) const;

  /**
   * Gets the world transform matrix of an exisiting transform component
   */
  Matrix& GetWorldMatrix( TransformId id );

  bool IsLocalMatrixDirty( TransformId id ) const
  {
    return mLocalMatrixDirty[mId[id]] == 1u;
  }

  void SetInheritPosition( TransformId id, bool inherit );

  void SetInheritScale( TransformId id, bool inherit );

  void SetInheritOrientation( TransformId id, bool inherit );

  bool GetInheritPosition( TransformId id );

  bool GetInheritScale( TransformId id );

  bool GetInheritOrientation( TransformId id );

  /**
   * Recomputes all world transform matrices
   */
  void Update();

  void ResetToBaseValue();

  Vector3& GetVector3PropertyValue( TransformId id, TransformManagerProperty property );
  const Vector3& GetVector3PropertyValue( TransformId id, TransformManagerProperty property ) const;
  float& GetVector3PropertyComponentValue(TransformId id, TransformManagerProperty property,unsigned int component );
  void SetVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value );
  void SetVector3PropertyComponentValue( TransformId id, TransformManagerProperty property, float value, unsigned int component );
  void BakeVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value );
  void BakeRelativeVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value );
  void BakeMultiplyVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value );
  void BakeVector3PropertyComponentValue( TransformId id, TransformManagerProperty property, float value, unsigned int component );
  void BakeXVector3PropertyValue( TransformId id, TransformManagerProperty property, float value );
  void BakeYVector3PropertyValue( TransformId id, TransformManagerProperty property, float value );
  void BakeZVector3PropertyValue( TransformId id, TransformManagerProperty property, float value );
  Quaternion& GetQuaternionPropertyValue( TransformId id );
  const Quaternion& GetQuaternionPropertyValue( TransformId id ) const;
  void SetQuaternionPropertyValue( TransformId id, const Quaternion& q );
  void BakeQuaternionPropertyValue( TransformId id, const Quaternion& q );
  void BakeRelativeQuaternionPropertyValue( TransformId id, const Quaternion& q );


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
  std::vector<TransformComponentAnimatable> mTxComponentAnimatable;
  std::vector<TransformComponentStatic> mTxComponentStatic;
  std::vector<unsigned int> mInheritanceMode;
  std::vector<TransformId> mComponentId;
  std::vector<Vector3> mSize;
  std::vector<TransformId> mParent;
  std::vector<Matrix> mWorld;
  std::vector<Matrix> mLocal;

  std::vector<TransformComponentAnimatable> mTxComponentAnimatableBaseValue;
  std::vector<Vector3> mSizeBase;

  std::vector<unsigned int> mComponentDirty;
  std::vector<unsigned int> mSizeDirty;
  std::vector<unsigned int> mLocalMatrixDirty;

  bool mReorder;
  unsigned int mComponentCount;
};


} //namespace SceneGraph
} //namespace Internal
} //namespace Dali


#endif /* TRANSFORM_MANAGER_H_ */
