
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
   * Recomputes all world transform matrices
   */
  void Update();

  Vector3& GetVector3PropertyValue( TransformId id, TransformManagerProperty property )
  {
    switch( property )
    {
      case TRANSFORM_PROPERTY_POSITION:
        return mTxComponent[ mId[id] ].mPosition;
      case TRANSFORM_PROPERTY_SCALE:
        return mTxComponent[ mId[id] ].mScale;
      default:
      {
        DALI_ASSERT_ALWAYS(false);
        return mTxComponent[ mId[id] ].mPosition;
      }
    }
  }
  float& GetVector3PropertyComponentValue(TransformId id, TransformManagerProperty property,unsigned int component )
  {
    switch( property )
    {
      case TRANSFORM_PROPERTY_POSITION:
        return mTxComponent[ mId[id] ].mPosition[component];
        break;
      case TRANSFORM_PROPERTY_SCALE:
        return mTxComponent[ mId[id] ].mScale[component];
        break;
      default:
      {
        DALI_ASSERT_ALWAYS(false);
        return mTxComponent[ mId[id] ].mPosition[component];
      }
    }
  }
  void SetVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value )
  {
    switch( property )
    {
      case TRANSFORM_PROPERTY_POSITION:
      {
        unsigned int index( mId[id] );
        mTxComponent[ index ].mPosition = value;
        mPositionDirty[ index ] = 1u;
        break;
      }
      case TRANSFORM_PROPERTY_SCALE:
      {
        unsigned int index( mId[id] );
        mTxComponent[ index ].mScale = value;
        mScaleDirty[ index ] = 1u;
        break;
      }
      default:
      {
        DALI_ASSERT_ALWAYS(false);
      }
    }
  }

  void SetVector3PropertyComponentValue( TransformId id, TransformManagerProperty property, float value, unsigned int component )
  {
    switch( property )
    {
      case TRANSFORM_PROPERTY_POSITION:
      {
        unsigned int index( mId[id] );
        mTxComponent[ index ].mPosition[component] = value;
        mPositionDirty[ index ] = 1u;
        break;
      }
      case TRANSFORM_PROPERTY_SCALE:
      {
        unsigned int index( mId[id] );
        mTxComponent[ index ].mScale[component] = value;
        mScaleDirty[ index ] = 1u;
        break;
      }
      default:
      {
        DALI_ASSERT_ALWAYS(false);
      }
    }
  }

  void BakeVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value )
  {
    switch( property )
    {
      case TRANSFORM_PROPERTY_POSITION:
      {
        unsigned int index( mId[id] );
        mTxComponent[ index ].mPosition = mPositionBase[index] = value;
        mPositionDirty[ index ] = 1u;
        break;
      }
      case TRANSFORM_PROPERTY_SCALE:
      {
        unsigned int index( mId[id] );
        mTxComponent[ index ].mScale = mScaleBase[index] = value;
        mScaleDirty[ index ] = 1u;
        break;
      }
      default:
      {
        DALI_ASSERT_ALWAYS(false);
      }
    }
  }

  void BakeVector3PropertyComponentValue( TransformId id, TransformManagerProperty property, float value, unsigned int component )
  {
    switch( property )
    {
      case TRANSFORM_PROPERTY_POSITION:
      {
        unsigned int index( mId[id] );
        mTxComponent[ index ].mPosition[component] = mPositionBase[index][component] = value;
        mPositionDirty[ index ] = 1u;
        break;
      }
      case TRANSFORM_PROPERTY_SCALE:
      {
        unsigned int index( mId[id] );
        mTxComponent[ index ].mScale[component] = mScaleBase[index][component] = value;
        mScaleDirty[ index ] = 1u;
        break;
      }
      default:
      {
        DALI_ASSERT_ALWAYS(false);
      }
    }
  }
  void BakeXVector3PropertyValue( TransformId id, TransformManagerProperty property, float value )
  {
    switch( property )
    {
      case TRANSFORM_PROPERTY_POSITION:
      {
        unsigned int index( mId[id] );
        mTxComponent[ index ].mPosition.x = mPositionBase[index].x = value;
        mPositionDirty[ index ] = 1u;
        break;
      }
      case TRANSFORM_PROPERTY_SCALE:
      {
        unsigned int index( mId[id] );
        mTxComponent[ index ].mScale.x = mScaleBase[index].x = value;
        mScaleDirty[ index ] = 1u;
        break;
      }
      default:
      {
        DALI_ASSERT_ALWAYS(false);
      }
    }
  }

  void BakeYVector3PropertyValue( TransformId id, TransformManagerProperty property, float value )
  {
    switch( property )
    {
      case TRANSFORM_PROPERTY_POSITION:
      {
        unsigned int index( mId[id] );
        mTxComponent[ index ].mPosition.y = mPositionBase[index].y = value;
        mPositionDirty[ index ] = 1u;
        break;
      }
      case TRANSFORM_PROPERTY_SCALE:
      {
        unsigned int index( mId[id] );
        mTxComponent[ index ].mScale.y = mScaleBase[index].y = value;
        mScaleDirty[ index ] = 1u;
        break;
      }
      default:
      {
        DALI_ASSERT_ALWAYS(false);
      }
    }
  }

  void BakeZVector3PropertyValue( TransformId id, TransformManagerProperty property, float value )
  {
    switch( property )
    {
      case TRANSFORM_PROPERTY_POSITION:
      {
        unsigned int index( mId[id] );
        mTxComponent[ index ].mPosition.z = mPositionBase[index].z = value;
        mPositionDirty[ index ] = 1u;
        break;
      }
      case TRANSFORM_PROPERTY_SCALE:
      {
        unsigned int index( mId[id] );
        mTxComponent[ index ].mScale.z = mScaleBase[index].z = value;
        mScaleDirty[ index ] = 1u;
        break;
      }
      default:
      {
        DALI_ASSERT_ALWAYS(false);
      }
    }
  }
  bool IsVector3PropertyClean( TransformId id, TransformManagerProperty property ) const
  {
    switch( property )
    {
      case TRANSFORM_PROPERTY_POSITION:
        return !mPositionDirty[ mId[id] ];
      case TRANSFORM_PROPERTY_SCALE:
        return !mScaleDirty[ mId[id] ];
      default:
      {
        DALI_ASSERT_ALWAYS(false);
        return false;
      }
    }
  }

  Quaternion& GetQuaternionPropertyValue( TransformId id )
  {
    return mTxComponent[ mId[id] ].mOrientation;
  }

  void SetQuaternionPropertyValue( TransformId id, const Quaternion& q )
  {
    unsigned int index( mId[id] );
    mTxComponent[ index ].mOrientation = q;
    mOrientationDirty[ index ] = 1u;
  }

  void BakeQuaternionPropertyValue( TransformId id, const Quaternion& q )
  {
    unsigned int index( mId[id] );
    mTxComponent[ index ].mOrientation = mOrientationBase[index] = q;
    mOrientationDirty[ index ] = 1u;
  }

  bool IsQuaternionPropertyClean(TransformId id) const
  {
    return !mOrientationDirty[mId[id]];
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

  std::vector<Vector3> mPositionBase;
  std::vector<Vector3> mScaleBase;
  std::vector<Quaternion> mOrientationBase;

  std::vector<unsigned short> mPositionDirty;
  std::vector<unsigned short> mScaleDirty;
  std::vector<unsigned short> mOrientationDirty;

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
  virtual const float& GetFloatComponent(size_t bufferIndex, unsigned int component)=0;
  virtual void Set(BufferIndex bufferIndex, const T& value) = 0;
  virtual void SetFloatComponent(BufferIndex bufferIndex, float value, unsigned int component){}
  virtual void Bake(BufferIndex bufferIndex, const T& value) = 0;
  virtual void BakeFloatComponent(BufferIndex bufferIndex, float value, unsigned int component){}
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
    return mTxManager->GetVector3PropertyValue( mId, mProperty );
  }

  const Vector3& Get(size_t bufferIndex) const
  {
    return mTxManager->GetVector3PropertyValue( mId, mProperty );
  }

  const float& GetFloatComponent(size_t bufferIndex, unsigned int component )
  {
    return mTxManager->GetVector3PropertyComponentValue( mId, mProperty, component );
  }

  void Set(BufferIndex bufferIndex, const Vector3& value)
  {
    mTxManager->SetVector3PropertyValue( mId, mProperty, value );
  }

  void SetComponent(BufferIndex bufferIndex, float value, unsigned int component)
  {
    mTxManager->SetVector3PropertyComponentValue( mId, mProperty, value, component);
  }

  void BakeComponent(BufferIndex bufferIndex, float value, unsigned int component)
  {
    mTxManager->BakeVector3PropertyComponentValue( mId, mProperty, value, component);
  }

  void Bake(BufferIndex bufferIndex, const Vector3& value)
  {
    mTxManager->BakeVector3PropertyValue(mId, mProperty, value );
  }

  void BakeX(BufferIndex bufferIndex, float value)
  {
    mTxManager->BakeXVector3PropertyValue(mId, mProperty, value );
  }

  void BakeY(BufferIndex bufferIndex, float value)
  {
    mTxManager->BakeYVector3PropertyValue(mId, mProperty, value );
  }

  void BakeZ(BufferIndex bufferIndex, float value)
  {
    mTxManager->BakeZVector3PropertyValue(mId, mProperty, value );
  }

  void SetFloatComponent(BufferIndex bufferIndex, float value, unsigned int component)
  {
    mTxManager->SetVector3PropertyComponentValue( mId, mProperty, value, component);
  }

  void BakeFloatComponent(BufferIndex bufferIndex, float value, unsigned int component)
  {
    mTxManager->BakeVector3PropertyComponentValue( mId, mProperty, value, component);
  }

  void BakeRelative(BufferIndex bufferIndex, const Vector3& value)
  {
    //FERRAN: TODO
    mTxManager->BakeVector3PropertyValue(mId, mProperty, value );
  }

  void BakeRelativeMultiply(BufferIndex bufferIndex, const Vector3& value)
  {
    //FERRAN: TODO
    mTxManager->BakeVector3PropertyValue(mId, mProperty, value );
  }

  virtual void ResetToBaseValue(BufferIndex updateBufferIndex)
  {
  }

  virtual bool IsClean() const
  {
    return mTxManager->IsVector3PropertyClean(mId, mProperty);
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
    return mTxManager->GetQuaternionPropertyValue( mId );
  }

  const Quaternion& Get(size_t bufferIndex) const
  {
    return mTxManager->GetQuaternionPropertyValue( mId );
  }

  const float& GetFloatComponent(size_t bufferIndex, unsigned int component)
  {
    return mTxManager->GetQuaternionPropertyValue( mId ).mVector[component];
  }

  void Set(BufferIndex bufferIndex, const Quaternion& value)
  {
    return mTxManager->SetQuaternionPropertyValue( mId, value );
  }

  void Bake(BufferIndex bufferIndex, const Quaternion& value)
  {
    return mTxManager->BakeQuaternionPropertyValue( mId, value );
  }

  void BakeRelative(BufferIndex bufferIndex, const Quaternion& value)
  {
    //FERRAN: TODO
    return mTxManager->BakeQuaternionPropertyValue( mId, value );
  }

  void BakeRelativeMultiply(BufferIndex bufferIndex, const Quaternion& value)
  {
    //FERRAN: TODO
    return mTxManager->BakeQuaternionPropertyValue( mId, value );
  }

  virtual void ResetToBaseValue(BufferIndex updateBufferIndex)
  {
  }

  virtual bool IsClean() const
  {
    return mTxManager->IsQuaternionPropertyClean(mId);
  }

  TransformManager* mTxManager;
  TransformId mId;
};

} //namespace SceneGraph
} //namespace Internal
} //namespace Dali


#endif /* TRANSFORM_MANAGER_H_ */
