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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/bitmap-image.h>
#include <dali/public-api/images/image.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{

  class TextureAtlas;

} // namespace Internal

/**
 * UV co-ordinates for a quad
 */
struct UVBlock
{
  Vector2 uv[4];
};

class DALI_IMPORT_API TextureAtlas : public Image
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
    unsigned int id;
  };

  /**
   * Block Size information
   */
  struct BlockSize
  {
    unsigned int width;
    unsigned int height;
  };

  /**
   * Block container
   */
  struct Block
  {
    unsigned int x;
    unsigned int y;
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
     * Virtual destructor.
     */
    virtual ~PackingAlgorithm() {}

    /**
    * @brief Initialize data for subsequent packing operations
    */
    virtual void Initialize( unsigned int width, unsigned int height, unsigned int spacing ) = 0;

    /**
    * @brief Determine block to add to the Atlas
    *
    * @param std::size_t width of the block
    * @param std::size_t height of the block
    *
    * @return structure describing the result of the operation and a reference ID for this block
    */
    virtual TextureAtlas::SelectedBlock Add( unsigned int width, unsigned int height ) = 0;

    /**
     * @brief Add multiple blocks to the Atlas
     * @details This is intended to allow optimal sorting in the algorithm with the a range of sizes known
     *
     * @param blocks List of blocks widths and heights
     * @return List of SelectedBlocks for placement by the Atlas
     */
    virtual std::vector< TextureAtlas::SelectedBlock > Add( std::vector< TextureAtlas::BlockSize >& blocks ) = 0;

    /**
     * @brief Remove a block from the Atlas
     *
     * @param std::size_t x position of the block
     * @param std::size_t y position of the block
     * @param std::size_t width of the block
     * @param std::size_t height of the block
     */
    virtual void Remove( const TextureAtlas::Block& block ) = 0;

    /**
     * @brief Force Algorithm to sort a set of blocks from scratch
     *
     * @param std::size_t count of blocks to sort
     * @param blocks pointer to a set of block sizes (any that have been rotated are unrotated )
     *
     * @return pointer to sorted list of blocks
     */
    virtual TextureAtlas::SelectedBlock* ForceSort( unsigned int count, TextureAtlas::BlockSize* ) = 0;

    /**
     * @brief Called when Force sort is complete (clean-up selected block list &c)
     */
    virtual void ForceSortComplete() = 0;

    /**
     * @brief Clone an instance of the packing algorithm
     */
    virtual PackingAlgorithm* Clone() = 0;

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
  static TextureAtlas New(unsigned int width,
                          unsigned int height,
                          unsigned int spacing,
                          Pixel::Format pixelformat=Pixel::RGBA8888);

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  TextureAtlas(const TextureAtlas& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  TextureAtlas& operator=(const TextureAtlas& rhs);

  /**
   * @brief Sets the Packing Algorithm to be used by Atlas
   *
   * @param packingAlgorithm Pointer to a Packing Algorithm object
   */
  void InstallPackingAlgorithm( PackingAlgorithm& packingAlgorithm );

  /**
   * @brief Add a bitmap to the Atlas
   *
   * @param bitmap Bitmap image to add
   *
   * @return result status and ID reference
   */
  AddResult Add( unsigned int width, unsigned int height );

  /**
   * @brief Add multiple bitmaps to the Atlas
   *
   * @param bitmaps List of bitmap images
   * @return List of results from adding to Atlas
   */
  //std::vector< AddResult > Add( const std::vector< const BitmapImage& >& bitmaps );

  /**
   * @brief Remove texture of ID from the Atlas
   *
   * @param std::size_t ID returned in TextureAtlasResult from add operation
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
  Result ForceSort();

  /**
   * @brief Get the vertex texture co-ordinates of the texture in the Altas
   *
   * @param std::size_t ID returned in TextureAtlasResult from add operation
   * @return UV co-ordinates for a quad
   */
  UVBlock GetUVCoordinates( unsigned int id );

  /**
   * @brief Downcast an Object handle to Texture Atlas.
   *
   * If handle points to a TextureAtlas the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a TextureAtlas or an uninitialized handle
   */
  static TextureAtlas DownCast( BaseHandle handle );


private:

  void Update();

public: // Not intended for application developers

  explicit DALI_INTERNAL TextureAtlas(Internal::TextureAtlas*);
};


} // namespace Dali

#endif // __DALI_TEXTURE_ATLAS_H__
