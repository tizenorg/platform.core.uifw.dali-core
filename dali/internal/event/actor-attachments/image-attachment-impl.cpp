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
#include <dali/internal/event/actor-attachments/image-attachment-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/update/node-attachments/scene-graph-image-attachment.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/public-api/common/dali-common.h>

using namespace std;

namespace
{
static Dali::Internal::SceneGraph::ImageAttachment::PixelArea EMPTY_PIXEL_AREA(0,0,0,0);
}

namespace Dali
{

namespace Internal
{

ImageAttachmentPtr ImageAttachment::New( const SceneGraph::Node& parentNode, Image* image )
{
  StagePtr stage = Stage::GetCurrent();

  ImageAttachmentPtr attachment( new ImageAttachment( *stage, image ) );

  // Transfer object ownership of scene-object to message
  SceneGraph::ImageAttachment* sceneObject = CreateSceneObject( image );
  AttachToNodeMessage( stage->GetUpdateManager(), parentNode, sceneObject );

  // Keep raw pointer for message passing
  attachment->mSceneObject = sceneObject;

  return attachment;
}

ImageAttachment::ImageAttachment(Stage& stage, Image* image)
: RenderableAttachment(stage),
  mSceneObject(NULL),
  mPixelArea(EMPTY_PIXEL_AREA),
  mStyle(Dali::ImageActor::STYLE_QUAD),
  mBorder(0.45,0.45,0.1,0.1),
  mIsPixelAreaSet(false),
  mBorderInPixels(false)
{
  mImageConnectable.Set( image, false );
}

ImageAttachment::~ImageAttachment()
{
}

void ImageAttachment::SetImage(Image* image)
{
  bool onStage = OnStage();
  // keep a reference to Image object
  mImageConnectable.Set( image, onStage );

  // Wait until the scene-graph attachment is connected, before providing resource ID
  if ( OnStage() )
  {
    unsigned int resourceId = (NULL != image) ? image->GetResourceId() : 0u;

    // sceneObject is being used in a separate thread; queue a message to set
    SetTextureIdMessage( mStage->GetUpdateInterface(), *mSceneObject, resourceId );
  }
}

Dali::Image ImageAttachment::GetImage()
{
  Dali::Image image;
  Image* current = mImageConnectable.Get();
  if ( current != NULL)
  {
    image = Dali::Image( current );
  }
  else
  {
    // returning uninitialized image (empty image)
  }
  return image;
}

void ImageAttachment::SetPixelArea(const PixelArea& pixelArea)
{
  // check to see if pixel area is actually different, using rect::operator==
  if( pixelArea != mPixelArea )
  {
    // Cache for public getters
    mPixelArea = pixelArea;
    mIsPixelAreaSet = true;

    // sceneObject is being used in a separate thread; queue a message to set
    SetPixelAreaMessage( mStage->GetUpdateInterface(), *mSceneObject, mPixelArea );
  }
}

void ImageAttachment::ClearPixelArea()
{
  // Cache for public getters
  mIsPixelAreaSet = false;

  // sceneObject is being used in a separate thread; queue a message to set
  ClearPixelAreaMessage( mStage->GetUpdateInterface(), *mSceneObject );
}

void ImageAttachment::SetStyle(Style style)
{
  // Cache for public getters
  mStyle = style;

  // sceneObject is being used in a separate thread; queue a message to set
  SetStyleMessage( mStage->GetUpdateInterface(), *mSceneObject, style );
}

void ImageAttachment::SetNinePatchBorder(const Vector4& border, bool inPixels)
{
  // Cache for public getters
  mBorder = border;
  mBorderInPixels = inPixels;

  // sceneObject is being used in a separate thread; queue a message to set
  SetNinePatchBorderMessage( mStage->GetUpdateInterface(), *mSceneObject, border, inPixels );
}

SceneGraph::ImageAttachment* ImageAttachment::CreateSceneObject( const Image* current )
{
  if ( current )
  {
    return SceneGraph::ImageAttachment::New( current->GetResourceId() );
  }

  return SceneGraph::ImageAttachment::New( 0u );
}

void ImageAttachment::OnStageConnection2()
{
  mImageConnectable.OnStageConnect();

  // Provide resource ID when scene-graph attachment is connected
  Image* image = mImageConnectable.Get();
  unsigned int resourceId = (NULL != image) ? image->GetResourceId() : 0u;
  if ( 0u != resourceId )
  {
    SetTextureIdMessage( mStage->GetUpdateInterface(), *mSceneObject, resourceId );
  }
}

void ImageAttachment::OnStageDisconnection2()
{
  // Remove resource ID when scene-graph attachment is disconnected
  SetTextureIdMessage( mStage->GetUpdateInterface(), *mSceneObject, 0u );

  mImageConnectable.OnStageDisconnect();
}

const SceneGraph::RenderableAttachment& ImageAttachment::GetSceneObject() const
{
  DALI_ASSERT_DEBUG( mSceneObject != NULL );
  return *mSceneObject;
}

} // namespace Internal

} // namespace Dali
