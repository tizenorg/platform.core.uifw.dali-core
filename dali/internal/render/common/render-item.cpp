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
#include <dali/internal/render/common/render-item.h>

// INTERNAL INCLUDES
#include <dali/internal/render/renderers/scene-graph-renderer.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

RenderItem::RenderItem()
: mModelViewMatrix( false ),
  mRenderer( NULL ),
  mDepthIndex(0)
{
}

RenderItem::~RenderItem()
{
}

void RenderItem::Reset()
{
  mRenderer = NULL;
}

void RenderItem::SetRenderer( Renderer* renderer )
{
  mRenderer = renderer;
}

const Renderer* RenderItem::GetRenderer() const
{
  return mRenderer;
}

Matrix& RenderItem::GetModelViewMatrix()
{
  return mModelViewMatrix;
}

const Matrix& RenderItem::GetModelViewMatrix() const
{
  return mModelViewMatrix;
}

int RenderItem::GetDepthIndex() const
{
  return mDepthIndex;
}

void RenderItem::SetDepthIndex( int depthIndex )
{
  mDepthIndex = depthIndex;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
