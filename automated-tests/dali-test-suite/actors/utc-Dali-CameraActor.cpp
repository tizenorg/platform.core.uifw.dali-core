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

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

#include <dali/internal/event/actors/camera-actor-impl.h>

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

TEST_FUNCTION( UtcDaliCameraActorConstructorVoid,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorConstructorRefObject,        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorNew,                         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorDownCast,                    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorDownCast2,                   NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorSetCameraOffStage,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorSetCameraOnStage,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorGetCamera,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorDefaultProperties,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorSetTarget,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorSetType01,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorSetType02,                   NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorSetFieldOfView,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorSetAspectRatio,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorSetNearClippingPlane,        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorSetFarClippingPlane,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorSetTargetPosition,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorSetInvertYAxis,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorModelView,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorSetPerspectiveProjection,    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorSetOrthographicProjection01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorSetOrthographicProjection02, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorSetOrthographicProjection03, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorReadProjectionMatrix,        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCameraActorAnimatedProperties,          POSITIVE_TC_IDX );


namespace
{
const float FLOAT_EPSILON = 0.001f;
const float TEST_ASPECT_RATIO = 0.123f;
const float TEST_FIELD_OF_VIEW = Radian(Degree(40.0f));
const float TEST_NEAR_PLANE_DISTANCE = 0.23f;
const float TEST_FAR_PLANE_DISTANCE = 0.973f;
}


BitmapImage CreateBitmapImage()
{
  BitmapImage image = BitmapImage::New(4,4,Pixel::RGBA8888);
  PixelBuffer* pixbuf = image.GetBuffer();

  // Using a 4x4 image gives a better blend with the GL implementation
  // than a 3x3 image
  for(size_t i=0; i<16; i++)
  {
    pixbuf[i*4+0] = 0xFF;
    pixbuf[i*4+1] = 0xFF;
    pixbuf[i*4+2] = 0xFF;
    pixbuf[i*4+3] = 0xFF;
  }

  return image;
}


// Called only once before first test is run.
static void Startup()
{
  // TODO - add a model loader to the test harness
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliCameraActorConstructorVoid()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::CameraActor()");

  CameraActor actor;

  DALI_TEST_CHECK(!actor);
}

static void UtcDaliCameraActorConstructorRefObject()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::CameraActor(Internal::CameraActor*)");

  CameraActor actor(NULL);

  DALI_TEST_CHECK(!actor);
}

static void UtcDaliCameraActorNew()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::New()");

  CameraActor actor = CameraActor::New();

  DALI_TEST_CHECK(actor);

  actor = NULL;

  DALI_TEST_CHECK(!actor);
}

static void UtcDaliCameraActorDownCast()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::DownCast()");

  CameraActor camera = CameraActor::New();
  Actor anActor = Actor::New();
  anActor.Add( camera );

  Actor child = anActor.GetChildAt(0);
  CameraActor cameraActor = CameraActor::DownCast( child );
  DALI_TEST_CHECK( cameraActor );

  cameraActor = NULL;
  DALI_TEST_CHECK( !cameraActor );

  cameraActor = DownCast< CameraActor >( child );
  DALI_TEST_CHECK( cameraActor );
}

static void UtcDaliCameraActorDownCast2()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::DownCast()");

  Actor actor1 = Actor::New();
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  CameraActor cameraActor = CameraActor::DownCast( child );
  DALI_TEST_CHECK( !cameraActor );

  Actor unInitialzedActor;
  cameraActor = CameraActor::DownCast( unInitialzedActor );
  DALI_TEST_CHECK( !cameraActor );

  cameraActor = DownCast< CameraActor >( unInitialzedActor );
  DALI_TEST_CHECK( !cameraActor );
}


static void UtcDaliCameraActorSetCameraOffStage()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetCamera()");

  CameraActor actor = CameraActor::New();

  actor.SetType(Camera::FREE_LOOK);
  actor.SetFieldOfView(TEST_FIELD_OF_VIEW);
  actor.SetAspectRatio(TEST_ASPECT_RATIO);
  actor.SetNearClippingPlane(TEST_NEAR_PLANE_DISTANCE);
  actor.SetFarClippingPlane(TEST_FAR_PLANE_DISTANCE);
  actor.SetProjectionMode(Camera::PERSPECTIVE_PROJECTION);

  actor.SetInvertYAxis(false);

  DALI_TEST_EQUALS(TEST_ASPECT_RATIO, actor.GetAspectRatio(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_FIELD_OF_VIEW, actor.GetFieldOfView(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_NEAR_PLANE_DISTANCE, actor.GetNearClippingPlane(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_FAR_PLANE_DISTANCE, actor.GetFarClippingPlane(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(Camera::PERSPECTIVE_PROJECTION, actor.GetProjectionMode(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, actor.GetInvertYAxis(), TEST_LOCATION);

  float value;
  std::string sValue;
  actor.GetProperty(CameraActor::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(TEST_ASPECT_RATIO, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(TEST_FIELD_OF_VIEW, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(TEST_NEAR_PLANE_DISTANCE, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(TEST_FAR_PLANE_DISTANCE, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::PROJECTION_MODE).Get(sValue);
  DALI_TEST_EQUALS("PERSPECTIVE_PROJECTION", sValue, TEST_LOCATION);
  bool bValue;
  actor.GetProperty(CameraActor::INVERT_Y_AXIS).Get(bValue);
  DALI_TEST_EQUALS(false, bValue, TEST_LOCATION);
}

static void UtcDaliCameraActorSetCameraOnStage()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetCamera()");

  CameraActor actor = CameraActor::New();
  Stage::GetCurrent().Add(actor);
  application.Render(0);
  application.SendNotification();

  actor.SetType(Camera::LOOK_AT_TARGET);
  actor.SetFieldOfView(TEST_FIELD_OF_VIEW);
  actor.SetAspectRatio(TEST_ASPECT_RATIO);
  actor.SetNearClippingPlane(TEST_NEAR_PLANE_DISTANCE);
  actor.SetFarClippingPlane(TEST_FAR_PLANE_DISTANCE);
  actor.SetInvertYAxis(false);

  DALI_TEST_EQUALS(false, actor.GetInvertYAxis(), TEST_LOCATION);

  // Will need 2 frames to ensure both buffers are set to same values:
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS(TEST_ASPECT_RATIO, actor.GetAspectRatio(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_FIELD_OF_VIEW, actor.GetFieldOfView(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_NEAR_PLANE_DISTANCE, actor.GetNearClippingPlane(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_FAR_PLANE_DISTANCE, actor.GetFarClippingPlane(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(false, actor.GetInvertYAxis(), TEST_LOCATION);

  std::string sValue;
  actor.GetProperty(CameraActor::TYPE).Get(sValue);
  DALI_TEST_EQUALS(sValue, "LOOK_AT_TARGET", TEST_LOCATION);

  float value;
  actor.GetProperty(CameraActor::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(TEST_ASPECT_RATIO, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(TEST_FIELD_OF_VIEW, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(TEST_NEAR_PLANE_DISTANCE, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(TEST_FAR_PLANE_DISTANCE, value, FLOAT_EPSILON, TEST_LOCATION);

  bool bValue;
  actor.GetProperty(CameraActor::INVERT_Y_AXIS).Get(bValue);
  DALI_TEST_EQUALS(false, bValue, TEST_LOCATION);
}


static void UtcDaliCameraActorGetCamera()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::GetCamera()");

  CameraActor actor = CameraActor::New();

  actor.SetAspectRatio(TEST_ASPECT_RATIO);

  DALI_TEST_EQUALS(actor.GetAspectRatio(), TEST_ASPECT_RATIO, FLOAT_EPSILON, TEST_LOCATION);

  actor.SetProperty(CameraActor::TYPE, "FREE_LOOK");
  actor.SetProperty(CameraActor::ASPECT_RATIO, TEST_ASPECT_RATIO);
  actor.SetProperty(CameraActor::FIELD_OF_VIEW, TEST_FIELD_OF_VIEW);
  actor.SetProperty(CameraActor::NEAR_PLANE_DISTANCE, TEST_NEAR_PLANE_DISTANCE);
  actor.SetProperty(CameraActor::FAR_PLANE_DISTANCE, TEST_FAR_PLANE_DISTANCE);

  DALI_TEST_EQUALS(Camera::FREE_LOOK, actor.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_ASPECT_RATIO, actor.GetAspectRatio(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_FIELD_OF_VIEW, actor.GetFieldOfView(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_NEAR_PLANE_DISTANCE, actor.GetNearClippingPlane(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_FAR_PLANE_DISTANCE, actor.GetFarClippingPlane(), FLOAT_EPSILON, TEST_LOCATION);
}

static void UtcDaliCameraActorDefaultProperties()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor DefaultProperties");

  CameraActor actor = CameraActor::New();

  actor.SetAspectRatio(TEST_ASPECT_RATIO);
  Stage::GetCurrent().Add(actor);
  application.Render(0);
  application.SendNotification();
  bool bValue;
  actor.GetProperty(CameraActor::INVERT_Y_AXIS).Get(bValue);
  DALI_TEST_EQUALS(true, bValue, TEST_LOCATION);

  std::vector<Property::Index> indices ;
  indices.push_back(CameraActor::TYPE);
  indices.push_back(CameraActor::PROJECTION_MODE);
  indices.push_back(CameraActor::FIELD_OF_VIEW       );
  indices.push_back(CameraActor::ASPECT_RATIO    );
  indices.push_back(CameraActor::NEAR_PLANE_DISTANCE      );
  indices.push_back(CameraActor::FAR_PLANE_DISTANCE       );
  indices.push_back(CameraActor::LEFT_PLANE_DISTANCE      );
  indices.push_back(CameraActor::RIGHT_PLANE_DISTANCE       );
  indices.push_back(CameraActor::TOP_PLANE_DISTANCE      );
  indices.push_back(CameraActor::BOTTOM_PLANE_DISTANCE       );
  indices.push_back(CameraActor::TARGET_POSITION );
  indices.push_back(CameraActor::PROJECTION_MATRIX );
  indices.push_back(CameraActor::VIEW_MATRIX);
  indices.push_back(CameraActor::INVERT_Y_AXIS );

  DALI_TEST_CHECK(actor.GetPropertyCount() == ( Actor::New().GetPropertyCount() + indices.size() ) );

  for(std::vector<Property::Index>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
  {
    DALI_TEST_EQUALS( *iter, actor.GetPropertyIndex(actor.GetPropertyName(*iter)), TEST_LOCATION);
    DALI_TEST_CHECK( ! actor.IsPropertyAnimatable(*iter) );

    switch(*iter)
    {
      if ( ( *iter == CameraActor::PROJECTION_MATRIX ) ||
           ( *iter == CameraActor::VIEW_MATRIX ) )
      {
        DALI_TEST_CHECK( ! actor.IsPropertyWritable(*iter) );
      }
      else
      {
        DALI_TEST_CHECK( actor.IsPropertyWritable(*iter) );
      }
    }

    DALI_TEST_CHECK( actor.GetPropertyType(*iter) == actor.GetPropertyType(*iter) ); // just checking call succeeds
  }

  // set/get one of them
  const float newAspect = TEST_ASPECT_RATIO * 2.f;

  actor.SetProperty( CameraActor::ASPECT_RATIO, Property::Value(newAspect) );
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS(actor.GetAspectRatio(), newAspect, TEST_LOCATION );
}

static void UtcDaliCameraActorSetTarget()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get/Set Target Position");

  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS( actor.GetTargetPosition(), Vector3::ZERO, TEST_LOCATION );

  Vector3 target( 10.0f, 20.0f, 30.0f);
  actor.SetTargetPosition( target );

  DALI_TEST_EQUALS( actor.GetTargetPosition(), target, TEST_LOCATION );

  Vector3 value;
  actor.GetProperty(CameraActor::TARGET_POSITION).Get(value);
  DALI_TEST_EQUALS(target, value, FLOAT_EPSILON, TEST_LOCATION);
}


static void UtcDaliCameraActorSetType01()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get/Set Type");

  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS( actor.GetType(), Dali::Camera::LOOK_AT_TARGET, TEST_LOCATION );

  actor.SetType(Dali::Camera::FREE_LOOK);
  DALI_TEST_EQUALS( actor.GetType(), Dali::Camera::FREE_LOOK, TEST_LOCATION );

  std::string sValue;
  actor.GetProperty(CameraActor::TYPE).Get(sValue);
  std::string result("FREE_LOOK");
  DALI_TEST_EQUALS(result, sValue, TEST_LOCATION);
}

static void UtcDaliCameraActorSetType02()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get/Set Type");

  CameraActor actor;

  Dali::Camera::Type cameraType = Dali::Camera::FREE_LOOK ;
  try
  {
    cameraType = actor.GetType();
  }
  catch ( Dali::DaliException& e )
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "camera", TEST_LOCATION);
  }

  const CameraActor aConstActor;

  try
  {
    cameraType = aConstActor.GetType();
  }
  catch ( Dali::DaliException& e )
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "camera", TEST_LOCATION);
  }

  DALI_TEST_EQUALS((int)cameraType, (int)Dali::Camera::FREE_LOOK, TEST_LOCATION);
}

static void UtcDaliCameraActorSetFieldOfView()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get/Set Field of view");

  CameraActor defaultCamera = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  const float defaultFov = defaultCamera.GetFieldOfView();

  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS( actor.GetFieldOfView(), defaultFov, TEST_LOCATION );

  float fov = Math::PI/3.0f;
  actor.SetFieldOfView(fov);
  DALI_TEST_EQUALS( actor.GetFieldOfView(), fov, TEST_LOCATION );

  float value;
  actor.GetProperty(CameraActor::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(fov, value, FLOAT_EPSILON, TEST_LOCATION);
}

static void UtcDaliCameraActorSetAspectRatio()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get/Set Aspect Ratio");

  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS( actor.GetAspectRatio(), static_cast<float>( TestApplication::DEFAULT_SURFACE_WIDTH ) / static_cast<float>( TestApplication::DEFAULT_SURFACE_HEIGHT ), TEST_LOCATION );

  float aspect = 16.0f/9.0f;
  actor.SetAspectRatio(aspect);
  DALI_TEST_EQUALS( actor.GetAspectRatio(), aspect, TEST_LOCATION );

  float value;
  actor.GetProperty(CameraActor::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(aspect, value, FLOAT_EPSILON, TEST_LOCATION);
}

static void UtcDaliCameraActorSetNearClippingPlane()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get/Set Near clipping plane");

  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS( actor.GetNearClippingPlane(), 800.0f, TEST_LOCATION );

  actor.SetNearClippingPlane(400.0f);
  DALI_TEST_EQUALS( actor.GetNearClippingPlane(), 400.0f, TEST_LOCATION );

  float value;
  actor.GetProperty(CameraActor::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(400.0f, value, FLOAT_EPSILON, TEST_LOCATION);
}

static void UtcDaliCameraActorSetFarClippingPlane()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get/Set Far clipping plane");

  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS( actor.GetFarClippingPlane(), 800.0f + (0xFFFF>>4), TEST_LOCATION );

  actor.SetFarClippingPlane(4000.0f);
  DALI_TEST_EQUALS( actor.GetFarClippingPlane(), 4000.0f, TEST_LOCATION );

  float value;
  actor.GetProperty(CameraActor::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(4000.0f, value, FLOAT_EPSILON, TEST_LOCATION);
}

static void UtcDaliCameraActorSetTargetPosition()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get/Set Target Position");

  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS( actor.GetTargetPosition(), Vector3::ZERO, TEST_LOCATION );

  Vector3 target( 10.0f, 20.0f, 30.0f);
  actor.SetTargetPosition( target );

  DALI_TEST_EQUALS( actor.GetTargetPosition(), target, TEST_LOCATION );

  Vector3 value;
  actor.GetProperty(CameraActor::TARGET_POSITION).Get(value);
  DALI_TEST_EQUALS(target, value, FLOAT_EPSILON, TEST_LOCATION);
}

static void UtcDaliCameraActorSetInvertYAxis()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get/Set InvertYAxis");

  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS( actor.GetInvertYAxis(), true, TEST_LOCATION );

  actor.SetInvertYAxis(false);

  DALI_TEST_EQUALS( actor.GetInvertYAxis(), false, TEST_LOCATION );

  bool bValue;
  actor.GetProperty(CameraActor::INVERT_Y_AXIS).Get(bValue);
  DALI_TEST_EQUALS(false, bValue, TEST_LOCATION);
}

static void UtcDaliCameraActorModelView()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Test view application");

  BitmapImage image = CreateBitmapImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetPosition(20.0f, 30.0f, 40.0f);
  actor.SetParentOrigin(ParentOrigin::CENTER);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  Matrix resultMatrix(true);
  resultMatrix.SetTransformComponents( Vector3::ONE, Quaternion::IDENTITY, actor.GetCurrentPosition());

  RenderTask task = Stage::GetCurrent().GetRenderTaskList().GetTask(0);
  CameraActor cameraActor = task.GetCameraActor();

  Matrix viewMatrix(false);
  cameraActor.GetProperty(CameraActor::VIEW_MATRIX).Get( viewMatrix );
  Matrix::Multiply(resultMatrix, resultMatrix, viewMatrix);

  DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue( "uModelView", resultMatrix ) );
}

static void UtcDaliCameraActorSetPerspectiveProjection()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetPerspectiveProjection( const Size& size )");

  CameraActor actor = CameraActor::New();
  actor.SetPerspectiveProjection( Size( 100.f, 150.f ) );

  DALI_TEST_CHECK(actor);

  float value;
  actor.GetProperty(CameraActor::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(0.666666f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(0.489957f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(150.f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(4245.f, value, FLOAT_EPSILON, TEST_LOCATION);

  DALI_TEST_EQUALS((int)actor.GetProjectionMode(), (int)Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION);

  std::string stringValue;
  actor.GetProperty(CameraActor::PROJECTION_MODE).Get(stringValue);
  DALI_TEST_EQUALS(stringValue, "PERSPECTIVE_PROJECTION", TEST_LOCATION);
}

static void UtcDaliCameraActorSetOrthographicProjection01()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetOrthographicProjection01()");

  CameraActor actor = CameraActor::New(Size(1080.0f, 1920.0f));
  DALI_TEST_CHECK(actor);

  Stage::GetCurrent().Add( actor );

  actor.SetOrthographicProjection( Size( 1080.0f, 1920.0f ) );
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  float defaultAspectRatio;
  float defaultFieldOfView;
  float defaultNearPlaneDistance;
  float defaultFarPlaneDistance;
  actor.GetProperty(CameraActor::ASPECT_RATIO).Get(defaultAspectRatio);
  actor.GetProperty(CameraActor::FIELD_OF_VIEW).Get(defaultFieldOfView);
  actor.GetProperty(CameraActor::NEAR_PLANE_DISTANCE).Get(defaultNearPlaneDistance);
  actor.GetProperty(CameraActor::FAR_PLANE_DISTANCE).Get(defaultFarPlaneDistance);
  Vector3 defaultPos = actor.GetCurrentPosition();

  actor.SetOrthographicProjection( Size( 1080.0f, 1920.0f ) );

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  float value;
  actor.GetProperty(CameraActor::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(defaultAspectRatio, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(defaultFieldOfView, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(defaultNearPlaneDistance, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(defaultFarPlaneDistance, value, FLOAT_EPSILON, TEST_LOCATION);

  actor.GetProperty(CameraActor::LEFT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-540.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::RIGHT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS( 540.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::TOP_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(960.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::BOTTOM_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-960.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  Vector3 pos = actor.GetCurrentPosition();
  DALI_TEST_EQUALS(defaultPos.z, pos.z, 0.001f, TEST_LOCATION);

  DALI_TEST_EQUALS((int)actor.GetProjectionMode(), (int)Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION);
}


static void UtcDaliCameraActorSetOrthographicProjection02()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetOrthographicProjection02()");

  CameraActor actor = CameraActor::New();
  DALI_TEST_CHECK(actor);

  float defaultAspectRatio;
  float defaultFieldOfView;
  float defaultNearPlaneDistance;
  float defaultFarPlaneDistance;
  actor.GetProperty(CameraActor::ASPECT_RATIO).Get(defaultAspectRatio);
  actor.GetProperty(CameraActor::FIELD_OF_VIEW).Get(defaultFieldOfView);
  actor.GetProperty(CameraActor::NEAR_PLANE_DISTANCE).Get(defaultNearPlaneDistance);
  actor.GetProperty(CameraActor::FAR_PLANE_DISTANCE).Get(defaultFarPlaneDistance);

  actor.SetOrthographicProjection( -100.0f, 200.0f, -300.0f, 500.0f, 400.0f, 4000.0f );

  float value;
  actor.GetProperty(CameraActor::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(defaultAspectRatio, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(defaultFieldOfView, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(400.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(4000.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  actor.GetProperty(CameraActor::LEFT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-100.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::RIGHT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS( 200.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::TOP_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-300.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::BOTTOM_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS( 500.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  DALI_TEST_EQUALS((int)actor.GetProjectionMode(), (int)Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION);

}


static void UtcDaliCameraActorSetOrthographicProjection03()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetOrthographicProjection03()");

  CameraActor actor = CameraActor::New();
  DALI_TEST_CHECK(actor);

  float defaultAspectRatio;
  float defaultFieldOfView;
  float defaultNearPlaneDistance;
  float defaultFarPlaneDistance;
  actor.GetProperty(CameraActor::ASPECT_RATIO).Get(defaultAspectRatio);
  actor.GetProperty(CameraActor::FIELD_OF_VIEW).Get(defaultFieldOfView);
  actor.GetProperty(CameraActor::NEAR_PLANE_DISTANCE).Get(defaultNearPlaneDistance);
  actor.GetProperty(CameraActor::FAR_PLANE_DISTANCE).Get(defaultFarPlaneDistance);

  actor.SetProjectionMode(Dali::Camera::ORTHOGRAPHIC_PROJECTION);

  actor.SetProperty(CameraActor::LEFT_PLANE_DISTANCE, -100.0f);
  actor.SetProperty(CameraActor::RIGHT_PLANE_DISTANCE, 200.0f);
  actor.SetProperty(CameraActor::TOP_PLANE_DISTANCE, -300.0f);
  actor.SetProperty(CameraActor::BOTTOM_PLANE_DISTANCE, 500.0f);
  actor.SetNearClippingPlane( 400.0f );
  actor.SetFarClippingPlane( 4000.0f );

  float value;
  actor.GetProperty(CameraActor::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(defaultAspectRatio, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(defaultFieldOfView, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(400.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(4000.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  actor.GetProperty(CameraActor::LEFT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-100.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::RIGHT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS( 200.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::TOP_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-300.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::BOTTOM_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS( 500.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  DALI_TEST_EQUALS((int)actor.GetProjectionMode(), (int)Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION);
  std::string stringValue;
  actor.GetProperty(CameraActor::PROJECTION_MODE).Get(stringValue);
  DALI_TEST_EQUALS(stringValue, "ORTHOGRAPHIC_PROJECTION", TEST_LOCATION);
}


const std::string SHADER_LIGHT_CAMERA_PROJECTION_MATRIX_PROPERTY_NAME( "uLightCameraProjectionMatrix" );
const std::string SHADER_LIGHT_CAMERA_VIEW_MATRIX_PROPERTY_NAME( "uLightCameraViewMatrix" );
const char* const RENDER_SHADOW_VERTEX_SOURCE =
  " uniform mediump mat4 uLightCameraProjectionMatrix;\n"
  " uniform mediump mat4 uLightCameraViewMatrix;\n"
  "\n"
  "void main()\n"
  "{\n"
    "  gl_Position = uProjection * uModelView * vec4(aPosition,1.0);\n"
    "  vec4 textureCoords = uLightCameraProjectionMatrix * uLightCameraViewMatrix * uModelMatrix  * vec4(aPosition,1.0);\n"
    "  vTexCoord = 0.5 + 0.5 * (textureCoords.xy/textureCoords.w);\n"
  "}\n";

const char* const RENDER_SHADOW_FRAGMENT_SOURCE =
  "uniform lowp vec4 uShadowColor;\n"
  "void main()\n"
  "{\n"
  "  lowp float alpha;\n"
  "  alpha = texture2D(sTexture, vec2(vTexCoord.x, vTexCoord.y)).a;\n"
  "  gl_FragColor = vec4(uShadowColor.rgb, uShadowColor.a * alpha);\n"
  "}\n";


static void UtcDaliCameraActorReadProjectionMatrix()
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::ReadProjectionMatrix()");

  CameraActor camera = Stage::GetCurrent().GetRenderTaskList().GetTask(0u).GetCameraActor();
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  Image image = CreateBitmapImage();
  ImageActor imageActor = ImageActor::New( image );
  imageActor.SetSize(100.0f, 100.0f);
  Stage::GetCurrent().Add(imageActor);

  Matrix projectionMatrix;
  Matrix viewMatrix;

  camera.GetProperty(CameraActor::CameraActor::PROJECTION_MATRIX).Get(projectionMatrix);
  camera.GetProperty(CameraActor::CameraActor::VIEW_MATRIX).Get(viewMatrix);

  Dali::Internal::CameraActor& internalCamera = GetImplementation(camera);
  const Matrix& projectionMatrix2 = internalCamera.GetProjectionMatrix();
  const Matrix& viewMatrix2 = internalCamera.GetViewMatrix();
  DALI_TEST_EQUALS(projectionMatrix, projectionMatrix2, TEST_LOCATION);
  DALI_TEST_EQUALS(viewMatrix, viewMatrix2, TEST_LOCATION);

  ShaderEffect shaderEffect = ShaderEffect::New( RENDER_SHADOW_VERTEX_SOURCE, RENDER_SHADOW_FRAGMENT_SOURCE);
  imageActor.SetShaderEffect(shaderEffect);

  shaderEffect.SetUniform( SHADER_LIGHT_CAMERA_PROJECTION_MATRIX_PROPERTY_NAME, Matrix::IDENTITY );
  shaderEffect.SetUniform( SHADER_LIGHT_CAMERA_VIEW_MATRIX_PROPERTY_NAME, Matrix::IDENTITY );

  Property::Index projectionMatrixPropertyIndex = shaderEffect.GetPropertyIndex(SHADER_LIGHT_CAMERA_PROJECTION_MATRIX_PROPERTY_NAME);
  Property::Index viewMatrixPropertyIndex = shaderEffect.GetPropertyIndex(SHADER_LIGHT_CAMERA_VIEW_MATRIX_PROPERTY_NAME);

  Constraint projectionMatrixConstraint = Constraint::New<Dali::Matrix>( projectionMatrixPropertyIndex, Source( camera, CameraActor::PROJECTION_MATRIX ), EqualToConstraint());
  Constraint viewMatrixConstraint = Constraint::New<Dali::Matrix>( viewMatrixPropertyIndex, Source( camera, CameraActor::VIEW_MATRIX ), EqualToConstraint());

  shaderEffect.ApplyConstraint(projectionMatrixConstraint);
  shaderEffect.ApplyConstraint(viewMatrixConstraint);

  application.SendNotification();
  application.Render();

  // Test effects of Constraint.
  DALI_TEST_CHECK(application.GetGlAbstraction().CheckUniformValue(SHADER_LIGHT_CAMERA_PROJECTION_MATRIX_PROPERTY_NAME.c_str(), projectionMatrix));

  DALI_TEST_CHECK(application.GetGlAbstraction().CheckUniformValue(SHADER_LIGHT_CAMERA_VIEW_MATRIX_PROPERTY_NAME.c_str(), viewMatrix));
}

static void UtcDaliCameraActorAnimatedProperties()
{
  TestApplication application;
  tet_infoline("Testing Dali::Internal::CameraActor::GetSceneObjectAnimatableProperty()");

  CameraActor camera = Stage::GetCurrent().GetRenderTaskList().GetTask(0u).GetCameraActor();
  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  Stage::GetCurrent().Add(actor);

  actor.ApplyConstraint(Constraint::New<Dali::Vector3>( Actor::POSITION, Source( camera, Actor::POSITION), EqualToConstraint()));

  camera.SetPosition(100.0f, 200.0f, 300.0f);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3(100.0f, 200.0f, 300.0f), TEST_LOCATION);
}
