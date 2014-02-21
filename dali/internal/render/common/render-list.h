#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_LIST_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_LIST_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/public-api/math/rect.h>
#include <dali/internal/render/common/render-item.h>
#include <dali/internal/common/owner-container.h>

namespace Dali
{

typedef Rect<int> ClippingBox;

namespace Internal
{

namespace SceneGraph
{

class Layer;
class Renderer;

class RenderItem;
typedef OwnerContainer< RenderItem* > RenderItemContainer;

struct RenderList;
typedef OwnerContainer< RenderList* > RenderListContainer;

/**
 * The RenderList structure provides the renderer with a list of renderers and
 * a set of flags to tell it what depth buffering is required.
 */
struct RenderList
{
public:

  /**
   * The RenderFlags describe how the objects are rendered using the depth buffer.
   */
  enum RenderFlags
  {
    DEPTH_TEST     = 0x01, ///< If depth testing should be used
    DEPTH_WRITE    = 0x02, ///< If the depth buffer is writable
    DEPTH_CLEAR    = 0x04, ///< If the depth buffer should first be cleared
    STENCIL_TEST   = 0x08, ///< If stencil testing should be used
    STENCIL_WRITE  = 0x10, ///< If the stencil buffer is writable
    STENCIL_CLEAR  = 0x20  ///< If the stencil buffer should first be cleared
  };

  /**
   * Constructor
   */
  RenderList()
  : mNextFree( 0 ),
    mRenderFlags( 0u ),
    mClippingBox( NULL ),
    mSourceLayer( NULL )
  {
  }

  /**
   * Destructor
   */
  ~RenderList()
  {
    // pointer container deletes the render items
    delete mClippingBox;
  }

  /**
   * Clear the render flags
   */
  void ClearFlags()
  {
    mRenderFlags = 0u;
  }

  /**
   * Set particular render flags
   * @param[in] flags The set of flags to bitwise or with existing flags
   */
  void SetFlags( unsigned int flags )
  {
    mRenderFlags |= flags;
  }

  /**
   * Retrieve the render flags.
   * @return the render flags.
   */
  unsigned int GetFlags() const
  {
    return mRenderFlags;
  }

  /**
   * Reset the render list for next frame
   */
  void Reset()
  {
    // we dont want to delete and re-create the render items every frame
    mNextFree = 0;
    mRenderFlags = 0u;

    delete mClippingBox;
    mClippingBox = NULL;
  }

  /**
   * Reserve space in the render list
   * @param size to reserve
   */
  void Reserve( RenderItemContainer::SizeType size )
  {
    mNextFree = 0;
    mItems.Reserve( size );
  }

  /**
   * @return the capacity of the render list
   */
  RenderItemContainer::SizeType Capacity()
  {
    return mItems.Capacity();
  }

  /**
   * Get next free render item
   * @return reference to the next available RenderItem
   */
  RenderItem& GetNextFreeItem()
  {
    // check if we have enough items, we can only be one behind at worst
    if( mItems.Count() <= mNextFree )
    {
      mItems.PushBack( new RenderItem ); // Push a new empty render item
    }
    // get the item mNextFree points to and increase by one
    RenderItem& item = *mItems[ mNextFree++ ];
    item.Reset();
    return item;
  }

  /**
   * Get item at a given position in the list
   */
  RenderItem& GetItem( RenderItemContainer::SizeType index ) const
  {
    DALI_ASSERT_DEBUG( index < GetCachedItemCount() );
    return *mItems[ index ];
  }

  /**
   * Get renderer from an item in the list
   */
  const Renderer* GetRenderer( RenderItemContainer::SizeType index ) const
  {
    DALI_ASSERT_DEBUG( index < GetCachedItemCount() );
    return mItems[ index ]->GetRenderer();
  }

  /**
   * Get the number of real items
   * Because of caching, the actual size may be bit more
   * @return The number of items
   */
  RenderItemContainer::SizeType Count() const
  {
    return mNextFree;
  }

  /**
   * @return the number of items cached by the list
   */
  RenderItemContainer::SizeType GetCachedItemCount() const
  {
    return mItems.Count();
  }

  /**
   * Tells the render list to reuse the items from the cache
   */
  void ReuseCachedItems()
  {
    mNextFree = mItems.Count();
  }

  /**
   * Predicate to inform if the list is empty
   */
  bool IsEmpty() const
  {
    return (mNextFree == 0);
  }

  /**
   * Set clipping
   * @param clipping on/off
   * @param box for clipping
   */
  void SetClipping( bool clipping, const ClippingBox& box )
  {
    if( clipping )
    {
      ClippingBox* newBox = new ClippingBox( box );
      delete mClippingBox;
      mClippingBox = newBox;
    }
  }

  /**
   * @return true if clipping is on
   */
  bool IsClipping() const
  {
    return (NULL != mClippingBox);
  }

  /**
   * @return the clipping box
   */
  const ClippingBox& GetClippingBox() const
  {
    return *mClippingBox;
  }

  /**
   * @return the container (for sorting)
   */
  RenderItemContainer& GetContainer()
  {
    return mItems;
  }

  /**
   * Do some housekeeping to keep memory consumption low
   */
  void ReleaseUnusedItems()
  {
    // release any non-used RenderItems
    if( mItems.Count() > mNextFree )
    {
      mItems.Resize( mNextFree );
    }
  }

  /**
   * @return the source layer these renderitems originate from
   */
  Layer* GetSourceLayer()
  {
    return mSourceLayer;
  }

  /**
   * @param layer these renderitems originate from
   */
  void SetSourceLayer( Layer* layer )
  {
    mSourceLayer = layer;
  }

private:

  /*
   * Copy constructor and assignment operator not defined
   */
  RenderList( const RenderList& rhs );
  const RenderList& operator=( const RenderList& rhs );

  RenderItemContainer mItems; ///< Each item is a renderer and matrix pair
  RenderItemContainer::SizeType mNextFree;              ///< index for the next free item to use

  unsigned int mRenderFlags; ///< The render flags

  ClippingBox* mClippingBox; ///< The clipping box, in window coordinates, when clipping is enabled
  Layer* mSourceLayer;       ///< The originating layer where the renderers are from

};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDER_LIST_H__
