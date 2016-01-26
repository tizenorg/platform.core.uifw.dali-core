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

namespace
{}

namespace Dali
{
namespace Internal
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
    mParent.resize(mComponentCount+1);
    mWorld.resize(mComponentCount+1);
    mLocal.resize(mComponentCount+1);
  }

  TransformId id = mId.Add(mComponentCount);
  mTxComponent[mComponentCount].mSize = Vector3::ONE;
  mTxComponent[mComponentCount].mPosition = Vector3::ZERO;
  mTxComponent[mComponentCount].mParentOrigin = Dali::ParentOrigin::DEFAULT - Vector3(0.5,0.5,0.5);
  mTxComponent[mComponentCount].mAnchorPoint = Vector3(0.5,0.5,0.5) - Dali::AnchorPoint::DEFAULT;
  mTxComponent[mComponentCount].mScale = Vector3::ONE;
  mTxComponent[mComponentCount].mOrientation = Quaternion::IDENTITY;
  mComponentId[mComponentCount] = id;
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
    mParent[index] = mParent[mComponentCount-1];

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

void TransformManager::SetTransform( TransformId tx, const Vector3& position, const Vector3& scale,  const Vector3& size, const Quaternion& orientation, const Vector3& parentOrigin, const Vector3& anchorPoint )
{
  unsigned int index = mId[tx];
  mTxComponent[index].mPosition = position;
  mTxComponent[index].mScale = scale;
  mTxComponent[index].mOrientation = orientation;
  mTxComponent[index].mParentOrigin = parentOrigin - Vector3(0.5,0.5,0.5);
  mTxComponent[index].mAnchorPoint = Vector3(0.5,0.5,0.5) - anchorPoint;
  mTxComponent[index].mSize = size;
}

const Matrix& TransformManager::GetWorldMatrix( TransformId id )
{
  return mWorld[ mId[id] ];
}

const Matrix& TransformManager::GetLocalMatrix( TransformId id )
{
  return mLocal[ mId[id] ];
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
  for( unsigned int i(0); i<mComponentCount; ++i )
  {
    if( mParent[i] != INVALID_TRANSFORM_ID)
    {
      const unsigned int& parentIndex = mId[mParent[i] ];

      //Compute local position
      Vector3 position = mTxComponent[i].mPosition +
          mTxComponent[i].mParentOrigin *  mTxComponent[parentIndex].mSize +
          mTxComponent[i].mAnchorPoint * mTxComponent[i].mSize;

      mLocal[i].SetTransformComponents( mTxComponent[i].mScale,mTxComponent[i].mOrientation,  position );

      Matrix::Multiply( mWorld[i], mLocal[i], mWorld[parentIndex]);
    }
    else
    {
      mWorld[i].SetTransformComponents( mTxComponent[i].mScale,mTxComponent[i].mOrientation, mTxComponent[i].mPosition );
    }
  }
}

void TransformManager::SwapComponents( unsigned int i, unsigned int j )
{
  std::swap( mTxComponent[i], mTxComponent[j] );
  std::swap( mParent[i], mParent[j] );
  std::swap( mComponentId[i], mComponentId[j] );

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

}
}
