#ifndef __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_H__

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

// EXTERNAL INCLUDES
#include <algorithm>
#include <utility>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/object/property.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class PropertyBase;
}

/**
 * An entry in an scene-graph property lookup.
 * The type field should be queried, before accessing the scene-graph property:
 *
 * @code
 * void Example(PropertyEntry entry)
 * {
 *   if (entry.mType == Property::VECTOR3)
 *   {
 *     SceneGraph::AnimatableProperty<Vector3>* property = dynamic_cast< SceneGraph::AnimatableProperty<Vector3>* >( entry.property );
 *     ...
 *   }
 * @endcode
 *
 */
class SceneGraphProperty
{
public:

  /**
   * Constructor for an uninitalized scene-graph property
   */
  SceneGraphProperty()
  : mType(Property::NONE),
    mValue(),
    mProperty(NULL)
  {
  }

  /**
   * Constructor for scene-graph owned properties
   * @param [in] newProperty A pointer to the scene-graph owned property.
   */
  SceneGraphProperty(const SceneGraph::PropertyBase* newProperty)
  : mType(Property::NONE),
    mValue(), // value is held by newProperty
    mProperty(newProperty)
  {
    DALI_ASSERT_DEBUG(mProperty && "Uninitialized scenegraph property") ;
  }

  /**
   * Constructor for event side only properties
   * @param [in] newValue The value of the scene-graph owned property.
   */
  SceneGraphProperty(Property::Value newValue)
  : mType(newValue.GetType()),
    mValue(newValue),
    mProperty(NULL)
  {
  }

  virtual ~SceneGraphProperty()
  {
  }

  /**
   * @return true if the property is animatable (i.e. if its a scene graph property)
   */
  bool IsAnimatable(void) const
  {
    return NULL != mProperty;
  }

  /**
   * @return true if the property can be written to
   */
  virtual bool IsWritable(void) const = 0;

  /**
   * @return the scene graph property
   */
  const SceneGraph::PropertyBase* GetSceneGraphProperty() const
  {
    DALI_ASSERT_DEBUG(mProperty && "Accessing uninitialized SceneGraph property") ;
    return mProperty;
  }

  Property::Type mType;    ///< The type of the property
  Property::Value mValue;  ///< The property value for a non animatable and custom property

protected:

  // Not implemented
  SceneGraphProperty( const SceneGraphProperty& );
  SceneGraphProperty& operator=( const SceneGraphProperty& );

  const SceneGraph::PropertyBase* mProperty; ///< A pointer to a scene-graph property; should not be modified from actor-thread.
};


/**
 * An entry in an animatable property lookup.
 * The type field should be queried, before accessing the animatable property:
 */
class AnimatableProperty : public SceneGraphProperty
{
public:

  /**
   * Constructor for animatable properties
   * @param [in] newProperty A pointer to the scene-graph owned property.
   */
  AnimatableProperty( Property::Type newType,
                        const SceneGraph::PropertyBase* newProperty )
  {
    mType = newType;
    mProperty = newProperty;
    DALI_ASSERT_DEBUG(mProperty && "Uninitialized scenegraph property") ;
  }

  /**
   * Constructor for event side only properties
   * @param [in] newValue The value of the scene-graph owned property.
   */
  AnimatableProperty( Property::Value newValue )
  {
    mType = newValue.GetType();
    mValue = newValue;
  }

  /**
   * @return true if the property can be written to
   */
  virtual bool IsWritable(void) const
  {
    return true ;
  }

private:

  // Not implemented
  AnimatableProperty();
  AnimatableProperty( const AnimatableProperty& );
  AnimatableProperty& operator=( const AnimatableProperty& );
};

class CustomProperty : public SceneGraphProperty
{
public:

  /**
   * Constructor for scene graph based properties
   * @param [in] newName The name of the custom property.
   * @param [in] newType The type ID of the custom property.
   * @param [in] newProperty A pointer to the scene-graph owned property.
   */
  CustomProperty( const std::string& newName,
                  Property::Type newType,
                  const SceneGraph::PropertyBase* newProperty)
  : mName(newName),
    mAccessMode(Property::ANIMATABLE)
  {
    mType = newType;
    mProperty = newProperty;
    DALI_ASSERT_DEBUG(mProperty && "Uninitialized scenegraph property") ;
  }

  /**
   * Constructor for event side only properties
   * @param [in] newName The name of the custom property.
   * @param [in] newValue The value of the custom property.
   * @param [in] newProperty A pointer to the scene-graph owned property.
   */
  CustomProperty( const std::string& newName,
                  Property::Value newValue,
                  Property::AccessMode accessMode )
  : mName(newName),
    mAccessMode(accessMode)
  {
    mType = newValue.GetType();
    mValue = newValue;
    DALI_ASSERT_DEBUG(accessMode != Property::ANIMATABLE && "Animatable must have scenegraph property") ;
  }

  /**
   * @return true if the property can be written to
   */
  virtual bool IsWritable(void) const
  {
    return (mAccessMode == Property::ANIMATABLE) || (mAccessMode == Property::READ_WRITE) ;
  }

  std::string mName;       ///< The name of the property

private:

  // Not implemented
  CustomProperty();
  CustomProperty( const CustomProperty& );
  CustomProperty& operator=( const CustomProperty& );

private:
  Property::AccessMode mAccessMode; ///< The mode of the property
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_H__
