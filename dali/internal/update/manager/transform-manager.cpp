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
#include <dali/public-api/common/compile-time-assert.h>

//EXTERNAL INCLUDES
#include <algorithm>
#include <cstring>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

namespace
{
 static float gDefaultTransformComponentAnimatableData[] = { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f };
 static float gDefaultTransformComponentStaticData[] = { 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f };

 DALI_COMPILE_TIME_ASSERT( sizeof(gDefaultTransformComponentAnimatableData) == sizeof(TransformComponentAnimatable) );
 DALI_COMPILE_TIME_ASSERT( sizeof(gDefaultTransformComponentStaticData) == sizeof(TransformComponentStatic) );
}

TransformManager::TransformManager()
:mComponentCount(0),
 mReorder(false)
{}

TransformManager::~TransformManager()
{}

TransformId TransformManager::CreateTransform()
{
  //Get id for the new component
  TransformId id = mId.Add(mComponentCount);

  if( mTxComponentAnimatable.Size() <= mComponentCount )
  {
    //Make room for another component
    mTxComponentAnimatable.Resize(mComponentCount+1);
    mTxComponentStatic.Resize(mComponentCount+1);
    mInheritanceMode.Resize(mComponentCount+1);
    mComponentId.Resize(mComponentCount+1);
    mSize.Resize(mComponentCount+1);
    mParent.Resize(mComponentCount+1);
    mWorld.Resize(mComponentCount+1);
    mLocal.Resize(mComponentCount+1);
    mTxComponentAnimatableBaseValue.Resize( mComponentCount + 1);
    mSizeBase.Resize(mComponentCount+1);
    mComponentDirty.Resize(mComponentCount+1);
    mLocalMatrixDirty.Resize(mComponentCount+1);
  }

  //Set default values
  memcpy( &mTxComponentAnimatable[mComponentCount], &gDefaultTransformComponentAnimatableData, sizeof( TransformComponentAnimatable ) );
  memcpy( &mTxComponentStatic[mComponentCount], &gDefaultTransformComponentStaticData, sizeof( TransformComponentStatic ) );
  memcpy( &mTxComponentAnimatableBaseValue[mComponentCount], &gDefaultTransformComponentAnimatableData, sizeof( TransformComponentAnimatable ) );
  mInheritanceMode[mComponentCount] = INHERIT_ALL;
  mComponentId[mComponentCount] = id;
  mSize[mComponentCount] = Vector3(0.0f,0.0f,0.0f);
  mParent[mComponentCount] = INVALID_TRANSFORM_ID;
  mLocal[mComponentCount].SetIdentity();
  mWorld[mComponentCount].SetIdentity();
  mSizeBase[mComponentCount] = Vector3(0.0f,0.0f,0.0f);
  mComponentDirty[mComponentCount] = 0u;
  mLocalMatrixDirty[mComponentCount] = 0u;

  mComponentCount++;

  return id;
}

void TransformManager::RemoveTransform(TransformId id)
{
  unsigned int index = mId[id];
  if( index < mComponentCount-1 )
  {
    //Move the last element to the gap
    mTxComponentAnimatable[index] = mTxComponentAnimatable[mComponentCount-1];
    mTxComponentStatic[index] = mTxComponentStatic[mComponentCount-1];
    mInheritanceMode[index] = mInheritanceMode[mComponentCount-1];
    mSize[index] = mSize[mComponentCount-1];
    mParent[index] = mParent[mComponentCount-1];
    mWorld[index] = mWorld[mComponentCount-1];
    mLocal[index] = mLocal[mComponentCount-1];
    mTxComponentAnimatableBaseValue[index] = mTxComponentAnimatableBaseValue[mComponentCount-1];
    mSizeBase[index] = mSizeBase[mComponentCount-1];
    mComponentDirty[index] = mComponentDirty[mComponentCount-1];
    mLocalMatrixDirty[index] = mLocalMatrixDirty[mComponentCount-1];

    TransformId lastItemId = mComponentId[mComponentCount-1];
    mId[ lastItemId ] = index;
    mComponentId[index] = lastItemId;

    mReorder = true;
  }

  mId.Remove( id );
  mComponentCount--;
}

void TransformManager::SetParent( TransformId id, TransformId parentId )
{
  DALI_ASSERT_ALWAYS( id != parentId );
  mParent[ mId[id] ] = parentId;
  mReorder = true;
}

const Matrix& TransformManager::GetWorldMatrix( TransformId id ) const
{
  return mWorld[ mId[id] ];
}

Matrix& TransformManager::GetWorldMatrix( TransformId id )
{
  return mWorld[ mId[id] ];
}

void TransformManager::SetInheritPosition( TransformId id, bool inherit )
{
  unsigned int index = mId[id];
  if( inherit )
  {
    mInheritanceMode[ index ] &= INHERIT_POSITION;
  }
  else
  {
    mInheritanceMode[ index ] &= ~INHERIT_POSITION;
  }

  mComponentDirty[index] = 1u;
}

void TransformManager::SetInheritScale( TransformId id, bool inherit )
{
  unsigned int index = mId[id];
  if( inherit )
  {
    mInheritanceMode[ index ] &= INHERIT_SCALE;
  }
  else
  {
    mInheritanceMode[ index ] &= ~INHERIT_SCALE;
  }

  mComponentDirty[index] = 1u;
}

void TransformManager::SetInheritOrientation( TransformId id, bool inherit )
{
  unsigned int index = mId[id];
  if( inherit )
  {
    mInheritanceMode[ index ] &= INHERIT_ORIENTATION;
  }
  else
  {
    mInheritanceMode[ index ] &= ~INHERIT_ORIENTATION;
  }

  mComponentDirty[index] = 1u;
}

void TransformManager::ResetToBaseValue()
{
  if( mComponentCount )
  {
    memcpy( &mTxComponentAnimatable[0], &mTxComponentAnimatableBaseValue[0], sizeof(TransformComponentAnimatable)*mComponentCount );
    memcpy( &mSize[0], &mSizeBase[0], sizeof(Vector3)*mComponentCount );
    memset( &mLocalMatrixDirty[0], 0u, sizeof(unsigned int)*mComponentCount );
  }
}

void TransformManager::Update()
{
  if( mReorder )
  {
    //If some transform component has change its parent or has been removed since last update
    //we need to reorder the vectors
    ReorderComponents();
    mReorder = false;
  }

  //Iterate through all components to compute its world matrix
  Vector3 anchorPosition;
  Vector3 localPosition;
  for( unsigned int i(0); i<mComponentCount; ++i )
  {
    if( DALI_LIKELY( mInheritanceMode[i] != DONT_INHERIT_TRANSFORM && mParent[i] != INVALID_TRANSFORM_ID ) )
    {
      const unsigned int& parentIndex = mId[mParent[i] ];
      if( DALI_LIKELY( mInheritanceMode[i] == INHERIT_ALL ) )
      {
        if( mComponentDirty[i] || mLocalMatrixDirty[parentIndex])
        {
          //Full transform inherited
          mComponentDirty[i] = 0u;
          mLocalMatrixDirty[i] = 1u;

          anchorPosition = ( Vector3(0.5f,0.5f,0.5f) - mTxComponentStatic[i].mAnchorPoint ) * mSize[i] * mTxComponentAnimatable[i].mScale;
          anchorPosition *= mTxComponentAnimatable[i].mOrientation;
          localPosition = mTxComponentAnimatable[i].mPosition + anchorPosition + (mTxComponentStatic[i].mParentOrigin - Vector3(0.5f,0.5f,0.5f)) *  mSize[parentIndex];
          mLocal[i].SetTransformComponents( mTxComponentAnimatable[i].mScale,mTxComponentAnimatable[i].mOrientation, localPosition );
        }

        Matrix::Multiply( mWorld[i], mLocal[i], mWorld[parentIndex]);
      }
      else
      {
        //Some components are not inherited
        Vector3 parentPosition, parentScale;
        Quaternion parentOrientation;
        const Matrix& parentMatrix = mWorld[parentIndex];
        parentMatrix.GetTransformComponents( parentPosition, parentOrientation, parentScale );

        Vector3 localScale = mTxComponentAnimatable[i].mScale;
        if( (mInheritanceMode[i] & INHERIT_SCALE) == 0 )
        {
          //Don't inherit scale
          localScale /= parentScale;
        }

        Quaternion localOrientation( mTxComponentAnimatable[i].mOrientation );
        if( (mInheritanceMode[i] & INHERIT_ORIENTATION) == 0 )
        {
          //Don't inherit orientation
          parentOrientation.Invert();
          localOrientation = parentOrientation * mTxComponentAnimatable[i].mOrientation;
        }

        if( (mInheritanceMode[i] & INHERIT_POSITION) == 0 )
        {
          //Don't inherit position
          mLocal[i].SetTransformComponents( localScale, localOrientation, Vector3::ZERO );
          Matrix::Multiply( mWorld[i], mLocal[i], parentMatrix );
          mWorld[i].SetTranslation( mTxComponentAnimatable[i].mPosition);
        }
        else
        {
          anchorPosition = ( Vector3(0.5f,0.5f,0.5f) - mTxComponentStatic[i].mAnchorPoint ) * mSize[i] * mTxComponentAnimatable[i].mScale;
          anchorPosition *= mTxComponentAnimatable[i].mOrientation;
          localPosition = mTxComponentAnimatable[i].mPosition + anchorPosition + (mTxComponentStatic[i].mParentOrigin - Vector3(0.5f,0.5f,0.5f)) *  mSize[parentIndex];
          mLocal[i].SetTransformComponents( localScale, localOrientation, localPosition );
          Matrix::Multiply( mWorld[i], mLocal[i], parentMatrix );
        }
      }
    }
    else  //Component has no parent or doesn't inherit transform
    {
      anchorPosition = ( Vector3(0.5f,0.5f,0.5f) - mTxComponentStatic[i].mAnchorPoint ) * mSize[i] * mTxComponentAnimatable[i].mScale;
      anchorPosition *= mTxComponentAnimatable[i].mOrientation;
      localPosition = mTxComponentAnimatable[i].mPosition + anchorPosition;
      mWorld[i].SetTransformComponents( mTxComponentAnimatable[i].mScale, mTxComponentAnimatable[i].mOrientation, localPosition );
    }
  }
}

void TransformManager::SwapComponents( unsigned int i, unsigned int j )
{
  std::swap( mTxComponentAnimatable[i], mTxComponentAnimatable[j] );
  std::swap( mTxComponentStatic[i], mTxComponentStatic[j] );
  std::swap( mInheritanceMode[i], mInheritanceMode[j] );
  std::swap( mSize[i], mSize[j] );
  std::swap( mParent[i], mParent[j] );
  std::swap( mComponentId[i], mComponentId[j] );
  std::swap( mTxComponentAnimatableBaseValue[i], mTxComponentAnimatableBaseValue[j] );
  std::swap( mSizeBase[i], mSizeBase[j] );
  std::swap( mLocal[i], mLocal[j] );
  std::swap( mComponentDirty[i], mComponentDirty[j] );

  mId[ mComponentId[i] ] = i;
  mId[ mComponentId[j] ] = j;
}

void TransformManager::ReorderComponents()
{
  Vector<SOrderItem> mOrderedComponents;
  mOrderedComponents.Resize(mComponentCount);

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

  std::sort( mOrderedComponents.Begin(), mOrderedComponents.End());
  for( size_t i(0); i<mOrderedComponents.Size()-1; ++i )
  {
    SwapComponents( mId[mOrderedComponents[i].id], i);
  }
}

Vector3& TransformManager::GetVector3PropertyValue( TransformId id, TransformManagerProperty property )
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      unsigned int index( mId[id] );
      mComponentDirty[ index ] = 1u;
      return mTxComponentAnimatable[ index ].mPosition;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mComponentDirty[ index ] = 1u;
      return mTxComponentAnimatable[ index ].mScale;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      unsigned int index( mId[id] );
      mComponentDirty[ index ] = 1u;
      return mTxComponentStatic[ index ].mParentOrigin;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      unsigned int index( mId[id] );
      mComponentDirty[ index ] = 1u;
      return mTxComponentStatic[ index ].mAnchorPoint;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      unsigned int index( mId[id] );
      mComponentDirty[ index ] = 1u;
      return mSize[ index ];
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
      return mTxComponentAnimatable[ mId[id] ].mPosition;
    }
  }
}

const Vector3& TransformManager::GetVector3PropertyValue( TransformId id, TransformManagerProperty property ) const
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      return mTxComponentAnimatable[ mId[id] ].mPosition;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      return mTxComponentAnimatable[ mId[id] ].mScale;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      return mTxComponentStatic[ mId[id] ].mParentOrigin;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      return mTxComponentStatic[ mId[id] ].mAnchorPoint;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      return mSize[ mId[id] ];
    }
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
    {
      unsigned int index( mId[id] );
      mComponentDirty[ index ] = 1u;
      return mTxComponentAnimatable[ index ].mPosition[component];
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mComponentDirty[ index ] = 1u;
      return mTxComponentAnimatable[ index ].mScale[component];
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      unsigned int index( mId[id] );
      mComponentDirty[ index ] = 1u;
      return mTxComponentStatic[ index ].mParentOrigin[component];
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      unsigned int index( mId[id] );
      mComponentDirty[ index ] = 1u;
      return mTxComponentStatic[ index ].mAnchorPoint[component];
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      unsigned int index( mId[id] );
      mComponentDirty[ index ] = 1u;
      return mSize[ index ][component];
    }
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

void TransformManager::BakeRelativeVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value )
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      unsigned int index( mId[id] );
      mTxComponentAnimatable[ index ].mPosition = mTxComponentAnimatableBaseValue[index].mPosition = mTxComponentAnimatable[ index ].mPosition + value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponentAnimatable[ index ].mScale = mTxComponentAnimatableBaseValue[index].mScale = mTxComponentAnimatable[ index ].mScale + value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mParentOrigin = mTxComponentStatic[ index ].mParentOrigin + value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mAnchorPoint = mTxComponentStatic[ index ].mAnchorPoint + value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      unsigned int index( mId[id] );
      mSize[ index ] = mSizeBase[index] = mSize[ index ] + value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeMultiplyVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value )
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      unsigned int index( mId[id] );
      mTxComponentAnimatable[ index ].mPosition = mTxComponentAnimatableBaseValue[index].mPosition = mTxComponentAnimatable[ index ].mPosition * value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mId[id] );
      mTxComponentAnimatable[ index ].mScale = mTxComponentAnimatableBaseValue[index].mScale = mTxComponentAnimatable[ index ].mScale * value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mParentOrigin = mTxComponentStatic[ index ].mParentOrigin * value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mAnchorPoint = mTxComponentStatic[ index ].mAnchorPoint * value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      unsigned int index( mId[id] );
      mSize[ index ] = mSizeBase[index] = mSize[ index ] * value;
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
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mParentOrigin.x = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mAnchorPoint.x = value;
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
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mParentOrigin.y = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mAnchorPoint.y = value;
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
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mParentOrigin.z = value;
      mComponentDirty[ index ] = 1u;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      unsigned int index( mId[id] );
      mTxComponentStatic[ index ].mAnchorPoint.z = value;
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
  unsigned int index( mId[id] );
  mComponentDirty[ index ] = 1u;
  return mTxComponentAnimatable[ index ].mOrientation;
}

const Quaternion& TransformManager::GetQuaternionPropertyValue( TransformId id ) const
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

void TransformManager::BakeRelativeQuaternionPropertyValue( TransformId id, const Quaternion& q )
{
  unsigned int index( mId[id] );
  mTxComponentAnimatable[ index ].mOrientation = mTxComponentAnimatableBaseValue[index].mOrientation = mTxComponentAnimatable[ index ].mOrientation * q;
  mComponentDirty[ index ] = 1u;
}

} //namespace SceneGraph
} //namespace Internal
} //namespace Dali
