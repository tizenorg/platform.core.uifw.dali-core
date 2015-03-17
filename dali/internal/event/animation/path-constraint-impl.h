#ifndef __DALI_INTERNAL_PATH_CONSTRAINT_H__
#define __DALI_INTERNAL_PATH_CONSTRAINT_H__

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

// INTERNAL INCLUDES
#include <dali/internal/event/common/object-impl.h>
#include <dali/public-api/animation/path-constraint.h>

namespace Dali
{

namespace Internal
{

typedef IntrusivePtr<PathConstraint> PathConstraintPtr;
typedef Dali::Vector<Object*>     ObjectContainer;
typedef ObjectContainer::Iterator ObjectIter;

/**
 * A 3D path
 */
class PathConstraint : public Object, public Object::Observer
{
public:

  /**
   * Construct a new PathConstraint
   */
  static PathConstraint* New(Dali::Path path, const Vector2& range );

  /**
   * Constructor
   */
  PathConstraint(Dali::Path path, const Vector2& range );

protected:
  /**
   * virtual destructor
   */
  virtual ~PathConstraint();

private:
  /**
   * @copydoc Dali::Internal::Object::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectAdded(Object& object){}

  /**
   * @copydoc Dali::Internal::Object::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectRemoved(Object& object){}

  /**
   * @copydoc Dali::Internal::Object::Observer::ObjectDestroyed()
   */
  virtual void ObjectDestroyed(Object& object);

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyName()
   */
  virtual const char* GetDefaultPropertyName(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex(const std::string& name) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  virtual void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue);

   /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObject()
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const{ return NULL; }

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const{ return NULL; }

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const{ return NULL; }

public:

  /**
   * @copydoc Dali::PathConstraint::Apply
   */
  void Apply( Property source, Property target, const Vector3& forward );

  /**
   * @copydoc Dali::PathConstraint::Remove
   */
  void Remove( Dali::Handle target );


private:

  /**
   * Undefined
   */
  PathConstraint(const PathConstraint& p);

  /**
   * Undefined
   */
  PathConstraint& operator=(const PathConstraint& rhs);

  Dali::Path        mPath;              ///< The path used to constrain objects
  ObjectContainer   mObservedObjects;   ///< The list of object which have been constrained by the PathConstraint
  Vector2           mRange;             ///< The range of values in the input property which will be mapped to 0..1
};

} // Internal

// Get impl of handle
inline Internal::PathConstraint& GetImplementation(Dali::PathConstraint& pathConstraint)
{
  DALI_ASSERT_ALWAYS( pathConstraint && "Path handle is empty" );
  Dali::RefObject& object = pathConstraint.GetBaseObject();
  return static_cast<Internal::PathConstraint&>(object);
}

inline const Internal::PathConstraint& GetImplementation(const Dali::PathConstraint& pathConstraint)
{
  DALI_ASSERT_ALWAYS( pathConstraint && "Path handle is empty" );
  const Dali::RefObject& object = pathConstraint.GetBaseObject();
  return static_cast<const Internal::PathConstraint&>(object);
}


} // Dali

#endif //__DALI_INTERNAL_KEY_FRAMES_H__
