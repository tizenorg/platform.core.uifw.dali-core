#ifndef __DALI_FUNCTION_H__
#define __DALI_FUNCTION_H__

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

namespace Dali
{

// Do not provide a definition of this class
class UndefinedClass;
typedef void (UndefinedClass::*GenericMemFuncType)();
typedef void (UndefinedClass::*GenericMemFuncType2)(int num, const char *str);

template< class T >
struct DestructorDispatcher
{
  static void Destruct( UndefinedClass* object )
  {
    delete reinterpret_cast< T* >( object );
  }
};

template< class T >
struct CopyConstructorDispatcher
{
  static UndefinedClass* CopyConstruct( const UndefinedClass* object )
  {
    T* copy = new T( *reinterpret_cast< const T* >( object ) );
    return reinterpret_cast< UndefinedClass* >( copy );
  }
};

template <typename FunctionSignature>
class Function;

/**
 * A Function delegate with 2 parameters and an non-void return type.
 */
template<typename ReturnType, class Param1, class Param2>
class Function< ReturnType ( Param1, Param2 ) >
{
public:

  /**
   * @brief Constructs a Function for a member function & object instance.
   *
   * @param thisPtr A pointer to the object intance.
   * @param memberFunction The member function to call.
   */
  template<class InheritedType, class MainType>
  Function( InheritedType* thisPtr, ReturnType (MainType::* memberFunction)(Param1 p1, Param2 p2) )
  : mDestructorDispatcher( NULL ),
    mCopyConstructorDispatcher( NULL )
  {
    mThis = reinterpret_cast<UndefinedClass*>( static_cast<MainType*>( thisPtr ) );
    mFunction = reinterpret_cast<GenericMemFuncType>( memberFunction );
  }

  /**
   * @brief Constructs a Function for a function object.
   *
   * @param functor The function object to call.
   */
  template<class FunctorType>
  Function( const FunctorType& functor )
  {
    mThis = reinterpret_cast<UndefinedClass*>( new FunctorType( functor ) );
    mFunction = reinterpret_cast<GenericMemFuncType>( &FunctorType::operator() );

    mDestructorDispatcher = reinterpret_cast< GenericDestructor >( &DestructorDispatcher<FunctorType>::Destruct );
    mCopyConstructorDispatcher = reinterpret_cast< GenericCopyConstructor >( &CopyConstructorDispatcher<FunctorType>::CopyConstruct );
  }

  // TODO - The destructor

  /**
   * @brief Calls the Function.
   *
   * @param p1 The first parameter.
   * @param p2 The second parameter.
   * @return The return value.
   */
  ReturnType operator()( Param1 p1, Param2 p2 ) const
  {
    return (mThis->*( reinterpret_cast<GenericMemFuncType2>(mFunction) ))( p1, p2 );
  }

  /**
   * @brief Copy constructor.
   *
   * @param copyMe The function to copy from.
   */
  Function( const Function& copyMe )
  {
    if( copyMe.mCopyConstructorDispatcher )
    {
      mThis = (*copyMe.mCopyConstructorDispatcher)( copyMe.mThis );
    }
    else
    {
      mThis = copyMe.mThis;
    }

    mFunction = copyMe.mFunction;
    mDestructorDispatcher = copyMe.mDestructorDispatcher;
    mCopyConstructorDispatcher = copyMe.mCopyConstructorDispatcher;
  }

  /**
   * @brief Undefined assigned operator.
   */
  Function& operator=( const Function& );

private:

  typedef void            (*GenericDestructor)(      UndefinedClass* object );
  typedef UndefinedClass* (*GenericCopyConstructor)( UndefinedClass* object );

  UndefinedClass* mThis;
  GenericMemFuncType mFunction;

  // Optionally used for functors
  GenericDestructor mDestructorDispatcher;
  GenericCopyConstructor mCopyConstructorDispatcher;
};

} // namespace Dali

#endif // __DALI_FUNCTION_H__
