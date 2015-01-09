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

//class PackingAlgorithm;

class DALI_IMPORT_API TextureAtlas
{
public:

  /**
   * Result of Atlas operations
   */
  enum Result
  {
    SUCCESS,
    ADD_SUCCESS_SORTED,
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
  struct AddResult
  {
    Result result;
    std::size_t id;
  };

  /**
   * Block Size information
   */
  struct BlockSize
  {
    std::size_t width;
    std::size_t height;
  };

  /**
   * Block container
   */
   struct Block
   {
    std::size_t x;
    std::size_t y;
    BlockSize size;
   };

  /**
   * Block information returned from Packing Algorithm
   */
  struct SelectedBlock
  {
    Block block;
    bool isRotated;
  };

   /**
  * @brief Interface for Packing Algorithms (algorithms will derive from this)
  * @details These algorithms are to be called by the TextureAtlas and not directly
  *          If a packing algorithm isn't installed, then the TextureAtlas will use a default
  */
  class PackingAlgorithm
  {
  public:

    /**
    * @brief Initialize data for subsequent packing operations
    */
    virtual void Initialize() = 0;

    /**
    * @brief Determine block to add to the Atlas
    *
    * @param std::size_t width of the block
    * @param std::size_t height of the block
    *
    * @return structure describing the result of the operation and a reference ID for this block
    */
    virtual SelectedBlock Add( std::size_t width, std::size_t height ) = 0;

    /**
    * @brief Remove a block from the Atlas
    *
    * @param std::size_t x position of the block
    * @param std::size_t y position of the block
    * @param std::size_t width of the block
    * @param std::size_t height of the block
    */
    virtual void Remove( std::size_t x, std::size_t y, std::size_t width, std::size_t height ) = 0;

    /**
    * @brief Force Algorithm to sort a set of blocks from scratch
    *
    * @param std::size_t count of blocks to sort
    * @param blocks pointer to a set of block sizes (any that have been rotated are unrotated )
    *
    * @return pointer to sorted list of blocks
    */
    virtual SelectedBlock* ForceSort( std::size_t count, TextureAtlas::BlockSize* ) = 0;

    /**
    * @brief Called when Force sort is complete (clean-up selected block list &c)
    */
    virtual void ForceSortComplete() = 0;

  protected:

    /**
     * Virtual protected destructor. No deletion though this interface
     */
    virtual ~PackingAlgorithm() {}

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
   * @param std::size_t width of new Atlas
   * @param std::size_t height of new Atlas
   * @param pixelformat pixel format
   *
   * @return Texture Atlas object
   *
   * Example code using a Texture Atlas
   *
   * @code
   * {
   *    std::size_t textureId = 0;
   *    TextureAtlas atlas = TextureAtlas::New( 2048u, 2048u ); // Create an RGBA Atlas sized 2048 x 2048
   *    atlas.InstallPackingAlgorithm( SimplePacker );          // Use a simpler form of packer than default
   *    TextureAtlas::AddResult result = atlas.Add( bitmap );   // add an existing BitmapImage
   *    if ( result.result == TextureAtlas::SUCCESS )
   *    {
   *       textureId = result.id;                               // Reference texture from this point
   *    }
   *
   *    ...
   *
   *    if ( textureId )
   *    {
   *       atlas.Remove( id );                                  // Finished with this image, so free up space in Atlas
   *    }
   * }
   * @endcode
   */
  static TextureAtlas New(std::size_t width,
                          std::size_t height,
                          Pixel::Format pixelformat=Pixel::RGBA8888);

  /**
   * @brief Sets the Packing Algorithm to be used by Atlas
   *
   * @param packingAlgorithm Pointer to a Packing Algorithm object
   */
  void InstallPackingAlgorithm( PackingAlgorithm& packingAlgorithm );

  /**
   * @brief Sets the policy for resizing the atlas
   */
  void SetResizePolicy( ResizePolicy policy );

  /**
   * @brief Add a bitmap to the Atlas
   *
   * @param bitmap Bitmap image to add
   *
   * @return result status and ID reference
   */
  AddResult Add( const BitmapImage& bitmap );

  /**
   * @brief Remove texture of ID from the Atlas
   *
   * @param std::size_t ID returned in TextureAtlasResult from add operation
   */
  void Remove( std::size_t id );

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
  Result ForceSort();

  /**
   * @brief Get the vertex texture co-ordinates of the texture in the Altas
   *
   * @param std::size_t ID returned in TextureAtlasResult from add operation
   * @return UV co-ordinates for a quad
   */
  UVBlock GetUVCoordinates( std::size_t id );

};

} // namespace Dali

#endif // __DALI_TEXTURE_ATLAS_H__
