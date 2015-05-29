#ifndef __DALI_INTERNAL_TYPE_INFO_H__
#define __DALI_INTERNAL_TYPE_INFO_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/type-info.h>

namespace Dali
{

namespace Internal
{

/**
 * A TypeInfo class to support registered type creation, and introspection of available
 * actions and signal connection.
 * It also supports doing actions and connecting signal handlers. Note this is properly
 * conducted through the BaseHandle interface which uses the TypeRegistry to walk
 * all base classes.
 */
class TypeInfo : public BaseObject
{
public:
  /**
   * Create TypeInfo
   * @param [name] the registered name
   * @param [baseName] the base type registered name
   * @param [creator] the creator function for this type
   */
  TypeInfo(const std::string &name, const std::string &baseName, Dali::TypeInfo::CreateFunction creator);

  /**
   *
   */
  ~TypeInfo();

  /**
   * @copydoc Dali::TypeInfo::GetName
   */
  const std::string& GetName() const;

  /**
   * @copydoc Dali::TypeInfo::GetBaseName
   */
  const std::string& GetBaseName() const;

  /**
   * @copydoc TypeInfo::CreateFunction
   */
  BaseHandle CreateInstance() const;

  /**
   * @copydoc Dali::TypeInfo::GetCreator
   */
  Dali::TypeInfo::CreateFunction GetCreator() const;

  /**
   * @copydoc Dali::TypeInfo::GetActionCount
   */
  size_t GetActionCount() const;

  /**
   * @copydoc Dali::TypeInfo::GetActionName
   */
  std::string GetActionName(size_t index) const;

  /**
   * @copydoc Dali::TypeInfo::GetSignalCount
   */
  size_t GetSignalCount() const;

  /**
   * @copydoc Dali::TypeInfo::GetSignalName
   */
  std::string GetSignalName(size_t index) const;

  /**
   * @copydoc Dali::TypeInfo::GetPropertyCount
   */
  size_t GetPropertyCount() const;

  /**
   * @copydoc Dali::TypeInfo::GetPropertyName
   */
  std::string GetPropertyName(size_t index) const;

  /**
   * Adds the property indices to the container specified.
   * @param[in/out] indices The container where the property indices are added.
   */
  void GetPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::TypeInfo::GetPropertyName() const
   */
  const std::string& GetPropertyName( Property::Index index ) const;

  /*
   * Add an action function
   */
  void AddActionFunction( const std::string &actionName, Dali::TypeInfo::ActionFunction function );

  /*
   * Add a function for connecting a signal.
   * @param[in] signalName The name of the signal.
   * @param[in] function The function used for connecting to the signal.
   */
  void AddConnectorFunction( const std::string& signalName, Dali::TypeInfo::SignalConnectorFunction function );

  /**
   * Adds an event-thread only property to the type.
   * @param[in] name The name of the property.
   * @param[in] index The index of the property.
   * @param[in] type The Property::Type.
   * @param[in] setFunc The function to call to set the property (Can be NULL).
   * @param[in] getFunc The function to call to retrieve the value of the property.
   */
  void AddProperty( const std::string& name, Property::Index index, Property::Type type, Dali::TypeInfo::SetPropertyFunction setFunc, Dali::TypeInfo::GetPropertyFunction getFunc );

  /**
   * Adds an animatable property to the type.
   * @param[in] name The name of the property.
   * @param[in] index The index of the property
   * @param[in] type The Property::Type.
   */
  void AddAnimatableProperty( const std::string& name, Property::Index index, Property::Type type );

  /**
   * Adds a component of an animatable property to the type.
   * The animatable property must have been type-registered and must be a Vector2, Vector3 or Vector4 type.
   * @param[in] name The name of the component.
   * @param[in] index The index of the property
   * @param[in] baseIndex The index of the base animatable property
   * @param[in] component The index The index of the component.
   */
  void AddAnimatablePropertyComponent( const std::string& name, Property::Index index, Property::Index baseIndex, unsigned int componentIndex );

  /**
   * Do an action on base object
   * @param [in] object The base object to act upon
   * @param [in] actionName The name of the desired action
   * @param [in] properties The arguments of the action
   * @return bool If the action could be executed
   */
  bool DoActionTo(BaseObject *object, const std::string &actionName, const Property::Map &properties);

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  bool ConnectSignal( BaseObject* object, ConnectionTrackerInterface* connectionTracker, const std::string& signalName, FunctorDelegate* functor );

  /**
   * Given a property name, retrieve the index.
   * @param[in] name The name of the property.
   * @return The index associated with that name.
   */
  Property::Index GetPropertyIndex( const std::string& name ) const;

  /**
   * Given a property index, retrieve the index of its base property.
   * @param[in] index The index of the property.
   * @return The index of the base property associated with the given property index.
   */
  Property::Index GetBasePropertyIndex( Property::Index index ) const;

  /**
   * Given a property index, retrieve its component index.
   * @param[in] index The index of the property.
   * @return The component index associated with that property index.
   */
  int GetComponentIndex( Property::Index index ) const;

  /**
   * Checks if there is a setter for the property. If there is then it is writable.
   * @param[in] index The property index.
   * @return True, if writable, false otherwise.
   */
  bool IsPropertyWritable( Property::Index index ) const;

  /**
   * Retrieve the Property::Type of the property at the given index.
   * @param[in] index The property index.
   * @return The Property::Type at that index.
   */
  Property::Type GetPropertyType( Property::Index index ) const;

  /**
   * Sets the value of a property at the index specified for the given object.
   * @param[in] object The object whose property is to be set.
   * @param[in] index The property index.
   * @param[in] value The value to set.
   */
  void SetProperty( BaseObject *object, Property::Index index, const Property::Value& value ) const;

  /**
   * Sets the value of a property with the name specified for the given object.
   * @param[in] object The object whose property is to be set.
   * @param[in] name The property name.
   * @param[in] value The value to set.
   */
  void SetProperty( BaseObject *object, const std::string& name, const Property::Value& value ) const;

  /**
   * Retrieves the value of a property at the index specified for the given object.
   * @param[in] object The object whose property is to be queried.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  Property::Value GetProperty( const BaseObject *object, Property::Index index ) const;

  /**
   * Retrieves the value of a property with the name specified for the given object.
   * @param[in] object The object whose property is to be queried.
   * @param[in] name The property name.
   * @return The current value of the property.
   */
  Property::Value GetProperty( const BaseObject *object, const std::string& name ) const;

private:

  struct RegisteredProperty
  {
    RegisteredProperty()
    : type( Property::NONE ),
      setFunc( NULL ),
      getFunc( NULL ),
      name(),
      basePropertyIndex(Property::INVALID_INDEX),
      componentIndex(Property::INVALID_COMPONENT_INDEX)
    {
    }

    RegisteredProperty( Property::Type propType, Dali::TypeInfo::SetPropertyFunction set, Dali::TypeInfo::GetPropertyFunction get, const std::string& propName, Property::Index basePropertyIndex, int componentIndex )
    : type( propType ),
      setFunc( set ),
      getFunc( get ),
      name( propName ),
      basePropertyIndex(basePropertyIndex),
      componentIndex(componentIndex)
    {
    }

    Property::Type type;
    Dali::TypeInfo::SetPropertyFunction setFunc;
    Dali::TypeInfo::GetPropertyFunction getFunc;
    std::string name;
    Property::Index basePropertyIndex;
    int componentIndex;
  };

  typedef std::pair<std::string, Dali::TypeInfo::SignalConnectorFunction > ConnectionPair;
  typedef std::pair<std::string, Dali::TypeInfo::ActionFunction > ActionPair;
  typedef std::pair<Property::Index, RegisteredProperty> RegisteredPropertyPair;

  typedef std::vector< ActionPair > ActionContainer;
  typedef std::vector< ConnectionPair > ConnectorContainer;
  typedef std::vector< RegisteredPropertyPair > RegisteredPropertyContainer;

  std::string mTypeName;
  std::string mBaseTypeName;
  Dali::TypeInfo::CreateFunction mCreate;
  ActionContainer mActions;
  ConnectorContainer mSignalConnectors;
  RegisteredPropertyContainer mRegisteredProperties;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::TypeInfo& GetImplementation(Dali::TypeInfo& typeInfo)
{
  DALI_ASSERT_ALWAYS(typeInfo);

  BaseObject& handle = typeInfo.GetBaseObject();

  return static_cast<Internal::TypeInfo&>(handle);
}

inline const Internal::TypeInfo& GetImplementation(const Dali::TypeInfo& typeInfo)
{
  DALI_ASSERT_ALWAYS(typeInfo);

  const BaseObject& handle = typeInfo.GetBaseObject();

  return static_cast<const Internal::TypeInfo&>(handle);
}

} // namespace Dali

#endif // header
