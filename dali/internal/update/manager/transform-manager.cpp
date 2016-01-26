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
  if( mTxComponentAnimatable.empty() || mTxComponentAnimatable.size() == mComponentCount )
  {
    //Make room for another component
    mTxComponentAnimatable.resize(mComponentCount+1);
    mTxComponentStatic.resize(mComponentCount+1);
    mComponentId.resize( mComponentCount+1);
    mSize.resize(mComponentCount+1);
    mParent.resize(mComponentCount+1);
    mWorld.resize(mComponentCount+1);
    mLocal.resize(mComponentCount+1);
    mTxComponentAnimatableBaseValue.resize( mComponentCount + 1);
    mSizeBase.resize(mComponentCount+1);

    mComponentDirty.resize(mComponentCount+1);
    mWorldMatrixDirty.resize(mComponentCount+1);

  }

  TransformId id = mId.Add(mComponentCount);
  mTxComponentAnimatable[mComponentCount].mScale = mTxComponentAnimatableBaseValue[mComponentCount].mScale = Vector3::ONE;
  mTxComponentAnimatable[mComponentCount].mOrientation = mTxComponentAnimatableBaseValue[mComponentCount].mOrientation = Quaternion::IDENTITY;
  mTxComponentAnimatable[mComponentCount].mPosition = mTxComponentAnimatableBaseValue[mComponentCount].mPosition = Vector3::ZERO;

  mTxComponentStatic[mComponentCount].mParentOrigin = Dali::ParentOrigin::DEFAULT - Vector3(0.5,0.5,0.5);
  mTxComponentStatic[mComponentCount].mAnchorPoint = Vector3(0.5,0.5,0.5) - Dali::AnchorPoint::DEFAULT;

  mComponentId[mComponentCount] = id;
  mSize[mComponentCount] = Vector3::ZERO;
  mWorld[mComponentCount].SetIdentity();

  mSizeBase[mComponentCount] = Vector3::ZERO;

  mComponentDirty[mComponentCount] = 0u;
  mWorldMatrixDirty[mComponentCount] = 0u;
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
    mTxComponentAnimatable[index] = mTxComponentAnimatable[mComponentCount-1];
    mTxComponentStatic[index] = mTxComponentStatic[mComponentCount-1];
    mSize[index] = mSize[mComponentCount-1];
    mParent[index] = mParent[mComponentCount-1];
    mTxComponentAnimatableBaseValue[index] = mTxComponentAnimatableBaseValue[mComponentCount-1];
    mSizeBase[index] = mSizeBase[mComponentCount-1];
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

const Matrix& TransformManager::GetWorldMatrix( TransformId id )
{
  return mWorld[ mId[id] ];
}

void TransformManager::ResetToBaseValue()
{
  memcpy( &mTxComponentAnimatable[0], &mTxComponentAnimatableBaseValue[0], sizeof(TransformComponentAnimatable)*mComponentCount );
  memcpy( &mSize[0], &mSizeBase[0], sizeof(Vector3)*mComponentCount );
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
      mWorldMatrixDirty[i] = 1u;
      if( mComponentDirty[i] )
      {
        mComponentDirty[i] = 0u;
        mWorldMatrixDirty[i] = 1u;

        anchorPosition = mTxComponentStatic[i].mAnchorPoint * mSize[i] * mTxComponentAnimatable[i].mScale;
        anchorPosition *= mTxComponentAnimatable[i].mOrientation;
        localPosition = mTxComponentAnimatable[i].mPosition + anchorPosition +
                   mTxComponentStatic[i].mParentOrigin *  mSize[parentIndex];

        mLocal[i].SetTransformComponents( mTxComponentAnimatable[i].mScale,mTxComponentAnimatable[i].mOrientation, localPosition );
      }

      Matrix::Multiply( mWorld[i], mLocal[i], mWorld[parentIndex]);
    }
    else
    {
      anchorPosition = mTxComponentStatic[i].mAnchorPoint * mSize[i] * mTxComponentAnimatable[i].mScale;
      anchorPosition *= mTxComponentAnimatable[i].mOrientation;
      localPosition = mTxComponentAnimatable[i].mPosition + anchorPosition;
      mWorld[i].SetTransformComponents( mTxComponentAnimatable[i].mScale,mTxComponentAnimatable[i].mOrientation, localPosition );
    }
  }
}

void TransformManager::SwapComponents( unsigned int i, unsigned int j )
{
  std::swap( mTxComponentAnimatable[i], mTxComponentAnimatable[j] );
  std::swap( mTxComponentStatic[i], mTxComponentStatic[j] );
  std::swap( mSize[i], mSize[j] );
  std::swap( mParent[i], mParent[j] );
  std::swap( mComponentId[i], mComponentId[j] );
  std::swap( mTxComponentAnimatableBaseValue[i], mTxComponentAnimatableBaseValue[j] );
  std::swap( mSizeBase[i], mSizeBase[j] );
  std::swap( mLocal[i], mLocal[j] );
  std::swap( mComponentDirty[i], mComponentDirty[j] );
  std::swap( mWorldMatrixDirty[i], mWorldMatrixDirty[j] );

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
      return mTxComponentAnimatable[ mId[id] ].mPosition;
    case TRANSFORM_PROPERTY_SCALE:
      return mTxComponentAnimatable[ mId[id] ].mScale;
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
      return mTxComponentStatic[ mId[id] ].mParentOrigin;
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
      return mTxComponentStatic[ mId[id] ].mAnchorPoint;
    case TRANSFORM_PROPERTY_SIZE:
      return mSize[ mId[id] ];
    default:
    {
      DALI_ASSERT_ALWAYS(false);
      return mTxComponentAnimatable[ mId[id] ].mPosition;
    }
  }
}
float& TransformManager::GetVector3PropertyComponentValue(TransformId id, TransformManagerProperty property,unsigned int component )
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
      return mTxComponentAnimatable[ mId[id] ].mPosition[component];
    case TRANSFORM_PROPERTY_SCALE:
      return mTxComponentAnimatable[ mId[id] ].mScale[component];
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
      return mTxComponentStatic[ mId[id] ].mParentOrigin[component];
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
      return mTxComponentStatic[ mId[id] ].mAnchorPoint[component];
    case TRANSFORM_PROPERTY_SIZE:
      return mSize[ mId[id] ][component];
    default:
    {
      DALI_ASSERT_ALWAYS(false);
      return mTxComponentAnimatable[ mId[id] ].mPosition[component];
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
      mTxComponentAnimatable[ index ].mPosition = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponentAnimatable[ index ].mScale = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mParentOrigin = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mAnchorPoint = value;
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
      mTxComponentAnimatable[ index ].mPosition[component] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponentAnimatable[ index ].mScale[component] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mParentOrigin[component] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mAnchorPoint[component] = value;
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
      mTxComponentAnimatable[ index ].mPosition = mTxComponentAnimatableBaseValue[index].mPosition = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponentAnimatable[ index ].mScale = mTxComponentAnimatableBaseValue[index].mScale = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mParentOrigin = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mAnchorPoint = value;
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
      mTxComponentAnimatable[ index ].mPosition[component] = mTxComponentAnimatableBaseValue[index].mPosition[component] = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponentAnimatable[ index ].mScale[component] = mTxComponentAnimatableBaseValue[index].mScale[component] = value;
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
      mTxComponentAnimatable[ index ].mPosition.x = mTxComponentAnimatableBaseValue[index].mPosition.x = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponentAnimatable[ index ].mScale.x = mTxComponentAnimatableBaseValue[index].mScale.x = value;
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
      mTxComponentAnimatable[ index ].mPosition.y = mTxComponentAnimatableBaseValue[index].mPosition.y = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponentAnimatable[ index ].mScale.y = mTxComponentAnimatableBaseValue[index].mScale.y = value;
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
      mTxComponentAnimatable[ index ].mPosition.z = mTxComponentAnimatableBaseValue[index].mPosition.z = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponentAnimatable[ index ].mScale.z = mTxComponentAnimatableBaseValue[index].mScale.z = value;
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
  return mTxComponentAnimatable[ mId[id] ].mOrientation;
}

void TransformManager::SetQuaternionPropertyValue( TransformId id, const Quaternion& q )
{
  unsigned int index( mId[id] );
  mTxComponentAnimatable[ index ].mOrientation = q;
  mComponentDirty[ index ] = 1u;
}

void TransformManager::BakeQuaternionPropertyValue( TransformId id, const Quaternion& q )
{
  unsigned int index( mId[id] );
  mTxComponentAnimatable[ index ].mOrientation = mTxComponentAnimatableBaseValue[index].mOrientation = q;
  mComponentDirty[ index ] = 1u;
}

} //namespace SceneGraph
} //namespace Internal
} //namespace Dali
