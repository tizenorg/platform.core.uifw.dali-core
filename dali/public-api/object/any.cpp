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
#include <dali/public-api/object/any.h>

namespace Dali
{

Any::Any()
: mContainer( NULL )
{
}

Any::~Any()
{
  // Call the implementation deletion function, which will invalidate mContainer

  if ( NULL != mContainer )
  {
    mContainer->mDeleteFunc( mContainer );
    mContainer = NULL;
  }
}

Any& Any::operator=( const Any& any )
{
  if( &any != this )
  {
    if( NULL == any.mContainer )
    {
      delete mContainer;
      mContainer = NULL;
    }
    else
    {
      AnyContainerBase* tmp = mContainer;

      if( NULL != mContainer )
      {
        // Check if two Any types have the same type. Avoids assignments of values with different types.
        if( mContainer->GetType() != any.GetType() )
        {
          DALI_ASSERT_ALWAYS( !"Any::operator=( const Any& Any ). Trying to assign two values with different types." );
        }
      }

      // Clone the correct templated object
      mContainer = any.mContainer->mCloneFunc( *any.mContainer );

      // Deletes previous container.
      delete tmp;
    }
  }

  return *this;
}

const std::type_info& Any::GetType() const
{
  return mContainer ? mContainer->GetType() : typeid( void );
}

void Any::AssertAlways( const char* assertMessage )
{
  DALI_ASSERT_ALWAYS( 0 && assertMessage );
}


} //namespace Dali
