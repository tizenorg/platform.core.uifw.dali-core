#ifndef __TEST_DYNAMICS_H__
#define __TEST_DYNAMICS_H__

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

#include <dali/integration-api/dynamics/dynamics-body-intf.h>
#include <dali/integration-api/dynamics/dynamics-body-settings.h>
#include <dali/integration-api/dynamics/dynamics-joint-intf.h>
#include <dali/integration-api/dynamics/dynamics-shape-intf.h>
#include <dali/integration-api/dynamics/dynamics-world-intf.h>
#include <dali/integration-api/dynamics/dynamics-world-settings.h>
#include <dali/integration-api/dynamics/dynamics-factory-intf.h>

namespace Dali
{

class TestDynamicsJoint : public Integration::DynamicsJoint
{
public:
  TestDynamicsJoint( TraceCallStack& trace )
  : mTrace( trace )
  {
    mTrace.PushCall( "DynamicsJoint::DynamicsJoint", "" );
  }

  virtual ~TestDynamicsJoint()
  {
    mTrace.PushCall( "DynamicsJoint::~DynamicsJoint", "" );
  }

public: // From Dali::Integration::DynamicsJoint

  void Initialize( Integration::DynamicsBody* bodyA, const Vector3& positionA, const Quaternion& rotationA, const Vector3& offsetA,
                   Integration::DynamicsBody* bodyB, const Vector3& positionB, const Quaternion& rotationB, const Vector3& offsetB )
  {
    mTrace.PushCall( "DynamicsJoint::Initialize", "" );
  }

  void SetLimit( const int axisIndex, const float lowerLimit, const float upperLimit )
  {
    mTrace.PushCall( "DynamicsJoint::SetLimit", "" );
  }

  void EnableSpring( int axisIndex, bool flag )
  {
    mTrace.PushCall( "DynamicsJoint::EnableSpring", "" );
  }

  void SetSpringStiffness( int axisIndex, float stiffness )
  {
    mTrace.PushCall( "DynamicsJoint::SetSpringStiffness", "" );
  }

  void SetSpringDamping( int axisIndex, float damping )
  {
    mTrace.PushCall( "DynamicsJoint::SetSpringDamping", "" );
  }

  void SetSpringCenterPoint( int axisIndex, float ratio )
  {
    mTrace.PushCall( "DynamicsJoint::SetSpringCenterPoint", "" );
  }

  void EnableMotor( int axisIndex, bool flag )
  {
    mTrace.PushCall( "DynamicsJoint::EnableMotor", "" );
  }

  void SetMotorVelocity( int axisIndex, float velocity )
  {
    mTrace.PushCall( "DynamicsJoint::SetMotorVelocity", "" );
  }

  void SetMotorForce( int axisIndex, float force )
  {
    mTrace.PushCall( "DynamicsJoint::SetMotorForce", "" );
  }

private:

  TraceCallStack& mTrace;

}; // class TestDynamicsJoint

class TestDynamicsShape : public Integration::DynamicsShape
{
public:

  TestDynamicsShape( TraceCallStack& trace )
  : mVertices( NULL ),
    mFaces( NULL ),
    mTrace( trace )
  {
    mTrace.PushCall( "DynamicsShape::DynamicsShape", "" );
  }

  virtual ~TestDynamicsShape()
  {
    mTrace.PushCall( "DynamicsShape::~DynamicsShape", "" );
  }

public:
  void Initialize( int type, const Vector3& dimensions )
  {
    mTrace.PushCall( "DynamicsShape::Initialize", "" );
  }

  void Initialize( int type, const MeshData::VertexContainer& vertices, const MeshData::FaceIndices& faceIndices )
  {
    mTrace.PushCall( "DynamicsShape::Initialize", "mesh" );
  }

public:

  MeshData::VertexContainer* mVertices;
  MeshData::FaceIndices*     mFaces;

private:

  TraceCallStack& mTrace;

}; // class TestDynamicsShape

class TestDynamicsBody : public Integration::DynamicsBody
{
public:
  TestDynamicsBody( TraceCallStack& trace )
  : mSettings( NULL ),
    mConserveVolume( false ),
    mConserveShape( false ),
    mKinematic( false ),
    mActivationState( true ),
    mCollisionGroup( 0 ),
    mCollisionMask( 0 ),
    mTrace( trace )
  {
    mTrace.PushCall( "DynamicsBody::DynamicsBody", "" );
  }

  virtual ~TestDynamicsBody()
  {
    mTrace.PushCall( "DynamicsBody::~DynamicsBody", "" );
  }

public: // From Dali::Integration::DynamicsBody

  void Initialize( const Integration::DynamicsBodySettings& bodySettings, Integration::DynamicsShape* shape,
                   Integration::DynamicsWorld& dynamicsWorld,
                   const Vector3& startPosition, const Quaternion& startRotation )
  {
    mTrace.PushCall( "DynamicsBody::~DynamicsBody", "" );
  }

  void SetMass( float mass )
  {
    mTrace.PushCall( "DynamicsBody::SetMass", "" );
  }

  void SetElasticity( float elasticity )
  {
    mTrace.PushCall( "DynamicsBody::SetElasticity", "" );
  }

  void SetLinearVelocity( const Vector3& velocity )
  {
    mTrace.PushCall( "DynamicsBody::SetLinearVelocity", "" );
  }

  Vector3 GetLinearVelocity() const
  {
    mTrace.PushCall( "DynamicsBody::GetLinearVelocity", "" );
    return Vector3::ZERO;
  }

  void SetAngularVelocity( const Vector3& velocity )
  {
    mTrace.PushCall( "DynamicsBody::SetAngularVelocity", "" );
  }

  Vector3 GetAngularVelocity() const
  {
    mTrace.PushCall( "DynamicsBody::GetAngularVelocity", "" );
    return Vector3::ZERO;
  }

  void SetKinematic( bool flag )
  {
    mTrace.PushCall( "DynamicsBody::SetKinematic", "" );
  }

  bool IsKinematic() const
  {
    mTrace.PushCall( "DynamicsBody::IsKinematic", "" );
    return mKinematic;
  }

  void SetSleepEnabled( bool flag )
  {
    mTrace.PushCall( "DynamicsBody::SetSleepEnabled", "" );
  }

  void WakeUp()
  {
    mTrace.PushCall( "DynamicsBody::WakeUp", "" );
  }

  void AddAnchor( unsigned int index, const Integration::DynamicsBody* anchorBody, bool collisions )
  {
    mTrace.PushCall( "DynamicsBody::AddAnchor", "" );
  }

  void ConserveVolume( bool flag )
  {
    mTrace.PushCall( "DynamicsBody::ConserveVolume", "" );
  }

  void ConserveShape( bool flag )
  {
    mTrace.PushCall( "DynamicsBody::ConserveShape", "" );
  }

  short int GetCollisionGroup() const
  {
    mTrace.PushCall( "DynamicsBody::GetCollisionGroup", "" );
    return mCollisionGroup;
  }

  void SetCollisionGroup( short int collisionGroup )
  {
    mTrace.PushCall( "DynamicsBody::SetCollisionGroup", "" );
    mCollisionGroup = collisionGroup;
  }

  short int GetCollisionMask() const
  {
    mTrace.PushCall( "DynamicsBody::GetCollisionMask", "" );
    return mCollisionMask;
  }

  void SetCollisionMask( short int collisionMask )
  {
    mTrace.PushCall( "DynamicsBody::SetCollisionMask", "" );
    mCollisionMask = collisionMask;
  }

  int GetType() const
  {
    mTrace.PushCall( "DynamicsBody::GetType", "" );
    return mSettings->type;
  }

  bool IsActive() const
  {
    mTrace.PushCall( "DynamicsBody::IsActive", "" );
    return mActivationState;
  }

  void SetTransform( const Vector3& position, const Quaternion& rotation )
  {
    mTrace.PushCall( "DynamicsBody::SetTransform", "" );
  }

  void GetTransform( Vector3& position, Quaternion& rotation )
  {
    mTrace.PushCall( "DynamicsBody::GetTransform", "" );
  }

  void GetSoftVertices( MeshData::VertexContainer& vertices ) const
  {
    mTrace.PushCall( "DynamicsBody::GetSoftVertices", "" );
  }

private:
  Dali::Integration::DynamicsBodySettings* mSettings;
  bool mConserveVolume:1;
  bool mConserveShape:1;
  bool mKinematic:1;
  bool mActivationState:1;

  short int mCollisionGroup;
  short int mCollisionMask;

  TraceCallStack& mTrace;
}; // class TestDynamicsBody

class TestDynamicsWorld : public Dali::Integration::DynamicsWorld
{
public:

  TestDynamicsWorld( TraceCallStack& trace )
  : mSettings( NULL ),
    mTrace( trace )
  {
    mTrace.PushCall( "DynamicsWorld::DynamicsWorld", "" );
  }

  virtual ~TestDynamicsWorld()
  {
    mTrace.PushCall( "DynamicsWorld::~DynamicsWorld", "" );
  }

  void Initialize( const Dali::Integration::DynamicsWorldSettings& worldSettings )
  {

    mTrace.PushCall( "DynamicsWorld::Initialize", "" );
  }

  void AddBody( Dali::Integration::DynamicsBody* body )
  {
    mTrace.PushCall( "DynamicsWorld::AddBody", "" );
  }

  void RemoveBody( Dali::Integration::DynamicsBody* body )
  {
    mTrace.PushCall( "DynamicsWorld::RemoveBody", "" );
  }

  void AddJoint( Dali::Integration::DynamicsJoint* joint )
  {
    mTrace.PushCall( "DynamicsWorld::AddJoint", "" );
  }

  void RemoveJoint( Dali::Integration::DynamicsJoint* joint )
  {
    mTrace.PushCall( "DynamicsWorld::RemoveJoint", "" );
  }

  void SetGravity( const Vector3& gravity )
  {
    mTrace.PushCall( "DynamicsWorld::SetGravity", "" );
  }

  void SetDebugDrawMode(int mode)
  {
    mTrace.PushCall( "DynamicsWorld::SetDebugDrawMode", "" );
  }

  const Integration::DynamicsDebugVertexContainer& DebugDraw()
  {
    mTrace.PushCall( "DynamicsWorld::DebugDraw", "" );
    return mDebugVertices;
  }

  void Update( float elapsedSeconds )
  {
    mTrace.PushCall( "DynamicsWorld::Update", "" );
  }

  void CheckForCollisions( Integration::CollisionDataContainer& contacts )
  {
    mTrace.PushCall( "DynamicsWorld::CheckForCollisions", "" );
  }

private:
  Dali::Integration::DynamicsWorldSettings* mSettings;
  Dali::Integration::DynamicsDebugVertexContainer mDebugVertices;

  TraceCallStack& mTrace;

}; // class TestDynamicsWorld

class TestDynamicsFactory : public Integration::DynamicsFactory
{

public:

  TestDynamicsFactory( TraceCallStack& trace ) : mTrace( trace ) {}

  virtual ~TestDynamicsFactory() {}

  virtual bool InitializeDynamics( const Integration::DynamicsWorldSettings& worldSettings ) { return true; }

  virtual void TerminateDynamics() {}

  virtual Integration::DynamicsWorld* CreateDynamicsWorld() { return new TestDynamicsWorld( mTrace ); }

  virtual Integration::DynamicsBody* CreateDynamicsBody() { return new TestDynamicsBody( mTrace ); }

  virtual Integration::DynamicsJoint* CreateDynamicsJoint() { return new TestDynamicsJoint( mTrace ); }

  virtual Integration::DynamicsShape* CreateDynamicsShape() { return new TestDynamicsShape( mTrace ); }

  TraceCallStack& mTrace;
}; // class TestDynamicsFactory

} // Dali

#endif // __TEST_DYNAMICS_H__
