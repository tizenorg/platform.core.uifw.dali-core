#ifndef __DALI_VARIANT_TYPE_H__
#define __DALI_VARIANT_TYPE_H__

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
 * VariantType uintVariable = 5u;
 * VariantType floatVariable( 4.5f );
 * VariantType strVariable( std::string( "Hello world" ) );
 *
 * uintVariable = 1u;
 * \endcode
 */
class VariantType
{
public:
  /**
   * Default constructor.
   */
  VariantType();

  /**
   * Destructor. Free resources.
   */
  ~VariantType();

  /**
   * Constructs a variant type with the given value.
   *
   * @param[in] value The given value.
   */
  template<typename Type>
  VariantType( const Type& value )
  : mContainer( new VariantTypeImpl<Type>( value ) )
  {
  }

  /**
   * Copy Constructor.
   * @param [in] variantType VariantType to be copied.
   */
  VariantType( const VariantType& variantType );

  /**
   * Assigns a given value to the variant type.
   *
   * @note Asserts if the given value has a different type than the stored one.
   *
   * @param[in] value The given value.
   */
  template<typename Type>
  VariantType& operator=( const Type& value )
  {
    VariantTypeInterface* tmp = mContainer;

    if( NULL != mContainer )
    {
      // Check if the value has the same type than the variant type.
      if( mContainer->GetType() != typeid( Type ) )
      {
        DALI_ASSERT_ALWAYS( !"VariantType::operator=( const Type& value ). Trying to assign two values with different types." );
      }
    }

    // Copies the value.
    mContainer = new VariantTypeImpl<Type>( value );

    // Deletes previous container.
    delete tmp;

    return *this;
  }

  /**
   * Assignment operator.
   *
   * @note Asserts if values of different types are assigned.
   *
   * @param [in] variantType VariantType to be assigned.
   */
  VariantType& operator=( const VariantType& variantType );

  /**
   * Returns the type info of the stored value.
   *
   * @note Returns the info of the void type if there is no value stored.
   */
  const std::type_info& GetType() const;

  /**
   * Retrieves the stored value in the variant type.
   *
   * @return The stored value.
   */
  template<typename Type>
  const Type& Get() const
  {
    if( NULL == mContainer )
    {
      DALI_ASSERT_ALWAYS( !"VariantType::Get(). Trying to retrieve a value of a non initialized VariantType." );
    }

    // Check if the value has the same value than the variant type.
    if( mContainer->GetType() != typeid( Type ) )
    {
      DALI_ASSERT_ALWAYS( !"VariantType::Get(). Trying to retrieve a value of a different type than the template one." );
    }

    return static_cast<const VariantTypeImpl<Type>*>( mContainer )->GetValue();
  }

  /**
   * Retrieves the stored value in the variant type.
   *
   * @note Asserts if trying to retrieve a value from a non initialized variant type.
   * @note Asserts if trying to retrieve a value of a different type than the current variant type.
   *
   * @param[out] value The stored value.
   */
  template<typename Type>
  void Get( Type& value ) const
  {
    if( NULL == mContainer )
    {
      DALI_ASSERT_ALWAYS( !"VariantType::Get(). Trying to retrieve a value of a non initialized VariantType." );
    }

    // Check if the value has the same value than the variant type.
    if( mContainer->GetType() != typeid( Type ) )
    {
      DALI_ASSERT_ALWAYS( !"VariantType::Get(). Trying to retrieve a value of a different type than the template one." );
    }

    value = static_cast<const VariantTypeImpl<Type>*>( mContainer )->GetValue();
  }

private:
  /**
   * Declares the interface of the container used to store a value of any type.
   */
  class VariantTypeInterface
  {
  public:
    /**
     * Virtual destructor.
     */
    virtual ~VariantTypeInterface() {}

    /**
     * Retrieves the type info of the stored value.
     * @return The type info.
     */
    virtual const std::type_info& GetType() const = 0;

    /**
     * Copies the current value.
     * @return a pointer with a copy of the current value.
     */
    virtual VariantTypeInterface* Copy() const = 0;
  };

  /**
   *
   */
  template<typename Type>
  class VariantTypeImpl : public VariantTypeInterface
  {
  public:
    /**
     * Constructs a container which stores the given value.
     * @param [in] value Value to be stored.
     */
    VariantTypeImpl( const Type& value )
    : mValue( value )
    {}

    /**
     * Default destructor.
     */
    ~VariantTypeImpl()
    {}

    /**
     * @copydoc VariantTypeInterface::GetType()
     */
    const std::type_info& GetType() const
    {
      return typeid( Type );
    }

    /**
     * @copydoc VariantTypeInterface::Copy()
     */
    VariantTypeInterface* Copy() const
    {
      return new VariantTypeImpl( mValue );
    }

    /**
     * Retrieves the stored value.
     * @return The stored value.
     */
    const Type& GetValue() const
    {
      return mValue;
    }

  private:
    Type mValue; ///< Stores the value.

    // Declared as private and not Implemented.
    VariantTypeImpl();
    VariantTypeImpl( VariantTypeImpl& );
    VariantTypeImpl& operator=( VariantTypeImpl& );
  };

  VariantTypeInterface* mContainer; ///< Pointer to the container which stores the value.
};

} // namespace Dali

/**
 * @}
 */

#endif // __DALI_VARIANT_TYPE_H__
