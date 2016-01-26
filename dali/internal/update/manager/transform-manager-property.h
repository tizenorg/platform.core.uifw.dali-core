#ifndef TRANSFORM_MANAGER_PROPERTY_H_
#define TRANSFORM_MANAGER_PROPERTY_H_

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

#include <dali/internal/update/manager/transform-manager.h>
#include <dali/internal/update/common/animatable-property.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

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
  virtual void ResetToBaseValue(BufferIndex updateBufferIndex){}
  virtual bool IsClean() const{ return false; }
};

class TransformManagerPropertyVector3 : public TransformManagerPropertyHandler<Vector3>
{
public:

  TransformManagerPropertyVector3()
  :mTxManager(0),
   mId(INVALID_TRANSFORM_ID),
   mProperty(TRANSFORM_PROPERTY_COUNT)
  {}

  TransformManagerPropertyVector3(TransformManagerProperty property)
  :mTxManager(0),
   mId(INVALID_TRANSFORM_ID),
   mProperty(property)
  {}

  TransformManagerPropertyVector3( const TransformManagerPropertyVector3& p )
  :mTxManager(p.mTxManager),
   mId(p.mId),
   mProperty(p.mProperty)
  {}

  TransformManagerPropertyVector3& operator=(const TransformManagerPropertyVector3& p)
  {
    mTxManager = p.mTxManager;
    mId = p.mId;
    mProperty = p.mProperty;
    return *this;
  }

  Dali::Property::Type GetType() const
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

  TransformManager* mTxManager;
  TransformId mId;
  TransformManagerProperty mProperty;
};

class TransformManagerPropertyQuaternion : public TransformManagerPropertyHandler<Quaternion>
{
public:

  TransformManagerPropertyQuaternion()
  :mTxManager(0),
   mId(INVALID_TRANSFORM_ID)
  {}

  TransformManagerPropertyQuaternion( const TransformManagerPropertyQuaternion& p )
  :mTxManager(p.mTxManager),
   mId(p.mId)
  {}

  TransformManagerPropertyQuaternion& operator=(const TransformManagerPropertyQuaternion& p)
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

  TransformManager* mTxManager;
  TransformId mId;
};

} //namespace SceneGraph
} //namespace Internal
} //namespace Dali

#endif // TRANSFORM_MANAGER_PROPERTY_H_
