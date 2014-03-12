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

// CLASS HEADER
#include <dali/public-api/common/variant-type.h>

namespace Dali
{

VariantType::VariantType()
: mContainer( NULL )
{
}

VariantType::~VariantType()
{
  delete mContainer;
}

VariantType::VariantType( const VariantType& variantType )
: mContainer( variantType.mContainer ? variantType.mContainer->Copy() : NULL )
{
}

VariantType& VariantType::operator=( const VariantType& variantType )
{
  if( &variantType != this )
  {
    if( !variantType.mContainer )
    {
      delete mContainer;
      mContainer = NULL;
    }
    else
    {
      VariantTypeInterface* tmp = mContainer;

      if( NULL != mContainer )
      {
        // Check if two variant types have the same type. Avoids assignments of values with different types.
        if( mContainer->GetType() != variantType.GetType() )
        {
          DALI_ASSERT_ALWAYS( !"VariantType::operator=( const VariantType& variantType ). Trying to assign two values with different types." );
        }
      }

      // Copies the value.
      mContainer = variantType.mContainer->Copy();

      // Deletes previous container.
      delete tmp;
    }
  }

  return *this;
}

const std::type_info& VariantType::GetType() const
{
  return mContainer ? mContainer->GetType() : typeid( void );
}

} //namespace Dali
