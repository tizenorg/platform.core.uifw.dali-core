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
  Dali::Internal::Render::Renderer* mRenderer;
  Dali::Internal::SceneGraph::RenderGeometry* mGeometry;
  Dali::Internal::Render::PropertyBuffer* mVertexBuffer;
  Dali::Internal::Render::PropertyBuffer* mIndexBuffer;
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
    item.SetSceneGraphRenderer( renderable.mRenderer );
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

}


void BatchRenderers( BufferIndex updateBufferIndex, size_t first, size_t last, RenderList& renderList, UpdateManager& updateManager, const Matrix& viewMatrix )
{
  RenderItem& firstItem = renderList.GetItem(first);

  Geometry::GeometryType geometryType( firstItem.GetSceneGraphRenderer().GetGeometry().GetGeometryType(updateBufferIndex) );
  if( geometryType == Dali::Geometry::TRIANGLE_STRIP || geometryType == Dali::Geometry::TRIANGLE_FAN  ||
      geometryType == Dali::Geometry::LINE_LOOP || geometryType == Dali::Geometry::LINE_STRIP )
  {
    return; //Cannot batch strips, fans or loops
  }



  //Property buffer format
  Render::PropertyBuffer::Format* batchVertexFormat = new Render::PropertyBuffer::Format();
  unsigned int vertexSize( 2 * sizeof(Vector2));
  unsigned int offset(0u);
  Render::PropertyBuffer::Component positionComponent = {"aPosition",offset,sizeof(Vector2),Property::VECTOR2};
  batchVertexFormat->components.push_back(positionComponent);
  offset += sizeof(Vector2);
  Render::PropertyBuffer::Component texCoordComponent = {"aTexCoord",offset,sizeof(Vector2),Property::VECTOR2};
  batchVertexFormat->components.push_back(texCoordComponent);
  offset += sizeof(Vector2);

  const CollectedUniformMap& uniformMap = firstItem.GetSceneGraphRenderer().GetUniformMap(updateBufferIndex);
  for( unsigned int uniformIndex(0); uniformIndex<uniformMap.Size(); ++uniformIndex )
  {
    const PropertyInputImpl* property = uniformMap[uniformIndex]->propertyPtr;
    //TODO: Implement the rest of property types
    if( property->GetType() == Property::VECTOR4 )
    {
      Render::PropertyBuffer::Component component = {uniformMap[uniformIndex]->uniformName.c_str(),offset,sizeof(Vector4),Property::VECTOR4};
      batchVertexFormat->components.push_back(component);
      offset += sizeof(Vector4);
      vertexSize += sizeof(Vector4);
    }
  }
  batchVertexFormat->size = vertexSize;

  //TODO:Properly calculate vertexCount and indexCount
  unsigned int batchedRendererCount = (last - first + 1);
  unsigned int vertexCount = 4 * batchedRendererCount;
  unsigned int indexCount = 6 * batchedRendererCount;
  unsigned int batchvertexBufferSize = vertexCount * vertexSize;
  Dali::Vector<char>* batchVertexBuffer = 0;
  Dali::Vector<char>* batchIndexBuffer = 0;


  float quadVertexData[] = {-0.5f, -0.5, 0.0f, 0.0f, 0.5f, -0.5f, 1.0f, 0.0f, -0.5f, 0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 1.0f, 1.0f};
  unsigned int quadIndices[] = {0,1,2,2,1,3};


  //Iterate through items, transform its vertex position and add the rest of attributes
  size_t count(0);
  size_t currentItem(0);
  for( unsigned itemIndex = first; itemIndex<=last; ++itemIndex )
  {
    RenderItem& item = renderList.GetItem(itemIndex);

    //To support other meshes than a simple quad with texture, we need to be able to read data from the vertex buffers (which is in a different thread)
    //Render::PropertyBuffer* vertexBuffer = item.GetSceneRenderer().GetGeometry().GetVertexBuffers()[0];
    //const Dali::Vector<char>* vertexData = vertexBuffer->GetData();
    //if(vertexData && !vertexData->Empty() )
    {
      if( !batchVertexBuffer )
      {
        batchVertexBuffer = new Dali::Vector<char>();
        batchVertexBuffer->Resize( batchvertexBufferSize);
        batchIndexBuffer = new Dali::Vector<char>();
        batchIndexBuffer->Resize( indexCount * sizeof(unsigned int) );
      }

      //for( unsigned int m(0); m<vertexBuffer->GetElementCount(); ++m )
      for( unsigned int m(0); m<16; m+=4 )
      {
        //const Vector2* positionOld = reinterpret_cast<const Vector2*>( &(*vertexData)[ m*sizeof(Vector2)] );
        //Vector4 positionTransformed = ( item.GetNode().GetWorldMatrix( updateBufferIndex ) * Vector4(positionOld->x*size.x, positionOld->y*size.y, 0.0f, 1.0f));
        const Vector3& size = item.GetNode().GetSize(updateBufferIndex);
        Vector4 positionTransformed = ( item.GetNode().GetWorldMatrix( updateBufferIndex ) * Vector4(quadVertexData[m]*size.x, quadVertexData[m+1]*size.y, 0.0f, 1.0f));

        //Copy position to new vertex buffer
        Vector2* position = reinterpret_cast<Vector2*>( &(*batchVertexBuffer)[count] );
        *position = Vector2(positionTransformed.x,positionTransformed.y);
        count += sizeof(Vector2);

        //Copy texture coordinates to new vertex buffer ( TODO: Only add texture coordinates if there is a texture )
        Vector2* texcoord = reinterpret_cast<Vector2*>( &(*batchVertexBuffer)[count] );
        *texcoord = Vector2(quadVertexData[m+2],quadVertexData[m+3]);
        count += sizeof(Vector2);

        //Uniforms.
        //TODO: Add node uniforms (It needs to preserve uniform precedence)
        const CollectedUniformMap& uniformMap = item.GetSceneGraphRenderer().GetUniformMap(updateBufferIndex);
        for( unsigned int uniformIndex(0); uniformIndex<uniformMap.Size(); ++uniformIndex )
        {
          const PropertyInputImpl* property = uniformMap[uniformIndex]->propertyPtr;
          if( property->GetType() == Property::VECTOR4 )
          {
            Vector4* attribute = reinterpret_cast<Vector4*>( &(*batchVertexBuffer)[count] );
            *attribute = property->GetVector4(updateBufferIndex);
            count += sizeof(Vector4);
          }
        }
      }

      //Index buffer
      //Render::PropertyBuffer* indexBuffer = item.GetSceneRenderer().GetGeometry().GetIndexBuffer();
      //const Dali::Vector<char>* indexData = indexBuffer->GetData();
      //for( unsigned int m(0); m<indexBuffer->GetElementCount(); ++m )
      for( unsigned int m(0); m<6; ++m )
      {
        //const unsigned int* indexOld =  reinterpret_cast<const unsigned int*>( &(*indexData)[m*sizeof(unsigned int)] );
        unsigned int* index =  reinterpret_cast<unsigned int*>( &(*batchIndexBuffer)[ ((currentItem*6) + m) * sizeof(unsigned int) ] );
        //*index = *indexOld + (4*currentItem);
        *index = quadIndices[m] + (4*currentItem);
      }

      ++currentItem;
    }
//    else
//    {
//      //No data in some of the buffers yet
//      if( batchVertexBuffer )
//      {
//        delete batchVertexBuffer;
//        batchVertexBuffer = 0;
//      }
//      if( batchIndexBuffer )
//      {
//        delete batchIndexBuffer;
//        batchIndexBuffer = 0;
//      }
//
//      break;
//    }
  }

  if( batchVertexBuffer )
  {
    //Create a new RenderGeometry
    RenderGeometry* batchGeometry = new RenderGeometry( geometryType, false );

    //Create vertex buffer
    Render::PropertyBuffer* vertexBuffer = new Render::PropertyBuffer();
    vertexBuffer->SetSize( vertexCount );
    vertexBuffer->SetFormat( batchVertexFormat );
    vertexBuffer->SetData(batchVertexBuffer);
    batchGeometry->AddPropertyBuffer( vertexBuffer, false );

    //Create index buffer
    Render::PropertyBuffer::Format* batchIndexFormat = new Render::PropertyBuffer::Format();
    Render::PropertyBuffer::Component indexComponent = {"uIndex",0,sizeof(unsigned int),Property::INTEGER};
    batchIndexFormat->components.push_back(indexComponent);
    batchIndexFormat->size = sizeof(unsigned int);
    Render::PropertyBuffer* indexBuffer = new Render::PropertyBuffer();
    indexBuffer->SetSize( indexCount );
    indexBuffer->SetData(batchIndexBuffer);
    indexBuffer->SetFormat(batchIndexFormat);
    batchGeometry->AddPropertyBuffer( indexBuffer, true );

    //Create the new renderer
    SceneGraph::Renderer& renderer = firstItem.GetSceneGraphRenderer();
    RenderDataProvider* dataProvider = new RenderDataProvider();
    dataProvider->mMaterialDataProvider = &renderer.GetMaterial();
    dataProvider->mUniformMapDataProvider = &renderer;
    dataProvider->mShader = renderer.GetBatchMaterial()->GetShader();

    size_t textureCount( renderer.GetMaterial().GetTextureCount() );
    dataProvider->mTextures.resize( textureCount );
    for( unsigned int textureIndex(0); textureIndex<textureCount; ++textureIndex )
    {
      dataProvider->mTextures[textureIndex] = Render::Texture( renderer.GetMaterial().GetTextureUniformName(textureIndex),
                                                    renderer.GetMaterial().GetTextureId(textureIndex),
                                                    renderer.GetMaterial().GetTextureSampler(textureIndex));
    }


    Render::Renderer* batchRenderer = Render::NewRenderer::New( dataProvider, batchGeometry );
    updateManager.GetSceneController().GetRenderMessageDispatcher().AddRenderer( *batchRenderer );
    updateManager.GetSceneController().GetRenderMessageDispatcher().AddGeometry( *batchGeometry );
    updateManager.AddPropertyBuffer( vertexBuffer );
    updateManager.AddPropertyBuffer( indexBuffer );

    Batch newBatch = {batchRenderer,batchGeometry,vertexBuffer,indexBuffer};
    mBatches.push_back(newBatch);

    //Modify the first item to render the batch
    firstItem.GetModelViewMatrix() = viewMatrix;
    firstItem.SetRenderer( batchRenderer );


    //Delete the rest of the items from the list
    renderList.Remove( first+1, last );
  }
  else
  {
    delete batchVertexFormat;
  }
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
  std::vector<size_t> batchRanges;
  bool batching = false;
  size_t first;
  SceneGraph::Shader* previousBatchShader(0);
  unsigned int renderableCount( renderList.Count() );
  for( unsigned int index = 0; index < renderableCount; ++index )
  {
    RenderItem& item = renderList.GetItem(index);
    SceneGraph::Material* currentBatchMaterial = item.GetSceneGraphRenderer().GetBatchMaterial();
    if( currentBatchMaterial )
    {
      SceneGraph::Shader* currentBatchShader = currentBatchMaterial->GetShader();
      if( !previousBatchShader || (currentBatchShader == previousBatchShader) )
      {
        if( !batching )
        {
          first = index;
          batching = true;
        }
      }
      else
      {
        if( batching && index-1-first>0 )
        {
          batchRanges.push_back(first);
          batchRanges.push_back(index-1);
        }

        batching = false;
        first = index;
      }

      previousBatchShader = currentBatchShader;
    }
    else
    {
      batching = false;
      previousBatchShader = 0;
    }
  }
  if(batching )
  {
    batchRanges.push_back(first);
    batchRanges.push_back(renderList.Count()-1);
  }

  //clear previous batched renderers.
  //TODO: Try to recycle these objects instead of addind and removing all the time
  for( size_t i(0); i<mBatches.size(); i++)
  {
    updateManager.GetSceneController().GetRenderMessageDispatcher().RemoveRenderer( *mBatches[i].mRenderer );
    updateManager.GetSceneController().GetRenderMessageDispatcher().RemoveGeometry( *mBatches[i].mGeometry );
    updateManager.GetSceneController().GetRenderMessageDispatcher().RemovePropertyBuffer( *mBatches[i].mGeometry, mBatches[i].mVertexBuffer);
    updateManager.GetSceneController().GetRenderMessageDispatcher().RemovePropertyBuffer( *mBatches[i].mGeometry, mBatches[i].mIndexBuffer );
    updateManager.RemovePropertyBuffer( mBatches[i].mVertexBuffer );
    updateManager.RemovePropertyBuffer( mBatches[i].mIndexBuffer );

  }
  mBatches.clear();

  for( size_t i(0); i<batchRanges.size(); i+=2)
  {
    if( batchRanges[i+1]-batchRanges[i] > 0 )
    {
      BatchRenderers( updateBufferIndex, batchRanges[i], batchRanges[i+1], renderList, updateManager, viewMatrix );
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
