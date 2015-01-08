#ifndef __DALI_TEXTURE_ATLAS_H__
#define __DALI_TEXTURE_ATLAS_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/images/bitmap-image.h>

namespace Dali
{

/**
 * UV co-ordinates for a quad
 */
struct UVBlock
{
  Vector2 uv[4];
};

class PackingAlgorithm;

class DALI_IMPORT_API TextureAtlas
{
public:

  /**
   * Result of Atlas operations
   */
  enum AtlasResult
  {
    SUCCESS,
    ADD_SUCCESS_SORTED,
    ADD_SUCCESS_SIZE_INCREASED,
    ADD_FAILED,
    ADD_MAX_SIZE_FAILED,
  };

  /**
   * Policy when Atlas should resize
   */
  enum ResizePolicy
  {
    RESIZE_AUTOMATICALLY,
    RESIZE_ERROR_ON_NO_ROOM
  };

  /**
   * Result of Atlas addition operation
   */
  struct TextureAtlasResult
  {
    AtlasResult result;
    unsigned int id;
  };

  /**
   * Block information returned from Packing Algorithm
   */
  struct SelectedBlock
  {
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
    bool isRotated;
  };

  /**
   * @brief Constructor which creates an uninitialized TextureAtlas object.
   *
   * Use TextureAtlas::New(...) to create an initialised object.
   */
  TextureAtlas();

  /**
   * Destructor
   */
  ~TextureAtlas();

  /**
   * @brief Create a new Texture Atlas
   *
   * @param unsigned int width of new Atlas
   * @param unsigned int height of new Atlas
   * @param pixelformat pixel format
   *
   * @return Texture Atlas object
   */
  static TextureAtlas New(unsigned int width,
                          unsigned int height,
                          Pixel::Format pixelformat=Pixel::RGBA8888);

  /**
   * @brief Sets the Packing Algorithm to be used by Atlas
   *
   * @param packingAlgorithm Pointer to a Packing Algorithm object
   */
  void InstallPackingAlgorithm( PackingAlgorithm* packingAlgorithm );

  /**
   * @brief Sets the policy for resizing the atlas
   */
  void SetResizePolicy( ResizePolicy policy );

  /**
   * @brief Add a bitmap to the Atlas
   *
   * @param bitmap Bitmap image to add
   */
  TextureAtlasResult Add( const BitmapImage& bitmap );

  /**
   * @brief Remove texture of ID from the Atlas
   *
   * @param unsigned int ID returned in TextureAtlasResult from add operation
   */
  void Remove( unsigned int id );

  /**
   * @brief Returns the dimensions of the Atlas in Vector form
   *
   * @return Current size of Atlas
   */
  Vector2 GetAtlasSize() const;

  /**
   * @brief Force the Packing Algorithm to sort current Atlas contents
   *
   * @return Sort success/fail
   */
  AtlasResult ForceSort();

  /**
   * @brief Get the vertex texture co-ordinates of the texture in the Altas
   *
   * @param unsigned int ID returned in TextureAtlasResult from add operation
   * @return UV co-ordinates for a quad
   */
  UVBlock GetUVCoordinates( unsigned int id );

};

/**
 * @brief Interface for Packing Algorithms (algorithms will derive from this)
 * @details These algorithms are to be called by the TextureAtlas and not directly
 *          If a packing algorithm isn't installed, then the TextureAtlas will use a default
 */
class PackingAlgorithm
{
public:
  virtual ~PackingAlgorithm() {}

  /**
   * @brief Initialize data for subsequent packing operations
   */
  virtual void Initialize() = 0;

  /**
   * @brief Determine block to add to the Atlas
   *
   * @param unsigned int width of the block
   * @param unsigned int height of the block
   *
   * @return structure describing the result of the operation and a reference ID for this block
   */
  virtual TextureAtlas::SelectedBlock Add( unsigned int width, unsigned int height ) = 0;

  /**
   * @brief Remove a block from the Atlas
   *
   * @param unsigned int x position of the block
   * @param unsigned int y position of the block
   * @param unsigned int width of the block
   * @param unsigned int width of the block
   */
  virtual void Remove( unsigned int x, unsigned int y, unsigned int width, unsigned int height ) = 0;
};

} // namespace Dali

#endif // __DALI_TEXTURE_ATLAS_H__
