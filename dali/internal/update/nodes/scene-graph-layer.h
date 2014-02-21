#ifndef __DALI_INTERNAL_SCENE_GRAPH_LAYER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_LAYER_H__

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
#include <dali/public-api/actors/layer.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment-declarations.h>

namespace Dali
{

namespace Internal
{

class RenderableAttachment;

// value types used by messages
template <> struct ParameterType< Dali::Layer::SortFunctionType >
: public BasicType< Dali::Layer::SortFunctionType > {};

namespace SceneGraph
{

/**
 * Layers have a "depth" relative to all other layers in the scene-graph.
 * Non-layer child nodes (and their attachments) are considered part of the layer.
 *
 * Layers are rendered separately, and the depth buffer is cleared before each layer is rendered.
 * Objects in higher layers, are rendered after (in front of) objects in lower layers.
 */
class Layer : public Node
{
public:

  typedef Dali::Layer::SortFunctionType SortFunctionType;

  // Creation methods

  /**
   * Construct a new Layer.
   * @return A smart-pointer to a newly allocated Node
   */
  static SceneGraph::Layer* New();

  /**
   * Virtual destructor
   */
  virtual ~Layer();

  /**
   * From Node, to convert a node to a layer.
   * @return The layer.
   */
  virtual Layer* GetLayer()
  {
    return this;
  }

  /**
   * Sets the sort-function of a layer.
   * @param [in] function The new sort-function.
   */
  void SetSortFunction( Dali::Layer::SortFunctionType function );

  /**
   * Retrieve the function used to sort semi-transparent geometry in this layer.
   * @return The sort function.
   */
  Dali::Layer::SortFunctionType GetSortFunction() const
  {
    return mSortFunction;
  }

  /**
   * Sets whether clipping is enabled for a layer.
   * @param [in] enabled True if clipping is enabled.
   */
  void SetClipping( bool enabled );

  /**
   * Query whether clipping is enabled for a layer.
   * @return True if clipping is enabled.
   */
  bool IsClipping() const
  {
    return mIsClipping;
  }

  /**
   * Sets the clipping box of a layer, in window coordinates.
   * The contents of the layer will not be visible outside this box, when clipping is
   * enabled. The default clipping box is empty (0,0,0,0).
   * @param [in] box The clipping box
   */
  void SetClippingBox( const ClippingBox& box );

  /**
   * Retrieves the clipping box of a layer, in window coordinates.
   * @return The clipping box
   */
  const ClippingBox& GetClippingBox() const
  {
    return mClippingBox;
  }

  /**
   * @copydoc Dali::Layer::SetDepthTestDisabled()
   */
  void SetDepthTestDisabled( bool disable );

  /**
   * @copydoc Dali::Layer::IsDepthTestDisabled()
   */
  bool IsDepthTestDisabled() const;

  /**
   * Enables the reuse of the model view matrices of all renderers for this layer
   * @param[in] updateBufferIndex The current update buffer index.
   * @param value to set
   */
  void SetReuseRenderers( BufferIndex updateBufferIndex, bool value )
  {
    mAllChildTransformsClean[ updateBufferIndex ] = value;
  }

  /**
   * @return True if all children have been clean for two consequtive frames
   */
  bool CanReuseRenderers()
  {
    return mAllChildTransformsClean[ 0 ] && mAllChildTransformsClean[ 1 ];
  }

private:

  /**
   * Private constructor.
   * See also Layer::New()
   */
  Layer();

  // Undefined
  Layer(const Layer&);

  // Undefined
  Layer& operator=(const Layer& rhs);

public: // For update-algorithms

  RenderableAttachmentContainer stencilRenderables;
  RenderableAttachmentContainer transparentRenderables;
  RenderableAttachmentContainer opaqueRenderables;
  RenderableAttachmentContainer overlayRenderables;

private:

  SortFunctionType mSortFunction; ///< Used to sort semi-transparent geometry

  ClippingBox mClippingBox;           ///< The clipping box, in window coordinates
  bool mAllChildTransformsClean[ 2 ]; ///< True if all child nodes transforms are clean,
                                      /// double buffered as we need two clean frames before we can reuse N-1 for N+1
                                      /// this allows us to cache render items when layer is "static"
  bool mIsClipping:1;                 ///< True when clipping is enabled
  bool mDepthTestDisabled:1;          ///< Whether depth test is disabled.
};

// Messages for Layer

/**
 * Create a message to set the sort-function of a layer
 * @param[in] layer The layer
 * @param[in] function The new sort-function.
 */
inline void SetSortFunctionMessage( EventToUpdate& eventToUpdate, const Layer& layer, Dali::Layer::SortFunctionType function )
{
  typedef MessageValue1< Layer, Dali::Layer::SortFunctionType > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &layer, &Layer::SetSortFunction, function );
}

/**
 * Create a message for enabling/disabling layer clipping
 * @param[in] layer The layer
 * @param[in] enabled True if clipping is enabled
 */
inline void SetClippingMessage( EventToUpdate& eventToUpdate, const Layer& layer, bool enabled )
{
  typedef MessageValue1< Layer, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &layer, &Layer::SetClipping, enabled );
}

/**
 * Create a message to set the clipping box of a layer
 * @param[in] layer The layer
 * @param[in] clippingbox The clipping box
 */
inline void SetClippingBoxMessage( EventToUpdate& eventToUpdate, const Layer& layer, const Dali::ClippingBox& clippingbox )
{
  typedef MessageValue1< Layer, Dali::ClippingBox > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &layer, &Layer::SetClippingBox, clippingbox );
}

/**
 * Create a message for disabling/enabling depth test.
 *
 * @see Dali::Layer::SetDepthTestDisabled().
 *
 * @param[in] layer The layer
 * @param[in] disable \e true disables depth test. \e false sets the default behaviour.
 */
inline void SetDepthTestDisabledMessage( EventToUpdate& eventToUpdate, const Layer& layer, bool disable )
{
  typedef MessageValue1< Layer, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &layer, &Layer::SetDepthTestDisabled, disable );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_LAYER_H__
