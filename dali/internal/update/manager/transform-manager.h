
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
#include <dali/internal/update/common/animatable-property.h>

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

class TransformManager;

struct Vector3PropertyComponent
{
  bool mIsClean;
  Vector3 mValue;
  Vector3 mBaseValue;

  void ResetToBaseValue()
  {
    mValue = mBaseValue;
    mIsClean = true;
  }
};

struct QuaternionPropertyComponent
{
  bool mIsClean;
  Quaternion mValue;
  Quaternion mBaseValue;

  void ResetToBaseValue()
  {
    mValue = mBaseValue;
    mIsClean = true;
  }
};

enum TransformManagerProperty
{
  TRANSFORM_PROPERTY_POSITION = 0,
  TRANSFORM_PROPERTY_SCALE,
  TRANSFORM_PROPERTY_ANCHOR_POINT,
  TRANSFORM_PROPERTY_PARENT_ORIGIN,
  TRANSFORM_PROPERTY_COUNT,
};

class TransformManager
{
public:

  struct TransformComponent
  {
    Vector3 mSize;
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
   * Gets the local transform matrix of an exisiting transform component
   */
  const Matrix& GetLocalMatrix( TransformId id );

  /**
   * Recomputes all world transform matrices
   */
  void Update();

  Vector3PropertyComponent& GetVector3Property( TransformId id, TransformManagerProperty property )
  {
    switch( property )
    {
      case TRANSFORM_PROPERTY_POSITION:
        return mPositionProperty[ mId[id] ];
      case TRANSFORM_PROPERTY_SCALE:
        return mScaleProperty[ mId[id] ];
      default:
      {
        DALI_ASSERT_ALWAYS(false);
        return mPositionProperty[ mId[id] ];
      }
    }
  }

  QuaternionPropertyComponent& GetQuaternionProperty( TransformId id )
  {
    return mOrientationProperty[ mId[id] ];
  }

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
  std::vector<TransformId> mParent;
  std::vector<Matrix> mWorld;
  std::vector<Matrix> mLocal;
  std::vector<Vector3PropertyComponent> mPositionProperty;
  std::vector<Vector3PropertyComponent> mScaleProperty;
  std::vector<QuaternionPropertyComponent> mOrientationProperty;



  bool mReorder;
  unsigned int mComponentCount;
};

template <typename T>
class TransformManagerPropertyHandler : public AnimatablePropertyBase
{
public:
  virtual Dali::Property::Type GetType() const = 0;
  virtual T& Get(size_t bufferIndex) = 0;
  virtual const T& Get(size_t bufferIndex) const = 0;
  virtual void Set(BufferIndex bufferIndex, const T& value) = 0;
  virtual void Bake(BufferIndex bufferIndex, const T& value) = 0;
  virtual void BakeX(BufferIndex bufferIndex, float value){}
  virtual void BakeY(BufferIndex bufferIndex, float value){}
  virtual void BakeZ(BufferIndex bufferIndex, float value){}
  virtual void BakeRelative(BufferIndex bufferIndex, const T& value) = 0;
  virtual void BakeRelativeMultiply(BufferIndex bufferIndex, const T& value) = 0;
  virtual void ResetToBaseValue(BufferIndex updateBufferIndex) = 0;
  virtual bool IsClean() const = 0;
};

class Vector3PropertyHandler : public TransformManagerPropertyHandler<Vector3>
{
public:

  Vector3PropertyHandler()
  :mTxManager(0),
   mId(INVALID_TRANSFORM_ID),
   mProperty(TRANSFORM_PROPERTY_COUNT)
  {}

  Vector3PropertyHandler(TransformManagerProperty property)
  :mTxManager(0),
   mId(INVALID_TRANSFORM_ID),
   mProperty(property)
  {}

  Vector3PropertyHandler( const Vector3PropertyHandler& p )
  :mTxManager(p.mTxManager),
   mId(p.mId),
   mProperty(p.mProperty)
  {}

  Vector3PropertyHandler& operator=(const Vector3PropertyHandler& p)
  {
    mTxManager = p.mTxManager;
    mId = p.mId;
    mProperty = p.mProperty;
    return *this;
  }

  virtual Dali::Property::Type GetType() const
  {
    return Dali::PropertyTypes::Get<Vector3>();
  }

  Vector3& Get(size_t bufferIndex)
  {
    //DALI_ASSERT_ALWAYS( false );
    Vector3PropertyComponent& property = mTxManager->GetVector3Property( mId, mProperty );
    return property.mValue;
  }

  const Vector3& Get(size_t bufferIndex) const
  {
    Vector3PropertyComponent& property = mTxManager->GetVector3Property( mId, mProperty );
    return property.mValue;
  }

  void Set(BufferIndex bufferIndex, const Vector3& value)
  {
    Vector3PropertyComponent& property = mTxManager->GetVector3Property( mId, mProperty );
    property.mValue = value;
    property.mIsClean = false;
  }

  void Bake(BufferIndex bufferIndex, const Vector3& value)
  {
    Vector3PropertyComponent& property = mTxManager->GetVector3Property( mId, mProperty );
    property.mValue = property.mBaseValue = value;
    property.mIsClean = false;
  }

  void BakeX(BufferIndex bufferIndex, float value)
  {
    Vector3PropertyComponent& property = mTxManager->GetVector3Property( mId, mProperty );
    property.mValue.x = property.mBaseValue.x = value;
    property.mIsClean = false;
  }

  void BakeY(BufferIndex bufferIndex, float value)
  {
    Vector3PropertyComponent& property = mTxManager->GetVector3Property( mId, mProperty );
    property.mValue.y = property.mBaseValue.y = value;
    property.mIsClean = false;
  }

  void BakeZ(BufferIndex bufferIndex, float value)
  {
    Vector3PropertyComponent& property = mTxManager->GetVector3Property( mId, mProperty );
    property.mValue.z = property.mBaseValue.z = value;
    property.mIsClean = false;
  }

  void BakeRelative(BufferIndex bufferIndex, const Vector3& value)
  {
    //FERRAN: TODO
    Vector3PropertyComponent& property = mTxManager->GetVector3Property( mId, mProperty );
    property.mValue = property.mBaseValue = value;
    property.mIsClean = false;
  }

  void BakeRelativeMultiply(BufferIndex bufferIndex, const Vector3& value)
  {
    //FERRAN: TODO
    Vector3PropertyComponent& property = mTxManager->GetVector3Property( mId, mProperty );
    property.mValue = property.mBaseValue = value;
    property.mIsClean = false;
  }

  virtual void ResetToBaseValue(BufferIndex updateBufferIndex)
  {
  }

  virtual bool IsClean() const
  {
    Vector3PropertyComponent& property = mTxManager->GetVector3Property( mId, mProperty );
    return property.mIsClean;
  }

  TransformManager* mTxManager;
  TransformId mId;
  TransformManagerProperty mProperty;
};

class QuaternionPropertyHandler : public TransformManagerPropertyHandler<Quaternion>
{
public:

  QuaternionPropertyHandler()
  :mTxManager(0),
   mId(INVALID_TRANSFORM_ID)
  {}



  QuaternionPropertyHandler( const QuaternionPropertyHandler& p )
  :mTxManager(p.mTxManager),
   mId(p.mId)
  {}

  QuaternionPropertyHandler& operator=(const QuaternionPropertyHandler& p)
  {
    mTxManager = p.mTxManager;
    mId = p.mId;
    return *this;
  }

  virtual Dali::Property::Type GetType() const
  {
    return Dali::PropertyTypes::Get<Quaternion>();
  }

  Quaternion& Get(size_t bufferIndex)
  {
    //DALI_ASSERT_ALWAYS( false );
    QuaternionPropertyComponent& property = mTxManager->GetQuaternionProperty( mId );
    return property.mValue;
  }

  const Quaternion& Get(size_t bufferIndex) const
  {
    QuaternionPropertyComponent& property = mTxManager->GetQuaternionProperty( mId );
    return property.mValue;
  }

  void Set(BufferIndex bufferIndex, const Quaternion& value)
  {
    QuaternionPropertyComponent& property = mTxManager->GetQuaternionProperty( mId );
    property.mValue = value;
    property.mIsClean = false;
  }

  void Bake(BufferIndex bufferIndex, const Quaternion& value)
  {
    QuaternionPropertyComponent& property = mTxManager->GetQuaternionProperty( mId );
    property.mValue = property.mBaseValue = value;
    property.mIsClean = false;
  }

  void BakeRelative(BufferIndex bufferIndex, const Quaternion& value)
  {
    //FERRAN: TODO
    QuaternionPropertyComponent& property = mTxManager->GetQuaternionProperty( mId );
    property.mValue = property.mBaseValue = value;
    property.mIsClean = false;
  }

  void BakeRelativeMultiply(BufferIndex bufferIndex, const Quaternion& value)
  {
    //FERRAN: TODO
    QuaternionPropertyComponent& property = mTxManager->GetQuaternionProperty( mId );
    property.mValue = property.mBaseValue = value;
    property.mIsClean = false;
  }

  virtual void ResetToBaseValue(BufferIndex updateBufferIndex)
  {
  }

  virtual bool IsClean() const
  {
    QuaternionPropertyComponent& property = mTxManager->GetQuaternionProperty( mId );
    return property.mIsClean;
  }

  TransformManager* mTxManager;
  TransformId mId;
};

} //namespace SceneGraph
} //namespace Internal
} //namespace Dali


#endif /* TRANSFORM_MANAGER_H_ */
