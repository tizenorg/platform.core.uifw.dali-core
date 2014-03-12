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
 * @addtogroup CAPI_DALI_FRAMEWORK
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
 * @note Assignments of values with different types are not allowed.
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
   * Constructs a Any type with the given value.
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
   * @param [in] Any Any to be copied.
   */
  Any( const Any& Any )
  {
    CloneFunc func = Any.mContainer->mCloneFunc;
    mContainer = (*func)( *Any.mContainer );
  }

  /**
   * Assigns a given value to the Any type.
   *
   * @note Asserts if the given value has a different type than the stored one.
   *
   * @param[in] value The given value.
   */
  template<typename Type>
  Any& operator=( const Type& value )
  {
    AnyContainerImpl<Type>* tmp = mContainer;

    if( NULL != mContainer )
    {
      // Check if the value has the same type than the Any type.
      if( mContainer->GetType() != typeid( Type ) )
      {
        DALI_ASSERT_ALWAYS( !"Any::operator=( const Type& value ). Trying to assign two values with different types." );
      }
    }

    // Copies the value.
    mContainer = new AnyContainerImpl<Type>( value );

    // Deletes previous container.
    delete tmp;
    return *this;
  }

  /**
   * Assignment operator.
   *
   * @note Asserts if values of different types are assigned.
   *
   * @param [in] Any Any to be assigned.
   */
  Any& operator=( const Any& Any );

  /**
   * Returns the type info of the stored value.
   *
   * @note Returns the info of the void type if there is no value stored.
   */
  const std::type_info& GetType() const;

  /**
   * Retrieves the stored value in the Any type.
   *
   * @return The stored value.
   */
  template<typename Type>
  const Type& Get() const
  {
    if( NULL == mContainer )
    {
      DALI_ASSERT_ALWAYS( !"Any::Get(). Trying to retrieve a value of a non initialized Any." );
    }

    // Check if the value has the same value than the Any type.
    if( mContainer->GetType() != typeid( Type ) )
    {
      DALI_ASSERT_ALWAYS( !"Any::Get(). Trying to retrieve a value of a different type than the template one." );
    }

    return static_cast<AnyContainerImpl<Type>*>( mContainer )->GetValue();
  }

  /**
   * Retrieves the stored value in the Any type.
   *
   * @note Asserts if trying to retrieve a value from a non initialized Any type.
   * @note Asserts if trying to retrieve a value of a different type than the current Any type.
   *
   * @param[out] value The stored value.
   */
  template<typename Type>
  void Get( Type& value ) const
  {
    if( NULL == mContainer )
    {
      DALI_ASSERT_ALWAYS( !"Any::Get(). Trying to retrieve a value of a non initialized Any." );
    }

    // Check if the value has the same value than the Any type.
    if( mContainer->GetType() != typeid( Type ) )
    {
      DALI_ASSERT_ALWAYS( !"Any::Get(). Trying to retrieve a value of a different type than the template one." );
    }

    value = static_cast<const AnyContainerImpl<Type>*>( mContainer )->GetValue();
  }

  struct AnyContainerBase;    // Forward declaration for typedef
  typedef AnyContainerBase* (*CloneFunc)( const AnyContainerBase& base );

  /**
   * @brief Base container to hold type for match verification and instance cloning function
   *
   */
  struct AnyContainerBase
  {
    /**
     * @brief Constructor of base container
     *
     * @param type typeid of container
     * @param cloneFunc Cloning function to replicate this container type
     */
    AnyContainerBase( const std::type_info& type, CloneFunc cloneFunc )
    : mType( type ),
      mCloneFunc( cloneFunc )
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
  };


  /**
   * @brief Templated Clone function from container base
   *
   */
  template<typename Type>
  struct AnyContainerImplCloner
  {
    static AnyContainerBase* Clone( const AnyContainerBase& base )
    {
      return new AnyContainerImpl< Type >( static_cast< AnyContainerImpl< Type> >( base ) );
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
    : AnyContainerBase( typeid( Type ), static_cast<CloneFunc>( &AnyContainerImplCloner<Type>::Clone ) ),
      mValue( value )
    {}

    /**
     * @brief Constructor to create new container of type from and existing container (cloning)
     *
     * @param base [description]
     * @param t [description]
     */
    AnyContainerImpl( const AnyContainerBase& base )
    : AnyContainerBase( typeid( Type ), static_cast<CloneFunc>( &AnyContainerImplCloner<Type>::Clone ) )
    {}

    /**
     * @brief Get the container's stored value
     *
     * @return value of Type
     */
    const Type& GetValue() const
    {
      return mValue;
    }

    private:
      const Type mValue;
  };

  AnyContainerBase* mContainer;

};

} // namespace Dali

/**
 * @}
 */

#endif // __DALI_ANY_TYPE_H__
