

#ifndef FREE_LIST_H_
#define FREE_LIST_H_

#include <dali/public-api/common/dali-vector.h>

namespace Dali
{

namespace Internal
{


struct FreeList
{
  FreeList()
  :mData(),
   mFirstFreeIndex(0)
  {}

  ~FreeList()
  {}

  //Add a new item to the list
  unsigned int Add( unsigned int value )
  {
    if( mData.Empty() || mFirstFreeIndex == mData.Size() )
    {
      //Make room for another item
      size_t size = mData.Size();
      mData.Resize( size+1 );
      mData[size] = size+1;
      mFirstFreeIndex = size;
    }

    //Update first free index
    unsigned int index = mFirstFreeIndex;
    mFirstFreeIndex = mData[mFirstFreeIndex];

    mData[index] = value;
    return index;
  }

  //Remove an item from the list
  void Remove( unsigned int index )
  {
    mData[index] = mFirstFreeIndex;
    mFirstFreeIndex = index;
  }

  unsigned int& operator[]( const unsigned int index )
  {
    return mData[index];
  }

  Dali::Vector<unsigned int> mData;
  unsigned int mFirstFreeIndex;
};

}
}

#endif /* FREE_LIST_H_ */
