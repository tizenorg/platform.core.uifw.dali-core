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
#include <iostream>
// CLASS HEADER
#include <dali/internal/update/manager/transform-manager.h>

//INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>

//EXTERNAL INCLUDES
#include <algorithm>
#include <cstring>

namespace
{}

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

TransformManager::TransformManager()
:mReorder(false),
 mComponentCount(0)
{}

TransformManager::~TransformManager()
{}

TransformId TransformManager::CreateTransform()
{
  if( mTxComponent.empty() || mTxComponent.size() == mComponentCount )
  {
    //Make room for another component
    mTxComponent.resize(mComponentCount+1);
    mComponentId.resize( mComponentCount+1);
    mSize.resize(mComponentCount+1);
    mParent.resize(mComponentCount+1);
    mWorld.resize(mComponentCount+1);
    mLocal.resize(mComponentCount+1);

    mPositionBase.resize(mComponentCount+1);
    mScaleBase.resize(mComponentCount+1);
    mSizeBase.resize(mComponentCount+1);
    mOrientationBase.resize(mComponentCount+1);

    mComponentDirty.resize(mComponentCount+1);
  }

  TransformId id = mId.Add(mComponentCount);
  mTxComponent[mComponentCount].mPosition = Vector3::ZERO;
  mTxComponent[mComponentCount].mParentOrigin = Dali::ParentOrigin::DEFAULT - Vector3(0.5,0.5,0.5);
  mTxComponent[mComponentCount].mAnchorPoint = Vector3(0.5,0.5,0.5) - Dali::AnchorPoint::DEFAULT;
  mTxComponent[mComponentCount].mScale = Vector3::ONE;
  mTxComponent[mComponentCount].mOrientation = Quaternion::IDENTITY;
  mComponentId[mComponentCount] = id;
  mSize[mComponentCount] = Vector3::ZERO;

  mScaleBase[mComponentCount] = Vector3::ONE;
  mSizeBase[mComponentCount] = Vector3::ZERO;

  mComponentDirty[mComponentCount] = 0u;
  mParent[mComponentCount] = INVALID_TRANSFORM_ID;

  mComponentCount++;

  return id;
}

void TransformManager::RemoveTransform(TransformId tx)
{
  unsigned int index = mId[tx];
  if( index < mComponentCount-1 )
  {
    //Move the last element to the gap
    mTxComponent[index] = mTxComponent[mComponentCount-1];
    mSize[index] = mSize[mComponentCount-1];
    mParent[index] = mParent[mComponentCount-1];
    mPositionBase[index] = mPositionBase[mComponentCount-1];
    mScaleBase[index] = mScaleBase[mComponentCount-1];
    mSizeBase[index] = mSizeBase[mComponentCount-1];
    mOrientationBase[index] = mOrientationBase[mComponentCount-1];
    mComponentDirty[index] = mComponentDirty[mComponentCount-1];

    TransformId lastItemId = mComponentId[mComponentCount-1];
    mId[ lastItemId ] = index;
    mComponentId[index] = lastItemId;
  }

  mId.Remove( tx );
  mComponentCount--;
}

void TransformManager::SetParent( TransformId tx, TransformId txParent )
{
  DALI_ASSERT_ALWAYS( tx != INVALID_TRANSFORM_ID && tx != txParent );
  mParent[ mId[tx] ] = txParent;
  mReorder = true;
}

TransformManager::TransformComponent TransformManager::GetTransformComponent( TransformId tx )
{
  return mTxComponent[ mId[tx] ];
}

void TransformManager::SetTransform( TransformId tx, const Vector3& /*position*/, const Vector3& /*scale*/,  const Vector3& size, const Quaternion& /*orientation*/, const Vector3& parentOrigin, const Vector3& anchorPoint )
{
  unsigned int index = mId[tx];
  mTxComponent[index].mParentOrigin = parentOrigin - Vector3(0.5,0.5,0.5);
  mTxComponent[index].mAnchorPoint = Vector3(0.5,0.5,0.5) - anchorPoint;
  mSize[index] = size;
  mComponentDirty[ index ] = 1u;
}

const Matrix& TransformManager::GetWorldMatrix( TransformId id )
{
  return mWorld[ mId[id] ];
}

void TransformManager::Update()
{
  if( mReorder )
  {
    //If some transform component has change its parent since last update
    //we need to reorder the vectors
    ReorderComponents();
    mReorder = false;
  }


  //Iterate through all components to compute its world matrix
  Vector3 anchorPosition;
  Vector3 localPosition;
  for( unsigned int i(0); i<mComponentCount; ++i )
  {
    if( mParent[i] != INVALID_TRANSFORM_ID)
    {
      const unsigned int& parentIndex = mId[mParent[i] ];

      if( mComponentDirty[i] )
      {
        mComponentDirty[i] = 0u;
        anchorPosition = mTxComponent[i].mAnchorPoint * mSize[i] * mTxComponent[i].mScale;
        anchorPosition *= mTxComponent[i].mOrientation;
        localPosition = mTxComponent[i].mPosition + anchorPosition +
                   mTxComponent[i].mParentOrigin *  mSize[parentIndex];

        mLocal[i].SetTransformComponents( mTxComponent[i].mScale,mTxComponent[i].mOrientation, localPosition );
      }

      Matrix::Multiply( mWorld[i], mLocal[i], mWorld[parentIndex]);
    }
    else
    {
//      anchorPosition = mTxComponent[i].mAnchorPoint * mSize[i] * mTxComponent[i].mScale;
//      anchorPosition *= mTxComponent[i].mOrientation;
//      localPosition = mTxComponent[i].mPosition + anchorPosition;

      mWorld[i].SetTransformComponents( mTxComponent[i].mScale,mTxComponent[i].mOrientation, mTxComponent[i].mPosition );
    }

    mTxComponent[i].mPosition = mPositionBase[i];
    mTxComponent[i].mScale = mScaleBase[i];
    mTxComponent[i].mOrientation = mOrientationBase[i];
    mSize[i] = mSizeBase[i];
  }
}

void TransformManager::SwapComponents( unsigned int i, unsigned int j )
{
  std::swap( mTxComponent[i], mTxComponent[j] );
  std::swap( mSize[i], mSize[j] );
  std::swap( mParent[i], mParent[j] );
  std::swap( mComponentId[i], mComponentId[j] );
  std::swap( mPositionBase[i], mPositionBase[j] );
  std::swap( mScaleBase[i], mScaleBase[j] );
  std::swap( mSizeBase[i], mSizeBase[j] );
  std::swap( mOrientationBase[i], mOrientationBase[j] );
  std::swap( mLocal[i], mLocal[j] );
  std::swap( mComponentDirty[i], mComponentDirty[j] );

  mId[ mComponentId[i] ] = i;
  mId[ mComponentId[j] ] = j;
}

void TransformManager::ReorderComponents()
{
  std::vector<SOrderItem> mOrderedComponents(mComponentCount);
  TransformId parentId;
  for( size_t i(0); i<mComponentCount; ++i )
  {
    mOrderedComponents[i].id = mComponentId[i];
    parentId = mParent[i];
    while( parentId != INVALID_TRANSFORM_ID )
    {
      mOrderedComponents[i].level++;
      parentId = mParent[ mId[parentId] ];
    }
  }

  std::sort( mOrderedComponents.begin(), mOrderedComponents.end());
  for( size_t i(0); i<mOrderedComponents.size()-1; ++i )
  {
    SwapComponents( mId[mOrderedComponents[i].id], i);
  }
}


Vector3& TransformManager::GetVector3PropertyValue( TransformId id, TransformManagerProperty property )
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
      return mTxComponent[ mId[id] ].mPosition;
    case TRANSFORM_PROPERTY_SCALE:
      return mTxComponent[ mId[id] ].mScale;
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
      return mTxComponent[ mId[id] ].mParentOrigin;
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
      return mTxComponent[ mId[id] ].mAnchorPoint;
    case TRANSFORM_PROPERTY_SIZE:
      return mSize[ mId[id] ];
    default:
    {
      DALI_ASSERT_ALWAYS(false);
      return mTxComponent[ mId[id] ].mPosition;
    }
  }
}
float& TransformManager::GetVector3PropertyComponentValue(TransformId id, TransformManagerProperty property,unsigned int component )
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
      return mTxComponent[ mId[id] ].mPosition[component];
    case TRANSFORM_PROPERTY_SCALE:
      return mTxComponent[ mId[id] ].mScale[component];
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
      return mTxComponent[ mId[id] ].mParentOrigin[component];
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
      return mTxComponent[ mId[id] ].mAnchorPoint[component];
    case TRANSFORM_PROPERTY_SIZE:
      return mSize[ mId[id] ][component];
    default:
    {
      DALI_ASSERT_ALWAYS(false);
      return mTxComponent[ mId[id] ].mPosition[component];
    }
  }
}
void TransformManager::SetVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value )
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mPosition = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mScale = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mParentOrigin = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mAnchorPoint = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      unsigned int index( mId[id] );
      mSize[ index ] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::SetVector3PropertyComponentValue( TransformId id, TransformManagerProperty property, float value, unsigned int component )
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mPosition[component] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mScale[component] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mParentOrigin[component] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mAnchorPoint[component] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      unsigned int index( mId[id] );
      mSize[ index ][component] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value )
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mPosition = mPositionBase[index] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mScale = mScaleBase[index] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mParentOrigin = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mAnchorPoint = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      unsigned int index( mId[id] );
      mSize[ index ] = mSizeBase[index] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeVector3PropertyComponentValue( TransformId id, TransformManagerProperty property, float value, unsigned int component )
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mPosition[component] = mPositionBase[index][component] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mScale[component] = mScaleBase[index][component] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      unsigned int index( mId[id] );
      mSize[ index ][component] = mSizeBase[index][component] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}
void TransformManager::BakeXVector3PropertyValue( TransformId id, TransformManagerProperty property, float value )
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mPosition.x = mPositionBase[index].x = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mScale.x = mScaleBase[index].x = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      unsigned int index( mId[id] );
      mSize[ index ].x = mSizeBase[index].x = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeYVector3PropertyValue( TransformId id, TransformManagerProperty property, float value )
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mPosition.y = mPositionBase[index].y = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mScale.y = mScaleBase[index].y = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      unsigned int index( mId[id] );
      mSize[ index ].y = mSizeBase[index].y = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeZVector3PropertyValue( TransformId id, TransformManagerProperty property, float value )
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mPosition.z = mPositionBase[index].z = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponent[ index ].mScale.z = mScaleBase[index].z = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      unsigned int index( mId[id] );
      mSize[ index ].z = mSizeBase[index].z = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

Quaternion& TransformManager::GetQuaternionPropertyValue( TransformId id )
{
  return mTxComponent[ mId[id] ].mOrientation;
}

void TransformManager::SetQuaternionPropertyValue( TransformId id, const Quaternion& q )
{
  unsigned int index( mId[id] );
  mTxComponent[ index ].mOrientation = q;
  mComponentDirty[ index ] = 1u;
}

void TransformManager::BakeQuaternionPropertyValue( TransformId id, const Quaternion& q )
{
  unsigned int index( mId[id] );
  mTxComponent[ index ].mOrientation = mOrientationBase[index] = q;
  mComponentDirty[ index ] = 1u;
}

} //namespace SceneGraph
} //namespace Internal
} //namespace Dali
