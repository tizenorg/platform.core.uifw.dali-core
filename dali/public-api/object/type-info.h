#ifndef __DALI_TYPE_INFO_H__
#define __DALI_TYPE_INFO_H__

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


// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{

class ConnectionTrackerInterface;
class FunctorDelegate;

namespace Internal DALI_INTERNAL
{
  class TypeInfo;
};

/**
 * @brief TypeInfo class for instantiation of registered types and introspection of
 * their actions and signals.
 *
 * See TypeRegistry for methods of type registration and TypeInfo retrieval.
 */
class DALI_IMPORT_API TypeInfo : public BaseHandle
{
public:
  typedef BaseHandle (*CreateFunction)(); ///< Function signature for creating an instance of the associated object type.

  typedef bool (*ActionFunction)(BaseObject*, const std::string&, const Property::Map&); ///< Function signature for creating scriptable actions

  /**
   * @brief Connects a callback function with the object's signals.
   *
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  typedef bool (*SignalConnectorFunction)(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor);

  /**
   * @brief Callback to set an event-thread only property.
   *
   * @see PropertyRegistration.
   * @param[in] object The object whose property should be set.
   * @param[in] index The index of the property being set.
   * @param[in] value The new value of the property for the object specified.
   */
  typedef void (*SetPropertyFunction)( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * @brief Callback to get the value of an event-thread only property.
   *
   * @see PropertyRegistration.
   * @param[in] object The object whose property value is required.
   * @param[in] index The index of the property required.
   * @return The current value of the property for the object specified.
   */
  typedef Property::Value (*GetPropertyFunction)( BaseObject* object, Property::Index index );

  /**
   * @brief Allows the creation of an empty TypeInfo handle.
   */
  TypeInfo();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~TypeInfo();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  TypeInfo(const TypeInfo& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  TypeInfo& operator=(const TypeInfo& rhs);

  /**
   * @brief Retrieve the type name for this type.
   *
   * @return string name
   */
  const std::string& GetName() const;

  /**
   * @brief Retrieve the base type name for this type.
   *
   * @return string of base name
   */
  const std::string& GetBaseName() const;

  /**
   * @brief Create an object from this type.
   *
   * @return the BaseHandle for the newly created object
   */
  BaseHandle CreateInstance() const;

  /**
   * @brief Retrieve the creator function for this type.
   *
   * @return the creator function
   */
  CreateFunction GetCreator() const;

  /**
   * @brief Retrieve the number of actions for this type.
   *
   * @return The count
   */
  size_t GetActionCount() const;

  /**
   * @brief Retrieve the action name for the index.
   *
   * @param[in] index Index to lookup
   * @return action name or empty string where index is invalid
   */
  std::string GetActionName(size_t index) const;

  /**
   * @brief Retrieve the number of signals for this type.
   *
   * @return The count
   */
  size_t GetSignalCount() const;

  /**
   * @brief Retrieve the signal name for the index.
   *
   * @param[in] index Index to lookup
   * @return signal name or empty string where index is invalid
   */
  std::string GetSignalName(size_t index) const;

  /**
   * @brief Retrieve the number of event side type registered properties for this type.
   *
   * This count does not include all properties
   * @return The count
   */
  size_t GetPropertyCount() const;

  /**
   * @brief Retrieve the property name for the index.
   *
   * @param[in] index Index to lookup
   * @return property name or empty string where index is invalid
   */
  std::string GetPropertyName(size_t index) const;

  /**
   * @brief Gets all base class names
   *
   * @param[in] vector list to add bases to
   */
  void GetBases(Dali::Vector<std::string>& vector) const;

  // Properties

  /**
   * @brief Retrieve all the property indices for this type.
   *
   * @param[out] indices Container of property indices
   * @note The container will be cleared
   */
  void GetPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @brief Given a property index, retrieve the property name associated with it.
   *
   * @exception DaliException If index is not valid.
   *
   * @param[in] index The property index.
   * @return The name of the property at the given index.
   */
  const std::string& GetPropertyName( Property::Index index ) const;

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali Get() method.
   *
   * @param [in] typeInfo A pointer to a Dali resource
   */
  explicit DALI_INTERNAL TypeInfo(Internal::TypeInfo* typeInfo);

};

} // namespace Dali

#endif // __DALI_TYPE_INFO_H__
