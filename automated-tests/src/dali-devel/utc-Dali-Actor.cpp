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
#include <mesh-builder.h>

//& set: DaliActor

using std::string;
using namespace Dali;


int UtcDaliActorAddRendererP(void)
{
  tet_infoline("Testing Actor::AddRenderer");
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial(1.0f);
  Renderer renderer0 = Renderer::New(geometry, material);

  actor.AddRenderer( renderer0 );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer0, TEST_LOCATION );

  Renderer renderer1 = Renderer::New(geometry, material);
  actor.AddRenderer( renderer1 );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 2u, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer0, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(1), renderer1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorAddRendererN(void)
{
  tet_infoline("Testing Actor::AddRenderer");
  TestApplication application;

  Actor actor = Actor::New();
  Renderer renderer;

  // try illegal Add
  try
  {
    actor.AddRenderer( renderer );
    tet_printf("Assertion test failed - no Exception\n" );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "Renderer handle is empty", TEST_LOCATION);
    DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliActorAddRendererOnStage(void)
{
  tet_infoline("Testing Actor::AddRenderer");
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial(1.0f);
  Renderer renderer = Renderer::New(geometry, material);

  application.SendNotification();
  application.Render(0);

  try
  {
    actor.AddRenderer( renderer );
    tet_result(TET_PASS);
  }
  catch(...)
  {
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliActorRemoveRendererP(void)
{
  tet_infoline("Testing Actor::RemoveRenderer");
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial(1.0f);
  Renderer renderer0 = Renderer::New(geometry, material);

  actor.AddRenderer( renderer0 );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer0, TEST_LOCATION );

  Renderer renderer1 = Renderer::New(geometry, material);
  actor.AddRenderer( renderer1 );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 2u, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer0, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(1), renderer1, TEST_LOCATION );

  actor.RemoveRenderer(renderer0);
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer1, TEST_LOCATION );

  actor.RemoveRenderer(renderer1);
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAGetRendererAtP(void)
{
  tet_infoline("Testing Actor::GetRendererAt");
  TestApplication application;

  Actor actor = Actor::New();

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial(1.0f);
  Renderer renderer0 = Renderer::New(geometry, material);
  Renderer renderer1 = Renderer::New(geometry, material);
  Renderer renderer2 = Renderer::New(geometry, material);
  Renderer renderer3 = Renderer::New(geometry, material);

  actor.AddRenderer( renderer0 );
  actor.AddRenderer( renderer1 );
  actor.AddRenderer( renderer2 );
  actor.AddRenderer( renderer3 );

  DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer0, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(1), renderer1, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(2), renderer2, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(3), renderer3, TEST_LOCATION );

  actor.RemoveRenderer( renderer0 );
  DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer1, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(1), renderer2, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(2), renderer3, TEST_LOCATION );

  actor.AddRenderer( renderer0 );
  DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer1, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(1), renderer2, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(2), renderer3, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(3), renderer0, TEST_LOCATION );

  actor.RemoveRenderer( renderer2 );
  DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer1, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(1), renderer3, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(2), renderer0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAGetRendererAtN(void)
{
  tet_infoline("Testing Actor::GetRendererAt");
  TestApplication application;

  Actor actor = Actor::New();

  // try illegal GetRendereAt
  try
  {
    Renderer renderer = actor.GetRendererAt( 0 );
    tet_printf("Assertion test failed - no Exception\n" );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "index < mRendererAttachment.size()", TEST_LOCATION);
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  END_TEST;
}
