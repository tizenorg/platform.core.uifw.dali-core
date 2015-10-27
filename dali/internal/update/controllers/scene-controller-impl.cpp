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

// CLASS HEADER
#include <dali/internal/update/controllers/scene-controller-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/render/renderers/render-image-renderer.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

SceneControllerImpl::SceneControllerImpl( RenderMessageDispatcher& renderMessageDispatcher,
                                          RenderQueue& renderQueue,
                                          DiscardQueue& discardQueue,
                                          TextureCache& textureCache,
                                          CompleteStatusManager& completeStatusManager )
: mRenderMessageDispatcher( renderMessageDispatcher ),
  mRenderQueue( renderQueue ),
  mDiscardQueue( discardQueue ),
  mTextureCache( textureCache ),
  mCompleteStatusManager( completeStatusManager )
{
}

SceneControllerImpl::~SceneControllerImpl()
{
}

Render::ImageRenderer* SceneControllerImpl::NewImageRenderer()
{
  Render::ImageRenderer* renderer( NULL );

  if( 0 == mRendererPool.Count() )
  {
    renderer = Render::ImageRenderer::New();
  }
  else
  {
    renderer = mRendererPool[ mRendererPool.Count() - 1 ];

    mRendererPool.Erase( mRendererPool.End() - 1 );
  }

  return renderer;
}

void SceneControllerImpl::FreeImageRenderer( Render::ImageRenderer& renderer )
{
  mRendererPool.PushBack( &renderer );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
