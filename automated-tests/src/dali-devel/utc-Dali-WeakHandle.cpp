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
#include <dali/devel-api/object/weak-handle.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

namespace
{

/*******************************************************************************
 *
 * Custom Actor
 *
 ******************************************************************************/
namespace Impl
{
struct MyTestCustomActor : public CustomActorImpl
{
  typedef Signal< void ()> SignalType;
  typedef Signal< void (float)> SignalTypeFloat;

  MyTestCustomActor() : CustomActorImpl( ActorFlags( REQUIRES_TOUCH_EVENTS ) )
  { }

  virtual ~MyTestCustomActor()
  { }

  void ResetCallStack()
  {
  }

  // From CustomActorImpl
  virtual void OnStageConnection( int depth )
  {
  }
  virtual void OnStageDisconnection()
  {
  }
  virtual void OnChildAdd(Actor& child)
  {
  }
  virtual void OnChildRemove(Actor& child)
  {
  }
  virtual void OnSizeSet(const Vector3& targetSize)
  {
  }
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize)
  {
  }
  virtual bool OnTouchEvent(const TouchEvent& event)
  {
    return true;
  }
  virtual bool OnHoverEvent(const HoverEvent& event)
  {
    return true;
  }
  virtual bool OnWheelEvent(const WheelEvent& event)
  {
    return true;
  }
  virtual bool OnKeyEvent(const KeyEvent& event)
  {
    return true;
  }
  virtual void OnKeyInputFocusGained()
  {
  }
  virtual void OnKeyInputFocusLost()
  {
  }
  virtual Vector3 GetNaturalSize()
  {
    return Vector3( 0.0f, 0.0f, 0.0f );
  }

  virtual float GetHeightForWidth( float width )
  {
    return 0.0f;
  }

  virtual float GetWidthForHeight( float height )
  {
    return 0.0f;
  }

  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container )
  {
  }

  virtual void OnSetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension )
  {
  }

  virtual void OnCalculateRelayoutSize( Dimension::Type dimension )
  {
  }

  virtual float CalculateChildSize( const Dali::Actor& child, Dimension::Type dimension )
  {
    return 0.0f;
  }

  virtual void OnLayoutNegotiated( float size, Dimension::Type dimension )
  {
  }

  virtual bool RelayoutDependentOnChildren( Dimension::Type dimension = Dimension::ALL_DIMENSIONS )
  {
    return false;
  }

public:

  SignalType mSignal;
};

}; // namespace Impl

class MyTestCustomActor : public CustomActor
{
public:

  typedef Signal< void ()> SignalType;
  typedef Signal< void (float)> SignalTypeFloat;

  MyTestCustomActor()
  {
  }

  static MyTestCustomActor New()
  {
    Impl::MyTestCustomActor* p = new Impl::MyTestCustomActor;
    return MyTestCustomActor( *p ); // takes ownership
  }

  virtual ~MyTestCustomActor()
  {
  }

  static MyTestCustomActor DownCast( BaseHandle handle )
  {
    MyTestCustomActor result;

    CustomActor custom = Dali::CustomActor::DownCast( handle );
    if ( custom )
    {
      CustomActorImpl& customImpl = custom.GetImplementation();

      Impl::MyTestCustomActor* impl = dynamic_cast<Impl::MyTestCustomActor*>(&customImpl);

      if (impl)
      {
        result = MyTestCustomActor(customImpl.GetOwner());
      }
    }

    return result;
  }

  SignalType& GetCustomSignal()
  {
    Dali::RefObject& obj = GetImplementation();
    return static_cast<Impl::MyTestCustomActor&>( obj ).mSignal;
  }

  MyTestCustomActor(Internal::CustomActor* internal)
  : CustomActor(internal)
  {
  }

  MyTestCustomActor( Impl::MyTestCustomActor& impl )
  : CustomActor( impl )
  {
  }
};

}

int UtcDaliWeakHandleConstructorVoid(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::WeakHandle::WeakHandle()");

  WeakHandle object;

  DALI_TEST_CHECK(!object.GetHandle());

  END_TEST;
}

int UtcDaliWeakHandleConstructorWithHandle(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::WeakHandle::WeakHandle(Handle)");

  Handle emptyHandle;
  WeakHandle emptyObject(emptyHandle);
  DALI_TEST_CHECK(!emptyObject.GetHandle());

  Actor actor = Actor::New();
  WeakHandle object(actor);
  DALI_TEST_CHECK(object.GetHandle() == actor);

  END_TEST;
}

int UtcDaliWeakHandleCopyConstructor(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::WeakHandle::WeakHandle(const WeakHandle&)");

  Actor actor = Actor::New();
  DALI_TEST_EQUALS(1, actor.GetBaseObject().ReferenceCount(), TEST_LOCATION); // reference count of the actor is not increased

  WeakHandle object(actor);
  DALI_TEST_CHECK(object.GetHandle() == actor);
  DALI_TEST_EQUALS(1, actor.GetBaseObject().ReferenceCount(), TEST_LOCATION); // reference count of the actor is not increased

  WeakHandle copy(object);
  DALI_TEST_CHECK(copy.GetHandle() == actor);
  DALI_TEST_EQUALS(1, actor.GetBaseObject().ReferenceCount(), TEST_LOCATION); // reference count of the actor is not increased

  END_TEST;
}

int UtcDaliWeakHandleAssignmentOperator(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::WeakHandle::operator=");

  Actor actor = Actor::New();
  DALI_TEST_EQUALS(1, actor.GetBaseObject().ReferenceCount(), TEST_LOCATION); // reference count of the actor is not increased

  WeakHandle object(actor);
  DALI_TEST_CHECK(object.GetHandle() == actor);
  DALI_TEST_EQUALS(1, actor.GetBaseObject().ReferenceCount(), TEST_LOCATION); // reference count of the actor is not increased

  WeakHandle copy = object;
  DALI_TEST_CHECK(copy.GetHandle() == actor);
  DALI_TEST_EQUALS(1, actor.GetBaseObject().ReferenceCount(), TEST_LOCATION); // reference count of the actor is not increased

  END_TEST;
}

int UtcDaliWeakHandleEqualityOperatorP(void)
{
  TestApplication application;
  tet_infoline("Positive Test Dali::WeakHandle::operator==");

  WeakHandle object;
  WeakHandle theSameObject;
  DALI_TEST_CHECK(object == theSameObject);

  Actor actor = Actor::New();

  object = WeakHandle(actor);
  DALI_TEST_CHECK(object.GetHandle() == actor);

  theSameObject = object;
  DALI_TEST_CHECK(theSameObject.GetHandle() == actor);
  DALI_TEST_CHECK(object == theSameObject);

  END_TEST;
}

int UtcDaliWeakHandleEqualityOperatorN(void)
{
  TestApplication application;
  tet_infoline("Negative Test Dali::WeakHandle::operator==");

  Actor actor = Actor::New();

  WeakHandle object(actor);
  DALI_TEST_CHECK(object.GetHandle() == actor);

  Actor differentActor = Actor::New();
  WeakHandle aDifferentWeakHandle(differentActor);

  DALI_TEST_CHECK(!(object == aDifferentWeakHandle));

  END_TEST;
}

int UtcDaliWeakHandleInequalityOperatorP(void)
{
  TestApplication application;
  tet_infoline("Positive Test Dali::WeakHandle::operator!=");

  Actor actor = Actor::New();

  WeakHandle object(actor);
  DALI_TEST_CHECK(object.GetHandle() == actor);

  Actor differentActor = Actor::New();
  WeakHandle aDifferentWeakHandle(differentActor);

  DALI_TEST_CHECK(object != aDifferentWeakHandle);
  END_TEST;
}

int UtcDaliWeakHandleInequalityOperatorN(void)
{
  TestApplication application;
  tet_infoline("Negative Test Dali::WeakHandle::operator!=");

  Actor actor = Actor::New();

  WeakHandle object(actor);
  DALI_TEST_CHECK(object.GetHandle() == actor);

  WeakHandle theSameWeakHandle = object;

  DALI_TEST_CHECK(!(object != theSameWeakHandle));
  END_TEST;
}

int UtcDaliWeakHandleGetHandle(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::WeakHandle::GetHandle()");

  Handle emptyHandle;
  WeakHandle emptyObject(emptyHandle);
  DALI_TEST_CHECK(!emptyObject.GetHandle());

  Actor actor = Actor::New();
  WeakHandle object(actor);
  DALI_TEST_CHECK(object.GetHandle() == actor);

  WeakHandle theSameObject = WeakHandle(actor);
  DALI_TEST_CHECK(object.GetHandle() == theSameObject.GetHandle());

  Actor differentActor = Actor::New();
  WeakHandle aDifferentWeakHandle(differentActor);
  DALI_TEST_CHECK(object.GetHandle() != aDifferentWeakHandle.GetHandle());

  END_TEST;
}

int UtcDaliWeakHandleGetObject(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::WeakHandle::GetObject()");

  ImageActor actor = ImageActor::New();
  WeakHandle imageObject(actor);
  DALI_TEST_CHECK(imageObject.GetHandle() == actor);

  ImageActor imageActor = imageObject.GetObject<ImageActor, Internal::ImageActor>();
  DALI_TEST_CHECK(imageActor == actor);

  MyTestCustomActor customActor = MyTestCustomActor::New();
  WeakHandle customObject(customActor);
  DALI_TEST_CHECK(customObject.GetHandle() == customActor);

  MyTestCustomActor testActor = customObject.GetObject<MyTestCustomActor, Internal::CustomActor>();
  DALI_TEST_CHECK(testActor == customActor);

  END_TEST;
}



