//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/

//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// HEADER CLASS
#include <dali/public-api/text/character.h>

// INTERNAL INCLUDES
#include <dali/internal/event/text/character-impl.h>

namespace Dali
{

Character::Character( const Character& character )
: mImpl( new Internal::Character( character.mImpl->GetCharacter() ) )
{
}

Character& Character::operator=( const Character& character )
{
  if( &character != this )
  {
    mImpl->SetCharacter( character.mImpl->GetCharacter() );
  }

  return *this;
}

Character::~Character()
{
  delete mImpl;
}

bool Character::operator==( const Character& character ) const
{
  return mImpl->GetCharacter() == character.mImpl->GetCharacter();
}

bool Character::operator!=( const Character& character ) const
{
  return mImpl->GetCharacter() != character.mImpl->GetCharacter();
}

Character::CharacterDirection Character::GetCharacterDirection() const
{
  return mImpl->GetCharacterDirection();
}

bool Character::IsWhiteSpace() const
{
  return mImpl->IsWhiteSpace();
}

bool Character::IsNewLine() const
{
  return mImpl->IsNewLine();
}

Character::Character( Internal::Character* impl )
: mImpl( impl )
{
}

const Internal::Character& Character::GetImplementation() const
{
  return *mImpl;
}

Internal::Character& Character::GetImplementation()
{
  return *mImpl;
}

} // namespace Dali
