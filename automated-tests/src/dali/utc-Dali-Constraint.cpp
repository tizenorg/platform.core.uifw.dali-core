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

void utc_dali_constraint_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_constraint_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////
namespace
{
template< typename T >
struct BasicFunctor
{
  void operator()( T& /* current */, const PropertyInputContainer& /* inputs */ )
  {
  }
};

template< typename T >
struct BasicCalledFunctor
{
  BasicCalledFunctor( bool& functorCalled ) : mCalled( functorCalled ) { }

  void operator()( T& /* current */, const PropertyInputContainer& /* inputs */ )
  {
    mCalled = true;
  }

  bool& mCalled;
};

template< typename T >
struct CalledCountFunctor
{
  CalledCountFunctor( int& callCount ) : mCallCount( callCount ) { }

  void operator()( T& /* current */, const PropertyInputContainer& /* inputs */ )
  {
    ++mCallCount;
  }

  int& mCallCount;
};

template< typename T >
struct SetValueFunctor
{
  SetValueFunctor( const T& value ) : mValue( value ) { }

  void operator()( T& current, const PropertyInputContainer& /* inputs */ )
  {
    current = mValue;
  }

  T mValue;
};
} // unnamed namespace
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::New(
//   Handle,
//   Property::Index,
//   void( *function )( T&, const PropertyInputContainer& ) )
///////////////////////////////////////////////////////////////////////////////
namespace UtcDaliConstraintNewFunction
{
bool gConstraintFunctionCalled = false;
void ConstraintFunction( Vector3& /* current */, const PropertyInputContainer& /* inputs */ )
{
  gConstraintFunctionCalled = true;
}
} // namespace UtcDaliConstraintNewFunction

int UtcDaliConstraintNewFunctionP(void)
{
  // Ensure that we can create a constraint using a C function and that it is called.

  TestApplication application;
  UtcDaliConstraintNewFunction::gConstraintFunctionCalled = false;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( UtcDaliConstraintNewFunction::gConstraintFunctionCalled, false, TEST_LOCATION );

  // Add a constraint
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, &UtcDaliConstraintNewFunction::ConstraintFunction );
  DALI_TEST_CHECK( constraint );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( UtcDaliConstraintNewFunction::gConstraintFunctionCalled, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintNewFunctionN(void)
{
  // Create a constraint with an uninitialised handle

  TestApplication application;

  // Add a constraint with an uninitialised handle
  try
  {
    Constraint constraint = Constraint::New< Vector3 >( Actor(), Actor::Property::POSITION, &UtcDaliConstraintNewFunction::ConstraintFunction );
    DALI_TEST_CHECK( false ); // Should not reach here
  }
  catch ( ... )
  {
    DALI_TEST_CHECK( true ); // Should assert!
  }

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::New(
//   Handle,
//   Property::Index,
//   const T& object )
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintNewFunctorP(void)
{
  // Ensure that we can create a constraint using a functor and that it is called.

  TestApplication application;
  bool functorCalled = false;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( functorCalled, false, TEST_LOCATION );

  // Add a constraint
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, BasicCalledFunctor< Vector3 >( functorCalled ) );
  DALI_TEST_CHECK( constraint );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( functorCalled, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintNewFunctorN(void)
{
  // Create a constraint with an uninitialised handle

  TestApplication application;
  bool functorCalled = false;

  // Add a constraint with an uninitialised handle
  try
  {
    Constraint constraint = Constraint::New< Vector3 >( Actor(), Actor::Property::POSITION, BasicCalledFunctor< Vector3 >( functorCalled ) );
    DALI_TEST_CHECK( false ); // Should not reach here
  }
  catch ( ... )
  {
    DALI_TEST_CHECK( true ); // Should assert!
  }

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::New(
//   Handle,
//   Property::Index,
//   const T& object,
//   void ( T::*memberFunction ) ( P&, const PropertyInputContainer& ) )
///////////////////////////////////////////////////////////////////////////////
namespace UtcDaliConstraintNewFunctorMember
{
struct Functor
{
  Functor( bool& positionCalled, bool& scaleCalled )
  : mPositionCalled( positionCalled ),
    mScaleCalled( scaleCalled )
  {
  }

  void Position( Vector3& /* current */, const PropertyInputContainer& /* inputs */ )
  {
    mPositionCalled = true;
  }

  void Scale( Vector3& /* current */, const PropertyInputContainer& /* inputs */ )
  {
    mScaleCalled = true;
  }

  bool& mPositionCalled;
  bool& mScaleCalled;
};
} // namespace UtcDaliConstraintNewFunctorMember

int UtcDaliConstraintNewFunctorMemberP(void)
{
  // Ensure that we can create a constraint using a functor and that it is called.

  TestApplication application;
  bool positionFunctorCalled = false;
  bool sizeFunctorCalled = false;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( positionFunctorCalled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( sizeFunctorCalled, false, TEST_LOCATION );

  // Add a constraint that calls Functor::Position
  Constraint constraint = Constraint::New< Vector3 >(
      actor,
      Actor::Property::POSITION,
      UtcDaliConstraintNewFunctorMember::Functor( positionFunctorCalled, sizeFunctorCalled ),
      &UtcDaliConstraintNewFunctorMember::Functor::Position );
  DALI_TEST_CHECK( constraint );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( positionFunctorCalled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( sizeFunctorCalled, false, TEST_LOCATION );

  // Add another constraint that calls Functor::Size
  Constraint constraint2 = Constraint::New< Vector3 >(
      actor,
      Actor::Property::SCALE,
      UtcDaliConstraintNewFunctorMember::Functor( positionFunctorCalled, sizeFunctorCalled ),
      &UtcDaliConstraintNewFunctorMember::Functor::Scale );
  DALI_TEST_CHECK( constraint2 );
  constraint2.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( positionFunctorCalled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( sizeFunctorCalled, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintNewFunctorMemberN(void)
{
  // Create a constraint with an uninitialised handle

  TestApplication application;
  bool positionFunctorCalled = false;
  bool sizeFunctorCalled = false;

  // Add a constraint with an uninitialised handle
  try
  {
    Constraint constraint = Constraint::New< Vector3 >(
        Actor(),
        Actor::Property::POSITION,
        UtcDaliConstraintNewFunctorMember::Functor( positionFunctorCalled, sizeFunctorCalled ),
        &UtcDaliConstraintNewFunctorMember::Functor::Position );
    DALI_TEST_CHECK( false ); // Should not reach here
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_CHECK( true ); // Should assert!
  }

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::Clone
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintCloneP(void)
{
  // Ensure we can clone for another actor and it's called appropriately

  TestApplication application;
  int calledCount = 0;

  Actor actor = Actor::New();
  Actor clone = Actor::New();

  Stage stage = Stage::GetCurrent();
  stage.Add( actor );
  stage.Add( clone );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( calledCount, 0, TEST_LOCATION );

  // Add a constraint to actor
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, CalledCountFunctor< Vector3 >( calledCount ) );
  DALI_TEST_CHECK( constraint );
  constraint.Apply();

  // Create a clone but don't apply
  Constraint constraintClone = constraint.Clone( clone );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( calledCount, 1, TEST_LOCATION );

  // Reset
  calledCount = 0;

  // Ensure constraint isn't called again if scene doesn't change
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( calledCount, 0, TEST_LOCATION );

  // Apply the clone constraint
  constraintClone.Apply();

  application.SendNotification();
  application.Render();

  // Should only be called once for the new constraint clone ONLY
  DALI_TEST_EQUALS( calledCount, 1, TEST_LOCATION );

  // Reset
  calledCount = 0;

  // Change the position of both actors
  actor.SetPosition( 100.0f, 100.0f );
  clone.SetPosition( 100.0f, 100.0f );

  application.SendNotification();
  application.Render();

  // Functor should have been called twice
  DALI_TEST_EQUALS( calledCount, 2, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintCloneN(void)
{
  // Attempt to clone an uninitialised constraint should cause an assert

  TestApplication application;

  Constraint constraint;

  try
  {
    Actor actor = Actor::New();
    Constraint clone = constraint.Clone( actor );
    DALI_TEST_CHECK( false );
  }
  catch ( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

namespace UtcDaliConstraintClone
{
struct Functor
{
  void operator()( Vector3& /* current */, const PropertyInputContainer& inputs )
  {
    DALI_TEST_CHECK( inputs[0]->GetType() == Property::VECTOR3 );
    DALI_TEST_CHECK( inputs[1]->GetType() == Property::ROTATION );
    DALI_TEST_CHECK( inputs[2]->GetType() == Property::VECTOR4 );
    DALI_TEST_CHECK( inputs[3]->GetType() == Property::BOOLEAN );
  }
};
} // namespace UtcDaliConstraintClone

int UtcDaliConstraintCloneCheckSourcesAndSetters(void)
{
  // Ensure all sources, the tag and remove-action are cloned appropriately

  TestApplication application;

  Actor actor = Actor::New();
  Actor clone = Actor::New();

  Stage stage = Stage::GetCurrent();
  stage.Add( actor );
  stage.Add( clone );

  application.SendNotification();
  application.Render();

  // Create a constraint, DON'T Apply it though
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, UtcDaliConstraintClone::Functor() );
  constraint.AddSource( LocalSource( Actor::Property::SIZE ) );
  constraint.AddSource( LocalSource( Actor::Property::ORIENTATION ) );
  constraint.AddSource( LocalSource( Actor::Property::COLOR ) );
  constraint.AddSource( LocalSource( Actor::Property::VISIBLE ) );
  constraint.SetRemoveAction( Constraint::Discard );
  constraint.SetTag( 123 );

  // Clone the constraint & apply the clone
  Constraint constraintClone = constraint.Clone( clone );
  constraintClone.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( constraint.GetRemoveAction(), constraintClone.GetRemoveAction(), TEST_LOCATION );
  DALI_TEST_EQUALS( constraint.GetTag(),          constraintClone.GetTag(),          TEST_LOCATION );

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::Constraint( const Constraint& )
// Constraint::operator=
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintCopyAndAssignment(void)
{
  // Ensure copy constructor & assignment operators work

  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, BasicFunctor< Vector3 >() );
  Constraint copied( constraint );
  Constraint assigned;
  DALI_TEST_CHECK( constraint == copied );
  DALI_TEST_CHECK( copied != assigned );

  assigned = constraint;
  DALI_TEST_CHECK( constraint == assigned );

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::DownCast
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintDownCast(void)
{
  // Ensure DownCast works as expected

  TestApplication application;

  Actor actor = Actor::New();
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, BasicFunctor< Vector3 >() );

  // Another BaseHandle type
  Constraint downCast = Constraint::DownCast( actor );
  DALI_TEST_CHECK( ! downCast );

  // A constraint
  downCast = Constraint::DownCast( constraint );
  DALI_TEST_CHECK( downCast );

  // An empty constraint
  downCast = Constraint::DownCast( Constraint() );
  DALI_TEST_CHECK( ! downCast );

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::GetTargetObject
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintGetTargetObjectP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, BasicFunctor< Vector3 >() );
  DALI_TEST_CHECK( constraint.GetTargetObject() == actor );

  Actor actor2 = Actor::New();
  DALI_TEST_CHECK( constraint.GetTargetObject() != actor2 );

  END_TEST;
}

int UtcDaliConstraintGetTargetObjectN(void)
{
  // Attempt to retrieve from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    Handle handle = constraint.GetTargetObject();
    DALI_TEST_CHECK( false ); // Should not reach here!
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::GetTargetProperty
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintGetTargetPropertyP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, BasicFunctor< Vector3 >() );
  DALI_TEST_EQUALS( constraint.GetTargetProperty(), Actor::Property::POSITION, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintGetTargetPropertyN(void)
{
  // Attempt to retrieve from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    Property::Index propertyIndex = constraint.GetTargetProperty();
    ( void )propertyIndex;
    DALI_TEST_CHECK( false ); // Should not reach here!
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::SetTag
// Constraint::GetTag
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintTagP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, BasicFunctor< Vector3 >() );
  DALI_TEST_EQUALS( constraint.GetTag(), 0, TEST_LOCATION );

  const int tag = 123;
  constraint.SetTag( tag );
  DALI_TEST_EQUALS( constraint.GetTag(), tag, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintSetTagN(void)
{
  // Attempt to set from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    constraint.SetTag( 123 );
    DALI_TEST_CHECK( false ); // Should not reach here!
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliConstraintGetTagN(void)
{
  // Attempt to retrieve from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    int tag = constraint.GetTag();
    ( void )tag;
    DALI_TEST_CHECK( false ); // Should not reach here!
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constraint::SetRemoveAction
// Constraint::GetRemoveAction
///////////////////////////////////////////////////////////////////////////////
int UtcDaliConstraintRemoveActionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, BasicFunctor< Vector3 >() );
  DALI_TEST_EQUALS( constraint.GetRemoveAction(), Constraint::DEFAULT_REMOVE_ACTION, TEST_LOCATION );

  constraint.SetRemoveAction( Constraint::Discard );
  DALI_TEST_EQUALS( constraint.GetRemoveAction(), Constraint::Discard, TEST_LOCATION );

  constraint.SetRemoveAction( Constraint::Bake );
  DALI_TEST_EQUALS( constraint.GetRemoveAction(), Constraint::Bake, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintSetRemoveActionN(void)
{
  // Attempt to set from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    constraint.SetRemoveAction( Constraint::Discard );
    DALI_TEST_CHECK( false ); // Should not reach here!
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliConstraintGetRemoveActionN(void)
{
  // Attempt to retrieve from uninitialised constraint

  TestApplication application;

  Constraint constraint;
  try
  {
    Constraint::RemoveAction removeAction = constraint.GetRemoveAction();
    ( void )removeAction;
    DALI_TEST_CHECK( false ); // Should not reach here!
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliConstraintBakeRemoveAction(void)
{
  // Ensure value is baked when constraint is removed

  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // Should not equal position by default
  Vector3 position( 10.0f, 20.0f, 30.0f );
  DALI_TEST_CHECK( actor.GetCurrentPosition() != position );

  // Create a constraint that constrains to position
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, SetValueFunctor< Vector3 >( position ) );
  constraint.SetRemoveAction( Constraint::Bake );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  // Remove the constraint, it should still be at position
  constraint.Remove();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  END_TEST;
}

int UtcDaliConstraintDiscardRemoveAction(void)
{
  // Ensure value is baked when constraint is removed

  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // Should not equal position by default
  Vector3 position( 10.0f, 20.0f, 30.0f );
  DALI_TEST_CHECK( actor.GetCurrentPosition() != position );

  // Create a constraint that constrains to position
  Constraint constraint = Constraint::New< Vector3 >( actor, Actor::Property::POSITION, SetValueFunctor< Vector3 >( position ) );
  constraint.SetRemoveAction( Constraint::Discard );
  constraint.Apply();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  // Remove the constraint, it should still be at position
  constraint.Remove();

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( actor.GetCurrentPosition() != position );

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////
