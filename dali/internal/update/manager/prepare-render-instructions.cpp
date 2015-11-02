/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/manager/prepare-render-instructions.h>

// INTERNAL INCLUDES
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/public-api/actors/layer.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/layer-impl.h> // for the default sorting function
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/rendering/scene-graph-material.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gRenderListLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RENDER_LISTS");
#endif

struct Batch
{
  Batch():mRenderer(0),mGeometry(0){}
  Batch(Dali::Internal::Render::Renderer* renderer, Dali::Internal::SceneGraph::RenderGeometry* geometry ):mRenderer(renderer),mGeometry(geometry){}
  ~Batch(){}
  Dali::Internal::Render::Renderer* mRenderer;
  Dali::Internal::SceneGraph::RenderGeometry* mGeometry;
};

std::vector<Batch> mBatches;
}

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * Add a renderer to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderList to add the item to
 * @param renderable attachment
 * @param viewMatrix used to calculate modelview matrix for the item
 * @param cameraAttachment The camera used to render
 * @param isLayer3d Whether we are processing a 3D layer or not
 */
inline void AddRendererToRenderList( BufferIndex updateBufferIndex,
                                     RenderList& renderList,
                                     RenderableAttachment& renderable,
                                     const Matrix& viewMatrix,
                                     SceneGraph::CameraAttachment& cameraAttachment,
                                     bool isLayer3d )
{
  const Node& parentNode = renderable.GetParent();
  const Matrix& worldMatrix = parentNode.GetWorldMatrix( updateBufferIndex );

  // Get the next free RenderItem and initialization
  RenderItem& item = renderList.GetNextFreeItem();
  const Render::Renderer& renderer = renderable.GetRenderer();
  item.SetRenderer( const_cast< Render::Renderer* >( &renderer ) );
  item.SetNode( const_cast< Node* >( &parentNode ) );

  item.SetIsOpaque( renderable.IsFullyOpaque(updateBufferIndex) );
  if( isLayer3d )
  {
    item.SetDepthIndex( renderable.GetDepthIndex() );
  }
  else
  {
    item.SetDepthIndex( renderable.GetDepthIndex() + static_cast<int>( parentNode.GetDepth() ) * Dali::Layer::TREE_DEPTH_MULTIPLIER );
  }

  // save MV matrix onto the item
  Matrix::Multiply( item.GetModelViewMatrix(), worldMatrix, viewMatrix );
}

/**
 * Add a renderer to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderList to add the item to
 * @param NodeRenderer NodeRenderer pair
 * @param viewMatrix used to calculate modelview matrix for the item
 * @param cameraAttachment The camera used to render
 * @param isLayer3d Whether we are processing a 3D layer or not
 */
inline void AddRendererToRenderList( BufferIndex updateBufferIndex,
                                     RenderList& renderList,
                                     NodeRenderer& renderable,
                                     const Matrix& viewMatrix,
                                     SceneGraph::CameraAttachment& cameraAttachment,
                                     bool isLayer3d )
{
  // Check for cull against view frustum
  const Matrix& worldMatrix = renderable.mNode->GetWorldMatrix( updateBufferIndex );
  bool inside = true;

  if ( renderable.mRenderer->GetMaterial().GetShader()->GeometryHintEnabled( Dali::ShaderEffect::HINT_DOESNT_MODIFY_GEOMETRY ) )
  {
    const Vector3& position = worldMatrix.GetTranslation3();
    const Vector3& scale = renderable.mNode->GetScale( updateBufferIndex );
    const Vector3& halfSize = renderable.mNode->GetSize( updateBufferIndex ) * scale * 0.5f;

    // Do a fast sphere check
    if ( cameraAttachment.CheckSphereInFrustum( updateBufferIndex, position, halfSize.Length() ) )
    {
      // Check geometry AABB
      //TODO: Take into account orientation
      if ( !cameraAttachment.CheckAABBInFrustum( updateBufferIndex, position, halfSize ) )
      {
        inside = false;
      }
    }
    else
    {
      inside = false;
    }
  }

  if ( inside )
  {
    // Get the next free RenderItem
    RenderItem& item = renderList.GetNextFreeItem();
    item.SetRenderer( &renderable.mRenderer->GetRenderer() );
    item.SetSceneRenderer( renderable.mRenderer );
    item.SetNode( renderable.mNode );
    item.SetIsOpaque( renderable.mRenderer->IsFullyOpaque(updateBufferIndex, *renderable.mNode ) );

    if( isLayer3d )
    {
      item.SetDepthIndex( renderable.mRenderer->GetDepthIndex() );
    }
    else
    {
      item.SetDepthIndex( renderable.mRenderer->GetDepthIndex() + static_cast<int>( renderable.mNode->GetDepth() ) * Dali::Layer::TREE_DEPTH_MULTIPLIER );
    }

    // save MV matrix onto the item
    Matrix::Multiply( item.GetModelViewMatrix(), worldMatrix, viewMatrix );
  }
}

/**
 * Add all renderers to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderList to add the items to
 * @param renderable attachments
 * NodeRendererContainer Node-Renderer pairs
 * @param viewMatrix used to calculate modelview matrix for the items
 * @param cameraAttachment The camera used to render
 * @param isLayer3d Whether we are processing a 3D layer or not
 */
inline void AddRenderersToRenderList( BufferIndex updateBufferIndex,
                                      RenderList& renderList,
                                      RenderableAttachmentContainer& attachments,
                                      NodeRendererContainer& renderers,
                                      const Matrix& viewMatrix,
                                      SceneGraph::CameraAttachment& cameraAttachment,
                                      bool isLayer3d )
{
  DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "AddRenderersToRenderList()\n");

  // Add renderer for each attachment
  unsigned int index(0);
  const RenderableAttachmentIter endIter = attachments.end();
  for ( RenderableAttachmentIter iter = attachments.begin(); iter != endIter; ++iter )
  {
    RenderableAttachment& attachment = **iter;
    AddRendererToRenderList( updateBufferIndex, renderList, attachment, viewMatrix, cameraAttachment, isLayer3d );

    DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "  List[%d].renderer = %p\n", index, &(attachment.GetRenderer()));
    ++index;
  }

  unsigned int rendererCount( renderers.Size() );
  for( unsigned int i(0); i<rendererCount; ++i )
  {
    AddRendererToRenderList( updateBufferIndex, renderList, renderers[i], viewMatrix, cameraAttachment, isLayer3d );
  }
}

/**
 * Try to reuse cached renderitems from the renderlist
 * This avoids recalculating the model view matrices in case this part of the scene was static
 * An example case is a toolbar layer that rarely changes or a popup on top of the rest of the stage
 * @param layer that is being processed
 * @param renderList that is cached from frame N-1
 * @param attachmentList that is being used
 */
inline bool TryReuseCachedRenderers( Layer& layer,
                                     RenderList& renderList,
                                     RenderableAttachmentContainer& attachmentList )
{
  bool retValue = false;
  size_t renderableCount = attachmentList.size();
  // check that the cached list originates from this layer and that the counts match
  if( ( renderList.GetSourceLayer() == &layer )&&
      ( renderList.GetCachedItemCount() == renderableCount ) )
  {
    // check that all the same renderers are there. This gives us additional security in avoiding rendering the wrong attachments
    // Attachments are not sorted, but render list is so at this stage renderers may be in different order
    // therefore we check a combined sum of all renderer addresses
    size_t checkSumNew = 0;
    size_t checkSumOld = 0;
    for( size_t index = 0; index < renderableCount; ++index )
    {
      RenderableAttachment* attachment = attachmentList[ index ];
      const Render::Renderer& renderer = attachment->GetRenderer();
      checkSumNew += size_t( &renderer );
      checkSumOld += size_t( &renderList.GetRenderer( index ) );
    }
    if( checkSumNew == checkSumOld )
    {
      // tell list to reuse its existing items
      renderList.ReuseCachedItems();
      retValue = true;
    }
  }
  return retValue;
}


/**
 * Function which sorts render items by depth index then by instance
 * ptrs of shader/geometry/material.
 * @param lhs item
 * @param rhs item
 * @return true if left item is greater than right
 */
bool CompareItems( const RendererWithSortAttributes& lhs, const RendererWithSortAttributes& rhs )
{
  // @todo MESH_REWORK Consider replacing all these sortAttributes with a single long int that
  // encapsulates the same data (e.g. the middle-order bits of the ptrs)
  if( lhs.renderItem->GetDepthIndex() == rhs.renderItem->GetDepthIndex() )
  {
    if( lhs.shader == rhs.shader )
    {
      if( lhs.textureResourceId == rhs.textureResourceId )
      {
        return lhs.geometry < rhs.geometry;
      }
      return lhs.textureResourceId < rhs.textureResourceId;
    }
    return lhs.shader < rhs.shader;;
  }
  return lhs.renderItem->GetDepthIndex() < rhs.renderItem->GetDepthIndex();
}
/**
 * Function which sorts the render items by Z function, then
 * by instance ptrs of shader/geometry/material.
 * @param lhs item
 * @param rhs item
 * @return true if left item is greater than right
 */
bool CompareItems3D( const RendererWithSortAttributes& lhs, const RendererWithSortAttributes& rhs )
{
  bool lhsIsOpaque = lhs.renderItem->IsOpaque();
  if( lhsIsOpaque ==  rhs.renderItem->IsOpaque())
  {
    if( lhsIsOpaque )
    {
      //If both RenderItems are opaque, sort using shader, then material then geometry
      if( lhs.shader == rhs.shader )
      {
        if( lhs.textureResourceId == rhs.textureResourceId )
        {
          return lhs.geometry < rhs.geometry;
        }
        return lhs.textureResourceId < rhs.textureResourceId;
      }
      return lhs.shader < rhs.shader;
    }
    else
    {
      //If both RenderItems are transparent, sort using z,then shader, then material, then geometry
      if( Equals(lhs.zValue, rhs.zValue) )
      {
        if( lhs.shader == rhs.shader )
        {
          if( lhs.textureResourceId == rhs.textureResourceId )
          {
            return lhs.geometry < rhs.geometry;
          }
          return lhs.textureResourceId < rhs.textureResourceId;
        }
        return lhs.shader < rhs.shader;
      }
      return lhs.zValue > rhs.zValue;
    }
  }
  else
  {
    return lhsIsOpaque;
  }
}

/**
 * Sort color render items
 * @param colorRenderList to sort
 * @param layer where the renderers are from
 * @param sortingHelper to use for sorting the renderitems (to avoid reallocating)
 */
inline void SortColorRenderItems( BufferIndex bufferIndex, RenderList& renderList, Layer& layer, RendererSortingHelper& sortingHelper )
{
  const size_t renderableCount = renderList.Count();
  // reserve space if needed
  const unsigned int oldcapacity = sortingHelper.size();
  if( oldcapacity < renderableCount )
  {
    sortingHelper.reserve( renderableCount );
    // add real objects (reserve does not construct objects)
    sortingHelper.insert( sortingHelper.begin() + oldcapacity,
                          (renderableCount - oldcapacity),
                          RendererWithSortAttributes() );
  }
  else
  {
    // clear extra elements from helper, does not decrease capability
    sortingHelper.resize( renderableCount );
  }

  // calculate the sorting value, once per item by calling the layers sort function
  // Using an if and two for-loops rather than if inside for as its better for branch prediction
  if( layer.UsesDefaultSortFunction() )
  {
    for( size_t index = 0; index < renderableCount; ++index )
    {
      RenderItem& item = renderList.GetItem( index );

      item.GetRenderer().SetSortAttributes( bufferIndex, sortingHelper[ index ] );

      // the default sorting function should get inlined here
      sortingHelper[ index ].zValue = Internal::Layer::ZValue( item.GetModelViewMatrix().GetTranslation3() ) - item.GetDepthIndex();

      // keep the renderitem pointer in the helper so we can quickly reorder items after sort
      sortingHelper[ index ].renderItem = &item;
    }
  }
  else
  {
    const Dali::Layer::SortFunctionType sortFunction = layer.GetSortFunction();
    for( size_t index = 0; index < renderableCount; ++index )
    {
      RenderItem& item = renderList.GetItem( index );

      item.GetRenderer().SetSortAttributes( bufferIndex, sortingHelper[ index ] );
      sortingHelper[ index ].zValue = (*sortFunction)( item.GetModelViewMatrix().GetTranslation3() ) - item.GetDepthIndex();

      // keep the renderitem pointer in the helper so we can quickly reorder items after sort
      sortingHelper[ index ].renderItem = &item;
    }
  }

  if( layer.GetBehavior() ==  Dali::Layer::LAYER_3D)
  {
    // sort the renderers back to front, Z Axis point from near plane to far plane
    std::stable_sort( sortingHelper.begin(), sortingHelper.end(), CompareItems3D );
  }
  else
  {
    // sort the renderers based on DepthIndex
    std::stable_sort( sortingHelper.begin(), sortingHelper.end(), CompareItems );
  }

  // reorder/repopulate the renderitems in renderlist to correct order based on sortinghelper
  DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "Sorted Transparent List:\n");
  RenderItemContainer::Iterator renderListIter = renderList.GetContainer().Begin();
  for( unsigned int index = 0; index < renderableCount; ++index, ++renderListIter )
  {
    *renderListIter = sortingHelper[ index ].renderItem;
    DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "  sortedList[%d] = %p\n", index, &sortingHelper[ index ].renderItem->GetRenderer() );
  }


  //Automatic Batching

//  const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
//    attribute mediump vec2 aPosition;\n
//    uniform mediump mat4 uMvpMatrix;\n
//    uniform mediump vec3 uSize;\n
//    \n
//    void main()\n
//    {\n
//      mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
//      vertexPosition.xyz *= uSize;\n
//      gl_Position = uMvpMatrix * vertexPosition;\n
//    }\n
//  );
//
//  const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
//    uniform lowp vec4 uColor;\n
//    uniform lowp vec4 uBlendColor;\n
//    \n
//    void main()\n
//    {\n
//      gl_FragColor = uBlendColor*uColor;\n
//    }\n
//  );

 //Shader colorShader;

 //Renderer newRenderer
}

/**
 * Add color renderers from the layer onto the next free render list
 * @param updateBufferIndex to use
 * @param layer to get the renderers from
 * @param viewmatrix for the camera from rendertask
 * @param cameraAttachment to use the view frustum
 * @param stencilRenderablesExist is true if there are stencil renderers on this layer
 * @param instruction to fill in
 * @param sortingHelper to use for sorting the renderitems (to avoid reallocating)
 * @param tryReuseRenderList whether to try to reuse the cached items from the instruction
 */
inline void AddColorRenderers( BufferIndex updateBufferIndex,
                               Layer& layer,
                               const Matrix& viewMatrix,
                               SceneGraph::CameraAttachment& cameraAttachment,
                               bool stencilRenderablesExist,
                               RenderInstruction& instruction,
                               RendererSortingHelper& sortingHelper,
                               bool tryReuseRenderList,
                               UpdateManager& updateManager )
{
  RenderList& renderList = instruction.GetNextFreeRenderList( layer.colorRenderables.size() );
  renderList.SetClipping( layer.IsClipping(), layer.GetClippingBox() );
  renderList.SetHasColorRenderItems( true );

  // try to reuse cached renderitems from last time around
  if( tryReuseRenderList )
  {
    if( TryReuseCachedRenderers( layer, renderList, layer.colorRenderables ) )
    {
      return;
    }
  }

  AddRenderersToRenderList( updateBufferIndex, renderList, layer.colorRenderables, layer.colorRenderers, viewMatrix, cameraAttachment, layer.GetBehavior() == Dali::Layer::LAYER_3D );
  SortColorRenderItems( updateBufferIndex, renderList, layer, sortingHelper );

  //Batching
  std::vector<size_t> ranges;
  bool batching = false;
  unsigned int renderableCount( renderList.Count() );
  for( unsigned int index = 0; index < renderableCount; ++index )
  {
    RenderItem& item = renderList.GetItem(index);
    Render::NewRenderer* renderer = item.GetRenderer().GetNewRenderer();
    if( renderer )
    {
      if( renderer->mRenderDataProvider->GetBatchMaterial() )
      {
        if( !batching )
        {
          ranges.push_back(index);
          batching = true;
        }
      }
      else if( batching )
      {
        ranges.push_back(index-1);
        batching = false;
      }
    }
  }
  if(batching )
  {
    ranges.push_back(renderList.Count()-1);
  }

  for( size_t i(0); i<mBatches.size(); i++)
  {
    updateManager.GetSceneController().GetRenderMessageDispatcher().RemoveRenderer( *mBatches[i].mRenderer );
    updateManager.GetSceneController().GetRenderMessageDispatcher().RemoveGeometry( *mBatches[i].mGeometry );
  }
  //clear previous batched renderers
  mBatches.clear();



  //BufferIndex previousBufferIndex = ( updateManager.GetEventBufferIndex() + 1 ) % 2;
  for( size_t i(0); i<ranges.size(); i+=2)
  {
    if( ranges[i+1]-ranges[i] > 0 )
    {
      std::cout<<"Batching renderers "<<ranges[i] <<", "<< ranges[i+1]<<std::endl;
      RenderItem& item = renderList.GetItem(ranges[i]);

      //Create a new geometry
      RenderGeometry* batchGeometry = new RenderGeometry( item.GetSceneRenderer().GetGeometry().GetGeometryType(updateBufferIndex), false );

      //Create the property buffer
      Render::PropertyBuffer::Format* batchVertexFormat = new Render::PropertyBuffer::Format();;
//      batchVertexFormat->components.resize(2);
//      batchVertexFormat->components[0].name = "aPosition";
//      batchVertexFormat->components[0].offset = 0u;
//      batchVertexFormat->components[0].size = sizeof(Vector2);
//      batchVertexFormat->components[0].type = Property::VECTOR2;
//      batchVertexFormat->components[1].name = "uBlendColor";
//      batchVertexFormat->components[1].offset = sizeof(Vector2);
//      batchVertexFormat->components[1].size = sizeof(Vector4);
//      batchVertexFormat->components[1].type = Property::VECTOR4;
//      batchVertexFormat->size = sizeof(Vector2)+sizeof(Vector4);

      unsigned int vertexSize( sizeof(Vector2));
      unsigned int offset(0u);
      Render::PropertyBuffer::Component positionComponent = {"aPosition",offset,sizeof(Vector2),Property::VECTOR2};
      batchVertexFormat->components.push_back(positionComponent);
      offset += sizeof(Vector2);

      //TODO: Ferran Read the uniform maps and add them as attributes
      const CollectedUniformMap& uniformMap = item.GetSceneRenderer().GetUniformMap(updateBufferIndex);
      for( unsigned int k(0); k<uniformMap.Size(); ++k )
      {
        std::cout<<"Uniform:"<<uniformMap[k]->uniformName.c_str();
        const PropertyInputImpl* property = uniformMap[k]->propertyPtr;
        if( property->GetType() == Property::VECTOR4 )
        {
          std::cout<<property->GetVector4(0);
          Render::PropertyBuffer::Component component = {uniformMap[k]->uniformName.c_str(),offset,sizeof(Vector4),Property::VECTOR4};
          batchVertexFormat->components.push_back(component);
          offset += sizeof(Vector4);
          vertexSize += sizeof(Vector4);
        }
        std::cout<<std::endl;
      }

      batchVertexFormat->size = vertexSize;

      unsigned int vertexCount = 4 * (ranges[i+1]-ranges[i]+1);
      unsigned int bufferSize = vertexCount * vertexSize;
      char* data = 0;//new char[bufferSize];

      //Iterate through items, transform its vertex position and add the rest of attributes
      size_t count(0);
      for( unsigned itemIndex = ranges[i]; itemIndex<=ranges[i+1]; ++itemIndex )
      {
        RenderItem& item = renderList.GetItem(itemIndex);
        Render::PropertyBuffer* vertexBuffer = item.GetSceneRenderer().GetGeometry().GetVertexBuffers()[0];
        const Dali::Vector<char>* vertexData = vertexBuffer->GetData();
        if(vertexData && !vertexData->Empty() )
        {
          if( !data )
          {
            data = new char[bufferSize];
          }
          for( unsigned int m(0); m<vertexBuffer->GetElementCount(); ++m )
          {
            const Vector2* positionOld = (const Vector2*)( &vertexData->operator[]( m*sizeof(Vector2) ) );
            const Matrix& worldTransform( item.GetNode().GetWorldMatrix( updateBufferIndex ) );
            Vector3 size = item.GetNode().GetSize(updateBufferIndex);
            Vector4 positionTransformed = ( worldTransform * Vector4(positionOld->x*size.x, positionOld->y*size.y, 0.0f, 1.0f));

            //Copy position to new vertex data
            Vector2* position = (Vector2*)(data + count);
            *position = Vector2(positionTransformed.x,positionTransformed.y);
            count += sizeof(Vector2);

            //@TODO: Copy the rest of the attributes

            //Uniforms
            const CollectedUniformMap& uniformMap = item.GetSceneRenderer().GetUniformMap(updateBufferIndex);
            for( unsigned int k(0); k<uniformMap.Size(); ++k )
            {
              const PropertyInputImpl* property = uniformMap[k]->propertyPtr;
              if( property->GetType() == Property::VECTOR4 )
              {
                Vector4* att = (Vector4*)(data+count);
                *att = property->GetVector4(updateBufferIndex);
                count += sizeof(Vector4);
              }
            }
          }
        }
        else
        {
          break;
        }
      }

      if( data )
      {

        Vector<char>* mBuffer = new Vector<char>();
        mBuffer->Resize(bufferSize);
        //std::copy( data, data + mBuffer->Size(), (char*)mBuffer );
        for( unsigned index = 0; index < mBuffer->Size(); index++ )
        {
          mBuffer->operator[](index) = data[index];
        }
        delete[] data;

        //Create the property buffer
        Render::PropertyBuffer* propertyBuffer = new Render::PropertyBuffer();
        propertyBuffer->SetSize( vertexCount );
        propertyBuffer->SetFormat( batchVertexFormat );
        propertyBuffer->SetData(mBuffer);
        batchGeometry->AddPropertyBuffer( propertyBuffer, false );
      }
      else
      {
        break;
      }

      //Create the new renderer
      SceneGraph::Renderer& renderer = item.GetSceneRenderer();
      RenderDataProvider* dataProvider = new RenderDataProvider();
      dataProvider->mMaterialDataProvider = &renderer.GetMaterial();
      dataProvider->mUniformMapDataProvider = &renderer;
      dataProvider->mShader = renderer.GetBatchMaterial().GetShader();

      //TODO:Ferran Textures
//      size_t textureCount( renderer.GetMaterial().GetTextureCount() );
//      dataProvider->mTextures.resize( textureCount );
//      for( unsigned int j(0); j<textureCount; ++j )
//      {
//        dataProvider->mTextures[j] = Render::Texture( renderer.GetMaterial().GetTextureUniformName(i),
//                                                      renderer.GetMaterial().GetTextureId(i),
//                                                      renderer.GetMaterial().GetTextureSampler(i));
//      }


      Render::Renderer* batchRenderer = Render::NewRenderer::New( dataProvider, batchGeometry );
      updateManager.GetSceneController().GetRenderMessageDispatcher().AddRenderer( *batchRenderer );
      updateManager.GetSceneController().GetRenderMessageDispatcher().AddGeometry( *batchGeometry );
      mBatches.push_back( Batch(batchRenderer,batchGeometry) );
      Matrix& modelView = item.GetModelViewMatrix();
      //std::cout<<"ModelView: "<<modelView<<std::endl;
      modelView = viewMatrix;
      //std::cout<<"View: "<<modelView<<std::endl;
      item.SetRenderer( batchRenderer );

      //Delete all but the first items from the list
      for( unsigned j = ranges[i]+1; j<=ranges[i+1]; ++j )
      {
        renderList.Erase(j);
      }
    }
  }

  //Set render flags
  unsigned int flags = 0u;
  if( stencilRenderablesExist )
  {
    flags = RenderList::STENCIL_BUFFER_ENABLED;
  }

  // Special optimization if depth test is disabled or if only one opaque rendered in the layer (for example background image)
  // and this renderer does not need depth test against itself (e.g. mesh)
  // and if this layer has got exactly one opaque renderer
  // and this renderer is not interested in depth testing
  // (i.e. is an image and not a mesh)

  if ( ( renderList.Count() == 1 ) &&
       ( !renderList.GetRenderer( 0 ).RequiresDepthTest() ) &&
       ( !renderList.GetItem(0).IsOpaque() ) )
  {
    //Nothing to do here
  }
  else if( !layer.IsDepthTestDisabled())
  {
    flags |= RenderList::DEPTH_BUFFER_ENABLED;
    flags |= RenderList::DEPTH_CLEAR;
  }

  renderList.ClearFlags();
  renderList.SetFlags( flags );
}

/**
 * Add overlay renderers from the layer onto the next free render list
 * @param updateBufferIndex to use
 * @param layer to get the renderers from
 * @param viewmatrix for the camera from rendertask
 * @param stencilRenderablesExist is true if there are stencil renderers on this layer
 * @param instruction to fill in
 * @param tryReuseRenderList whether to try to reuse the cached items from the instruction
 */
inline void AddOverlayRenderers( BufferIndex updateBufferIndex,
                                 Layer& layer,
                                 const Matrix& viewMatrix,
                                 SceneGraph::CameraAttachment& cameraAttachment,
                                 bool stencilRenderablesExist,
                                 RenderInstruction& instruction,
                                 bool tryReuseRenderList )
{
  RenderList& overlayRenderList = instruction.GetNextFreeRenderList( layer.overlayRenderables.size() );
  overlayRenderList.SetClipping( layer.IsClipping(), layer.GetClippingBox() );
  overlayRenderList.SetHasColorRenderItems( false );

  //Set render flags
  overlayRenderList.ClearFlags();
  if(stencilRenderablesExist)
  {
    overlayRenderList.SetFlags(RenderList::STENCIL_BUFFER_ENABLED);
  }

  // try to reuse cached renderitems from last time around
  if( tryReuseRenderList )
  {
    if( TryReuseCachedRenderers( layer, overlayRenderList, layer.overlayRenderables ) )
    {
      return;
    }
  }
  AddRenderersToRenderList( updateBufferIndex, overlayRenderList, layer.overlayRenderables, layer.overlayRenderers, viewMatrix, cameraAttachment, layer.GetBehavior() == Dali::Layer::LAYER_3D );
}

/**
 * Add stencil renderers from the layer onto the next free render list
 * @param updateBufferIndex to use
 * @param layer to get the renderers from
 * @param viewmatrix for the camera from rendertask
 * @param instruction to fill in
 * @param tryReuseRenderList whether to try to reuse the cached items from the instruction
 */
inline void AddStencilRenderers( BufferIndex updateBufferIndex,
                                 Layer& layer,
                                 const Matrix& viewMatrix,
                                 SceneGraph::CameraAttachment& cameraAttachment,
                                 RenderInstruction& instruction,
                                 bool tryReuseRenderList )
{
  RenderList& stencilRenderList = instruction.GetNextFreeRenderList( layer.stencilRenderables.size() );
  stencilRenderList.SetClipping( layer.IsClipping(), layer.GetClippingBox() );
  stencilRenderList.SetHasColorRenderItems( false );

  //Set render flags
  stencilRenderList.ClearFlags();
  stencilRenderList.SetFlags(RenderList::STENCIL_CLEAR | RenderList::STENCIL_WRITE | RenderList::STENCIL_BUFFER_ENABLED );

  // try to reuse cached renderitems from last time around
  if( tryReuseRenderList )
  {
    if( TryReuseCachedRenderers( layer, stencilRenderList, layer.stencilRenderables ) )
    {
      return;
    }
  }
  AddRenderersToRenderList( updateBufferIndex, stencilRenderList, layer.stencilRenderables, layer.stencilRenderers, viewMatrix, cameraAttachment, layer.GetBehavior() == Dali::Layer::LAYER_3D );
}

/**
 * Prepare a single render instruction
 * @param updateBufferIndex to use
 * @param sortedLayers to prepare the instruction from
 * @param renderTask to get the view matrix
 * @param sortingHelper to use for sorting the renderitems (to avoid reallocating)
 * @param renderTracker An optional render tracker object
 * @param instructions container
 */
void PrepareRenderInstruction( BufferIndex updateBufferIndex,
                               SortedLayerPointers& sortedLayers,
                               RenderTask& renderTask,
                               RendererSortingHelper& sortingHelper,
                               RenderTracker* renderTracker,
                               RenderInstructionContainer& instructions,
                               UpdateManager& updateManager )
{
  // Retrieve the RenderInstruction buffer from the RenderInstructionContainer
  // then populate with instructions.
  RenderInstruction& instruction = instructions.GetNextInstruction( updateBufferIndex );
  renderTask.PrepareRenderInstruction( instruction, updateBufferIndex );
  bool viewMatrixHasNotChanged = !renderTask.ViewMatrixUpdated();

  const Matrix& viewMatrix = renderTask.GetViewMatrix( updateBufferIndex );
  SceneGraph::CameraAttachment& cameraAttachment = renderTask.GetCameraAttachment();

  const SortedLayersIter endIter = sortedLayers.end();
  for ( SortedLayersIter iter = sortedLayers.begin(); iter != endIter; ++iter )
  {
    Layer& layer = **iter;

    const bool stencilRenderablesExist( !layer.stencilRenderables.empty() || !layer.stencilRenderers.Empty() );
    const bool colorRenderablesExist( !layer.colorRenderables.empty() || !layer.colorRenderers.Empty() );
    const bool overlayRenderablesExist( !layer.overlayRenderables.empty() || !layer.overlayRenderers.Empty() );
    const bool tryReuseRenderList( viewMatrixHasNotChanged && layer.CanReuseRenderers(renderTask.GetCamera()) );

    // Ignore stencils if there's nothing to test
    if( stencilRenderablesExist &&
        ( colorRenderablesExist || overlayRenderablesExist ) )
    {
      AddStencilRenderers( updateBufferIndex, layer, viewMatrix, cameraAttachment, instruction, tryReuseRenderList );
    }

    if ( colorRenderablesExist )
    {
      AddColorRenderers( updateBufferIndex,
                         layer,
                         viewMatrix,
                         cameraAttachment,
                         stencilRenderablesExist,
                         instruction,
                         sortingHelper,
                         tryReuseRenderList,
                         updateManager);
    }

    if ( overlayRenderablesExist )
    {
      AddOverlayRenderers( updateBufferIndex, layer, viewMatrix, cameraAttachment, stencilRenderablesExist,
                           instruction, tryReuseRenderList );
    }
  }

  instruction.mRenderTracker = renderTracker;
  instruction.mCullMode = renderTask.GetCullMode();

  // inform the render instruction that all renderers have been added and this frame is complete
  instruction.UpdateCompleted();
}

} // SceneGraph

} // Internal

} // Dali
