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
#include "dali-test-suite-utils/dali-test-suite-utils.h"
#include <mesh-builder.h>

using namespace Dali;

void handle_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void handle_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

/// Allows the creation of a BaseObject
class BaseObjectType : public BaseObject
{
public:
  BaseObjectType()
  {
  }
};

Handle ImplicitCopyConstructor(Handle passedByValue)
{
  // object + copy + passedByValue, ref count == 3
  DALI_TEST_CHECK(passedByValue);
  if (passedByValue)
  {
    DALI_TEST_EQUALS(3, passedByValue.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  }

  return passedByValue;
}

} // anon namespace


int UtcDaliHandleConstructorVoid(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Handle::Handle()");

  Handle object;

  DALI_TEST_CHECK(!object);

  END_TEST;
}

int UtcDaliHandleCopyConstructor(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Handle::Handle(const Handle&)");

  // Initialize an object, ref count == 1
  Handle object = Actor::New();

  DALI_TEST_EQUALS(1, object.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  // Copy the object, ref count == 2
  Handle copy(object);
  DALI_TEST_CHECK(copy);
  if (copy)
  {
    DALI_TEST_EQUALS(2, copy.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  }

  {
    // Pass by value, and return another copy, ref count == 3
    Handle anotherCopy = ImplicitCopyConstructor(copy);

    DALI_TEST_CHECK(anotherCopy);
    if (anotherCopy)
    {
      DALI_TEST_EQUALS(3, anotherCopy.GetBaseObject().ReferenceCount(), TEST_LOCATION);
    }
  }

  // anotherCopy out of scope, ref count == 2
  DALI_TEST_CHECK(copy);
  if (copy)
  {
    DALI_TEST_EQUALS(2, copy.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliHandleAssignmentOperator(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Handle::operator=");

  Handle object = Actor::New();

  DALI_TEST_CHECK(object);
  DALI_TEST_EQUALS(1, object.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Handle copy;
  DALI_TEST_CHECK(!copy);

  copy = object;
  DALI_TEST_CHECK(copy);
  DALI_TEST_EQUALS(2, copy.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(&(copy.GetBaseObject()) == &(object.GetBaseObject()));
  END_TEST;
}

int UtcDaliHandleSupports(void)
{
  tet_infoline("Positive Test Dali::Handle::Supports()");
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( true == actor.Supports( Handle::DYNAMIC_PROPERTIES ) );
  END_TEST;
}

int UtcDaliHandleGetPropertyCount(void)
{
  tet_infoline("Positive Test Dali::Handle::GetPropertyCount()");
  TestApplication application;

  Actor actor = Actor::New();
  int defaultPropertyCount( actor.GetPropertyCount() );

  // Register a dynamic property
  actor.RegisterProperty( "testProperty",  float(123.0f) );
  DALI_TEST_CHECK( (defaultPropertyCount + 1u) == actor.GetPropertyCount() );
  END_TEST;
}

int UtcDaliHandleGetPropertyName(void)
{
  tet_infoline("Positive Test Dali::Handle::GetPropertyName()");
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( "parentOrigin" == actor.GetPropertyName( Actor::Property::PARENT_ORIGIN ) );

  // Register a dynamic property
  std::string name("thisNameShouldMatch");
  Property::Index index = actor.RegisterProperty( name, float(123.0f) );
  DALI_TEST_CHECK( name == actor.GetPropertyName( index ) );

  END_TEST;
}

int UtcDaliHandleGetPropertyIndex(void)
{
  tet_infoline("Positive Test Dali::Handle::GetPropertyIndex()");
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( Actor::Property::PARENT_ORIGIN == actor.GetPropertyIndex("parentOrigin") );

  // Register a dynamic property
  std::string name("thisNameShouldMatch");
  Property::Index index = actor.RegisterProperty( name, float(123.0f) );
  DALI_TEST_CHECK( index == actor.GetPropertyIndex( name ) );
  END_TEST;
}

int UtcDaliHandleIsPropertyWritable(void)
{
  tet_infoline("Positive Test Dali::Handle::IsPropertyWritable()");
  TestApplication application;

  Actor actor = Actor::New();

  // Actor properties which are writable:
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::PARENT_ORIGIN ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::PARENT_ORIGIN_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::PARENT_ORIGIN_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::PARENT_ORIGIN_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::ANCHOR_POINT ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::ANCHOR_POINT_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::ANCHOR_POINT_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::ANCHOR_POINT_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SIZE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SIZE_WIDTH  ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SIZE_HEIGHT ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SIZE_DEPTH  ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::POSITION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::POSITION_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::POSITION_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::POSITION_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::ORIENTATION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SCALE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SCALE_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SCALE_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SCALE_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::VISIBLE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::COLOR ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::COLOR_RED ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::COLOR_GREEN ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::COLOR_BLUE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::COLOR_ALPHA ) );

  // World-properties are not writable:
  DALI_TEST_CHECK( false == actor.IsPropertyWritable( Actor::Property::WORLD_POSITION ) );
  DALI_TEST_CHECK( false == actor.IsPropertyWritable( Actor::Property::WORLD_ORIENTATION ) );
  DALI_TEST_CHECK( false == actor.IsPropertyWritable( Actor::Property::WORLD_SCALE ) );
  DALI_TEST_CHECK( false == actor.IsPropertyWritable( Actor::Property::WORLD_COLOR ) );
  DALI_TEST_CHECK( false == actor.IsPropertyWritable( Actor::Property::WORLD_POSITION_X ) );
  DALI_TEST_CHECK( false == actor.IsPropertyWritable( Actor::Property::WORLD_POSITION_Y ) );
  DALI_TEST_CHECK( false == actor.IsPropertyWritable( Actor::Property::WORLD_POSITION_Z ) );

  END_TEST;
}

int UtcDaliHandleIsPropertyAnimatable(void)
{
  tet_infoline("Positive Test Dali::Handle::IsPropertyAnimatable()");
  TestApplication application;

  Actor actor = Actor::New();

  // Actor properties which are animatable:
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::PARENT_ORIGIN ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::PARENT_ORIGIN_X ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::PARENT_ORIGIN_Y ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::PARENT_ORIGIN_Z ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::ANCHOR_POINT ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::ANCHOR_POINT_X ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::ANCHOR_POINT_Y ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::ANCHOR_POINT_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SIZE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SIZE_WIDTH  ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SIZE_HEIGHT ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SIZE_DEPTH  ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::POSITION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::POSITION_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::POSITION_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::POSITION_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::ORIENTATION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SCALE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SCALE_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SCALE_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SCALE_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::VISIBLE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::COLOR ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::COLOR_RED ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::COLOR_GREEN ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::COLOR_BLUE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::COLOR_ALPHA ) );

  // World-properties can not be animated
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::WORLD_POSITION ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::WORLD_ORIENTATION ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::WORLD_SCALE ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::WORLD_COLOR ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::WORLD_POSITION_X ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::WORLD_POSITION_Y ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::WORLD_POSITION_Z ) );

  END_TEST;
}

int UtcDaliHandleIsPropertyAConstraintInput(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Actor properties which can be used as a constraint input:
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::PARENT_ORIGIN ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::PARENT_ORIGIN_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::PARENT_ORIGIN_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::PARENT_ORIGIN_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::ANCHOR_POINT ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::ANCHOR_POINT_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::ANCHOR_POINT_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::ANCHOR_POINT_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SIZE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SIZE_WIDTH  ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SIZE_HEIGHT ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SIZE_DEPTH  ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::POSITION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::POSITION_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::POSITION_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::POSITION_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::ORIENTATION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SCALE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SCALE_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SCALE_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SCALE_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::VISIBLE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::COLOR ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::COLOR_RED ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::COLOR_GREEN ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::COLOR_BLUE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::COLOR_ALPHA ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::WORLD_POSITION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::WORLD_ORIENTATION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::WORLD_SCALE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::WORLD_COLOR ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::WORLD_POSITION_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::WORLD_POSITION_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::WORLD_POSITION_Z ) );

  // Actor properties that cannot be used as a constraint input
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::NAME ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::SENSITIVE ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::LEAVE_REQUIRED ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::INHERIT_ORIENTATION ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::INHERIT_SCALE ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::COLOR_MODE ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::POSITION_INHERITANCE ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::DRAW_MODE ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::SIZE_MODE_FACTOR ) );

  END_TEST;
}


int UtcDaliHandleGetPropertyType(void)
{
  tet_infoline("Positive Test Dali::Handle::GetPropertyType()");
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( Property::VECTOR3  == actor.GetPropertyType( Actor::Property::PARENT_ORIGIN ) );
  DALI_TEST_CHECK( Property::VECTOR3  == actor.GetPropertyType( Actor::Property::ANCHOR_POINT ) );
  DALI_TEST_CHECK( Property::VECTOR3  == actor.GetPropertyType( Actor::Property::SIZE ) );
  DALI_TEST_CHECK( Property::VECTOR3  == actor.GetPropertyType( Actor::Property::POSITION ) );
  DALI_TEST_CHECK( Property::ROTATION == actor.GetPropertyType( Actor::Property::ORIENTATION ) );
  DALI_TEST_CHECK( Property::VECTOR3  == actor.GetPropertyType( Actor::Property::SCALE ) );
  DALI_TEST_CHECK( Property::BOOLEAN  == actor.GetPropertyType( Actor::Property::VISIBLE ) );
  DALI_TEST_CHECK( Property::VECTOR4  == actor.GetPropertyType( Actor::Property::COLOR ) );

  // Register some dynamic properties
  Property::Index boolIndex     = actor.RegisterProperty( "boolProperty",      bool(true) );
  Property::Index floatIndex    = actor.RegisterProperty( "floatProperty",     float(123.0f) );
  Property::Index intIndex      = actor.RegisterProperty( "intProperty",       123 );
  Property::Index vector2Index  = actor.RegisterProperty( "vector2Property",   Vector2(1.0f, 2.0f) );
  Property::Index vector3Index  = actor.RegisterProperty( "vector3Property",   Vector3(1.0f, 2.0f, 3.0f) );
  Property::Index vector4Index  = actor.RegisterProperty( "vector4Property",   Vector4(1.0f, 2.0f, 3.0f, 4.0f) );
  Property::Index rotationIndex = actor.RegisterProperty( "rotationProperty",  AngleAxis(Degree(180.0f), Vector3::YAXIS) );

  DALI_TEST_CHECK( Property::BOOLEAN  == actor.GetPropertyType( boolIndex ) );
  DALI_TEST_CHECK( Property::FLOAT    == actor.GetPropertyType( floatIndex ) );
  DALI_TEST_CHECK( Property::INTEGER  == actor.GetPropertyType( intIndex ) );
  DALI_TEST_CHECK( Property::VECTOR2  == actor.GetPropertyType( vector2Index ) );
  DALI_TEST_CHECK( Property::VECTOR3  == actor.GetPropertyType( vector3Index ) );
  DALI_TEST_CHECK( Property::VECTOR4  == actor.GetPropertyType( vector4Index ) );
  DALI_TEST_CHECK( Property::ROTATION == actor.GetPropertyType( rotationIndex ) );

  // Non animatable properties
  Property::Index nonAnimStringIndex = actor.RegisterProperty( "manFromDelmonte",   std::string("yes"), Property::READ_WRITE);
  Property::Index nonAnimV2Index = actor.RegisterProperty( "v2", Vector2(1.f, 2.f), Property::READ_WRITE);
  Property::Index nonAnimV3Index = actor.RegisterProperty( "v3", Vector3(1.f, 2.f, 3.f), Property::READ_WRITE);
  Property::Index nonAnimV4Index = actor.RegisterProperty( "v4", Vector4(1.f, 2.f, 3.f, 4.f), Property::READ_WRITE);
  Property::Index nonAnimBooleanIndex = actor.RegisterProperty( "bool", true, Property::READ_WRITE);
  Property::Index nonAnimFloatIndex = actor.RegisterProperty( "float", 0.f, Property::READ_WRITE);
  Property::Index nonAnimIntegerIndex = actor.RegisterProperty( "int", 0, Property::READ_WRITE);

  DALI_TEST_CHECK( nonAnimStringIndex  != Property::INVALID_INDEX );
  DALI_TEST_CHECK( nonAnimV2Index      != Property::INVALID_INDEX );
  DALI_TEST_CHECK( nonAnimV3Index      != Property::INVALID_INDEX );
  DALI_TEST_CHECK( nonAnimV4Index      != Property::INVALID_INDEX );
  DALI_TEST_CHECK( nonAnimBooleanIndex != Property::INVALID_INDEX );
  DALI_TEST_CHECK( nonAnimFloatIndex   != Property::INVALID_INDEX );
  DALI_TEST_CHECK( nonAnimIntegerIndex != Property::INVALID_INDEX );

  DALI_TEST_CHECK( Property::STRING   == actor.GetPropertyType( nonAnimStringIndex ) );
  DALI_TEST_CHECK( Property::VECTOR2  == actor.GetPropertyType( nonAnimV2Index ) );
  DALI_TEST_CHECK( Property::VECTOR3  == actor.GetPropertyType( nonAnimV3Index ) );
  DALI_TEST_CHECK( Property::VECTOR4  == actor.GetPropertyType( nonAnimV4Index ) );
  DALI_TEST_CHECK( Property::BOOLEAN  == actor.GetPropertyType( nonAnimBooleanIndex ) );
  DALI_TEST_CHECK( Property::FLOAT    == actor.GetPropertyType( nonAnimFloatIndex ) );
  DALI_TEST_CHECK( Property::INTEGER  == actor.GetPropertyType( nonAnimIntegerIndex ) );

  DALI_TEST_CHECK( !actor.IsPropertyAnimatable( nonAnimStringIndex ) );
  DALI_TEST_CHECK( !actor.IsPropertyAnimatable( nonAnimV2Index ) );
  DALI_TEST_CHECK( !actor.IsPropertyAnimatable( nonAnimV3Index ) );
  DALI_TEST_CHECK( !actor.IsPropertyAnimatable( nonAnimV4Index ) );
  DALI_TEST_CHECK( !actor.IsPropertyAnimatable( nonAnimBooleanIndex ) );
  DALI_TEST_CHECK( !actor.IsPropertyAnimatable( nonAnimFloatIndex ) );
  DALI_TEST_CHECK( !actor.IsPropertyAnimatable( nonAnimIntegerIndex ) );

  DALI_TEST_EQUALS( "yes" , actor.GetProperty( nonAnimStringIndex ).Get<std::string>(), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(1.f, 2.f) , actor.GetProperty( nonAnimV2Index ).Get<Vector2>(), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector3(1.f, 2.f, 3.f) , actor.GetProperty( nonAnimV3Index ).Get<Vector3>(), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector4(1.f, 2.f, 3.f, 4.f) , actor.GetProperty( nonAnimV4Index ).Get<Vector4>(), TEST_LOCATION );
  DALI_TEST_EQUALS( true, actor.GetProperty( nonAnimBooleanIndex ).Get<bool>(), TEST_LOCATION );
  DALI_TEST_EQUALS( 0.f, actor.GetProperty( nonAnimFloatIndex ).Get<float>(), TEST_LOCATION );
  DALI_TEST_EQUALS( 0, actor.GetProperty( nonAnimIntegerIndex ).Get<int>(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliHandleNonAnimtableProperties(void)
{
  tet_infoline("Test Non Animatable Properties");
  TestApplication application;

  Actor actor = Actor::New();

  Property::Index nonAnimStringIndex = actor.RegisterProperty( "manFromDelmonte",   std::string("no"), Property::READ_WRITE);

  //// modify writable?
  try
  {
    actor.SetProperty( nonAnimStringIndex, Property::Value("yes") );
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_CHECK(!"exception");
  }

  DALI_TEST_CHECK( "yes"  == actor.GetProperty( nonAnimStringIndex ).Get<std::string>() );

  //// cannot modify read only?
  Property::Index readonly = actor.RegisterProperty( "float", 0.f, Property::READ_ONLY);

  DALI_TEST_CHECK(!actor.IsPropertyAnimatable(readonly));
  DALI_TEST_CHECK(!actor.IsPropertyWritable(readonly));

  bool exception = false;
  try
  {
    actor.SetProperty( readonly, Property::Value(1.f) );
  }
  catch (Dali::DaliException& e)
  {
    exception = true;
  }

  DALI_TEST_CHECK(!exception);// trying to set a read-only property is a no-op

  DALI_TEST_EQUALS( 0.f, actor.GetProperty( readonly ).Get<float>(), TEST_LOCATION );

  /// animatable can be set
  Property::Index write_anim = actor.RegisterProperty( "write_float", 0.f, Property::ANIMATABLE);

  DALI_TEST_CHECK(actor.IsPropertyAnimatable(write_anim));
  DALI_TEST_CHECK(actor.IsPropertyWritable(write_anim));

  exception = false;
  try
  {
    actor.SetProperty( write_anim, Property::Value(1.f) );
  }
  catch (Dali::DaliException& e)
  {
    exception = true;
  }

  DALI_TEST_CHECK(!exception);

  //// animate a non animatable property is a noop?
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  bool relativeValue(true);

  exception = false;

  try
  {
    animation.AnimateBy(Property(actor, nonAnimStringIndex), relativeValue, AlphaFunction::EASE_IN);
    animation.Play();
    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds*0100.0f)/* some progress */);
  }
  catch (Dali::DaliException& e)
  {
    exception = true;
  }

  DALI_TEST_CHECK(!exception);
  DALI_TEST_EQUALS( "yes", actor.GetProperty( nonAnimStringIndex ).Get<std::string>(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliHandleNonAnimtableCompositeProperties(void)
{
  tet_infoline("Test Non Animatable Composite Properties");
  TestApplication application;

  Actor actor = Actor::New();

  Property::Value value(Property::ARRAY);
  Property::Array* array = value.GetArray();
  DALI_TEST_CHECK( array );

  array->PushBack( Property::Value( 0.1f ) );
  array->PushBack( "a string" );
  array->PushBack( Property::Value( Vector3(1,2,3) ) );

  DALI_TEST_EQUALS( 3, array->Count(), TEST_LOCATION );

  Property::Index propertyIndex = actor.RegisterProperty( "composite", value, Property::READ_WRITE );

  Property::Value out = actor.GetProperty( propertyIndex );
  Property::Array* outArray = out.GetArray();
  DALI_TEST_CHECK( outArray != NULL );

  DALI_TEST_CHECK( Property::FLOAT     == outArray->GetElementAt(0).GetType());
  DALI_TEST_CHECK( Property::STRING    == outArray->GetElementAt(1).GetType());
  DALI_TEST_CHECK( Property::VECTOR3   == outArray->GetElementAt(2).GetType());

  DALI_TEST_EQUALS( 0.1f,            outArray->GetElementAt(0).Get<float>(),       TEST_LOCATION);
  DALI_TEST_EQUALS( "a string",     outArray->GetElementAt(1).Get<std::string>(),  TEST_LOCATION);
  DALI_TEST_EQUALS( Vector3(1,2,3), outArray->GetElementAt(2).Get<Vector3>(),      TEST_LOCATION);

  // composite types not animatable
  bool exception = false;
  try
  {
    actor.RegisterProperty( "compositemap", value, Property::ANIMATABLE);
  }
  catch (Dali::DaliException& e)
  {
    exception = true;
    DALI_TEST_PRINT_ASSERT( e );
  }

  DALI_TEST_EQUALS(exception, true, TEST_LOCATION);

  // Map of maps
  Property::Value mapOfMaps(Property::MAP);
  Property::Map* map = mapOfMaps.GetMap();

  map->Insert( "key", Property::Value(Property::MAP) );
  map->Insert( "2key", "a string" );

  DALI_TEST_EQUALS( "a string",  (*map)["2key"].Get<std::string>(),  TEST_LOCATION);

  Property::Map* innerMap = map->Find("key")->GetMap();
  innerMap->Insert( "subkey", 5.f );

  DALI_TEST_CHECK( NULL != map->Find("key")->GetMap()->Find("subkey") );
  DALI_TEST_EQUALS( 5.f, map->Find("key")->GetMap()->Find("subkey")->Get<float>(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliHandleSetProperty01(void)
{
  tet_infoline("Positive Test Dali::Handle::SetProperty()");
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( ParentOrigin::TOP_LEFT == actor.GetProperty( Actor::Property::PARENT_ORIGIN ).Get<Vector3>() );

  actor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( ParentOrigin::CENTER == actor.GetProperty( Actor::Property::PARENT_ORIGIN ).Get<Vector3>() );
  END_TEST;
}

int UtcDaliHandleSetProperty02(void)
{
  tet_infoline("Positive Test Dali::Handle::SetProperty()");
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK( !actor.IsPropertyWritable( Actor::Property::WORLD_POSITION ) );

  // World position is not writable so this is a no-op and should not crash
  actor.SetProperty( Actor::Property::WORLD_POSITION, Vector3(1,2,3) );

  END_TEST;
}

int UtcDaliHandleRegisterProperty(void)
{
  tet_infoline("Positive Test Dali::Handle::RegisterProperty()");
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( ParentOrigin::TOP_LEFT == actor.GetProperty( Actor::Property::PARENT_ORIGIN ).Get<Vector3>() );

  END_TEST;
}

int UtcDaliHandleGetProperty(void)
{
  tet_infoline("Positive Test Dali::Handle::GetProperty()");
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK( ParentOrigin::TOP_LEFT == actor.GetProperty( Actor::Property::PARENT_ORIGIN   ).Get<Vector3>() );
  DALI_TEST_CHECK( AnchorPoint::CENTER    == actor.GetProperty( Actor::Property::ANCHOR_POINT    ).Get<Vector3>() );
  DALI_TEST_CHECK( Vector3::ZERO          == actor.GetProperty( Actor::Property::SIZE            ).Get<Vector3>() );
  DALI_TEST_CHECK( Vector3::ZERO          == actor.GetProperty( Actor::Property::POSITION        ).Get<Vector3>() );
  DALI_TEST_CHECK( Vector3::ONE           == actor.GetProperty( Actor::Property::SCALE           ).Get<Vector3>() );
  DALI_TEST_CHECK( true                   == actor.GetProperty( Actor::Property::VISIBLE         ).Get<bool>() );
  DALI_TEST_CHECK( Color::WHITE           == actor.GetProperty( Actor::Property::COLOR           ).Get<Vector4>() );
  END_TEST;
}

int UtcDaliHandleDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Handle::DownCast()");

  Actor actor = Actor::New();

  BaseHandle baseHandle = actor;

  Handle handle = Handle::DownCast(baseHandle);

  DALI_TEST_CHECK( handle );

  baseHandle = BaseHandle();

  handle = Handle::DownCast(baseHandle);

  DALI_TEST_CHECK( !handle );

  END_TEST;
}

int UtcDaliHandleDownCastNegative(void)
{
  TestApplication application;

  // BaseObject is NOT an Object, so this DownCast should fail
  BaseHandle handle( new BaseObjectType );
  Handle customHandle1 = Handle::DownCast( handle );
  DALI_TEST_CHECK( ! customHandle1 );

  // A DownCast on an empty handle will also fail
  Handle empty;
  Handle customHandle2 = Handle::DownCast( empty );
  DALI_TEST_CHECK( ! customHandle2 );
  END_TEST;
}

int UtcDaliHandleGetPropertyIndices(void)
{
  TestApplication application;
  Property::IndexContainer indices;

  // Actor
  Actor actor = Actor::New();
  actor.GetPropertyIndices( indices );
  DALI_TEST_CHECK( indices.Size() );
  DALI_TEST_EQUALS( indices.Size(), actor.GetPropertyCount(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliHandleRegisterPropertyTypes(void)
{
  TestApplication application;

  struct PropertyTypeAnimatable
  {
    const char * name;
    Property::Value value;
    bool animatable;
  };

  Property::Array array;
  Property::Map map;

  PropertyTypeAnimatable properties[] =
  {
    { "Property::BOOLEAN",          true,              true  },
    { "Property::FLOAT",            1.0f,              true  },
    { "Property::INTEGER",          1,                 true  },
    { "Property::VECTOR2",          Vector2::ONE,      true  },
    { "Property::VECTOR3",          Vector3::ONE,      true  },
    { "Property::VECTOR4",          Vector4::ONE,      true  },
    { "Property::MATRIX3",          Matrix3::IDENTITY, true  },
    { "Property::MATRIX",           Matrix::IDENTITY,  true  },
    { "Property::RECTANGLE",        Rect<int>(),       false },
    { "Property::ROTATION",         AngleAxis(),       true  },
    { "Property::STRING",           std::string("Me"), false },
    { "Property::ARRAY",            array,             false },
    { "Property::MAP",              map,               false },
  };

  unsigned int numOfProperties( sizeof( properties ) / sizeof( properties[0] ) );

  for ( unsigned int i = 0; i < numOfProperties; ++i )
  {
    tet_printf( "Testing: %s\n", properties[i].name );

    bool exception = false;
    try
    {
      Actor actor = Actor::New();
      actor.RegisterProperty( "manFromDelmonte",   properties[i].value );
    }
    catch (Dali::DaliException& e)
    {
      exception = true;
    }

    DALI_TEST_CHECK( properties[i].animatable != exception );
  }
  END_TEST;
}

int UtcDaliHandleCustomProperty(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  float startValue(1.0f);
  Property::Index index = handle.RegisterProperty( "testProperty",  startValue );
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );

  handle.SetProperty( index, 5.0f );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == 5.0f );
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == 5.0f );
  END_TEST;
}
int UtcDaliHandleWeightNew(void)
{
  TestApplication application;

  Handle handle = WeightObject::New();;
  DALI_TEST_CHECK( handle.GetProperty<float>(WeightObject::WEIGHT) == 0.0f );

  END_TEST;
}
