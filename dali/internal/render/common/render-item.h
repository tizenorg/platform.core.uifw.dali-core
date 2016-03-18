#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/math/matrix.h>
#include <dali/internal/update/nodes/node.h>
//todor
#include <iostream>
#include <string>

namespace Dali
{

namespace Internal
{

namespace Render
{
class Renderer;
}

namespace SceneGraph
{

/**
 * A RenderItem contains all the data needed for rendering
 */
struct RenderItem
{
  /**
   * Construct a new RenderItem
   */
  static RenderItem* New();

  /**
   * Non-virtual destructor; RenderItem is not suitable as a base class.
   */
  ~RenderItem();

  /**
   * Overriden delete operator.
   * Deletes the RenderItem from its global memory pool
   */
  void operator delete( void* ptr );

  Matrix            mModelMatrix;
  Matrix            mModelViewMatrix;
  Vector3           mSize;
  Render::Renderer* mRenderer;
  Node*             mNode;
  int               mDepthIndex;
  bool              mIsOpaque:1;

  //todornew
  void SetName( std::string name )
  {
    mName = name;
  }
  void SetName2( std::string name )
  {
    //mName2 = name;
  }
  std::string GetName() const
  {
    return mName;
    //return std::string( "blah" );
  }
  void SetClippingMode( Dali::ClippingMode::Type mode )
  {
    //std::cout << "todor SETTING clipping mode of RenderItem:" << mName << " to:" << (int)mode << std::endl;
    mClippingMode = mode;
  }
  Dali::ClippingMode::Type GetClippingMode() const
  {
    //std::cout << "todor GETTING clipping mode of RenderItem:" << mName << " :" << (int)mClippingMode << std::endl;
    return mClippingMode;
  }
  void SetClippingId( int id )
  {
    mClippingId = id;
  }
  int GetClippingId() const
  {
    return mClippingId;
  }
  //todordel
#if 0
  void SetVisited()
  {
    ++mVisitedId;
  }
  bool IsVisited( int visitedId )//const
  {
    return mVisitedId == visitedId;
  }
#endif

private:

  /**
   * Private constructor. See RenderItem::New
   */
  RenderItem();

  // RenderItems should not be copied as they are heavy
  RenderItem( const RenderItem& item );
  RenderItem& operator = ( const RenderItem& item );

  //todor
  //Matrix                       mModelViewMatrix;
  //Render::Renderer*            mRenderer;
  //todor MOVE
  Dali::ClippingMode::Type     mClippingMode;//todor
  int                          mClippingId;//todor
  //int                          mDepthIndex;
  //bool                         mIsOpaque:1;
  //int mVisitedId;
  std::string mName;//todor
  //char pad1[128];
  //std::string mName2;//todor del
  //char pad2[128];
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDER_ITEM_H__
