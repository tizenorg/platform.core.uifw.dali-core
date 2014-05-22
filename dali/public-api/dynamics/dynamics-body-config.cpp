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

// CLASS HEADER
#include <dali/public-api/dynamics/dynamics-body-config.h>

// INTERNAL HEADERS
#ifdef DYNAMICS_SUPPORT
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/event/dynamics/dynamics-body-config-impl.h>
#include <dali/internal/event/dynamics/dynamics-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#endif

namespace Dali
{

DynamicsBodyConfig DynamicsBodyConfig::New()
{
#ifdef DYNAMICS_SUPPORT
  Internal::DynamicsBodyConfigPtr internal( new Internal::DynamicsBodyConfig() );

  return DynamicsBodyConfig( internal.Get() );
#else
  return DynamicsBodyConfig();
#endif
}

DynamicsBodyConfig::DynamicsBodyConfig()
{
}

DynamicsBodyConfig::~DynamicsBodyConfig()
{
}

void DynamicsBodyConfig::SetType( const DynamicsBodyConfig::BodyType type )
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetType( type );
#endif
}

DynamicsBodyConfig::BodyType DynamicsBodyConfig::GetType() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetType();
#else
  return DynamicsBodyConfig::RIGID;
#endif
}

void DynamicsBodyConfig::SetShape( const DynamicsShape::ShapeType type, const Vector3& dimensions )
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetShape( type, dimensions );
#endif
}

void DynamicsBodyConfig::SetShape( DynamicsShape shape )
{
#ifdef DYNAMICS_SUPPORT
  Internal::DynamicsShapePtr internal( &(GetImplementation(shape)) );

  GetImplementation(*this).SetShape( internal );
#endif
}

DynamicsShape DynamicsBodyConfig::GetShape() const
{
#ifdef DYNAMICS_SUPPORT
  Internal::DynamicsShapePtr internal( GetImplementation(*this).GetShape() );

  return DynamicsShape( internal.Get() );
#else
  return DynamicsShape();
#endif
}

float DynamicsBodyConfig::GetMass() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetMass();
#else
  return float();
#endif
}

void DynamicsBodyConfig::SetMass( float mass )
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetMass( mass );
#endif
}

float DynamicsBodyConfig::GetElasticity() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetElasticity();
#else
  return float();
#endif
}

void DynamicsBodyConfig::SetElasticity( float elasticity )
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetElasticity( elasticity );
#endif
}

float DynamicsBodyConfig::GetFriction() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetFriction();
#else
  return float();
#endif
}

void DynamicsBodyConfig::SetFriction(float friction)
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetFriction( friction );
#endif
}

float DynamicsBodyConfig::GetLinearDamping() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetLinearDamping();
#else
  return float();
#endif
}

void DynamicsBodyConfig::SetLinearDamping( float damping )
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetLinearDamping( damping );
#endif
}

float DynamicsBodyConfig::GetAngularDamping() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetAngularDamping();
#else
  return float();
#endif
}

void DynamicsBodyConfig::SetAngularDamping(float damping)
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetAngularDamping( damping );
#endif
}

float DynamicsBodyConfig::GetLinearSleepVelocity() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetLinearSleepVelocity();
#else
  return float();
#endif
}

void DynamicsBodyConfig::SetLinearSleepVelocity( float sleepVelocity )
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetLinearSleepVelocity( sleepVelocity );
#endif
}

float DynamicsBodyConfig::GetAngularSleepVelocity() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetAngularSleepVelocity();
#else
  return float();
#endif
}

void DynamicsBodyConfig::SetAngularSleepVelocity(float sleepVelocity)
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetAngularSleepVelocity( sleepVelocity );
#endif
}

short int DynamicsBodyConfig::GetCollisionGroup() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetCollisionGroup();
#else
  return short();
#endif
}

void DynamicsBodyConfig::SetCollisionGroup(const short int collisionGroup)
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetCollisionGroup(collisionGroup);
#endif
}

short int DynamicsBodyConfig::GetCollisionMask() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetCollisionMask();
#else
  return short();
#endif
}

void DynamicsBodyConfig::SetCollisionMask(const short int collisionMask)
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetCollisionMask(collisionMask);
#endif
}

float DynamicsBodyConfig::GetStiffness() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetStiffness();
#else
  return float();
#endif
}

void DynamicsBodyConfig::SetStiffness( float stiffness )
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetStiffness( stiffness );
#endif
}

float DynamicsBodyConfig::GetAnchorHardness() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetAnchorHardness();
#else
  return float();
#endif
}

void DynamicsBodyConfig::SetAnchorHardness( float hardness )
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetAnchorHardness( hardness );
#endif
}

float DynamicsBodyConfig::GetVolumeConservation() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetVolumeConservation();
#else
  return float();
#endif
}

void DynamicsBodyConfig::SetVolumeConservation(float conservation)
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetVolumeConservation(conservation);
#endif
}

float DynamicsBodyConfig::GetShapeConservation() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetShapeConservation();
#else
  return float();
#endif
}

void DynamicsBodyConfig::SetShapeConservation(float conservation)
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetShapeConservation(conservation);
#endif
}

DynamicsBodyConfig::DynamicsBodyConfig( Internal::DynamicsBodyConfig* internal )
#ifdef DYNAMICS_SUPPORT
: BaseHandle(internal)
#else
: BaseHandle(NULL)
#endif
{
}

} // namespace Dali
