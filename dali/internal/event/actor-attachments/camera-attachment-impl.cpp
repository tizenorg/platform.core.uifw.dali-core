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

// CLASS HEADER
#include <dali/internal/event/actor-attachments/camera-attachment-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/node-attachments/scene-graph-camera-attachment.h>

using namespace std;

namespace Dali
{

namespace Internal
{

CameraAttachmentPtr CameraAttachment::New( const SceneGraph::Node& parentNode )
{
  StagePtr stage = Stage::GetCurrent();

  CameraAttachmentPtr attachment( new CameraAttachment( *stage ) );

  // Transfer object ownership of scene-object to message
  SceneGraph::CameraAttachment* sceneObject = CreateSceneObject();
  AttachToNodeMessage( stage->GetUpdateManager(), parentNode, sceneObject );

  // Keep raw pointer for message passing
  attachment->mSceneObject = sceneObject;

  return attachment;
}

CameraAttachment::CameraAttachment( Stage& stage )
: ActorAttachment( stage ),
  mSceneObject( NULL ),
  mType( SceneGraph::CameraAttachment::DEFAULT_TYPE ),
  mProjectionMode( SceneGraph::CameraAttachment::DEFAULT_MODE ),
  mInvertYAxis( SceneGraph::CameraAttachment::DEFAULT_INVERT_Y_AXIS ),
  mFieldOfView( SceneGraph::CameraAttachment::DEFAULT_FIELD_OF_VIEW ),
  mAspectRatio( SceneGraph::CameraAttachment::DEFAULT_ASPECT_RATIO ),
  mStereoBias( SceneGraph::CameraAttachment::DEFAULT_STEREO_BIAS ),
  mLeftClippingPlane( SceneGraph::CameraAttachment::DEFAULT_LEFT_CLIPPING_PLANE ),
  mRightClippingPlane( SceneGraph::CameraAttachment::DEFAULT_RIGHT_CLIPPING_PLANE ),
  mTopClippingPlane( SceneGraph::CameraAttachment::DEFAULT_TOP_CLIPPING_PLANE ),
  mBottomClippingPlane( SceneGraph::CameraAttachment::DEFAULT_BOTTOM_CLIPPING_PLANE ),
  mNearClippingPlane( SceneGraph::CameraAttachment::DEFAULT_NEAR_CLIPPING_PLANE ),
  mFarClippingPlane( SceneGraph::CameraAttachment::DEFAULT_FAR_CLIPPING_PLANE ),
  mTargetPosition( SceneGraph::CameraAttachment::DEFAULT_TARGET_POSITION )
{
}

CameraAttachment::~CameraAttachment()
{
}

SceneGraph::CameraAttachment* CameraAttachment::CreateSceneObject()
{
  return SceneGraph::CameraAttachment::New();
}

void CameraAttachment::SetType(Dali::Camera::Type type)
{
  if( type != mType )
  {
    mType = type;

    // sceneObject is being used in a separate thread; queue a message to set
    SetTypeMessage( mStage->GetUpdateInterface(), *mSceneObject, type );
  }
}

Dali::Camera::Type CameraAttachment::GetType() const
{
  return mType;
}

void CameraAttachment::SetProjectionMode(Dali::Camera::ProjectionMode projectionMode)
{
  if( ! Equals(projectionMode, mProjectionMode) )
  {
    mProjectionMode = projectionMode;

    // sceneObject is being used in a separate thread; queue a message to set
    SetProjectionModeMessage( mStage->GetUpdateInterface(), *mSceneObject, projectionMode );
  }
}

Dali::Camera::ProjectionMode CameraAttachment::GetProjectionMode() const
{
  return mProjectionMode;
}

void CameraAttachment::SetFieldOfView( float fieldOfView )
{
  if( ! Equals(fieldOfView, mFieldOfView) )
  {
    mFieldOfView = fieldOfView;

    // sceneObject is being used in a separate thread; queue a message to set
    SetFieldOfViewMessage( mStage->GetUpdateInterface(), *mSceneObject, fieldOfView );
  }
}

float CameraAttachment::GetFieldOfView() const
{
  return mFieldOfView;
}

void CameraAttachment::SetAspectRatio( float aspectRatio )
{
  if( ! Equals(aspectRatio, mAspectRatio) )
  {
    mAspectRatio = aspectRatio;

    // sceneObject is being used in a separate thread; queue a message to set
    SetAspectRatioMessage( mStage->GetUpdateInterface(), *mSceneObject, aspectRatio );
  }
}

float CameraAttachment::GetAspectRatio() const
{
  return mAspectRatio;
}

void CameraAttachment::SetStereoBias(float stereoBias)
{
  if( ! Equals(stereoBias, mStereoBias) )
  {
    mStereoBias = stereoBias;

    // sceneObject is being used in a separate thread; queue a message to set
    SetStereoBiasMessage( mStage->GetUpdateInterface(), *mSceneObject, stereoBias );
  }
}

float CameraAttachment::GetStereoBias(float stereoBias) const
{
  return mStereoBias;
}

void CameraAttachment::SetLeftClippingPlane( float leftClippingPlane )
{
  if( ! Equals(leftClippingPlane, mLeftClippingPlane ) )
  {
    mLeftClippingPlane = leftClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetLeftClippingPlaneMessage( mStage->GetUpdateInterface(), *mSceneObject, leftClippingPlane );
  }
}

float CameraAttachment::GetLeftClippingPlane() const
{
  return mLeftClippingPlane;
}

void CameraAttachment::SetRightClippingPlane( float rightClippingPlane )
{
  if( ! Equals(rightClippingPlane, mRightClippingPlane ) )
  {
    mRightClippingPlane = rightClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetRightClippingPlaneMessage( mStage->GetUpdateInterface(), *mSceneObject, rightClippingPlane );
  }
}

float CameraAttachment::GetRightClippingPlane() const
{
  return mRightClippingPlane;
}

void CameraAttachment::SetTopClippingPlane( float topClippingPlane )
{
  if( ! Equals(topClippingPlane, mTopClippingPlane ) )
  {
    mTopClippingPlane = topClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetTopClippingPlaneMessage( mStage->GetUpdateInterface(), *mSceneObject, topClippingPlane );
  }
}

float CameraAttachment::GetTopClippingPlane() const
{
  return mTopClippingPlane;
}

void CameraAttachment::SetBottomClippingPlane( float bottomClippingPlane )
{
  if( ! Equals(bottomClippingPlane, mBottomClippingPlane ) )
  {
    mBottomClippingPlane = bottomClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetBottomClippingPlaneMessage( mStage->GetUpdateInterface(), *mSceneObject, bottomClippingPlane );
  }
}

float CameraAttachment::GetBottomClippingPlane() const
{
  return mBottomClippingPlane;
}

void CameraAttachment::SetNearClippingPlane( float nearClippingPlane )
{
  if( ! Equals(nearClippingPlane, mNearClippingPlane ) )
  {
    mNearClippingPlane = nearClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetNearClippingPlaneMessage( mStage->GetUpdateInterface(), *mSceneObject, nearClippingPlane );
  }
}

float CameraAttachment::GetNearClippingPlane() const
{
  return mNearClippingPlane;
}

void CameraAttachment::SetFarClippingPlane( float farClippingPlane )
{
  if( ! Equals( farClippingPlane, mFarClippingPlane ) )
  {
    mFarClippingPlane = farClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetFarClippingPlaneMessage( mStage->GetUpdateInterface(), *mSceneObject, farClippingPlane );
  }
}

float CameraAttachment::GetFarClippingPlane() const
{
  return mFarClippingPlane;
}

void CameraAttachment::SetTargetPosition( Vector3 targetPosition )
{
  if( targetPosition != mTargetPosition )
  {
    mTargetPosition = targetPosition;

    SetTargetPositionMessage( mStage->GetUpdateInterface(),  *mSceneObject, targetPosition );
  }
}

Vector3 CameraAttachment::GetTargetPosition()
{
  return mTargetPosition;
}

void CameraAttachment::SetInvertYAxis( bool invertYAxis )
{
  if( invertYAxis != mInvertYAxis )
  {
    mInvertYAxis = invertYAxis;

    // sceneObject is being used in a separate thread; queue a message to set
    SetInvertYAxisMessage( mStage->GetUpdateInterface(), *mSceneObject, invertYAxis );
  }
}

bool CameraAttachment::GetInvertYAxis() const
{
  return mInvertYAxis;
}

const Matrix& CameraAttachment::GetViewMatrix() const
{
  const SceneGraph::CameraAttachment& sceneObject = *mSceneObject;

  return sceneObject.GetViewMatrix( mStage->GetEventBufferIndex() );
}

const Matrix& CameraAttachment::GetProjectionMatrix() const
{
  const SceneGraph::CameraAttachment& sceneObject = *mSceneObject;

  return sceneObject.GetProjectionMatrix( mStage->GetEventBufferIndex() );
}

const Matrix& CameraAttachment::GetInverseViewProjectionMatrix() const
{
  const SceneGraph::CameraAttachment& sceneObject = *mSceneObject;

  return sceneObject.GetInverseViewProjectionMatrix( mStage->GetEventBufferIndex() );
}

const PropertyInputImpl* CameraAttachment::GetViewMatrixProperty() const
{
  DALI_ASSERT_DEBUG( OnStage() );

  const SceneGraph::CameraAttachment& sceneObject = *mSceneObject;

  return sceneObject.GetViewMatrix();
}

const PropertyInputImpl* CameraAttachment::GetProjectionMatrixProperty() const
{
  DALI_ASSERT_DEBUG( OnStage() );

  const SceneGraph::CameraAttachment& sceneObject = *mSceneObject;

  return sceneObject.GetProjectionMatrix();
}

void CameraAttachment::OnStageConnection()
{
  // do nothing
}

void CameraAttachment::OnStageDisconnection()
{
  // do nothing
}

} // namespace Internal

} // namespace Dali
