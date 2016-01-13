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

void renderer_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void renderer_test_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliRendererNew01(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial();
  Renderer renderer = Renderer::New(geometry, material);

  DALI_TEST_EQUALS( (bool)renderer, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRendererNew02(void)
{
  TestApplication application;
  Renderer renderer;
  DALI_TEST_EQUALS( (bool)renderer, false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRendererCopyConstructor(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial();
  Renderer renderer = Renderer::New(geometry, material);

  Renderer rendererCopy( renderer );
  DALI_TEST_EQUALS( (bool)rendererCopy, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererAssignmentOperator(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial();
  Renderer renderer = Renderer::New(geometry, material);

  Renderer renderer2;
  DALI_TEST_EQUALS( (bool)renderer2, false, TEST_LOCATION );

  renderer2 = renderer;
  DALI_TEST_EQUALS( (bool)renderer2, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRendererDownCast01(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial();
  Renderer renderer = Renderer::New(geometry, material);

  BaseHandle handle(renderer);
  Renderer renderer2 = Renderer::DownCast(handle);
  DALI_TEST_EQUALS( (bool)renderer2, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRendererDownCast02(void)
{
  TestApplication application;

  Handle handle = Handle::New(); // Create a custom object
  Renderer renderer = Renderer::DownCast(handle);
  DALI_TEST_EQUALS( (bool)renderer, false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRendererSetGetGeometry(void)
{
  TestApplication application;
  tet_infoline( "Test SetGeometry, GetGeometry" );

  Geometry geometry1 = CreateQuadGeometry();
  geometry1.RegisterProperty( "uFadeColor", Color::RED );

  Geometry geometry2 = CreateQuadGeometry();
  geometry2.RegisterProperty( "uFadeColor", Color::GREEN );

  Material material = CreateMaterial();
  Renderer renderer = Renderer::New(geometry1, material);
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render(0);

  // Expect that the first geometry's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::RED, TEST_LOCATION );

  DALI_TEST_EQUALS( renderer.GetGeometry(), geometry1, TEST_LOCATION );

  // Set geometry2 to the renderer
  renderer.SetGeometry( geometry2 );

  application.SendNotification();
  application.Render(0);

  // Expect that the second geometry's fade color property is accessed
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  DALI_TEST_EQUALS( renderer.GetGeometry(), geometry2, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererSetGetMaterial(void)
{
  TestApplication application;
  tet_infoline( "Test SetMaterial, GetMaterial" );

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableCullFaceCallTrace(true);

  Material material1 = CreateMaterial();
  material1.RegisterProperty( "uFadeColor", Color::RED );

  Material material2 = CreateMaterial();
  material2.RegisterProperty( "uFadeColor", Color::GREEN );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, material1);
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render(0);

  // Expect that the first material's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::RED, TEST_LOCATION );

  DALI_TEST_EQUALS( renderer.GetMaterial(), material1, TEST_LOCATION );

  // set the second material to the renderer
  renderer.SetMaterial( material2 );

  application.SendNotification();
  application.Render(0);

  // Expect that the second material's fade color property is accessed
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  DALI_TEST_EQUALS( renderer.GetMaterial(), material2, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererSetGetDepthIndex(void)
{
  TestApplication application;

  tet_infoline("Test SetDepthIndex, GetDepthIndex");

  Material material = CreateMaterial();
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, material);
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetDepthIndex(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS( renderer.GetProperty<int>(Renderer::Property::DEPTH_INDEX), 0, TEST_LOCATION );

  renderer.SetDepthIndex(1);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetDepthIndex(), 1, TEST_LOCATION );
  DALI_TEST_EQUALS( renderer.GetProperty<int>(Renderer::Property::DEPTH_INDEX), 1, TEST_LOCATION );

  renderer.SetDepthIndex(10);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetDepthIndex(), 10, TEST_LOCATION );
  DALI_TEST_EQUALS( renderer.GetProperty<int>(Renderer::Property::DEPTH_INDEX), 10, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererConstraint01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform renderer property can be constrained");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = renderer.RegisterProperty( "uFadeColor", initialColor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION );

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>( renderer, colorIndex, TestConstraintNoBlue );
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

  // Expect no blue component in either buffer - yellow
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION );

  renderer.RemoveConstraints();
  renderer.SetProperty(colorIndex, Color::WHITE );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(colorIndex), Color::WHITE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererConstraint02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map renderer property can be constrained");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
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
  Property::Index colorIndex = renderer.RegisterProperty( "uFadeColor", initialColor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, initialColor, TEST_LOCATION );

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>( renderer, colorIndex, TestConstraintNoBlue );
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

   // Expect no blue component in either buffer - yellow
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  application.Render(0);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  renderer.RemoveConstraints();
  renderer.SetProperty(colorIndex, Color::WHITE );
  application.SendNotification();
  application.Render(0);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::WHITE, TEST_LOCATION );

  END_TEST;
}



int UtcDaliRendererAnimatedProperty01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform renderer property can be animated");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = renderer.RegisterProperty( "uFadeColor", initialColor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION );

  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, initialColor);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( renderer, colorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(colorIndex), Color::WHITE * 0.5f, TEST_LOCATION );

  application.Render(500);

  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(colorIndex), Color::TRANSPARENT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererAnimatedProperty02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map renderer property can be animated");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
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
  Property::Index colorIndex = renderer.RegisterProperty( "uFadeColor", initialColor );

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
  animation.AnimateBetween( Property( renderer, colorIndex ), keyFrames );
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




int UtcDaliRendererUniformMapPrecendence01(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.AddTexture( image, "sTexture" );

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  renderer.RegisterProperty( "uFadeColor", Color::RED );

  actor.RegisterProperty( "uFadeColor", Color::GREEN );

  Property::Index materialFadeColorIndex = material.RegisterProperty( "uFadeColor", Color::BLUE );

  shader.RegisterProperty( "uFadeColor", Color::MAGENTA );

  geometry.RegisterProperty( "uFadeColor", Color::YELLOW );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the actor's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  // Animate material's fade color property. Should be no change to uniform
  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( material, materialFadeColorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  application.Render(500);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererUniformMapPrecendence02(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.AddTexture( image, "sTexture" );

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Don't add property / uniform map to renderer

  actor.RegisterProperty( "uFadeColor", Color::GREEN );

  Property::Index materialFadeColorIndex = material.RegisterProperty( "uFadeColor", Color::BLUE );

  shader.RegisterProperty( "uFadeColor", Color::MAGENTA );

  geometry.RegisterProperty( "uFadeColor", Color::YELLOW );


  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the actor's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  // Animate material's fade color property. Should be no change to uniform
  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( material, materialFadeColorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  application.Render(500);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  END_TEST;
}


int UtcDaliRendererUniformMapPrecendence03(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.AddTexture( image, "sTexture" );

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Don't add property / uniform map to renderer or actor

  material.RegisterProperty( "uFadeColor", Color::BLUE );

  shader.RegisterProperty( "uFadeColor", Color::MAGENTA );

  Property::Index geometryFadeColorIndex = geometry.RegisterProperty( "uFadeColor", Color::YELLOW );

  geometry.RegisterProperty( "uFadeColor", Color::BLACK );


  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the material's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::BLUE, TEST_LOCATION );

  // Animate geometry's fade color property. Should be no change to uniform
  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( geometry, geometryFadeColorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::BLUE, TEST_LOCATION );

  application.Render(500);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::BLUE, TEST_LOCATION );

  END_TEST;
}


int UtcDaliRendererUniformMapPrecendence04(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.AddTexture( image, "sTexture" );

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Don't add property / uniform map to renderer/actor/material
  shader.RegisterProperty( "uFadeColor", Color::MAGENTA );

  Property::Index geometryFadeColorIndex = geometry.RegisterProperty( "uFadeColor", Color::YELLOW );

  geometry.RegisterProperty( "uFadeColor", Color::BLACK );


  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the sampler's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::MAGENTA, TEST_LOCATION );

  // Animate geometry's fade color property. Should be no change to uniform
  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( geometry, geometryFadeColorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::MAGENTA, TEST_LOCATION );

  application.Render(500);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::MAGENTA, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererUniformMapPrecendence05(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Don't add property / uniform map to renderer/actor/material/sampler

  shader.RegisterProperty( "uFadeColor", Color::MAGENTA );

  Property::Index geometryFadeColorIndex = geometry.RegisterProperty( "uFadeColor", Color::YELLOW );

  geometry.RegisterProperty( "uFadeColor", Color::BLACK );


  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the shader's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::MAGENTA, TEST_LOCATION );

  // Animate geometry's fade color property. Should be no change to uniform
  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( geometry, geometryFadeColorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::MAGENTA, TEST_LOCATION );

  application.Render(500);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::MAGENTA, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererUniformMapMultipleUniforms01(void)
{
  TestApplication application;

  tet_infoline("Test the uniform maps are collected from all objects (same type)");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  renderer.RegisterProperty( "uUniform1", Color::RED );
  actor.RegisterProperty( "uUniform2", Color::GREEN );
  material.RegisterProperty( "uUniform3", Color::BLUE );
  shader.RegisterProperty( "uUniform4", Color::MAGENTA );
  geometry.RegisterProperty( "uUniform5", Color::YELLOW );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that each of the object's uniforms are set
  Vector4 uniform1Value(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uUniform1", uniform1Value ) );
  DALI_TEST_EQUALS( uniform1Value, Color::RED, TEST_LOCATION );

  Vector4 uniform2Value(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uUniform2", uniform2Value ) );
  DALI_TEST_EQUALS( uniform2Value, Color::GREEN, TEST_LOCATION );

  Vector4 uniform3Value(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uUniform3", uniform3Value ) );
  DALI_TEST_EQUALS( uniform3Value, Color::BLUE, TEST_LOCATION );

  Vector4 uniform5Value(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uUniform4", uniform5Value ) );
  DALI_TEST_EQUALS( uniform5Value, Color::MAGENTA, TEST_LOCATION );

  Vector4 uniform6Value(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uUniform5", uniform6Value ) );
  DALI_TEST_EQUALS( uniform6Value, Color::YELLOW, TEST_LOCATION );

  END_TEST;
}


int UtcDaliRendererUniformMapMultipleUniforms02(void)
{
  TestApplication application;

  tet_infoline("Test the uniform maps are collected from all objects (different types)");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Property::Value value1(Color::RED);
  renderer.RegisterProperty( "uFadeColor", value1 );

  Property::Value value2(1.0f);
  actor.RegisterProperty( "uFadeProgress", value2 );

  Property::Value value3(Vector3(0.5f, 0.5f, 1.0f));
  material.RegisterProperty( "uFadePosition", value3);

  Property::Value value5(Matrix3::IDENTITY);
  shader.RegisterProperty( "uANormalMatrix", value5 );

  Property::Value value6(Matrix::IDENTITY);
  geometry.RegisterProperty( "uAWorldMatrix", value6 );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that each of the object's uniforms are set
  Vector4 uniform1Value(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", uniform1Value ) );
  DALI_TEST_EQUALS( uniform1Value, value1.Get<Vector4>(), TEST_LOCATION );

  float uniform2Value(0.0f);
  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uFadeProgress", uniform2Value ) );
  DALI_TEST_EQUALS( uniform2Value, value2.Get<float>(), TEST_LOCATION );

  Vector3 uniform3Value(Vector3::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector3>( "uFadePosition", uniform3Value ) );
  DALI_TEST_EQUALS( uniform3Value, value3.Get<Vector3>(), TEST_LOCATION );

  Matrix3 uniform5Value;
  DALI_TEST_CHECK( gl.GetUniformValue<Matrix3>( "uANormalMatrix", uniform5Value ) );
  DALI_TEST_EQUALS( uniform5Value, value5.Get<Matrix3>(), TEST_LOCATION );

  Matrix uniform6Value;
  DALI_TEST_CHECK( gl.GetUniformValue<Matrix>( "uAWorldMatrix", uniform6Value ) );
  DALI_TEST_EQUALS( uniform6Value, value6.Get<Matrix>(), TEST_LOCATION );

  END_TEST;
}


int UtcDaliRendererRenderOrder2DLayer(void)
{
  TestApplication application;
  tet_infoline("Test the rendering order in a 2D layer is correct");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);

  Actor actor0 = Actor::New();
  actor0.SetAnchorPoint(AnchorPoint::CENTER);
  actor0.SetParentOrigin(AnchorPoint::CENTER);
  actor0.SetPosition(0.0f,0.0f);
  Image image0 = BufferImage::New( 64, 64, Pixel::RGB888 );
  Material material0 = Material::New( shader );
  material0.AddTexture( image0, "sTexture0" );
  Renderer renderer0 = Renderer::New( geometry, material0 );
  actor0.AddRenderer(renderer0);
  actor0.SetSize(1, 1);
  Stage::GetCurrent().Add(actor0);
  application.SendNotification();
  application.Render(0);

  Actor actor1 = Actor::New();
  actor1.SetAnchorPoint(AnchorPoint::CENTER);
  actor1.SetParentOrigin(AnchorPoint::CENTER);
  actor1.SetPosition(0.0f,0.0f);
  Image image1= BufferImage::New( 64, 64, Pixel::RGB888 );
  Material material1 = Material::New( shader );
  material1.AddTexture( image1, "sTexture1" );
  Renderer renderer1 = Renderer::New( geometry, material1 );
  actor1.AddRenderer(renderer1);
  actor1.SetSize(1, 1);
  Stage::GetCurrent().Add(actor1);
  application.SendNotification();
  application.Render(0);

  Actor actor2 = Actor::New();
  actor2.SetAnchorPoint(AnchorPoint::CENTER);
  actor2.SetParentOrigin(AnchorPoint::CENTER);
  actor2.SetPosition(0.0f,0.0f);
  Image image2= BufferImage::New( 64, 64, Pixel::RGB888 );
  Material material2 = Material::New( shader );
  material2.AddTexture( image2, "sTexture2" );
  Renderer renderer2 = Renderer::New( geometry, material2 );
  actor2.AddRenderer(renderer2);
  actor2.SetSize(1, 1);
  Stage::GetCurrent().Add(actor2);
  application.SendNotification();
  application.Render(0);

  Actor actor3 = Actor::New();
  actor3.SetAnchorPoint(AnchorPoint::CENTER);
  actor3.SetParentOrigin(AnchorPoint::CENTER);
  actor3.SetPosition(0.0f,0.0f);
  Image image3 = BufferImage::New( 64, 64, Pixel::RGB888 );
  Material material3 = Material::New( shader );
  material3.AddTexture( image3, "sTexture3" );
  Renderer renderer3 = Renderer::New( geometry, material3 );
  actor3.AddRenderer(renderer3);
  actor3.SetSize(1, 1);
  Stage::GetCurrent().Add(actor3);
  application.SendNotification();
  application.Render(0);

  /*
   * Create the following hierarchy:
   *
   *            actor2
   *              /
   *             /
   *          actor1
   *           /
   *          /
   *       actor0
   *        /
   *       /
   *    actor3
   *
   *  Expected rendering order : actor2 - actor1 - actor0 - actor3
   */
  actor2.Add(actor1);
  actor1.Add(actor0);
  actor0.Add(actor3);
  application.SendNotification();
  application.Render(0);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);
  application.SendNotification();
  application.Render(0);

  int textureBindIndex[4];
  for( unsigned int i(0); i<4; ++i )
  {
    std::stringstream params;
    params << GL_TEXTURE_2D<<", "<<i+1;
    textureBindIndex[i] = gl.GetTextureTrace().FindIndexFromMethodAndParams("BindTexture", params.str() );
  }

  //Check that actor1 has been rendered after actor2
  DALI_TEST_GREATER( textureBindIndex[1], textureBindIndex[2], TEST_LOCATION );

  //Check that actor0 has been rendered after actor1
  DALI_TEST_GREATER( textureBindIndex[0], textureBindIndex[1], TEST_LOCATION );

  //Check that actor3 has been rendered after actor0
  DALI_TEST_GREATER( textureBindIndex[3], textureBindIndex[0], TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererRenderOrder2DLayerMultipleRenderers(void)
{
  TestApplication application;
  tet_infoline("Test the rendering order in a 2D layer is correct using multiple renderers per actor");

  /*
   * Creates the following hierarchy:
   *
   *             actor0------------------------>actor1
   *            /   |   \                    /   |   \
   *          /     |     \                /     |     \
   *        /       |       \            /       |       \
   * renderer0 renderer1 renderer2 renderer3 renderer4 renderer5
   *
   *  renderer0 has depth index 2
   *  renderer1 has depth index 0
   *  renderer2 has depth index 1
   *
   *  renderer3 has depth index 1
   *  renderer4 has depth index 0
   *  renderer5 has depth index -1
   *
   *  Expected rendering order: renderer1 - renderer2 - renderer0 - renderer5 - renderer4 - renderer3
   */

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);

  Actor actor0 = Actor::New();
  actor0.SetAnchorPoint(AnchorPoint::CENTER);
  actor0.SetParentOrigin(AnchorPoint::CENTER);
  actor0.SetPosition(0.0f,0.0f);
  actor0.SetSize(1, 1);
  Stage::GetCurrent().Add(actor0);

  Actor actor1 = Actor::New();
  actor1.SetAnchorPoint(AnchorPoint::CENTER);
  actor1.SetParentOrigin(AnchorPoint::CENTER);
  actor1.SetPosition(0.0f,0.0f);
  actor1.SetSize(1, 1);
  actor0.Add(actor1);

  //Renderer0
  Image image0 = BufferImage::New( 64, 64, Pixel::RGB888 );
  Material material0 = Material::New( shader );
  material0.AddTexture( image0, "sTexture0" );
  Renderer renderer0 = Renderer::New( geometry, material0 );
  renderer0.SetDepthIndex( 2 );
  actor0.AddRenderer(renderer0);
  application.SendNotification();
  application.Render(0);

  //Renderer1
  Image image1= BufferImage::New( 64, 64, Pixel::RGB888 );
  Material material1 = Material::New( shader );
  material1.AddTexture( image1, "sTexture1" );
  Renderer renderer1 = Renderer::New( geometry, material1 );
  renderer1.SetDepthIndex( 0 );
  actor0.AddRenderer(renderer1);
  application.SendNotification();
  application.Render(0);

  //Renderer2
  Image image2= BufferImage::New( 64, 64, Pixel::RGB888 );
  Material material2 = Material::New( shader );
  material2.AddTexture( image2, "sTexture2" );
  Renderer renderer2 = Renderer::New( geometry, material2 );
  renderer2.SetDepthIndex( 1 );
  actor0.AddRenderer(renderer2);
  application.SendNotification();
  application.Render(0);

  //Renderer3
  Image image3 = BufferImage::New( 64, 64, Pixel::RGB888 );
  Material material3 = Material::New( shader );
  material3.AddTexture( image3, "sTexture3" );
  Renderer renderer3 = Renderer::New( geometry, material3 );
  renderer3.SetDepthIndex( 1 );
  actor1.AddRenderer(renderer3);
  application.SendNotification();
  application.Render(0);

  //Renderer4
  Image image4= BufferImage::New( 64, 64, Pixel::RGB888 );
  Material material4 = Material::New( shader );
  material4.AddTexture( image4, "sTexture4" );
  Renderer renderer4 = Renderer::New( geometry, material4 );
  renderer4.SetDepthIndex( 0 );
  actor1.AddRenderer(renderer4);
  application.SendNotification();
  application.Render(0);

  //Renderer5
  Image image5= BufferImage::New( 64, 64, Pixel::RGB888 );
  Material material5 = Material::New( shader );
  material5.AddTexture( image5, "sTexture5" );
  Renderer renderer5 = Renderer::New( geometry, material5 );
  renderer5.SetDepthIndex( -1 );
  actor1.AddRenderer(renderer5);
  application.SendNotification();
  application.Render(0);


  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);
  application.SendNotification();
  application.Render(0);

  int textureBindIndex[6];
  for( unsigned int i(0); i<6; ++i )
  {
    std::stringstream params;
    params << GL_TEXTURE_2D<<", "<<i+1;
    textureBindIndex[i] = gl.GetTextureTrace().FindIndexFromMethodAndParams("BindTexture", params.str() );
  }

  //Check that renderer3 has been rendered after renderer4
  DALI_TEST_GREATER( textureBindIndex[3], textureBindIndex[4], TEST_LOCATION );

  //Check that renderer0 has been rendered after renderer2
  DALI_TEST_GREATER( textureBindIndex[4], textureBindIndex[5], TEST_LOCATION );

  //Check that renderer0 has been rendered after renderer2
  DALI_TEST_GREATER( textureBindIndex[5], textureBindIndex[0], TEST_LOCATION );

  //Check that renderer0 has been rendered after renderer2
  DALI_TEST_GREATER( textureBindIndex[0], textureBindIndex[2], TEST_LOCATION );

  //Check that renderer2 has been rendered after renderer1
  DALI_TEST_GREATER( textureBindIndex[2], textureBindIndex[1], TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererRenderOrder2DLayerOverlay(void)
{
  TestApplication application;
  tet_infoline("Test the rendering order in a 2D layer is correct for overlays");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);

  Actor actor0 = Actor::New();
  actor0.SetAnchorPoint(AnchorPoint::CENTER);
  actor0.SetParentOrigin(AnchorPoint::CENTER);
  Image image0 = BufferImage::New( 64, 64, Pixel::RGB888 );
  Material material0 = Material::New( shader );
  material0.AddTexture( image0, "sTexture0" );
  Renderer renderer0 = Renderer::New( geometry, material0 );
  actor0.AddRenderer(renderer0);
  actor0.SetPosition(0.0f,0.0f);
  actor0.SetSize(100, 100);
  Stage::GetCurrent().Add(actor0);
  actor0.SetDrawMode( DrawMode::OVERLAY_2D );
  application.SendNotification();
  application.Render(0);

  Actor actor1 = Actor::New();
  actor1.SetAnchorPoint(AnchorPoint::CENTER);
  actor1.SetParentOrigin(AnchorPoint::CENTER);
  Image image1= BufferImage::New( 64, 64, Pixel::RGB888 );
  Material material1 = Material::New( shader );
  material1.AddTexture( image1, "sTexture1" );
  Renderer renderer1 = Renderer::New( geometry, material1 );
  actor1.SetPosition(0.0f,0.0f);
  actor1.AddRenderer(renderer1);
  actor1.SetSize(100, 100);
  Stage::GetCurrent().Add(actor1);
  actor1.SetDrawMode( DrawMode::OVERLAY_2D );
  application.SendNotification();
  application.Render(0);

  Actor actor2 = Actor::New();
  actor2.SetAnchorPoint(AnchorPoint::CENTER);
  actor2.SetParentOrigin(AnchorPoint::CENTER);
  Image image2= BufferImage::New( 64, 64, Pixel::RGB888 );
  Material material2 = Material::New( shader );
  material2.AddTexture( image2, "sTexture2" );
  Renderer renderer2 = Renderer::New( geometry, material2 );
  actor2.AddRenderer(renderer2);
  actor2.SetPosition(0.0f,0.0f);
  actor2.SetSize(100, 100);
  Stage::GetCurrent().Add(actor2);
  application.SendNotification();
  application.Render(0);

  Actor actor3 = Actor::New();
  actor3.SetAnchorPoint(AnchorPoint::CENTER);
  actor3.SetParentOrigin(AnchorPoint::CENTER);
  Image image3 = BufferImage::New( 64, 64, Pixel::RGB888 );
  Material material3 = Material::New( shader );
  material3.AddTexture( image3, "sTexture3" );
  Renderer renderer3 = Renderer::New( geometry, material3 );
  actor3.SetPosition(0.0f,0.0f);
  actor3.AddRenderer(renderer3);
  actor3.SetSize(100, 100);
  Stage::GetCurrent().Add(actor3);
  actor3.SetDrawMode( DrawMode::OVERLAY_2D );
  application.SendNotification();
  application.Render(0);

  Actor actor4 = Actor::New();
  actor4.SetAnchorPoint(AnchorPoint::CENTER);
  actor4.SetParentOrigin(AnchorPoint::CENTER);
  Image image4 = BufferImage::New( 64, 64, Pixel::RGB888 );
  Material material4 = Material::New( shader );
  material4.AddTexture( image4, "sTexture4" );
  Renderer renderer4 = Renderer::New( geometry, material4 );
  actor4.AddRenderer(renderer4);
  actor4.SetPosition(0.0f,0.0f);
  actor4.SetSize(100, 100);
  Stage::GetCurrent().Add(actor4);
  application.SendNotification();
  application.Render(0);

  /*
   * Create the following hierarchy:
   *
   *               actor2
   *             (Regular actor)
   *              /      \
   *             /        \
   *         actor1       actor4
   *       (Overlay)     (Regular actor)
   *          /
   *         /
   *     actor0
   *    (Overlay)
   *      /
   *     /
   *  actor3
   * (Overlay)
   *
   *  Expected rendering order : actor2 - actor4 - actor1 - actor0 - actor3
   */
  Stage::GetCurrent().Add( actor2 );
  actor2.Add(actor1);
  actor2.Add(actor4);
  actor1.Add(actor0);
  actor0.Add(actor3);
  application.SendNotification();
  application.Render(0);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);
  application.SendNotification();
  application.Render(0);

  int textureBindIndex[5];
  for( unsigned int i(0); i<5; ++i )
  {
    std::stringstream params;
    params << GL_TEXTURE_2D<<", "<<i+1;
    textureBindIndex[i] = gl.GetTextureTrace().FindIndexFromMethodAndParams("BindTexture", params.str() );
  }

  //Check that actor4 has been rendered after actor2
  DALI_TEST_GREATER( textureBindIndex[4], textureBindIndex[2], TEST_LOCATION );

  //Check that actor1 has been rendered after actor4
  DALI_TEST_GREATER( textureBindIndex[1], textureBindIndex[4], TEST_LOCATION );

  //Check that actor0 has been rendered after actor1
  DALI_TEST_GREATER( textureBindIndex[0], textureBindIndex[1], TEST_LOCATION );

  //Check that actor3 has been rendered after actor0
  DALI_TEST_GREATER( textureBindIndex[3], textureBindIndex[0], TEST_LOCATION );

  END_TEST;
}
