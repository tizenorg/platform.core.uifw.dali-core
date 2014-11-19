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

#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

namespace
{
typedef std::vector<GLuint> TextureIds;

/**
 * Create an image actor, and test that it creates a new texture. Test also that the
 * texture is bound.
 *
 * Note, as context tries to cache texture binds per texture unit,
 * each test case should create another image actor.
 */
ImageActor CreateNewImageActor(
  TestApplication& application,
  unsigned int width,
  unsigned int height,
  Pixel::Format format,
  GLuint textureId )
{
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();

  std::vector<GLuint> textureIds;
  textureIds.push_back(textureId);
  gl.SetNextTextureIds(textureIds);

  textureTrace.Reset();
  textureTrace.Enable(true);

  BitmapImage image = BitmapImage::New(width, height, format);
  ImageActor actor = ImageActor::New(image);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( textureTrace.FindMethod("GenTextures") );
  DALI_TEST_CHECK( textureTrace.FindMethod("TexImage2D") );
  DALI_TEST_CHECK( textureTrace.FindMethod("BindTexture") );

  const TextureIds& boundTextures = gl.GetBoundTextures(gl.GetActiveTextureUnit());
  TextureIds::const_iterator iter = std::find(boundTextures.begin(), boundTextures.end(), textureId);

  DALI_TEST_CHECK( iter != boundTextures.end() );

  return actor;
}

/**
 * Create an image actor, and test that it uses a recycled
 * texture. Test also that the texture is bound.
 *
 * Note, as context tries to cache texture binds per texture unit,
 * each test case should create another image actor.
 */
ImageActor CreateRecycledImageActor(
  TestApplication& application,
  unsigned int width,
  unsigned int height,
  Pixel::Format format,
  GLuint textureId )
{
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();

  textureTrace.Reset();
  textureTrace.Enable(true);

  BitmapImage image = BitmapImage::New(width, height, format);
  ImageActor actor = ImageActor::New(image);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( ! textureTrace.FindMethod("GenTextures") );
  DALI_TEST_CHECK( ! textureTrace.FindMethod("TexImage2D") );
  DALI_TEST_CHECK( textureTrace.FindMethod("TexSubImage2D") );
  DALI_TEST_CHECK( textureTrace.FindMethod("BindTexture") );

  const TextureIds& boundTextures = gl.GetBoundTextures(gl.GetActiveTextureUnit());
  TextureIds::const_iterator iter = std::find(boundTextures.begin(), boundTextures.end(), textureId);

  DALI_TEST_CHECK( iter != boundTextures.end() );

  return actor;
}

} // anonymous namespace

/*
1. Check that a texture generated with the recycling config is correctly
recycled on deletion:
  A) create texture with default config
  B) Display actor using texture
  C) Check for GenTextures & TexImage2D calls
  D) delete actor and texture
  E) Check for no DeleteTextures call.
  F) Create 2nd texture with default config
  G) Check for no genTextures call and for TexSubImage2D call
*/

int UtcDaliTextureRecyclingCheckRecycling01(void)
{
  TestApplication application;
  tet_infoline("Check that a texture generated with the recycling config "
               "is correctly recycled on deletion");

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Create different actor to ensure each image is bound when used.");
  ImageActor oddActor = CreateNewImageActor( application, 200, 200, Pixel::RGBA8888, 23);

  tet_infoline( "Create and display test actor of recyclable size.");
  ImageActor actor = CreateNewImageActor( application, 100, 100, Pixel::RGB888, 17);

  tet_infoline( "Destroy test actor. Expect it to get recycled.");
  UnparentAndReset(actor);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( ! textureTrace.FindMethod("DeleteTextures") );
  gl.ClearBoundTextures();

  tet_infoline( "Create 2nd test actor of recyclable size. Expect it to re-use recycled texture" );
  actor = CreateRecycledImageActor( application, 100, 100, Pixel::RGB888, 17);

  END_TEST;
}

int UtcDaliTextureRecyclingCheckRecycling02(void)
{
  TestApplication application;

  tet_infoline("Check that generating max num recycling textures and deleting all of them results in no texture deletion.\n\n"
               "Check that generating max num recycling textures doesn't call genTextures or TexImage2D but instead calls TexSubImage2D.\n");

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Create different actor to ensure each image is bound when used.");
  ImageActor oddActor = CreateNewImageActor( application, 200, 200, Pixel::RGBA8888, 23);

  ActorContainer actorContainer;

  tet_infoline( "Create and display test actors of recyclable size.");
  for( int i=0; i<10 ; i++ )
  {
    ImageActor actor = CreateNewImageActor( application, 100, 100, Pixel::RGB888, 17+i);
    actorContainer.push_back(actor);
  }

  tet_infoline( "Destroy test actors. Expect them to get recycled.");
  for( int i=0; i<10; i++ )
  {
    // Deleting them from the back of the container so that the recycle list
    // is ordered with texture id's in reverse order.
    ImageActor actor = ImageActor::DownCast(actorContainer.back());
    actorContainer.erase( actorContainer.begin() + actorContainer.size()-1 );
    UnparentAndReset(actor);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK( ! textureTrace.FindMethod("DeleteTextures") );
  }
  gl.ClearBoundTextures();

  tet_infoline( "Create 2nd set of test actors of recyclable size. Expect them to re-use recycled textures" );
  for( int i=0; i<10 ; i++ )
  {
    // Recycling pulls from the back of the list, so we should get TextureId 17 first.
    ImageActor actor = CreateRecycledImageActor( application, 100, 100, Pixel::RGB888, 17+i);
    actorContainer.push_back(actor);
  }

  END_TEST;
}


int UtcDaliTextureRecyclingCheckRecycling03(void)
{
  TestApplication application;

  tet_infoline("Check that generating max num recycling textures + 1, then deleting all of them results in only 1 texture deletion.\n\n"
               "Check that generating max num recycling textures doesn't call genTextures or TexImage2D but instead calls TexSubImage2D.\n");

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Create different actor to ensure each image is bound when used.");
  ImageActor oddActor = CreateNewImageActor( application, 200, 200, Pixel::RGBA8888, 100);

  ActorContainer actorContainer;

  tet_infoline( "Create and display N+1 test actors of recyclable size.");
  for( int i=0; i<10 ; i++ )
  {
    ImageActor actor = CreateNewImageActor( application, 100, 100, Pixel::RGB888, 17+i);
    actorContainer.push_back(actor);
  }
  ImageActor extraActor = CreateNewImageActor( application, 100, 100, Pixel::RGB888, 200);

  tet_infoline( "Destroy N test actors. Expect them to get recycled.");
  for( int i=0; i<10; i++ )
  {
    // Deleting them from the back of the container so that the recycle list
    // is ordered with texture id's in reverse order.
    ImageActor actor = ImageActor::DownCast(actorContainer.back());
    actorContainer.erase( actorContainer.begin() + actorContainer.size()-1 );
    UnparentAndReset(actor);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK( ! textureTrace.FindMethod("DeleteTextures") );
  }

  tet_infoline( "Destroy last test actor. Expect texture to get deleted");
  UnparentAndReset(extraActor);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( textureTrace.FindMethod("DeleteTextures") );

  gl.ClearBoundTextures();

  tet_infoline( "Create 2nd set of test actors of recyclable size. Expect them to re-use recycled textures" );
  for( int i=0; i<10 ; i++ )
  {
    // Recycling pulls from the back of the list, so we should get TextureId 17 first.
    ImageActor actor = CreateRecycledImageActor( application, 100, 100, Pixel::RGB888, 17+i);
    actorContainer.push_back(actor);
  }

  tet_infoline( "Create another actor of recyclable size. Expect this to be created from scratch");
  ImageActor testActor = CreateNewImageActor( application, 100, 100, Pixel::RGB888, 300);

  END_TEST;
}


int UtcDaliTextureRecyclingCheckNoRecycling(void)
{
  TestApplication application;

  tet_infoline("Check that creating textures with a non-recycling config, drawing and deleting them correctly calls GenTextures, TexImage2D & DeleteTextures.\n");

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  ActorContainer actorContainer;

  tet_infoline( "Create and display N test actors of non-recyclable size.");
  for( int i=0; i<10 ; i++ )
  {
    unsigned int width = 100;
    unsigned int height = 100;
    Pixel::Format pixelFormat = Pixel::RGB888;
    switch(i%3)
    {
      case 0: width = 200; break;
      case 1: height = 200; break;
      case 2: pixelFormat = Pixel::RGBA8888; break;
    }
    ImageActor actor = CreateNewImageActor( application, width, height, pixelFormat, 17+i);
    actorContainer.push_back(actor);
  }

  tet_infoline( "Destroy N test actors. Expect them to get deleted.");
  for( int i=0; i<10; i++ )
  {
    textureTrace.Reset();
    ImageActor actor = ImageActor::DownCast(actorContainer.back());
    actorContainer.erase( actorContainer.begin() + actorContainer.size()-1 );
    UnparentAndReset(actor);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK( textureTrace.FindMethod("DeleteTextures") );
  }

  END_TEST;
}


int UtcDaliTextureRecyclingContextLoss(void)
{
  TestApplication application;

  tet_infoline("Check that creating, drawing and deleting maxNum textures with a recycle config; then stopping the context empties the recycle list.");

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Create different actor to ensure each image is bound when used.");
  ImageActor oddActor = CreateNewImageActor( application, 200, 200, Pixel::RGBA8888, 100);


  ActorContainer actorContainer;

  tet_infoline( "Create and display N test actors of recyclable size.");
  for( int i=0; i<10 ; i++ )
  {
    ImageActor actor = CreateNewImageActor( application, 100, 100, Pixel::RGB888, 17+i);
    actorContainer.push_back(actor);
  }

  tet_infoline( "Destroy N test actors. Expect them to get recycled.");
  for( int i=0; i<10; i++ )
  {
    // Deleting them from the back of the container so that the recycle list
    // is ordered with texture id's in reverse order.
    ImageActor actor = ImageActor::DownCast(actorContainer.back());
    actorContainer.erase( actorContainer.begin() + actorContainer.size()-1 );
    UnparentAndReset(actor);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK( ! textureTrace.FindMethod("DeleteTextures") );
  }
  textureTrace.Reset();

  // Force context loss
  application.GetCore().ContextDestroyed();
  // Can't call GL after context loss
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION);

  application.GetCore().ContextCreated();

  tet_infoline( "Create 2nd set of test actors of recyclable size. Expect them to not use recycled textures" );
  for( int i=0; i<10 ; i++ )
  {
    // Recycling pulls from the back of the list, so we should get TextureId 17 first.
    ImageActor actor = CreateNewImageActor( application, 100, 100, Pixel::RGB888, 17+i);
    actorContainer.push_back(actor);
  }

  END_TEST;
}

int UtcDaliTextureRecyclingConfigChange01(void)
{
  TestApplication application;

  tet_infoline("Check that changing the recycling config size deletes any recycled "
               "textures and that only textures of the new size get recycled on deletion.");


  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  ActorContainer actorContainer;

  ImageActor extraActor = CreateNewImageActor( application, 128, 128, Pixel::RGBA8888, 100);

  tet_infoline( "Create and display N test actors of recyclable size.");
  for( int i=0; i<10 ; i++ )
  {
    unsigned int width = 100;
    unsigned int height = 100;
    Pixel::Format pixelFormat = Pixel::RGB888;
    ImageActor actor = CreateNewImageActor( application, width, height, pixelFormat, 17+i);
    actorContainer.push_back(actor);
  }

  tet_infoline( "Destroy N test actors. Expect them to get recycled.");
  textureTrace.Reset();
  for( int i=0; i<10; i++ )
  {
    ImageActor actor = ImageActor::DownCast(actorContainer.back());
    actorContainer.erase( actorContainer.begin() + actorContainer.size()-1 );
    UnparentAndReset(actor);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK( ! textureTrace.FindMethod("DeleteTextures") );
  }

  tet_infoline( "Change the recycling config. Expect all recycled textures to be deleted.\n");
  textureTrace.Reset();
  Config::SetRecycledTextureWidth(200u);
  Config::SetRecycledTextureHeight(200u);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 10, TEST_LOCATION );

  tet_infoline( "Create actor with original size. Expect it to use new texture.\n");
  ImageActor actor = CreateNewImageActor( application, 100, 100, Pixel::RGB888, 30);

  tet_infoline( "Delete the actor. Expect the texture to be deleted.\n");
  textureTrace.Reset();
  UnparentAndReset(actor);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( textureTrace.FindMethod("DeleteTextures") );

  tet_infoline("Create and display a texture of the new size. Ensure that GenTextures is called.\n");
  actor = CreateNewImageActor( application, 200, 200, Pixel::RGB888, 30);

  tet_infoline("Delete it. Ensure that DeleteTextures is not called.\n");
  textureTrace.Reset();
  UnparentAndReset(actor);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( ! textureTrace.FindMethod("DeleteTextures") );

  tet_infoline("Create and display another texture of the new size. Ensure that GenTextures is not called, and that TexSubImage2D is called.\n" );

  actor = CreateRecycledImageActor( application, 200, 200, Pixel::RGB888, 30);

  END_TEST;
}


int UtcDaliTextureRecyclingConfigChange02(void)
{
  TestApplication application;

  tet_infoline("Check that changing the recycling config pixel format deletes any recycled "
               "textures and that only textures of the new pixel format get recycled on deletion.");

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  ActorContainer actorContainer;

  ImageActor extraActor = CreateNewImageActor( application, 128, 128, Pixel::RGBA8888, 100);

  tet_infoline( "Create and display N test actors of recyclable size & format.");
  for( int i=0; i<10 ; i++ )
  {
    unsigned int width = 100;
    unsigned int height = 100;
    Pixel::Format pixelFormat = Pixel::RGB888;
    ImageActor actor = CreateNewImageActor( application, width, height, pixelFormat, 17+i);
    actorContainer.push_back(actor);
  }

  tet_infoline( "Destroy N test actors. Expect them to get recycled.");
  textureTrace.Reset();
  for( int i=0; i<10; i++ )
  {
    ImageActor actor = ImageActor::DownCast(actorContainer.back());
    actorContainer.erase( actorContainer.begin() + actorContainer.size()-1 );
    UnparentAndReset(actor);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK( ! textureTrace.FindMethod("DeleteTextures") );
  }

  tet_infoline( "Change the recycling config. Expect all recycled textures to be deleted.\n");
  textureTrace.Reset();
  Config::SetRecycledTexturePixelFormat(Pixel::RGBA8888);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 10, TEST_LOCATION );

  tet_infoline( "Create actor with original pixel format. Expect it to use new texture.\n");
  ImageActor actor = CreateNewImageActor( application, 100, 100, Pixel::RGB888, 30);

  tet_infoline( "Delete the actor. Expect the texture to be deleted.\n");
  textureTrace.Reset();
  UnparentAndReset(actor);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( textureTrace.FindMethod("DeleteTextures") );

  tet_infoline("Create and display a texture of the new pixel format. Ensure that GenTextures is called.\n");
  actor = CreateNewImageActor( application, 100, 100, Pixel::RGBA8888, 30);

  tet_infoline("Delete it. Ensure that DeleteTextures is not called.\n");
  textureTrace.Reset();
  UnparentAndReset(actor);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( ! textureTrace.FindMethod("DeleteTextures") );

  tet_infoline("Create and display another texture of the new pixel format. Ensure that GenTextures is not called, and that TexSubImage2D is called.\n" );

  actor = CreateRecycledImageActor( application, 100, 100, Pixel::RGBA8888, 30);

  END_TEST;
}


int UtcDaliTextureRecyclingConfigChange03(void)
{
  TestApplication application;

  tet_infoline("Test that only increasing the max number of recycling textures does not remove the existing recyclable textures. Also check that more textures can now be recycled.\n");

  tet_infoline("Create, display and destroy M=max num recycling textures to fill the recycling list.");

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  ActorContainer actorContainer;

  for( int i=0; i<10 ; i++ )
  {
    ImageActor actor = CreateNewImageActor( application, 100, 100, Pixel::RGB888, 17+i);
    actorContainer.push_back(actor);
  }
  textureTrace.Reset();
  for( int i=0; i<10; i++ )
  {
    ImageActor actor = ImageActor::DownCast(actorContainer.back());
    actorContainer.erase( actorContainer.begin() + actorContainer.size()-1 );
    UnparentAndReset(actor);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK( ! textureTrace.FindMethod("DeleteTextures") );
  }

  tet_infoline("Increase the max number of recycled textures via the config to N.");
  Config::SetNumberOfRecycledTextures(20);
  application.SendNotification();
  application.Render();

  tet_infoline("Create and display M textures. Check that for each, GenTextures is not called and TexSubImage2D is called.");
  for( int i=0; i<10 ; i++ )
  {
    ImageActor actor = CreateRecycledImageActor( application, 100, 100, Pixel::RGB888, 17+i);
    actorContainer.push_back(actor);
  }

  tet_infoline("Create and display N-M textures. Check that GenTextures and TexImage2D are called.");
  for( int i=0; i<10 ; i++ )
  {
    ImageActor actor = CreateNewImageActor( application, 100, 100, Pixel::RGB888, 27+i);
    actorContainer.push_back(actor);
  }

  tet_infoline("Destroy N textures. Check that they are all recycled");
  textureTrace.Reset();
  for( int i=0; i<20; i++)
  {
    ImageActor actor = ImageActor::DownCast(actorContainer.back());
    actorContainer.erase( actorContainer.begin() + actorContainer.size()-1 );
    UnparentAndReset(actor);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK( ! textureTrace.FindMethod("DeleteTextures") );
  }

  tet_infoline("Create and display N textures. Check that they use all recycled textures");

  for( int i=0; i<20 ; i++ )
  {
    ImageActor actor = CreateRecycledImageActor( application, 100, 100, Pixel::RGB888, 17+i);
    actorContainer.push_back(actor);
  }

  END_TEST;
}

int UtcDaliTextureRecyclingConfigChange04(void)
{
  TestApplication application;

  tet_infoline("Test that decreasing the max number of recycling textures when the list is full deletes the excess recyclable textures.");

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  ActorContainer actorContainer;

  ImageActor extraActor = CreateNewImageActor( application, 128, 128, Pixel::RGBA8888, 100);

  tet_infoline( "Create and display N test actors of recyclable size & format.");
  for( int i=0; i<10 ; i++ )
  {
    unsigned int width = 100;
    unsigned int height = 100;
    Pixel::Format pixelFormat = Pixel::RGB888;
    ImageActor actor = CreateNewImageActor( application, width, height, pixelFormat, 17+i);
    actorContainer.push_back(actor);
  }

  tet_infoline( "Destroy N test actors. Expect them to get recycled.");
  textureTrace.Reset();
  for( int i=0; i<10; i++ )
  {
    ImageActor actor = ImageActor::DownCast(actorContainer.back());
    actorContainer.erase( actorContainer.begin() + actorContainer.size()-1 );
    UnparentAndReset(actor);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK( ! textureTrace.FindMethod("DeleteTextures") );
  }

  Config::SetNumberOfRecycledTextures(5);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 5, TEST_LOCATION );


  END_TEST;
}

int UtcDaliTextureRecyclingConfigChange05(void)
{
  TestApplication application;

  tet_infoline( "Test that decreasing the max number of recycling textures to N when < N recyclable textures are in it does not remove those recyclable textures." );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  ActorContainer actorContainer;

  ImageActor extraActor = CreateNewImageActor( application, 128, 128, Pixel::RGBA8888, 100);

  tet_infoline( "Create and display M test actors of recyclable size & format.");
  for( int i=0; i<5 ; i++ )
  {
    ImageActor actor = CreateNewImageActor( application, 100, 100, Pixel::RGB888, 17+i);
    actorContainer.push_back(actor);
  }

  tet_infoline( "Destroy M test actors. Expect them to get recycled.");
  textureTrace.Reset();
  for( int i=0; i<5; i++ )
  {
    ImageActor actor = ImageActor::DownCast(actorContainer.back());
    actorContainer.erase( actorContainer.begin() + actorContainer.size()-1 );
    UnparentAndReset(actor);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK( ! textureTrace.FindMethod("DeleteTextures") );
  }

  tet_infoline("Reduce the max number of recyclable textures to > M. Expect no texture deletion.\n");

  Config::SetNumberOfRecycledTextures(6);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION );

  tet_infoline("Create M actors. Expect them to use recycled textures\n");
  for( int i=0; i<5 ; i++ )
  {
    ImageActor actor = CreateRecycledImageActor( application, 100, 100, Pixel::RGB888, 17+i);
  }

  END_TEST;
}
