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
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>

#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/actor-attachments/actor-attachment-impl.h>
#include <dali/internal/event/animation/constraint-impl.h>

namespace Dali
{

Actor::Actor()
{
}

Actor Actor::New()
{
  Internal::ActorPtr internal = Internal::Actor::New();

  return Actor(internal.Get());
}

Actor Actor::DownCast( BaseHandle handle )
{
  return Actor( dynamic_cast<Dali::Internal::Actor*>(handle.GetObjectPtr()) );
}

Actor::~Actor()
{
}

Actor::Actor(const Actor& copy)
: Handle(copy)
{
}

Actor& Actor::operator=(const Actor& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

const std::string& Actor::GetName() const
{
  return GetImplementation(*this).GetName();
}

void Actor::SetName(const std::string& name)
{
  GetImplementation(*this).SetName(name);
}

unsigned int Actor::GetId() const
{
  return GetImplementation(*this).GetId();
}

bool Actor::IsRoot() const
{
  return GetImplementation(*this).IsRoot();
}

bool Actor::OnStage() const
{
  return GetImplementation(*this).OnStage();
}

bool Actor::IsLayer() const
{
  return GetImplementation(*this).IsLayer();
}

Layer Actor::GetLayer()
{
  return GetImplementation(*this).GetLayer();
}

void Actor::Add(Actor actor)
{
  GetImplementation(*this).Add(GetImplementation(actor));
}

void Actor::Insert(unsigned int index, Actor actor)
{
  GetImplementation(*this).Insert(index, GetImplementation(actor));
}

void Actor::Remove(Actor actor)
{
  GetImplementation(*this).Remove(GetImplementation(actor));
}

void Actor::Unparent()
{
  GetImplementation(*this).Unparent();
}

unsigned int Actor::GetChildCount() const
{
  return GetImplementation(*this).GetChildCount();
}

Actor Actor::GetChildAt(unsigned int index) const
{
  return GetImplementation(*this).GetChildAt(index);
}

Actor Actor::FindChildByName(const std::string& actorName)
{
  Internal::ActorPtr child = GetImplementation(*this).FindChildByName(actorName);
  return Actor(child.Get());
}

Actor Actor::FindChildById(const unsigned int id)
{
  Internal::ActorPtr child = GetImplementation(*this).FindChildById(id);
  return Actor(child.Get());
}

Actor Actor::GetParent() const
{
  Internal::Actor* parent = GetImplementation(*this).GetParent();

  return Actor(parent);
}

void Actor::SetParentOrigin(const Vector3& origin)
{
  GetImplementation(*this).SetParentOrigin(origin);
}

Vector3 Actor::GetCurrentParentOrigin() const
{
  return GetImplementation(*this).GetCurrentParentOrigin();
}

void Actor::SetAnchorPoint(const Vector3& anchorPoint)
{
  GetImplementation(*this).SetAnchorPoint(anchorPoint);
}

Vector3 Actor::GetCurrentAnchorPoint() const
{
  return GetImplementation(*this).GetCurrentAnchorPoint();
}

void Actor::SetSize(float width, float height)
{
  GetImplementation(*this).SetSize(width, height);
}

void Actor::SetSize(float width, float height, float depth)
{
  GetImplementation(*this).SetSize(width, height, depth);
}

void Actor::SetSize(const Vector2& size)
{
  GetImplementation(*this).SetSize(size);
}

void Actor::SetSize(const Vector3& size)
{
  GetImplementation(*this).SetSize(size);
}

Vector3 Actor::GetSize() const
{
  return GetImplementation(*this).GetSize();
}

Vector3 Actor::GetCurrentSize() const
{
  return GetImplementation(*this).GetCurrentSize();
}

Vector3 Actor::GetNaturalSize() const
{
  return GetImplementation(*this).GetNaturalSize();
}

void Actor::SetPosition(float x, float y)
{
  GetImplementation(*this).SetPosition(x, y);
}

void Actor::SetPosition(float x, float y, float z)
{
  GetImplementation(*this).SetPosition(x, y, z);
}

void Actor::SetPosition(const Vector3& position)
{
  GetImplementation(*this).SetPosition(position);
}

void Actor::SetX(float x)
{
  GetImplementation(*this).SetX(x);
}

void Actor::SetY(float y)
{
  GetImplementation(*this).SetY(y);
}

void Actor::SetZ(float z)
{
  GetImplementation(*this).SetZ(z);
}

void Actor::TranslateBy(const Vector3& distance)
{
  GetImplementation(*this).TranslateBy(distance);
}

Vector3 Actor::GetCurrentPosition() const
{
  return GetImplementation(*this).GetCurrentPosition();
}

Vector3 Actor::GetCurrentWorldPosition() const
{
  return GetImplementation(*this).GetCurrentWorldPosition();
}

void Actor::SetPositionInheritanceMode( PositionInheritanceMode mode )
{
  GetImplementation(*this).SetPositionInheritanceMode( mode );
}

PositionInheritanceMode Actor::GetPositionInheritanceMode() const
{
  return GetImplementation(*this).GetPositionInheritanceMode();
}

void Actor::SetOrientation(const Degree& angle, const Vector3& axis)
{
  GetImplementation(*this).SetOrientation(Radian(angle), axis);
}

void Actor::SetOrientation(const Radian& angle, const Vector3& axis)
{
  GetImplementation(*this).SetOrientation(angle, axis);
}

void Actor::SetOrientation(const Quaternion& orientation)
{
  GetImplementation(*this).SetOrientation(orientation);
}

void Actor::RotateBy(const Degree& angle, const Vector3& axis)
{
  GetImplementation(*this).RotateBy(Radian(angle), axis);
}

void Actor::RotateBy(const Radian& angle, const Vector3& axis)
{
  GetImplementation(*this).RotateBy(angle, axis);
}

void Actor::RotateBy(const Quaternion& relativeRotation)
{
  GetImplementation(*this).RotateBy(relativeRotation);
}

Quaternion Actor::GetCurrentOrientation() const
{
  return GetImplementation(*this).GetCurrentOrientation();
}

void Actor::SetInheritOrientation(bool inherit)
{
  GetImplementation(*this).SetInheritOrientation(inherit);
}

bool Actor::IsOrientationInherited() const
{
  return GetImplementation(*this).IsOrientationInherited();
}

Quaternion Actor::GetCurrentWorldOrientation() const
{
  return GetImplementation(*this).GetCurrentWorldOrientation();
}

void Actor::SetScale(float scale)
{
  GetImplementation(*this).SetScale(scale);
}

void Actor::SetScale(float scaleX, float scaleY, float scaleZ)
{
  GetImplementation(*this).SetScale(scaleX, scaleY, scaleZ);
}

void Actor::SetScale(const Vector3& scale)
{
  GetImplementation(*this).SetScale(scale);
}

void Actor::ScaleBy(const Vector3& relativeScale)
{
  GetImplementation(*this).ScaleBy(relativeScale);
}

Vector3 Actor::GetCurrentScale() const
{
  return GetImplementation(*this).GetCurrentScale();
}

Vector3 Actor::GetCurrentWorldScale() const
{
  return GetImplementation(*this).GetCurrentWorldScale();
}

void Actor::SetInheritScale( bool inherit )
{
  GetImplementation(*this).SetInheritScale( inherit );
}

bool Actor::IsScaleInherited() const
{
  return GetImplementation(*this).IsScaleInherited();
}

void Actor::SetSizeMode(SizeMode mode)
{
  GetImplementation(*this).SetSizeMode(mode);
}

SizeMode Actor::GetSizeMode() const
{
  return GetImplementation(*this).GetSizeMode();
}

void Actor::SetSizeModeFactor(const Vector3& factor)
{
  GetImplementation(*this).SetSizeModeFactor(factor);
}

Vector3 Actor::GetSizeModeFactor() const
{
  return GetImplementation(*this).GetSizeModeFactor();
}

Matrix Actor::GetCurrentWorldMatrix() const
{
  return GetImplementation(*this).GetCurrentWorldMatrix();
}

void Actor::SetVisible(bool visible)
{
  GetImplementation(*this).SetVisible(visible);
}

bool Actor::IsVisible() const
{
  return GetImplementation(*this).IsVisible();
}

void Actor::SetOpacity(float opacity)
{
  GetImplementation(*this).SetOpacity(opacity);
}

float Actor::GetCurrentOpacity() const
{
  return GetImplementation(*this).GetCurrentOpacity();
}

void Actor::SetColor(const Vector4& color)
{
  GetImplementation(*this).SetColor(color);
}

Vector4 Actor::GetCurrentColor() const
{
  return GetImplementation(*this).GetCurrentColor();
}

void Actor::SetColorMode( ColorMode colorMode )
{
  GetImplementation(*this).SetColorMode(colorMode);
}

ColorMode Actor::GetColorMode() const
{
  return GetImplementation(*this).GetColorMode();
}

Vector4 Actor::GetCurrentWorldColor() const
{
  return GetImplementation(*this).GetCurrentWorldColor();
}

void Actor::SetDrawMode( DrawMode::Type drawMode )
{
  GetImplementation(*this).SetDrawMode( drawMode );
}

DrawMode::Type Actor::GetDrawMode() const
{
  return GetImplementation(*this).GetDrawMode();
}

void Actor::SetSensitive(bool sensitive)
{
  GetImplementation(*this).SetSensitive(sensitive);
}

bool Actor::IsSensitive() const
{
  return GetImplementation(*this).IsSensitive();
}

bool Actor::ScreenToLocal(float& localX, float& localY, float screenX, float screenY) const
{
  return GetImplementation(*this).ScreenToLocal(localX, localY, screenX, screenY);
}

void Actor::SetLeaveRequired(bool required)
{
  GetImplementation(*this).SetLeaveRequired(required);
}

bool Actor::GetLeaveRequired() const
{
  return GetImplementation(*this).GetLeaveRequired();
}

void Actor::SetKeyboardFocusable( bool focusable )
{
  GetImplementation(*this).SetKeyboardFocusable(focusable);
}

bool Actor::IsKeyboardFocusable() const
{
  return GetImplementation(*this).IsKeyboardFocusable();
}

Actor::TouchSignalType& Actor::TouchedSignal()
{
  return GetImplementation(*this).TouchedSignal();
}

Actor::HoverSignalType& Actor::HoveredSignal()
{
  return GetImplementation(*this).HoveredSignal();
}

Actor::MouseWheelEventSignalType& Actor::MouseWheelEventSignal()
{
  return GetImplementation(*this).MouseWheelEventSignal();
}

Actor::OnStageSignalType& Actor::OnStageSignal()
{
  return GetImplementation(*this).OnStageSignal();
}

Actor::OffStageSignalType& Actor::OffStageSignal()
{
  return GetImplementation(*this).OffStageSignal();
}

Actor::Actor(Internal::Actor* internal)
: Handle(internal)
{
}

void UnparentAndReset( Actor& actor )
{
  if( actor )
  {
    actor.Unparent();
    actor.Reset();
  }
}

} // namespace Dali
