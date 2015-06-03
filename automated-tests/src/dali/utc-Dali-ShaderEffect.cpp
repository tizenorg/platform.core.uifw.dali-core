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

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_shader_effect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_shader_effect_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

static const char* VertexSource =
"This is a custom vertex shader\n"
"made on purpose to look nothing like a normal vertex shader inside dali\n";

static const char* FragmentSource =
"This is a custom fragment shader\n"
"made on purpose to look nothing like a normal fragment shader inside dali\n";

const int GETSOURCE_BUFFER_SIZE = 0x10000;


struct TestConstraintToVector3
{
  TestConstraintToVector3(Vector3 target)
  : mTarget(target)
  {
  }

  void operator()( Vector3& current, const PropertyInputContainer& /* inputs */ )
  {
    current = mTarget;
  }

  Vector3 mTarget;
};

struct TestConstraintFromPositionToVector3
{
  TestConstraintFromPositionToVector3()
  {
  }

  void operator()( Vector3& current, const PropertyInputContainer& inputs )
  {
    current = inputs[0]->GetVector3();
  }
};

struct TestConstraintToVector3Double
{
  TestConstraintToVector3Double(Vector3 target)
  : mTarget(target)
  {
  }

  void operator()( Vector3& current, const PropertyInputContainer& /* inputs */ )
  {
    current = mTarget * 2.0f;
  }

  Vector3 mTarget;
};

static const char* TestImageFilename = "icon_wrt.png";

Integration::Bitmap* CreateBitmap( unsigned int imageHeight, unsigned int imageWidth, unsigned int initialColor )
{
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::RETAIN );
  Integration::PixelBuffer* pixbuffer = bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  imageWidth,imageHeight,imageWidth,imageHeight );
  unsigned int bytesPerPixel = GetBytesPerPixel(  Pixel::RGBA8888 );

  memset( pixbuffer,  initialColor , imageHeight*imageWidth*bytesPerPixel);

  return bitmap;
}

} // anon namespace


int UtcDaliShaderEffectMethodNew01(void)
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK(effect);
  END_TEST;
}

int UtcDaliShaderEffectMethodNew02(void)
{
  TestApplication application;

  ShaderEffect effect;

  try
  {
    // New must be called to create a ShaderEffect or it wont be valid.
    effect.SetUniform( "uUniform", 0 );
    DALI_TEST_CHECK( false );
  }
  catch (Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_CHECK( !effect );
  }
  END_TEST;
}

int UtcDaliShaderEffectMethodNew05(void)
{
  TestApplication application;

  // heap constructor / destructor
  DefaultFunctionCoverage<ShaderEffect> shaderEffect;

  END_TEST;
}

int UtcDaliShaderEffectMethodDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::ShaderEffect::DownCast()");

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );

  BaseHandle object(effect);

  ShaderEffect effect2 = ShaderEffect::DownCast(object);
  DALI_TEST_CHECK(effect2);

  ShaderEffect effect3 = DownCast< ShaderEffect >(object);
  DALI_TEST_CHECK(effect3);

  BaseHandle unInitializedObject;
  ShaderEffect effect4 = ShaderEffect::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!effect4);

  ShaderEffect effect5 = DownCast< ShaderEffect >(unInitializedObject);
  DALI_TEST_CHECK(!effect5);
  END_TEST;
}

int UtcDaliShaderEffectMethodDelete01(void)
{
   TestApplication application;

   // get the default shaders built, this is not required but makes it
   // easier to debug the TET case and isolate the custom shader compilation.
   application.SendNotification();
   application.Render();

   GLuint beforeShaderCompiled = application.GetGlAbstraction().GetLastShaderCompiled();

   // create a new shader effect
   // the vertex and fragment shader will be cached in the ShaderFactory
   ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );

   // destroy the shader effect
   effect.Reset();

   // Create the same shader effect again, this should now use the cached version
   // held in the shader factory
   effect= ShaderEffect::New( VertexSource, FragmentSource );

   // Compile the shader effect
   application.SendNotification();
   application.Render();

   GLuint lastShaderCompiled = application.GetGlAbstraction().GetLastShaderCompiled();
   // no shaders were compiled as they are now compiled on demand and this shader was not used
   DALI_TEST_EQUALS( beforeShaderCompiled, lastShaderCompiled, TEST_LOCATION );

   END_TEST;
}

int UtcDaliShaderEffectMethodSetUniformFloat(void)
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  effect.SetUniform( "uFloat", 1.0f );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uFloat", 1.0f ) );
  END_TEST;
}

int UtcDaliShaderEffectMethodSetUniformVector2(void)
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  effect.SetUniform( "uVec2", Vector2( 2.0f, 3.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec2", Vector2( 2.0f, 3.0f ) ) );
  END_TEST;
}

int UtcDaliShaderEffectMethodSetUniformVector3(void)
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  effect.SetUniform( "uVec3", Vector3( 4.0f, 5.0f, 6.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 4.0f, 5.0f, 6.0f ) ) );
  END_TEST;
}

int UtcDaliShaderEffectMethodSetUniformVector4(void)
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  effect.SetUniform( "uVec4", Vector4( 7.0f, 8.0f, 9.0f, 10.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec4", Vector4( 7.0f, 8.0f, 9.0f, 10.0f ) ) );
  END_TEST;
}

int UtcDaliShaderEffectMethodSetUniformMatrix(void)
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  effect.SetUniform( "uModelView", Matrix::IDENTITY );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uModelView", Matrix::IDENTITY ) );
  END_TEST;
}

int UtcDaliShaderEffectMethodSetUniformMatrix3(void)
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  Matrix3 matIdentity(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
  effect.SetUniform( "uMatrix3", matIdentity );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue("uMatrix3", matIdentity) );
  END_TEST;
}

int UtcDaliShaderEffectMethodSetUniformViewport(void)
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  effect.SetUniform( "uVec2", Vector2( 0.0f, 0.0f ), ShaderEffect::COORDINATE_TYPE_VIEWPORT_POSITION );
  effect.SetUniform( "uVec2Dir", Vector2( 1.0f, 2.0f ), ShaderEffect::COORDINATE_TYPE_VIEWPORT_DIRECTION );

  application.SendNotification();
  application.Render();

  const Vector2& stageSize(Stage::GetCurrent().GetSize());

  DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue( "uVec2", Vector2( stageSize.x/2, -stageSize.y/2 ) ) );

  DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue( "uVec2Dir", Vector2( -1.0f, 2.0f ) ) );

  // change coordinate types
  effect.SetUniform( "uVec2", Vector2( 0.1f, 0.2f ), ShaderEffect::COORDINATE_TYPE_DEFAULT );
  effect.SetUniform( "uVec2Dir", Vector2( 1.0f, 2.0f ), ShaderEffect::COORDINATE_TYPE_VIEWPORT_POSITION );
  actor.SetPixelArea( ImageActor::PixelArea( 0, 0, 10, 10 ) );

  application.SendNotification();
  application.Render();

  Vector2 outValue;
  application.GetGlAbstraction().GetUniformValue( "uVec2", outValue );
  DALI_TEST_EQUALS( outValue, Vector2( 0.1f, 0.2f ), TEST_LOCATION );

  application.GetGlAbstraction().GetUniformValue( "uVec2Dir", outValue );
  DALI_TEST_EQUALS( outValue, Vector2( stageSize.x *.5f - 1.f, -stageSize.y * .5f + 2.f), TEST_LOCATION );

  END_TEST;
}

int UtcDaliShaderEffectMethodSetEffectImage(void)
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  effect.SetEffectImage(image);

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();

  GLuint programId, uniformId;
  bool uniformWasSet = application.GetGlAbstraction().GetUniformIds( "sEffect", programId, uniformId );
  // we dont care about the value of the sampler uniform as thats internal to DALi core and subject to change
  DALI_TEST_CHECK( uniformWasSet );
  END_TEST;
}

int UtcDaliShaderEffectMethodSetEffectImageAndDelete(void)
{
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );

  BufferImage effectImage = CreateBufferImage();
  effect.SetEffectImage(effectImage);

  ImageActor actor = ImageActor::New();

  actor.SetShaderEffect(effect);
  effect.Reset();

  Stage::GetCurrent().Add(actor);

  // do an update / render cycle
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  printf("removing image actor from stage and Reseting handle\n");
  Stage::GetCurrent().Remove(actor);
  actor.Reset();

  tet_printf("### Update & Render  \n");

  application.SendNotification();
  application.Render(16);

  tet_printf("#### Update Only  \n");

  tet_printf("effectImage.Reset \n");

  // this releases the effect texture resource,
  // Update will send a DispatchDiscardTexture message to render
  effectImage.Reset();
  application.SendNotification();
  application.UpdateOnly(16);

  tet_printf("#### Update Only \n");

  // at this point shader is deleted, during clear discard queue
  // and it sends a Shader:: DispatchRemoveObserver message to render thread
  application.UpdateOnly(16);

  tet_printf("#### Render Only  \n");
  // This is where it used to crash, there is a message in the queue to perform DispatchDiscardTexture
  // which tries to call observer->TextureDiscarded, where observer == shader that was deleted
  // in previous update.
  application.RenderOnly();


  // process the discard texture message
  application.RenderOnly();
  application.SendNotification();
  application.Render(16);

  tet_result(TET_PASS);

  END_TEST;
}

int UtcDaliShaderEffectMethodApplyConstraint(void)
{
  // Test whether Shader's uniform can be constrained to a stationary constraint.
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  effect.SetUniform( "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  Property::Index uVecProperty = effect.GetPropertyIndex("uVec3");

  application.SendNotification();
  application.Render();

  // Test effects of SetUniform...
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) ) );

  Constraint constraint = Constraint::New<Vector3>( effect, uVecProperty, TestConstraintToVector3(Vector3(4.0f, 9.0f, 16.0f)) );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  // Test effects of Constraint.
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 4.0f, 9.0f, 16.0f ) ) );
  END_TEST;
}


int UtcDaliShaderEffectMethodApplyConstraintFromActor(void)
{
  // Test whether Shader's uniform can be constrained to Actor's position.
  TestApplication application;

  const Vector3 targetPosition = Vector3( 100.0f, 70.0f, 20.0f);

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  effect.SetUniform( "uVec3", Vector3( 50.0f, 25.0f, 0.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetPosition(targetPosition);
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  Property::Index uVecProperty = effect.GetPropertyIndex("uVec3");

  Constraint constraint = Constraint::New<Vector3>( effect, uVecProperty, TestConstraintFromPositionToVector3() );
  constraint.AddSource( Source( actor, Actor::Property::POSITION ) );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  // Test effects of Constraint.
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", targetPosition ) );
  END_TEST;
}

int UtcDaliShaderEffectMethodApplyConstraintFromActor2(void)
{
  // Test whether Shader's uniform can be constrained to Actor's position.
  // While Actor's position is constrained to another point * 2.0f
  TestApplication application;

  const Vector3 targetPosition = Vector3( 25.0f, 36.0f, 49.0f );

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  effect.SetUniform( "uVec3", Vector3( 50.0f, 25.0f, 0.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetPosition(Vector3( 100.0f, 70.0f, 20.0f));
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  Property::Index uVecProperty = effect.GetPropertyIndex("uVec3");

  Constraint shaderConstraint = Constraint::New<Vector3>( effect, uVecProperty, TestConstraintFromPositionToVector3() );
  shaderConstraint.AddSource( Source(actor, Actor::Property::POSITION) );
  shaderConstraint.Apply();

  Constraint actorConstraint = Constraint::New<Vector3>( actor, Actor::Property::POSITION, TestConstraintToVector3Double(targetPosition) );
  actorConstraint.Apply();

  application.SendNotification();
  application.Render();

  // Test effects of Constraint.
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", targetPosition * 2.0f ) );
  END_TEST;
}

int UtcDaliShaderEffectMethodRemoveConstraints(void)
{
  // Test if constrains can be removed before they are ever applyed.
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  effect.SetUniform( "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  Property::Index uVecProperty = effect.GetPropertyIndex("uVec3");

  application.SendNotification();
  application.Render();

  // Test effects of SetUniform...
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) ) );

  Constraint constraint = Constraint::New<Vector3>( effect, uVecProperty, TestConstraintToVector3(Vector3(4.0f, 9.0f, 16.0f)) );
  constraint.Apply();

  // Remove the constraints
  effect.RemoveConstraints();

  application.SendNotification();
  application.Render();

  // Test effects of Constraint.
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) ) );
  END_TEST;
}

int UtcDaliShaderEffectMethodRemoveConstraints2(void)
{
  // Test whether Shader's uniform constrains can be removed after they are applyed.
  TestApplication application;

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  effect.SetUniform( "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) );

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  Property::Index uVecProperty = effect.GetPropertyIndex("uVec3");

  application.SendNotification();
  application.Render();

  // Test effects of SetUniform...
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) ) );

  Constraint constraint = Constraint::New<Vector3>( effect, uVecProperty, TestConstraintToVector3(Vector3(4.0f, 9.0f, 16.0f)) );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  // Reset the value and remove the constraints
  effect.SetUniform( "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) );
  effect.RemoveConstraints();

  application.SendNotification();
  application.Render();

  // Test effects of Constraint.
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          "uVec3", Vector3( 1.0f, 2.0f, 3.0f ) ) );
  END_TEST;
}

int UtcDaliShaderEffectPropertyIndices(void)
{
  TestApplication application;
  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );

  Property::IndexContainer indices;
  effect.GetPropertyIndices( indices );
  DALI_TEST_CHECK( ! indices.empty() );
  DALI_TEST_EQUALS( indices.size(), effect.GetPropertyCount(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliShaderBinaries(void)
{
  TestApplication application;
  // these will not affect the "first round" of dali render as core is already initialized and it has queried the defaults
  application.GetGlAbstraction().SetBinaryFormats( 1 );
  application.GetGlAbstraction().SetNumBinaryFormats( 1 );
  application.GetGlAbstraction().SetProgramBinaryLength( 1 );

  GLuint lastShaderCompiledBefore = application.GetGlAbstraction().GetLastShaderCompiled();

  ShaderEffect effect = ShaderEffect::New( VertexSource, FragmentSource );
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();
  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);
  // binary was not requested by DALi
  DALI_TEST_CHECK( !(application.GetGlAbstraction().GetProgramBinaryCalled()) );

  GLuint lastShaderCompiledAfter = application.GetGlAbstraction().GetLastShaderCompiled();

  // check that the shader was compiled
  DALI_TEST_EQUALS( lastShaderCompiledAfter, lastShaderCompiledBefore + 2, TEST_LOCATION );

  // simulate context loss to get core to re-initialize its GL
  application.GetCore().ContextDestroyed();
  application.GetCore().ContextCreated();

  application.SendNotification();
  application.Render(16);

  // shader is recompiled
  GLuint finalShaderCompiled = application.GetGlAbstraction().GetLastShaderCompiled();
  // check that the shader was compiled
  DALI_TEST_EQUALS( lastShaderCompiledAfter + 2, finalShaderCompiled, TEST_LOCATION );

  // binary was requested by DALi
  DALI_TEST_CHECK( application.GetGlAbstraction().GetProgramBinaryCalled() );

  END_TEST;
}

int UtcDaliShaderEffectFromProperties01(void)
{
  TestApplication application;
  tet_infoline("UtcDaliShaderEffectFromProperties01()");

  std::string fragmentShaderPrefix = "#define TEST_FS 1\n#extension GL_OES_standard_derivatives : enable";
  std::string vertexShaderPrefix = "#define TEST_VS 1";
  std::string vertexShader(VertexSource);
  std::string fragmentShader(FragmentSource);

  // Call render to compile default shaders.
  application.SendNotification();
  application.Render();

  GLuint lastShaderCompiledBefore = application.GetGlAbstraction().GetLastShaderCompiled();

  // create from type registry

  TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( "ShaderEffect" );
  DALI_TEST_CHECK( typeInfo );
  ShaderEffect effect = ShaderEffect::DownCast( typeInfo.CreateInstance() );
  DALI_TEST_CHECK( effect );

  Property::Value programMap = Property::Value(Property::MAP);

  programMap.SetValue("vertex", vertexShader);
  programMap.SetValue("fragment", fragmentShader);

  programMap.SetValue("vertex-prefix", vertexShaderPrefix);
  programMap.SetValue("fragment-prefix", fragmentShaderPrefix);

  programMap.SetValue("geometry-type", "GEOMETRY_TYPE_IMAGE");

  effect.SetProperty(effect.GetPropertyIndex("program"), programMap);

  Property::Value imageMap = Property::Value(Property::MAP);
  imageMap.SetValue("filename", Property::Value(TestImageFilename));
  effect.SetProperty(effect.GetPropertyIndex("image"), imageMap);

  // do a update & render to get the image request
  application.SendNotification();
  application.Render();

  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  // create the image
  Integration::Bitmap* bitmap = CreateBitmap( 10, 10, 0xFF );
  Integration::ResourcePointer resourcePtr(bitmap);
  TestPlatformAbstraction& platform = application.GetPlatform();
  platform.SetResourceLoaded(request->GetId(), request->GetType()->id, resourcePtr);

  BufferImage image(CreateBufferImage());
  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("TestImageFilenameActor");
  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();
  GLuint lastShaderCompiledAfter = application.GetGlAbstraction().GetLastShaderCompiled();

  // we should have compiled 2 shaders.
  DALI_TEST_EQUALS(lastShaderCompiledAfter, lastShaderCompiledBefore + 2, TEST_LOCATION );

  std::string actualVertexShader = application.GetGlAbstraction().GetShaderSource( lastShaderCompiledBefore + 1 );
  DALI_TEST_EQUALS( vertexShaderPrefix, actualVertexShader.substr( 0, vertexShaderPrefix.length() ), TEST_LOCATION );
  DALI_TEST_EQUALS( vertexShader, actualVertexShader.substr( actualVertexShader.length() - vertexShader.length() ), TEST_LOCATION );

  std::string actualFragmentShader = application.GetGlAbstraction().GetShaderSource( lastShaderCompiledBefore + 2 );
  DALI_TEST_EQUALS( fragmentShaderPrefix, actualFragmentShader.substr( 0, fragmentShaderPrefix.length() ), TEST_LOCATION );
  DALI_TEST_EQUALS( fragmentShader, actualFragmentShader.substr( actualFragmentShader.length() - fragmentShader.length() ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliShaderEffectFromProperties02(void)
{
  try
  {
    TestApplication application;
    tet_infoline("UtcDaliShaderEffectFromProperties02()");

    // Call render to compile default shaders.
    application.SendNotification();
    application.Render();
    application.Render();
    application.Render();

    // create from type registry (currently only way to get ShaderEffect with no shader setup in constructor
    TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( "ShaderEffect" );
    DALI_TEST_CHECK( typeInfo );
    ShaderEffect effect = ShaderEffect::DownCast( typeInfo.CreateInstance() );
    DALI_TEST_CHECK( effect );

    Property::Value programMap = Property::Value(Property::MAP);

    programMap.SetValue("vertex",   std::string(VertexSource));
    programMap.SetValue("fragment", std::string(FragmentSource));

    // programMap.SetValue("geometry-type", "GEOMETRY_TYPE_IMAGE");
    // dont set by value
    programMap.SetValue("geometry-type", GeometryType( GEOMETRY_TYPE_IMAGE ));

    effect.SetProperty(effect.GetPropertyIndex("program"), programMap);

    tet_result( TET_FAIL );
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
  }
  END_TEST;
}

int UtcDaliShaderEffectFromProperties03(void)
{
  try
  {
    TestApplication application;
    tet_infoline("UtcDaliShaderEffectFromProperties03()");

    // Call render to compile default shaders.
    application.SendNotification();
    application.Render();
    application.Render();
    application.Render();

    // create from type registry (currently only way to get ShaderEffect with no shader setup in constructor
    TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( "ShaderEffect" );
    DALI_TEST_CHECK( typeInfo );
    ShaderEffect effect = ShaderEffect::DownCast( typeInfo.CreateInstance() );
    DALI_TEST_CHECK( effect );

    // dont set unknown
    effect.SetProperty( effect.GetPropertyIndex("geometry-hints"), "HINT_2" );

    tet_result( TET_FAIL );
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
  }
  END_TEST;
}
