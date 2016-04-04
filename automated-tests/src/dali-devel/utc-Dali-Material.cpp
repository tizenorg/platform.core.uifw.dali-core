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

#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

#include <mesh-builder.h>

namespace
{
void TestConstraintNoBlue( Vector4& current, const PropertyInputContainer& inputs )
{
  current.b = 0.0f;
}
}


void material_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void material_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliMaterialNew01(void)
{
  TestApplication application;

  Shader shader = Shader::New("vertexSrc", "fragmentSrc");
  Material material = Material::New(shader);

  DALI_TEST_CHECK( material );
  END_TEST;
}

int UtcDaliMaterialNew02(void)
{
  TestApplication application;
  Material material;
  DALI_TEST_CHECK( !material );
  END_TEST;
}

int UtcDaliMaterialCopyConstructor(void)
{
  TestApplication application;

  Shader shader = Shader::New("vertexSrc", "fragmentSrc");
  Image image = BufferImage::New(32, 32, Pixel::RGBA8888);
  Material material = Material::New(shader);
  material.AddTexture( image, "sTexture" );

  Material materialCopy(material);

  DALI_TEST_CHECK( materialCopy );

  END_TEST;
}

int UtcDaliMaterialAssignmentOperator(void)
{
  TestApplication application;

  Shader shader = Shader::New("vertexSrc", "fragmentSrc");
  Image image = BufferImage::New(32, 32, Pixel::RGBA8888);
  Material material = Material::New(shader);

  Material material2;
  DALI_TEST_CHECK( !material2 );

  material2 = material;
  DALI_TEST_CHECK( material2 );

  END_TEST;
}

int UtcDaliMaterialDownCast01(void)
{
  TestApplication application;
  Shader shader = Shader::New("vertexSrc", "fragmentSrc");
  Material material = Material::New(shader);

  BaseHandle handle(material);
  Material material2 = Material::DownCast(handle);
  DALI_TEST_CHECK( material2 );

  END_TEST;
}

int UtcDaliMaterialDownCast02(void)
{
  TestApplication application;

  Handle handle = Handle::New(); // Create a custom object
  Material material = Material::DownCast(handle);
  DALI_TEST_CHECK( !material );
  END_TEST;
}

int UtcDaliMaterialSetShader(void)
{
  TestApplication application;

  tet_infoline("Test SetShader(shader) ");

  Shader shader1 = Shader::New( "vertexSrc1", "fragmentSrc1" );
  shader1.RegisterProperty( "uFadeColor", Color::CYAN );

  Shader shader2 = Shader::New( "vertexSrc1", "fragmentSrc1" );
  shader2.RegisterProperty( "uFadeColor", Color::MAGENTA );

  // shader1
  Material material = Material::New(shader1);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render(0);
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::CYAN, TEST_LOCATION );

  // shader2
  material.SetShader( shader2 );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::MAGENTA, TEST_LOCATION );

  // shader1
  material.SetShader( shader1 );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::CYAN, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMaterialGetShader(void)
{
  TestApplication application;

  tet_infoline("Test GetShader() ");

  Shader shader1 = Shader::New( "vertexSrc1", "fragmentSrc1" );
  Shader shader2 = Shader::New( "vertexSrc1", "fragmentSrc1" );

  // shader1
  Material material = Material::New(shader1);
  DALI_TEST_EQUALS( shader1, material.GetShader(), TEST_LOCATION );

  // shader2
  material.SetShader( shader2 );
  DALI_TEST_EQUALS( shader2, material.GetShader(), TEST_LOCATION );

  // shader1
  material.SetShader( shader1 );
  DALI_TEST_EQUALS( shader1, material.GetShader(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliMaterialGetNumberOfTextures(void)
{
  TestApplication application;

  tet_infoline("Test GetNumberOfTextures()");

  Image image = BufferImage::New(32, 32, Pixel::RGBA8888);
  Material material = CreateMaterial();

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add( actor );

  material.AddTexture( image, "sTexture0" );
  material.AddTexture( image, "sTexture1" );
  DALI_TEST_EQUALS( material.GetNumberOfTextures(), 2u, TEST_LOCATION );

  material.AddTexture( image, "sTexture2" );
  material.AddTexture( image, "sTexture3" );
  material.AddTexture( image, "sTexture4" );
  DALI_TEST_EQUALS( material.GetNumberOfTextures(), 5u, TEST_LOCATION );

  material.RemoveTexture(3);
  DALI_TEST_EQUALS( material.GetNumberOfTextures(), 4u, TEST_LOCATION );

  material.RemoveTexture(3);
  material.RemoveTexture(0);
  DALI_TEST_EQUALS( material.GetNumberOfTextures(), 2u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMaterialConstraint(void)
{
  TestApplication application;

  tet_infoline("Test that a custom material property can be constrained");

  Shader shader = Shader::New( "VertexSource", "FragmentSource");
  Material material = Material::New( shader );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = material.RegisterProperty( "uFadeColor", initialColor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( material.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION );

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>( material, colorIndex, TestConstraintNoBlue );
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

  // Expect no blue component in either buffer - yellow
  DALI_TEST_EQUALS( material.GetProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( material.GetProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION );

  material.RemoveConstraints();
  material.SetProperty(colorIndex, Color::WHITE );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( material.GetProperty<Vector4>(colorIndex), Color::WHITE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMaterialConstraint02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map material property can be constrained");

  Shader shader = Shader::New( "VertexSource", "FragmentSource");
  Material material = Material::New( shader );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = material.RegisterProperty( "uFadeColor", initialColor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, initialColor, TEST_LOCATION );

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>( material, colorIndex, TestConstraintNoBlue );
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

   // Expect no blue component in either buffer - yellow
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  application.Render(0);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  material.RemoveConstraints();
  material.SetProperty(colorIndex, Color::WHITE );
  application.SendNotification();
  application.Render(0);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::WHITE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMaterialAnimatedProperty01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform material property can be animated");

  Shader shader = Shader::New( "VertexSource", "FragmentSource");
  Material material = Material::New( shader );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = material.RegisterProperty( "uFadeColor", initialColor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( material.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION );

  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, initialColor);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( material, colorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS( material.GetProperty<Vector4>(colorIndex), Color::WHITE * 0.5f, TEST_LOCATION );

  application.Render(500);

  DALI_TEST_EQUALS( material.GetProperty<Vector4>(colorIndex), Color::TRANSPARENT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMaterialAnimatedProperty02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map material property can be animated");

  Shader shader = Shader::New( "VertexSource", "FragmentSource");
  Material material = Material::New( shader );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = material.RegisterProperty( "uFadeColor", initialColor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, initialColor, TEST_LOCATION );

  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, initialColor);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( material, colorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::WHITE * 0.5f, TEST_LOCATION );

  application.Render(500);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::TRANSPARENT, TEST_LOCATION );

  END_TEST;
}


int UtcDaliMaterialSetTextureUniformName01(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Material material = CreateMaterial();
  material.AddTexture( image, "sTexture" );

  int textureIndex = material.GetTextureIndex( "sTexture" );
  DALI_TEST_EQUALS( textureIndex, 0, TEST_LOCATION );

  material.SetTextureUniformName( 0, "sEffectTexture" );
  textureIndex = material.GetTextureIndex( "sEffectTexture" );
  DALI_TEST_EQUALS( textureIndex, 0, TEST_LOCATION );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetSize(400, 400);

  Stage::GetCurrent().Add( actor );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  application.SendNotification();
  application.Render();

  // Test that the relevant texture unit was bound
  DALI_TEST_EQUALS( textureTrace.FindMethodAndParams("BindTexture", "3553, 1"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethodAndParams("BindTexture", "3553, 2"), false, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMaterialSetTextureUniformName02(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Image image2 = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Material material = CreateMaterial();
  material.AddTexture( image, "sTexture");
  material.SetTextureUniformName( 0, "sEffectTexture" );
  material.AddTexture( image2, "sTexture2");

  int textureIndex = material.GetTextureIndex( "sEffectTexture" );
  DALI_TEST_EQUALS( textureIndex, 0, TEST_LOCATION );

  textureIndex = material.GetTextureIndex( "sTexture2" );
  DALI_TEST_EQUALS( textureIndex, 1, TEST_LOCATION );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetSize(400, 400);

  Stage::GetCurrent().Add( actor );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  application.SendNotification();
  application.Render();

  // Test that the relevant texture unit was bound
  DALI_TEST_EQUALS( textureTrace.FindMethodAndParams("BindTexture", "3553, 1"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethodAndParams("BindTexture", "3553, 2"), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMaterialAddTexture01(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Material material = CreateMaterial();
  material.AddTexture( image, "sTexture");

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetSize(400, 400);

  Stage::GetCurrent().Add( actor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );
  application.SendNotification();
  application.Render();

  int textureUnit=-1;
  DALI_TEST_CHECK( gl.GetUniformValue<int>( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  texParameterTrace.Enable( false );

  // Verify gl state
  // There are three calls to TexParameteri when the texture is first created
  // as the texture is using default sampling parametrers there shouldn't be any more calls to TexParameteri
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 3, TEST_LOCATION);

  END_TEST;
}

int UtcDaliMaterialAddTexture02(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Material material = CreateMaterial();

  Sampler sampler = Sampler::New();
  sampler.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );
  material.AddTexture( image, "sTexture", sampler );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetSize(400, 400);

  Stage::GetCurrent().Add( actor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );
  application.SendNotification();
  application.Render();

  int textureUnit=-1;
  DALI_TEST_CHECK( gl.GetUniformValue<int>( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  texParameterTrace.Enable( false );

  // Verify gl state
  // There are three calls to TexParameteri when the texture is first created
  // Texture minification and magnification filters are now different than default so
  //there should have been two extra TexParameteri calls to set the new filter mode
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 4, TEST_LOCATION);

  END_TEST;
}

int UtcDaliMaterialRemoveTexture(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Material material = CreateMaterial();
  material.RemoveTexture(0);
  DALI_TEST_EQUALS( material.GetNumberOfTextures(), 0u, TEST_LOCATION );

  material.RemoveTexture(1);
  DALI_TEST_EQUALS( material.GetNumberOfTextures(), 0u, TEST_LOCATION );

  Sampler sampler = Sampler::New();
  sampler.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );
  material.AddTexture( image, "sTexture", sampler );
  DALI_TEST_EQUALS( material.GetNumberOfTextures(), 1u, TEST_LOCATION );

  material.RemoveTexture(1);
  DALI_TEST_EQUALS( material.GetNumberOfTextures(), 1u, TEST_LOCATION );

  material.RemoveTexture(0);
  DALI_TEST_EQUALS( material.GetNumberOfTextures(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMaterialSetSampler(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Material material = CreateMaterial();
  material.AddTexture( image, "sTexture");

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetSize(400, 400);

  Stage::GetCurrent().Add( actor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );
  application.SendNotification();
  application.Render();

  int textureUnit=-1;
  DALI_TEST_CHECK( gl.GetUniformValue<int>( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  texParameterTrace.Enable( false );

  // Verify gl state
  // There are three calls to TexParameteri when the texture is first created
  // as the texture is using default sampling parametrers there shouldn't be any more calls to TexParameteri
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 3, TEST_LOCATION);

  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  Sampler sampler = Sampler::New();
  sampler.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );
  material.SetTextureSampler(0, sampler );


  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify gl state
  //There should have been two calls to TexParameteri to set the new filtering mode
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 2, TEST_LOCATION);


  END_TEST;
}

int UtcDaliMaterialGetTextureIndex(void)
{
  TestApplication application;

  Image image0 = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Image image1 = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Image image2 = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Image image3 = BufferImage::New( 64, 64, Pixel::RGBA8888 );


  Material material = CreateMaterial();
  material.AddTexture( image0, "sTexture0");
  material.AddTexture( image1, "sTexture1");
  material.AddTexture( image2, "sTexture2");
  material.AddTexture( image3, "sTexture3");

  int textureIndex = material.GetTextureIndex( "sTexture0" );
  DALI_TEST_EQUALS( textureIndex, 0, TEST_LOCATION );

  textureIndex = material.GetTextureIndex( "sTexture1" );
  DALI_TEST_EQUALS( textureIndex, 1, TEST_LOCATION );

  textureIndex = material.GetTextureIndex( "sTexture2" );
  DALI_TEST_EQUALS( textureIndex, 2, TEST_LOCATION );

  textureIndex = material.GetTextureIndex( "sTexture3" );
  DALI_TEST_EQUALS( textureIndex, 3, TEST_LOCATION );

  material.RemoveTexture(1);

  textureIndex = material.GetTextureIndex( "sTexture0" );
  DALI_TEST_EQUALS( textureIndex, 0, TEST_LOCATION );

  textureIndex = material.GetTextureIndex( "sTexture2" );
  DALI_TEST_EQUALS( textureIndex, 1, TEST_LOCATION );

  textureIndex = material.GetTextureIndex( "sTexture3" );
  DALI_TEST_EQUALS( textureIndex, 2, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMaterialGetTextureP(void)
{
  TestApplication application;

  Image image0 = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Image image1 = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Image image2 = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Image image3 = BufferImage::New( 64, 64, Pixel::RGBA8888 );


  Material material = CreateMaterial();
  material.AddTexture( image0, "sTexture0");
  material.AddTexture( image1, "sTexture1");
  material.AddTexture( image2, "sTexture2");
  material.AddTexture( image3, "sTexture3");

  Image textureImage0 = material.GetTexture( "sTexture0" );
  DALI_TEST_EQUALS( textureImage0, image0, TEST_LOCATION );

  Image textureImage1 = material.GetTexture( "sTexture1" );
  DALI_TEST_EQUALS( textureImage1, image1, TEST_LOCATION );

  Image textureImage2 = material.GetTexture( "sTexture2" );
  DALI_TEST_EQUALS( textureImage2, image2, TEST_LOCATION );

  Image textureImage3 = material.GetTexture( "sTexture3" );
  DALI_TEST_EQUALS( textureImage3, image3, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMaterialGetTextureN(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Material material = CreateMaterial();
  material.AddTexture( image, "sTexture");

  Image textureImage = material.GetTexture( "sTextureTEST" );
  DALI_TEST_CHECK( !textureImage );

  END_TEST;
}
