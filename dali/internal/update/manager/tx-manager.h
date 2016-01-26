
#ifndef TX_MANAGER_H_
#define TX_MANAGER_H_

#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/internal/update/manager/free-list.h>
#include <algorithm>
#include <vector>
#include <iostream>

namespace Dali
{
namespace Internal
{

typedef unsigned int TxId;

static TxId INVALID_ID = -1;

struct TxComponent
{
  TxComponent()
  :mPosition(0.0f,0.0f,0.0f),
   mScale(1.0f,1.0f,1.0f),
   mSize(1.0f,1.0f,1.0f),
   mOrientation(0.0f,0.0f,0.0f,1.0f),
   mParentOrigin(-0.5f,-0.5f,-0.5f),
   mAnchorPoint(0.0f,0.0f,0.0f)
  {}

  Vector3 mPosition;
  Vector3 mScale;
  Vector3 mSize;
  Quaternion mOrientation;
  Vector3 mParentOrigin;
  Vector3 mAnchorPoint;
};

class TxManager
{
public:

  /**
   * Add a new transform component to the manager
   */
  TxId CreateTransform(const Vector3& position = Vector3(0.0f,0.0f,0.0f), const Vector3& scale = Vector3(1.0f,1.0f,1.0f), const Quaternion& orientation  = Quaternion(0.0f,0.0f,0.0f, 1.0f))
  {
    if( mTxComponent.empty() || mTxComponent.size() == mComponentCount )
    {
      //Make room for another component
      mTxComponent.resize(mComponentCount+1);
      mParent.resize(mComponentCount+1);
      mTxComponent.resize(mComponentCount+1);
      mWorld.resize(mComponentCount+1);
      mLocal.resize(mComponentCount+1);
      mComponentId.resize( mComponentCount+1);
    }

    TxId id = mId.Add(mComponentCount);
    mTxComponent[mComponentCount].mPosition = position;
    mTxComponent[mComponentCount].mScale = scale;
    mTxComponent[mComponentCount].mOrientation = orientation;
    mTxComponent[mComponentCount].mSize = Vector3(1.0f,1.0f,1.0f);
    mComponentId[mComponentCount] = id;
    mParent[mComponentCount] = INVALID_ID;

    mComponentCount++;

    return id;
  }

  /**
   * Removes an existing transform component
   */
  void RemoveTransform(TxId tx)
  {
    unsigned int index = mId[tx];
    if( index < mComponentCount-1 )
    {
      //Move the last element to the gap
      mTxComponent[index] = mTxComponent[mComponentCount-1];

      TxId lastItemId = mComponentId[mComponentCount-1];
      mId[ lastItemId ] = index;
      mComponentId[index] = lastItemId;
    }

    mId.Remove( tx );
    mComponentCount--;

  }

  /**
   * Sets the parent transform of an existing component
   */
  void SetParent( TxId tx, TxId txParent )
  {
    mParent[ mId[tx] ] = txParent;
    mReorder = true;
  }

  /**
   * Get an existing transform component
   */
  TxComponent GetTx( TxId tx )
  {
    return mTxComponent[ mId[tx] ];
  }

  void SetTransform( TxId tx, const Vector3& position, const Vector3& scale,  const Vector3& size, const Quaternion& orientation )
  {
    unsigned int index = mId[tx];
    mTxComponent[index].mPosition = position;
    mTxComponent[index].mScale = scale;
    mTxComponent[index].mSize = size;
    mTxComponent[index].mOrientation = orientation;
  }

  /**
   * Sets the position of an existing transform component
   */
  void SetPosition( TxId tx, const Vector3& position )
  {
    mTxComponent[ mId[tx] ].mPosition = position;
  }

  /*
   * Sets the scale of an existing transform component
   */
  void SetScale( TxId tx, const Vector3& scale )
  {
    mTxComponent[ mId[tx] ].mScale = scale;
  }

  /**
   * Sets the orientation of an existing transform component
   */
  void SetOrientation( TxId tx, const Quaternion& orientation )
  {
    mTxComponent[ mId[tx] ].mOrientation = orientation;
  }

  /**
   * Gets the world transform matrix of an exisiting transform component
   */
  const Matrix& GetWorldTx( TxId id )
  {
    return mWorld[ mId[id] ];
  }

  /**
   * Recomputes all world transform matrices
   */
  void Update()
  {
    if( mReorder )
    {
      //If some trnasform component has change its parent since last update
      //we need to reorder the vectors
      ReorderComponents();
      mReorder = false;
    }

    //Iterate through all components to compute its world matrix
    for( unsigned int i(0); i<mComponentCount; ++i )
    {
      if( mParent[i] != INVALID_ID )
      {
        unsigned int parentIndex = mId[mParent[i] ];
        Vector3 position = mTxComponent[i].mPosition + mTxComponent[i].mParentOrigin * mTxComponent[ parentIndex ].mSize;
        mLocal[i].SetTransformComponents( mTxComponent[i].mScale,mTxComponent[i].mOrientation,  position );
        Matrix::Multiply( mWorld[i], mLocal[i], mWorld[ parentIndex ]);
      }
      else
      {
        mWorld[i].SetTransformComponents( mTxComponent[i].mScale,mTxComponent[i].mOrientation, mTxComponent[i].mPosition );
      }
    }
  }

private:

  /*
   * Swaps two components in the vectors
   */
  void SwapComponents( unsigned int i, unsigned int j )
  {
    std::swap( mTxComponent[i], mTxComponent[j] );
    std::swap( mParent[i], mParent[j] );
    std::swap( mComponentId[i], mComponentId[j] );

    mId[ mComponentId[i] ] = i;
    mId[ mComponentId[j] ] = j;
  }

  struct SOrderItem
  {
    TxId  id;
    int level;

    SOrderItem():id(INVALID_ID),level(0){}
    SOrderItem( TxId i , int l ):id(i),level(l){}
    bool operator<(const SOrderItem& item) const {return level < item.level;}
  };

  //Reorder components in hierarchic order ( shouldn't be a common operation )
  void ReorderComponents()
  {
    std::vector<SOrderItem> mOrderedComponents(mComponentCount);
    TxId parentId;
    for( size_t i(0); i<mComponentCount; ++i )
    {
      mOrderedComponents[i].id = mComponentId[i];
      parentId = mParent[i];
      while( parentId != INVALID_ID )
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

  FreeList mId;
  std::vector<TxComponent> mTxComponent;
  std::vector<TxId> mComponentId;
  std::vector<TxId> mParent;
  std::vector<Matrix> mWorld;
  std::vector<Matrix> mLocal;

  bool mReorder;
  unsigned int mComponentCount;
};

}
}

#endif /* TX_MANAGER_H_ */
