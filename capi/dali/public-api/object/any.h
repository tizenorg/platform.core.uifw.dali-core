#ifndef __DALI_ANY_TYPE_H__
#define __DALI_ANY_TYPE_H__

/*
Copyright (c) 2000-2014 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

/**
 * @addtogroup CAPI_DALI_OBJECT_MODULE
 * @{
 */

// EXTERNAL INCLUDES
#include <typeinfo>   // operator typeid


// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

/**
 * Stores a value of any type.
 *
 * @note Assignments of values with different types are now allowed to replicate boost::any behaviour
 *
 * Examples of use:
 * \code{.cpp}
 * Any uintVariable = 5u;
 * Any floatVariable( 4.5f );
 * Any strVariable( std::string( "Hello world" ) );
 *
 * uintVariable = 1u;
 * \endcode
 */
class Any
{
public:
  /**
   * Default constructor.
   */
  Any();

  /**
   * Destructor. Free resources.
   */
  ~Any();

  /**
   * @brief Constructs a Any type with the given value.
   *
   * @param[in] value The given value.
   */
  template<typename Type>
  Any( const Type& value )
  : mContainer( new AnyContainerImpl<Type>( value ) )
  {
  }

  /**
   * Copy Constructor.
   * @param [in] any Any to be copied.
   */
  Any( const Any& any )
  {
    // If container isn't empty then copy the container?
    if ( NULL != any.mContainer )
    {
      mContainer = any.mContainer->mCloneFunc( *any.mContainer );
    }
    else
    {
      // Otherwise mark new container as empty
      mContainer = NULL;
    }
  }

  /**
   * @brief Assigns a given value to the Any type.
   *
   * @note If the types are different, then the current container will be re-created.
   *
   * @param[in] value The given value.
   */
  template<typename Type>
  Any& operator=( const Type& value )
  {

    // If the container is empty then assign the new value
    if ( NULL == mContainer )
    {
      mContainer = new AnyContainerImpl< Type >( value );
    }
    else
    {
      // Check to see if this type is compatible with current container?
      if ( mContainer->GetType() == typeid( Type ) )
      {
        // Same type so just set the new value
        static_cast< AnyContainerImpl< Type >* >( mContainer )->SetValue( value );
      }
      else
      {
        // Incompatible types, so delete old container and assign a new one with this type and value
        mContainer->mDeleteFunc( mContainer );
        mContainer = new AnyContainerImpl< Type >( value );
      }
    }
    return *this;
  }

  /**
   * @brief Assignment operator.
   *
   * @note Asserts if values of different types are assigned.
   *
   * @param [in] any Any to be assigned.
   */
  Any& operator=( const Any& any );

  /**
   * @brief Returns the type info of the stored value.
   *
   * @note Returns the info of the void type if there is no value stored.
   */
  const std::type_info& GetType() const;

  /**
   * @brief Retrieves the stored value in the Any type.
   *
   * @return The stored value.
   */
  template<typename Type>
  const Type& Get() const
  {

    DALI_ASSERT_ALWAYS( NULL != mContainer );

    // Check if the value has the same value than the Any type.
    if( mContainer->GetType() != typeid( Type ) )
    {
      DALI_ASSERT_ALWAYS( !"Any::Get(). Trying to retrieve a value of a different type than the template one." );
    }

    return static_cast< AnyContainerImpl< Type >* >( mContainer )->GetValue();
  }

  /**
   * @brief Return pointer of Type to the value stored
   *
   * @return pointer to the value or NULL if no value is contained
   */
  template<typename Type>
  Type* GetPointer()
  {
    if( NULL == mContainer )
    {
      return NULL;
    }
     // Check if the value has the same value than the Any type.
    if( mContainer->GetType() != typeid( Type ) )
    {
      DALI_ASSERT_ALWAYS( !"Any::GetPointer(). Trying to retrieve a pointer to a value of a different type than the template one." );
    }
    return static_cast< AnyContainerImpl< Type >* >( mContainer )->GetPointerToValue();
  }

  /**
   * @brief Returns whether container holds a value
   *
   * @return true or false
   */
  bool Empty()
  {
    return ( NULL == mContainer ) ? true : false;
  }

  struct AnyContainerBase;    // Forward declaration for typedef
  typedef AnyContainerBase* (*CloneFunc)( const AnyContainerBase& base );
  typedef void (*DeleteFunc)( const AnyContainerBase* base );

  /**
   * Base container to hold type for match verification and instance cloning function
   *
   */
  struct AnyContainerBase
  {
    /**
     * @brief Constructor of base container
     *
     * @param type typeid of container
     * @param cloneFunc Cloning function to replicate this container type
     * @param deleteFunc Deleting function to destroy this container type
     */
    AnyContainerBase( const std::type_info& type, CloneFunc cloneFunc, DeleteFunc deleteFunc )
    : mType( type ),
      mCloneFunc( cloneFunc ),
      mDeleteFunc( deleteFunc )
    {}

    /**
     * @brief Get the typeid of this container
     *
     * @return type
     */
    const std::type_info& GetType() const
    {
      return mType;
    }

    const::std::type_info& mType;       // typeID
    CloneFunc mCloneFunc;               // cloning function for this container
    DeleteFunc mDeleteFunc;             // deleting function for this container
  };


  /**
   * @brief Templated Clone function from container base
   *
   * @param base reference to container
   */
  template<typename Type>
  struct AnyContainerImplCloner
  {
    static AnyContainerBase* Clone( const AnyContainerBase& base )
    {
      return new AnyContainerImpl< Type >( static_cast< AnyContainerImpl< Type > >( base ) );
    }
  };

  /**
   * @brief Templated Delete function from container base
   *
   * @param base pointer to container
   */
  template<typename Type>
  struct AnyContainerImplDelete
  {
    static void Delete( const AnyContainerBase* base )
    {
      delete ( static_cast< const AnyContainerImpl< Type >* > ( base ) );
    }
  };

  /**
   * @brief Templated class to hold value for type
   *
   */
  template<typename Type>
  class AnyContainerImpl : public AnyContainerBase
  {
  public:

    /**
     * @brief Constructor to create container holding value of type Type
     *
     * @param value Value of Type
     */
    AnyContainerImpl( const Type& value )
    : AnyContainerBase( typeid( Type ),
                        static_cast< CloneFunc >( &AnyContainerImplCloner< Type >::Clone ),
                        static_cast< DeleteFunc >( &AnyContainerImplDelete< Type >::Delete ) ),
                        mValue( value )
    {}

    /**
     * @brief Constructor to create new container of type from and existing container (cloning)
     *
     * @param base reference to base container to copy from
     */
    AnyContainerImpl( const AnyContainerBase& base )
    : AnyContainerBase( typeid( Type ),
                        static_cast< CloneFunc >( &AnyContainerImplCloner< Type >::Clone ),
                        static_cast< DeleteFunc >( &AnyContainerImplDelete< Type >::Delete ) )
    {
      mValue = static_cast< const AnyContainerImpl& >( base ).GetValue();
    }

    /**
     * @brief Get the container's stored value
     *
     * @return value of Type
     */
    const Type& GetValue() const
    {
      return mValue;
    }

    void SetValue( const Type& value )
    {
      mValue = value;
    }

    /**
     * @brief Get a pointer to the value held
     *
     * @return pointer to the value of type Type
     */
    Type* GetPointerToValue()
    {
      return static_cast< Type* >( &mValue );
    }

    private:
      Type mValue;
  };

  AnyContainerBase* mContainer;

};

/**
 * AnyCast helper functions ( replicates boost functionality, but without exception generation )
 */

/**
 * @brief Extract a pointer to the held type of an Any object from a pointer to that Any object (NULL if empty )
 *
 * @param any Pointer to an Any object
 *
 * @return Pointer to the Type held
 */
template<typename Type>inline Type* AnyCast( Any* any )
{
  return any->GetPointer<Type>();
}

/**
 * @brief Extract a const pointer to the held type of an Any object from a pointer to that Any object (NULL if empty )
 *
 * @param any const Pointer to an Any object
 *
 * @return const Pointer to the Type held
 */
template<typename Type>inline const Type* AnyCast( const Any* any )
{
  return any->GetPointer<Type>();
}

/**
 * @brief Extract a held value of type Type from an Any object from a reference to that Any object
 *
 * @param any reference to an Any object
 *
 * @return Type value of type Type
 */
template<typename Type>inline Type AnyCast( Any& any )
{
  return any.Get<Type>();
}

/**
 * @brief Extract a held value of type Type from an Any object from a const reference to that Any object
 *
 * @param any reference to an Any object
 *
 * @return Type value of type Type
 */
template<typename Type>inline Type AnyCast( const Any& any )
{
  return any.Get<Type>();
}

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_ANY_TYPE_H__
