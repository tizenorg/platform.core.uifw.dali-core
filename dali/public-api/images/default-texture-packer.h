#ifndef __DALI_DEFAULT_TEXTURE_PACKER_H__
#define __DALI_DEFAULT_TEXTURE_PACKER_H__

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

#include <dali/public-api/images/texture-atlas.h>

namespace Dali
{


class TextureAtlas;

class DefaultTexturePacker : public TextureAtlas::PackingAlgorithm
{
public:

	virtual ~DefaultTexturePacker();

    virtual void Initialize( unsigned int width, unsigned int height, unsigned int spacing );

    virtual TextureAtlas::SelectedBlock Add( unsigned int width, unsigned int height );

    virtual std::vector< TextureAtlas::SelectedBlock > Add( std::vector< TextureAtlas::BlockSize >& blocks );

    virtual void Remove( const TextureAtlas::Block& block );

    virtual TextureAtlas::SelectedBlock* ForceSort( unsigned int count, TextureAtlas::BlockSize* );

    virtual void ForceSortComplete();

    virtual TextureAtlas::PackingAlgorithm* Clone()
    {
    	return new DefaultTexturePacker(*this);
    }

 private:

  struct FreeBlock
  {
    Dali::TextureAtlas::Block         block;
    unsigned int                      alternates[ 2 ];
  };

  struct FittingBlock
  {
      unsigned int                    index;
      bool                            isRotated;
  };

  std::vector< FreeBlock >            mFreeBlocks;
  unsigned int                        mFreeBlockCount;
  Dali::TextureAtlas* mAtlas;
  unsigned int mWidth;
  unsigned int mHeight;
  unsigned int mSpacing;

  void DiscardBlock( const unsigned int block );
  void DisplayFreeBlocks();

};

class SimpleTexturePacker : public TextureAtlas::PackingAlgorithm
{
public:

	virtual ~SimpleTexturePacker();

    virtual void Initialize( unsigned int width, unsigned int height, unsigned int spacing );

    virtual TextureAtlas::SelectedBlock Add( unsigned int width, unsigned int height );

    virtual std::vector< TextureAtlas::SelectedBlock > Add( std::vector< TextureAtlas::BlockSize >& blocks );

    virtual void Remove( const TextureAtlas::Block& block );

    virtual TextureAtlas::SelectedBlock* ForceSort( unsigned int count, TextureAtlas::BlockSize* );

    virtual void ForceSortComplete();

    virtual TextureAtlas::PackingAlgorithm* Clone()
    {
    	return new SimpleTexturePacker(*this);
    }

 private:

  struct FreeBlock
  {
    Dali::TextureAtlas::Block         block;
    unsigned int                      alternates[ 2 ];
  };

  struct FittingBlock
  {
      unsigned int                    index;
      bool                            isRotated;
  };

  std::vector< FreeBlock >            mFreeBlocks;
  unsigned int                        mFreeBlockCount;
  unsigned int mWidth;
  unsigned int mHeight;
  unsigned int mSpacing;
  unsigned int mNextX;
  unsigned int mNextY;

  void DiscardBlock( const unsigned int block );
  void DisplayFreeBlocks();

};



} // namespace Dali

#endif // __DALI_TEXTURE_PACKER_H__