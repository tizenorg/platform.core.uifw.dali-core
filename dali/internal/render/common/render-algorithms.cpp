/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/render/common/render-algorithms.h>

// INTERNAL INCLUDES
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/common/render-list.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/renderers/render-renderer.h>
//todor
#include <iomanip>

#define ENABLED_NEW_CLIPPING

using Dali::Internal::SceneGraph::RenderItem;
using Dali::Internal::SceneGraph::RenderList;
using Dali::Internal::SceneGraph::RenderListContainer;
using Dali::Internal::SceneGraph::RenderInstruction;

namespace Dali
{

namespace Internal
{

namespace Render
{

/**
 * Sets up the scissor test if required.
 * @param[in] renderList The render list from which to get the clipping flag
 * @param[in] context The context
 */
inline void SetScissorTest( const RenderList& renderList, Context& context )
{
  // Scissor testing
  if( renderList.IsClipping() )
  {
    context.SetScissorTest( true );

    const Dali::ClippingBox& clip = renderList.GetClippingBox();
    context.Scissor(clip.x, clip.y, clip.width, clip.height);
  }
  else
  {
    context.SetScissorTest( false );
  }
}

#if 0
/**
 * Sets the render flags for depth testing and stencil buffer
 *
 * @param[in] renderList The render list from which to get the render flags
 * @param[in] context The context
 */
inline void SetRenderFlags2( const RenderList& renderList, Context& context )
{
  //todor
#if 0
  glEnable(GL_STENCIL_TEST);

      // Draw floor
      glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
      glStencilMask(0xFF); // Write to stencil buffer
      glDepthMask(GL_FALSE); // Don't write to depth buffer
      glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)

      glDrawArrays(GL_TRIANGLES, 36, 6);

      // Draw cube reflection
      glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
      glStencilMask(0x00); // Don't write anything to stencil buffer
      glDepthMask(GL_TRUE); // Write to depth buffer

      model = glm::scale(
          glm::translate(model, glm::vec3(0, 0, -1)),
          glm::vec3(1, 1, -1)
      );
      glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
      glDrawArrays(GL_TRIANGLES, 0, 36);

  glDisable(GL_STENCIL_TEST);
#endif

  //const unsigned int renderFlags = renderList.GetFlags();
  unsigned int renderFlags = renderList.GetFlags();
  bool x = false;
  if( x )
  {
    renderFlags |= RenderList::STENCIL_WRITE | RenderList::STENCIL_BUFFER_ENABLED;
  }

  //std::cout << "todor: SetRenderFlags" << std::hex << renderFlags << std::endl;

  bool enableDepthBuffer = ( ( renderFlags & RenderList::DEPTH_BUFFER_ENABLED ) != 0u );

  GLbitfield clearMask   = ( renderFlags & RenderList::DEPTH_CLEAR ) ? GL_DEPTH_BUFFER_BIT : 0u;

  context.EnableDepthBuffer( enableDepthBuffer );

  // Stencil enabled, writing, and clearing...
  const bool enableStencilBuffer( renderFlags & RenderList::STENCIL_BUFFER_ENABLED );
  const bool enableStencilWrite( renderFlags & RenderList::STENCIL_WRITE );

  context.EnableStencilBuffer( enableStencilBuffer );

  if( enableStencilBuffer )
  {
    context.StencilFunc( (enableStencilWrite ? GL_ALWAYS : GL_EQUAL), 1, 0xFF );
    //todor
    //context.StencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    context.StencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);//todor
    //context.StencilMask( 0xff );//todor
    //context.Clear();//todor


    clearMask |= (renderFlags & RenderList::STENCIL_CLEAR) ? GL_STENCIL_BUFFER_BIT : 0u;
  }

  // Write to stencil buffer or color buffer, but not both
  context.StencilMask( enableStencilWrite ? 0xFF : 0x00 );
  context.ColorMask( !enableStencilWrite );

  // Clear depth and/or stencil buffer.
  if( clearMask )
  {
    // only clear if the depth and/or stencil buffer have been written to after a previous clear
    context.Clear( clearMask, Context::CHECK_CACHED_VALUES );
    //context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );//todor
  }
}
#endif

#if 0
/**
 * Sets the render flags for depth testing and stencil buffer
 *
 * @param[in] renderList The render list from which to get the render flags
 * @param[in] context The context
 */
inline void SetRenderFlags( const RenderList& renderList, Context& context )
{
  const unsigned int renderFlags = renderList.GetFlags();

  bool enableDepthBuffer = ( ( renderFlags & RenderList::DEPTH_BUFFER_ENABLED ) != 0u );

  GLbitfield clearMask   = ( renderFlags & RenderList::DEPTH_CLEAR ) ? GL_DEPTH_BUFFER_BIT : 0u;

  context.EnableDepthBuffer( enableDepthBuffer );

  // Stencil enabled, writing, and clearing...
  const bool enableStencilBuffer( renderFlags & RenderList::STENCIL_BUFFER_ENABLED );
  const bool enableStencilWrite( renderFlags & RenderList::STENCIL_WRITE );

  context.EnableStencilBuffer( enableStencilBuffer );

  if( enableStencilBuffer )
  {
    context.StencilFunc( (enableStencilWrite ? GL_ALWAYS : GL_EQUAL), 1, 0xFF );
    context.StencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

    clearMask |= (renderFlags & RenderList::STENCIL_CLEAR) ? GL_STENCIL_BUFFER_BIT : 0u;
  }

  // Write to stencil buffer or color buffer, but not both
  context.StencilMask( enableStencilWrite ? 0xFF : 0x00 );
  context.ColorMask( !enableStencilWrite );

  // Clear depth and/or stencil buffer.
  if( clearMask )
  {
    // only clear if the depth and/or stencil buffer have been written to after a previous clear
    context.Clear( clearMask, Context::CHECK_CACHED_VALUES );
  }
}
#endif

#if 0
//todor
inline void SetRenderFlagsTEST( unsigned int flags, Context& context, int index )
{
  //todor
#if 0
  glEnable(GL_STENCIL_TEST);

      // Draw floor
      glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
      glStencilMask(0xFF); // Write to stencil buffer
      glDepthMask(GL_FALSE); // Don't write to depth buffer
      glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)

      glDrawArrays(GL_TRIANGLES, 36, 6);

      // Draw cube reflection
      glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
      glStencilMask(0x00); // Don't write anything to stencil buffer
      glDepthMask(GL_TRUE); // Write to depth buffer

      model = glm::scale(
          glm::translate(model, glm::vec3(0, 0, -1)),
          glm::vec3(1, 1, -1)
      );
      glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
      glDrawArrays(GL_TRIANGLES, 0, 36);

  glDisable(GL_STENCIL_TEST);
#endif

  //const unsigned int renderFlags = renderList.GetFlags();
  unsigned int renderFlags = flags;
#if 0
  bool x = false;
  if( x )
  {
    renderFlags |= RenderList::STENCIL_WRITE | RenderList::STENCIL_BUFFER_ENABLED;
  }
#endif
#if 0
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(/*whatever*/);
  glStencilOp(GL_INCR, GL_INCR, GL_INCR);    //increase the stencil value
  //draw both rectangles

  glStencilFunc(GL_EQUAL, 2, 0xFFFFFFFF);    //only draw where both rectangles are
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);    //don't change the stencil buffer
  //draw things
#endif
  //std::cout << "todor: SetRenderFlags" << std::hex << renderFlags << std::endl;

  bool enableDepthBuffer = ( ( renderFlags & RenderList::DEPTH_BUFFER_ENABLED ) != 0u );
  //enableDepthBuffer = false;

  GLbitfield clearMask = ( renderFlags & RenderList::DEPTH_CLEAR ) ? GL_DEPTH_BUFFER_BIT : 0u;

  context.EnableDepthBuffer( enableDepthBuffer );


  // Stencil enabled, writing, and clearing...
  const bool enableStencilBuffer( renderFlags & RenderList::STENCIL_BUFFER_ENABLED );
  const bool enableStencilWrite( renderFlags & RenderList::STENCIL_WRITE );

  int firstClippingActor = 2;
  bool isClippingActor = index == firstClippingActor || index == 4 || index == 7 || index == 9; //todor


  bool resetStencil = false;
  if( index == 9 )
  {
    resetStencil = true;
  }

  int testIndex = index;

  //if( ( resetStencil || index == 0 ) && enableStencilWrite )
  if( resetStencil && enableStencilWrite )
  {
    std::cout << "todor: reset stencil #######################################################" << std::endl;
    context.ClearStencil( 0xfe ); // clear down to 1st bitplane
    //clearMask |= GL_STENCIL_BUFFER_BIT;
    //context.Clear( clearMask, Context::FORCE_CLEAR );
    //testIndex -= 9;
    //testIndex += 0;
  }

  if( index >= 9 )
  {
    testIndex -= 9;
    testIndex += 4;
  }

#if 0
  if( index == 0 )
  {
    clearMask |= (renderFlags & RenderList::STENCIL_CLEAR) ? GL_STENCIL_BUFFER_BIT : 0u;
  }
#endif

  //context.Clear( clearMask, Context::CHECK_CACHED_VALUES );
  context.EnableStencilBuffer( enableStencilBuffer );

  //testIndex -= 1;
  int stencilDepth = 0;
  if( testIndex <= 2 )
  {
    stencilDepth = 0;
  }
  else if( testIndex <= 4 )
  {
    stencilDepth = 1;
  }
  else if( testIndex <= 7 )
  {
    stencilDepth = 2;
  }
  else
  {
    stencilDepth = 3;
  }
  int stencilMask = 1 << stencilDepth;

  int oldMask = stencilMask / 2;
  if( oldMask == 0 )
  {
    oldMask = 1;
  }
  int drawMask = 1;
  for( int i = 0; i < stencilDepth - 1; ++i )
  {
    drawMask = ( drawMask << 1 ) | 1;
  }

  std::cout << "todor: index: " << index << " BE:" << enableStencilBuffer << " WE:" << enableStencilWrite << " drawmask:" << drawMask << " shfdrawmask:" << ( drawMask >> 1 ) << " oldstnmask:" << oldMask << " stnmask:" << stencilMask << std::endl;

  if( enableStencilBuffer )
  //if (false)
  //if( true )
  {
    if( isClippingActor && enableStencilWrite )
    {
      //std::cout << "todor: index: " << index << " clipping with mask: " << std::hex << stencilMask << std::endl;
      //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), stencilMask, 0xff );//todor works
#if 1
      //if( true || firstClippingActor )
      //std::cout << "todor: index: " << index << " mask:" << drawMask << " stnmask:" << stencilMask << std::endl;
      if( index == firstClippingActor || resetStencil ) // dont need resetstencil todor
      {
        context.StencilFunc( GL_ALWAYS, 1, 0x1 );
      }
      else
      {
        //context.StencilFunc( GL_LEQUAL, drawMask, 0xff );
        context.StencilFunc( GL_EQUAL, drawMask, 0xff );
      }
      context.StencilMask( stencilMask );//todor
#if 0
      //if( false )
      if( resetStencil )
      {
        //context.StencilOp( GL_ZERO, GL_ZERO, GL_ZERO );
        //context.StencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
      }
      else
#endif
      {
        context.StencilOp( GL_KEEP, GL_INCR, GL_INCR );
        //context.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
      }
      //StencilBits = (StencilBits & ~StencilMask) | (StencilOp(...) & StencilMask); //todor
      //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), stencilMask << 1, stencilMask << 1);
      //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), testIndex, 0xff );
      //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), 1, 0xFF );
#endif
    }
    else
    {
      //std::cout << "todor: stn for idx: " << index << std::endl;
      //context.StencilFunc( GL_EQUAL, stencilMask, 0xFFFFFFFF ); //todor works
#if 1
      //std::cout << "todor: checking mask: " << std::hex << drawMask << std::endl;
      //context.StencilFunc( GL_EQUAL, stencilMask, stencilMask );//todor
      //context.StencilFunc( GL_ALWAYS, 1, 0xff );
      //context.StencilMask( 0xff );//todor
      context.StencilFunc( GL_EQUAL, drawMask, 0xff );
      context.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
      //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), 1, 0xFF );
      //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), 1, 0xFF );
#endif
    }

#if 0
    //todor
    if( enableStencilWrite )
    {
      //context.StencilOp( GL_INCR, GL_INCR, GL_INCR );
      if( false )
      //if( resetStencil )
      {
        context.StencilOp( GL_ZERO, GL_ZERO, GL_ZERO );
      }
      else
      {
        context.StencilOp( GL_KEEP, GL_REPLACE, GL_REPLACE );
        //context.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
      }
    }
    else
    {
      context.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
    }
#endif
    //context.StencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    //context.StencilOp(GL_INVERT, GL_INVERT, GL_REPLACE);//todor
    //context.StencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    //context.StencilMask( 0xff );//todor
    //context.Clear();//todor

    //if( index != 3 )
    {
      //clearMask |= ( renderFlags & RenderList::STENCIL_CLEAR ) ? GL_STENCIL_BUFFER_BIT : 0u; //use
    }
  }
  else
  {
    //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), 1, 0xFF );
    //context.StencilFunc( GL_EQUAL, 1, 0xFF );//todor works
    context.StencilFunc( GL_EQUAL, drawMask, 0xff );
#if 0

    std::cout << "todor: checking mask: " << std::hex << drawMask << std::endl;
    //context.StencilFunc( GL_EQUAL, stencilMask, stencilMask );//todor
    context.StencilFunc( GL_EQUAL, drawMask, 0xff );
    //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), 1, 0xFF );
    //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), 1, 0xFF );
#endif
    context.StencilMask( 0x00 );
#if 1
    context.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
#endif
  }

  // Write to stencil buffer or color buffer, but not both
  //context.StencilMask( enableStencilWrite ? 0xFF : 0x00 );
  //context.StencilMask( enableStencilWrite ? 0xFFFFFFFF : 0x00 );

  //context.StencilMask( enableStencilWrite ? stencilMask : 0x00 );
  if( resetStencil )
  {
    context.StencilMask( ~0 );
  }
  else
  {
    if( !enableStencilBuffer )
    {
      context.StencilMask( enableStencilWrite ? 0xFF : 0x00 );
    }
  }

  context.ColorMask( !enableStencilWrite );
  //context.ColorMask( true );

  // Clear depth and/or stencil buffer.
  if( clearMask )
  {
    // only clear if the depth and/or stencil buffer have been written to after a previous clear
    context.Clear( clearMask, Context::CHECK_CACHED_VALUES );
    //context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );//todor
  }

  //context.EnableStencilBuffer( false ); //todor
}
#endif

#if 0
//todor
inline void SetRenderFlagsTEST2( unsigned int flags, Context& context, int index, bool before )
{
  //todor
#if 0
  glEnable(GL_STENCIL_TEST);

      // Draw floor
      glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
      glStencilMask(0xFF); // Write to stencil buffer
      glDepthMask(GL_FALSE); // Don't write to depth buffer
      glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)

      glDrawArrays(GL_TRIANGLES, 36, 6);

      // Draw cube reflection
      glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
      glStencilMask(0x00); // Don't write anything to stencil buffer
      glDepthMask(GL_TRUE); // Write to depth buffer

      model = glm::scale(
          glm::translate(model, glm::vec3(0, 0, -1)),
          glm::vec3(1, 1, -1)
      );
      glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
      glDrawArrays(GL_TRIANGLES, 0, 36);

  glDisable(GL_STENCIL_TEST);
#endif
  //const unsigned int renderFlags = renderList.GetFlags();
  unsigned int renderFlags = flags;
#if 0
  bool x = false;
  if( x )
  {
    renderFlags |= RenderList::STENCIL_WRITE | RenderList::STENCIL_BUFFER_ENABLED;
  }
#endif
#if 0
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(/*whatever*/);
  glStencilOp(GL_INCR, GL_INCR, GL_INCR);    //increase the stencil value
  //draw both rectangles

  glStencilFunc(GL_EQUAL, 2, 0xFFFFFFFF);    //only draw where both rectangles are
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);    //don't change the stencil buffer
  //draw things
#endif
  //std::cout << "todor: SetRenderFlags" << std::hex << renderFlags << std::endl;

  bool enableDepthBuffer = ( ( renderFlags & RenderList::DEPTH_BUFFER_ENABLED ) != 0u );
  //enableDepthBuffer = false;
  GLbitfield clearMask = ( renderFlags & RenderList::DEPTH_CLEAR ) ? GL_DEPTH_BUFFER_BIT : 0u;
  context.EnableDepthBuffer( enableDepthBuffer );

  int firstClippingActor = 2;
  // DISABLE tmp stencil code
  bool stencilTestCode = true;
  if( !stencilTestCode )
  {
    index = 100;
    firstClippingActor = 100000000;
  }
  bool isClippingActor = index == firstClippingActor || index == 4 || index == 7 || index == 9; //todor
  bool resetStencil = index == 9;

  int stencilDepthC = 0;
  int stencilDepthN = 0;
  if( index >= 9 )
  {
    index -= 9;
    index += 4;
  }
  switch( index )
  {
    case 0: stencilDepthC = 0; stencilDepthN = 0; break;
    case 1: stencilDepthC = 0; stencilDepthN = 0; break;
    case 2: stencilDepthC = 0; stencilDepthN = 1; break;
    case 3: stencilDepthC = 1; stencilDepthN = 1; break;
    case 4: stencilDepthC = 1; stencilDepthN = 2; break;
    case 5: stencilDepthC = 2; stencilDepthN = 2; break;
    case 6: stencilDepthC = 2; stencilDepthN = 2; break;
    case 7: stencilDepthC = 2; stencilDepthN = 3; break;
    case 8: stencilDepthC = 3; stencilDepthN = 3; break;
    default: break;
  }
#if 0
  int stencilMaskN = 0;
  if( stencilDepthN > 0 )
    stencilMaskN = 1 << ( stencilDepthN - 1 );//todor?
  int stencilMaskC = 0;
  if( stencilDepthC > 0 )
    stencilMaskC = 1 << ( stencilDepthC - 1 );//todor?
#endif
#if 0
  int stencilMaskN = 1 << ( stencilDepthN - 1 );
  int stencilMaskC = 1 << ( stencilDepthC - 1 );
#endif
#if 0
  int oldMask = stencilMask / 2;
  if( oldMask == 0 )
  {
    oldMask = 1;
  }
#endif
#if 0
  int drawMaskN = 1;
  for( int i = 0; i < stencilDepthN - 1; ++i )//todor?
  {
    drawMaskN = ( drawMaskN << 1 ) | 1;
  }
  int drawMaskC = 1;
  for( int i = 0; i < stencilDepthC - 1; ++i )//todor?
  {
    drawMaskC = ( drawMaskC << 1 ) | 1;
  }
#endif

  int stencilMaskNew     = 1 << ( stencilDepthN - 1 );
  int stencilMaskCurrent = 1 << ( stencilDepthC - 1 );
  int drawMaskNew        = ( 1 << stencilDepthN ) - 1;
  int drawMaskCurrent    = ( 1 << stencilDepthC ) - 1;

  //std::cout << "todor: SDIN:" << stencilDepthN << ", OUT:" << stencilMaskNew << std::endl;

  renderFlags = 0;
  //std::cout << "todor: index: " << index << " render flags start: " << renderFlags << std::endl;
  if( index >= firstClippingActor )
  {
    if( before )
    {
      renderFlags |= RenderList::STENCIL_BUFFER_ENABLED;
    }
  }
  if( isClippingActor && before )
  //if( isClippingActor )
  {
    renderFlags |= RenderList::STENCIL_WRITE;
  }

  // Stencil enabled, writing, and clearing...
  const bool enableStencilBuffer( renderFlags & RenderList::STENCIL_BUFFER_ENABLED );
  const bool enableStencilWrite( renderFlags & RenderList::STENCIL_WRITE );

  if( stencilDepthC || stencilDepthN || stencilMaskNew || stencilMaskCurrent || drawMaskNew || drawMaskCurrent ||
      enableStencilBuffer || enableStencilWrite || clearMask || resetStencil )
  {}
  //int testIndex = index;
#if 0
  if( index == 0 && before ) std::cout << "################################################" << std::endl;
  std::cout << "todor: index: " << index << " SE:" << enableStencilBuffer << " WE:" << enableStencilWrite <<
      " stencilDepthC:" << stencilDepthC << " stencilDepthN:" << stencilDepthN <<
      " stencilMaskC:" << stencilMaskCurrent << " stencilMaskN:" << stencilMaskNew <<
      " drawMaskC:" << drawMaskCurrent << " drawMaskN:" << drawMaskNew <<
      std::endl;
#endif

  // ##############################################
  // ##############################################
  // ##############################################
  // ##############################################
  // ##############################################



  context.EnableStencilBuffer( enableStencilBuffer );

#if 0
  if( index == 0 && before )
  {
    //clearMask |= GL_STENCIL_BUFFER_BIT;
    //context.Clear( clearMask, Context::CHECK_CACHED_VALUES );
    context.StencilMask( 0xff );
    context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );
  }
#endif

  // Note: If we traverse up the scenegraph to a previous stencil,
  // we need to clear some of the stencil-buffers bitplanes.
  // Here, all but the first bitplane are cleared (hardcoded for testing).
  if( ( index == 0 || resetStencil ) && before )
  {
    //std::cout << "### reset ###" << std::endl;
    int buffersToClear = 0;
    if( index == 0 )
    {
      buffersToClear = 8;
    }
    else
    {
      buffersToClear = 7;
    }

    int stencilClearMask = ( ( 1 << ( 8 - buffersToClear ) ) - 1 ) ^ 0xff;
    //std::cout << "todor: index:" << index << ", stencilClearMask:" << stencilClearMask << std::endl;
    context.StencilMask( stencilClearMask );
    //context.StencilMask( 0xfe );
    clearMask |= GL_STENCIL_BUFFER_BIT;
    //context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );
  }

  if( clearMask )
  {
    // TODO: Only clear if the depth and/or stencil buffer have been written to after a previous clear
    context.Clear( clearMask, Context::CHECK_CACHED_VALUES );
    //context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );//todor
  }


  if( enableStencilBuffer && before )
  {
    if( enableStencilWrite )
    {
      if( stencilDepthN == 1 )
      {
        context.StencilFunc( GL_ALWAYS, 1, 0xff );
      }
      else
      {
        context.StencilFunc( GL_EQUAL, drawMaskNew, drawMaskCurrent );
      }
    }
    else
    {
      context.StencilFunc( GL_EQUAL, drawMaskCurrent, drawMaskCurrent );
    }
  }
  else
  {
    context.StencilFunc( GL_ALWAYS, 1, 0xff );
  }

  if( enableStencilWrite && before )
  {
    context.StencilMask( drawMaskNew );
    context.StencilOp( GL_KEEP, GL_REPLACE, GL_REPLACE );
  }
  else
  {
    context.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
  }

  // Note: Optionally we can write to the ColorMask as well as the stencil buffer
  context.ColorMask( !enableStencilWrite );














#if 0
  //if( ( resetStencil || index == 0 ) && enableStencilWrite )
  if( resetStencil && enableStencilWrite )
  {
    std::cout << "todor: reset stencil #######################################################" << std::endl;
    context.ClearStencil( 0xfe ); // clear down to 1st bitplane
    //clearMask |= GL_STENCIL_BUFFER_BIT;
    //context.Clear( clearMask, Context::FORCE_CLEAR );
    //testIndex -= 9;
    //testIndex += 0;
  }


#if 0
  if( index == 0 )
  {
    clearMask |= (renderFlags & RenderList::STENCIL_CLEAR) ? GL_STENCIL_BUFFER_BIT : 0u;
  }
#endif



  std::cout << "todor: index: " << index << " BE:" << enableStencilBuffer << " WE:" << enableStencilWrite << " drawmask:" << drawMask << " shfdrawmask:" << ( drawMask >> 1 ) << " oldstnmask:" << oldMask << " stnmask:" << stencilMask << std::endl;

  if( enableStencilBuffer )
  //if (false)
  //if( true )
  {
    if( isClippingActor && enableStencilWrite )
    {
      //std::cout << "todor: index: " << index << " clipping with mask: " << std::hex << stencilMask << std::endl;
      //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), stencilMask, 0xff );//todor works
#if 1
      //if( true || firstClippingActor )
      //std::cout << "todor: index: " << index << " mask:" << drawMask << " stnmask:" << stencilMask << std::endl;
      if( index == firstClippingActor || resetStencil ) // dont need resetstencil todor
      {
        context.StencilFunc( GL_ALWAYS, 1, 0x1 );
      }
      else
      {
        //context.StencilFunc( GL_LEQUAL, drawMask, 0xff );
        context.StencilFunc( GL_EQUAL, drawMask, 0xff );
      }
      context.StencilMask( stencilMask );//todor
#if 0
      //if( false )
      if( resetStencil )
      {
        //context.StencilOp( GL_ZERO, GL_ZERO, GL_ZERO );
        //context.StencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
      }
      else
#endif
      {
        context.StencilOp( GL_KEEP, GL_INCR, GL_INCR );
        //context.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
      }
      //StencilBits = (StencilBits & ~StencilMask) | (StencilOp(...) & StencilMask); //todor
      //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), stencilMask << 1, stencilMask << 1);
      //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), testIndex, 0xff );
      //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), 1, 0xFF );
#endif
    }
    else
    {
      //std::cout << "todor: stn for idx: " << index << std::endl;
      //context.StencilFunc( GL_EQUAL, stencilMask, 0xFFFFFFFF ); //todor works
#if 1
      //std::cout << "todor: checking mask: " << std::hex << drawMask << std::endl;
      //context.StencilFunc( GL_EQUAL, stencilMask, stencilMask );//todor
      //context.StencilFunc( GL_ALWAYS, 1, 0xff );
      //context.StencilMask( 0xff );//todor
      context.StencilFunc( GL_EQUAL, drawMask, 0xff );
      context.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
      //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), 1, 0xFF );
      //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), 1, 0xFF );
#endif
    }

#if 0
    //todor
    if( enableStencilWrite )
    {
      //context.StencilOp( GL_INCR, GL_INCR, GL_INCR );
      if( false )
      //if( resetStencil )
      {
        context.StencilOp( GL_ZERO, GL_ZERO, GL_ZERO );
      }
      else
      {
        context.StencilOp( GL_KEEP, GL_REPLACE, GL_REPLACE );
        //context.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
      }
    }
    else
    {
      context.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
    }
#endif
    //context.StencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    //context.StencilOp(GL_INVERT, GL_INVERT, GL_REPLACE);//todor
    //context.StencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    //context.StencilMask( 0xff );//todor
    //context.Clear();//todor

    //if( index != 3 )
    {
      //clearMask |= ( renderFlags & RenderList::STENCIL_CLEAR ) ? GL_STENCIL_BUFFER_BIT : 0u; //use
    }
  }
  else
  {
    //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), 1, 0xFF );
    //context.StencilFunc( GL_EQUAL, 1, 0xFF );//todor works
    context.StencilFunc( GL_EQUAL, drawMask, 0xff );
#if 0

    std::cout << "todor: checking mask: " << std::hex << drawMask << std::endl;
    //context.StencilFunc( GL_EQUAL, stencilMask, stencilMask );//todor
    context.StencilFunc( GL_EQUAL, drawMask, 0xff );
    //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), 1, 0xFF );
    //context.StencilFunc( ( enableStencilWrite ? GL_ALWAYS : GL_EQUAL ), 1, 0xFF );
#endif
    context.StencilMask( 0x00 );
#if 1
    context.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
#endif
  }

  // Write to stencil buffer or color buffer, but not both
  //context.StencilMask( enableStencilWrite ? 0xFF : 0x00 );
  //context.StencilMask( enableStencilWrite ? 0xFFFFFFFF : 0x00 );

  //context.StencilMask( enableStencilWrite ? stencilMask : 0x00 );
  if( resetStencil )
  {
    context.StencilMask( ~0 );
  }
  else
  {
    if( !enableStencilBuffer )
    {
      context.StencilMask( enableStencilWrite ? 0xFF : 0x00 );
    }
  }

  context.ColorMask( !enableStencilWrite );
  //context.ColorMask( true );

  // Clear depth and/or stencil buffer.
  if( clearMask )
  {
    // only clear if the depth and/or stencil buffer have been written to after a previous clear
    context.Clear( clearMask, Context::CHECK_CACHED_VALUES );
    //context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );//todor
  }

  //context.EnableStencilBuffer( false ); //todor
#endif
}
#endif

#if 1
/**
 * todor: SetRenderFlags for depth buffer only.
 * Sets the render flags for depth testing.
 *
 * @param[in] renderList The render list from which to get the render flags
 * @param[in] context The context
 */
inline void SetRenderFlags( const RenderList& renderList, Context& context )
{
  const unsigned int renderFlags = renderList.GetFlags();

  GLbitfield clearMask = ( renderFlags & RenderList::DEPTH_CLEAR ) ? GL_DEPTH_BUFFER_BIT : 0u;

  context.EnableDepthBuffer( ( renderFlags & RenderList::DEPTH_BUFFER_ENABLED ) != 0u );

  // Clear depth and/or stencil buffer.
  if( clearMask )
  {
    // only clear if the depth and/or stencil buffer have been written to after a previous clear
    context.Clear( clearMask, Context::CHECK_CACHED_VALUES );
  }
}
#endif

#if 0
inline void SetRenderFlagsStencilBuffer2( unsigned int flags_CURRENTLY_UNUSED, Context& context, int index, bool& hitClippingRenderer )
{
  // DISABLE tmp stencil code
#if 0
  bool stencilTestCode = true;
#else
  bool stencilTestCode = false;
#endif

  int firstClippingRenderer = 2;
  if( !stencilTestCode )
  {
    index = 100;
    firstClippingRenderer = 100000000;
  }
  bool isClippingRenderer = index == firstClippingRenderer || index == 4 || index == 7 || index == 9;
  int eraseStencilCount = 0;
  if( index == 9 )
  {
    // Note: If we traverse up the scenegraph to a previous stencil,
    // we need to clear some of the stencil-buffers bitplanes.
    // Here, all but the first bitplane are cleared (hardcoded for testing).
    eraseStencilCount = 7;
  }
  int stencilDepthC = 0;
  int stencilDepthN = 0;
  if( index >= 9 )
  {
    index -= 9;
    index += 4;
  }
  switch( index )
  {
    case 0: stencilDepthC = 0; stencilDepthN = 0; break;
    case 1: stencilDepthC = 0; stencilDepthN = 0; break;
    case 2: stencilDepthC = 0; stencilDepthN = 1; break;
    case 3: stencilDepthC = 1; stencilDepthN = 1; break;
    case 4: stencilDepthC = 1; stencilDepthN = 2; break;
    case 5: stencilDepthC = 2; stencilDepthN = 2; break;
    case 6: stencilDepthC = 2; stencilDepthN = 2; break;
    case 7: stencilDepthC = 2; stencilDepthN = 3; break;
    case 8: stencilDepthC = 3; stencilDepthN = 3; break;
    default: break;
  }

  // ################################################
  // At this point, from the outside world we need:
  //
  // Current stencil depth                                                    ( int  stencilDepthC )
  // New stencil depth                                                        ( int  stencilDepthN )
  // If this is a clipping renderer                                           ( bool isClippingRenderer )
  // If we need to traverse UP the stencil group tree, by how many stencils   ( int  eraseStencilCount )
  // ################################################

  bool stencilBufferEnabled = false;
  bool stencilWriteEnabled = false;
  bool isFirstClippingRenderer = false;

  if( isClippingRenderer )
  {
    if( !hitClippingRenderer )
    {
      isFirstClippingRenderer = true;
    }
    hitClippingRenderer = true;
    stencilWriteEnabled = true;
  }

  // TODO: Can be removed later.
  if( hitClippingRenderer )
  {
    stencilBufferEnabled = true;
  }

  context.EnableStencilBuffer( stencilBufferEnabled );

  if( stencilBufferEnabled )
  {
    if( isFirstClippingRenderer )
    {
      // We are enabling the stencil-buffer for the first time within this render list.
      // Clear the buffer at this point.
      //std::cout << "### Erasing ALL bitplanes of the stencil buffer ###" << std::endl;
      context.StencilMask( 0xff );
      context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );
    }
    else if( eraseStencilCount > 0 )
    {
      // Note: If we traverse up the scenegraph to a previous stencil,
      // we need to clear some of the stencil-buffers bitplanes.
      //std::cout << "### Erasing " << eraseStencilCount << " ###" << std::endl;
      int stencilClearMask = ( ( 1 << ( 8 - eraseStencilCount ) ) - 1 ) ^ 0xff;
      context.StencilMask( stencilClearMask );
      context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );
    }

    int drawMaskCurrent = ( 1 << stencilDepthC ) - 1;

    if( stencilWriteEnabled )
    {
      int drawMaskNew = ( 1 << stencilDepthN ) - 1;

      if( stencilDepthN == 1 )
      {
        context.StencilFunc( GL_ALWAYS, 1, 0xff );
      }
      else
      {
        context.StencilFunc( GL_EQUAL, drawMaskNew, drawMaskCurrent );
      }

      context.StencilMask( drawMaskNew );
      context.StencilOp( GL_KEEP, GL_REPLACE, GL_REPLACE );
    }
    else
    {
      context.StencilFunc( GL_EQUAL, drawMaskCurrent, drawMaskCurrent );

      context.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
    }
  }
  else
  {
    //context.StencilFunc( GL_ALWAYS, 1, 0xff );
  }

  // Note: Optionally we can write to the ColorMask as well as the stencil buffer
  context.ColorMask( !stencilWriteEnabled );
}
#endif

#if 0
inline void SetRenderFlagsStencilBuffer( unsigned int flags_CURRENTLY_UNUSED, Context& context, int index, bool& hitClippingRenderer )
{
  // DISABLE tmp stencil code
#if 1
  bool stencilTestCode = true;
#else
  bool stencilTestCode = false;
#endif

  int firstClippingRenderer = 2;
  if( !stencilTestCode )
  {
    index = 100;
    firstClippingRenderer = 100000000;
  }
  bool isClippingRenderer = index == firstClippingRenderer || index == 4 || index == 7 || index == 9;
  int eraseStencilCount = 0;
  if( index == 9 )
  {
    // Note: If we traverse up the scenegraph to a previous stencil,
    // we need to clear some of the stencil-buffers bitplanes.
    // Here, all but the first bitplane are cleared (hardcoded for testing).
    eraseStencilCount = 7;
  }
  int stencilDepthC = 0;
  int stencilDepthN = 0;
  if( index >= 9 )
  {
    index -= 9;
    index += 4;
  }
  switch( index )
  {
    case 0: stencilDepthC = 0; stencilDepthN = 0; break;
    case 1: stencilDepthC = 0; stencilDepthN = 0; break;
    case 2: stencilDepthC = 0; stencilDepthN = 1; break;
    case 3: stencilDepthC = 1; stencilDepthN = 1; break;
    case 4: stencilDepthC = 1; stencilDepthN = 2; break;
    case 5: stencilDepthC = 2; stencilDepthN = 2; break;
    case 6: stencilDepthC = 2; stencilDepthN = 2; break;
    case 7: stencilDepthC = 2; stencilDepthN = 3; break;
    case 8: stencilDepthC = 3; stencilDepthN = 3; break;
    default: break;
  }

  // ################################################
  // At this point, from the outside world we need:
  //
  // Current stencil depth to clip against (when drawing a renderer)          ( int  stencilDepthC )
  // New stencil depth                     (when WRITING a new stencil)       ( int  stencilDepthN )
  // If this is a clipping renderer                                           ( bool isClippingRenderer )
  // If we need to traverse UP the stencil group tree, by how many stencils   ( int  eraseStencilCount )
  // ################################################

  bool stencilBufferEnabled = false;
  bool stencilWriteEnabled = false;
  bool isFirstClippingRenderer = false;

  if( isClippingRenderer )
  {
    if( !hitClippingRenderer )
    {
      isFirstClippingRenderer = true;
    }
    hitClippingRenderer = true;
    stencilWriteEnabled = true;
  }

  // TODO: Can be removed later.
  if( hitClippingRenderer )
  {
    stencilBufferEnabled = true;
  }

  context.EnableStencilBuffer( stencilBufferEnabled );
  // Note: Optionally we can write to the ColorMask as well as the stencil buffer
  context.ColorMask( !stencilWriteEnabled );

  if( stencilBufferEnabled )
  {
    if( isFirstClippingRenderer )
    {
      // We are enabling the stencil-buffer for the first time within this render list.
      // Clear the buffer at this point.
      //std::cout << "### Erasing ALL bitplanes of the stencil buffer ###" << std::endl;
      context.StencilMask( 0xff );
      context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );
    }
    else if( eraseStencilCount > 0 )
    {
      // Note: If we traverse up the scenegraph to a previous stencil,
      // we need to clear some of the stencil-buffers bitplanes.
      //std::cout << "### Erasing " << eraseStencilCount << " ###" << std::endl;
      int stencilClearMask = ( ( 1 << ( 8 - eraseStencilCount ) ) - 1 ) ^ 0xff;
      context.StencilMask( stencilClearMask );
      context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );
    }

    if( stencilWriteEnabled )
    {
      int stencilMaskNew = 1 << ( stencilDepthN - 1 );
      context.StencilFunc( GL_ALWAYS, stencilMaskNew, 0u );
      context.StencilMask( stencilMaskNew );
      context.StencilOp( GL_KEEP, GL_REPLACE, GL_REPLACE );
    }
    else
    {
      int drawMaskCurrent = ( 1 << stencilDepthC ) - 1;
      context.StencilFunc( GL_EQUAL, drawMaskCurrent, 0xff );
      context.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
    }
  }
}
#endif

#if 1
inline void SetupStencilBuffer( const RenderItem& item, Context& context, int& lastStencilDepth )
{
  const Dali::Internal::SceneGraph::Node& node = item.GetNode();
  int clippingId = node.GetClippingId();

  if( clippingId == 0 )
  {
    // Exit immediately if there are no clipping actions to perform (EG. we have not yet hit a clipping node).
    // Set the stencil test to always pass.
    context.EnableStencilBuffer( false );//todor: optimise so happens once?
    return;
  }

  const Dali::ClippingMode::Type clippingMode = node.GetClippingMode();
  const int currentStencilDepth = node.GetClippingDepth();
  if( CLIP_DEBUG ) std::cout << "todor: ============================================== RENDERING: Node:" << node.GetName() << " Mode:" << clippingMode << " Id:" << clippingId << " Depth:" << currentStencilDepth << std::endl;

  context.EnableStencilBuffer( true );//todor: optimise so happens once?

  // Optionally we can write to the ColorMask as well as the stencil buffer.
  // If Clipping is enabled, we are ONLY writing to the stencil buffer.
  context.ColorMask( clippingMode != Dali::ClippingMode::CLIPPING_ENABLED );

  // If we have a clipping mode specified, we are writing to the stencil buffer.
  if( clippingMode != Dali::ClippingMode::CLIPPING_DISABLED )
  {
    // If clipping Id is 1, this is the first clipping renderer within this render-list.
    // todor: May have to make this persistent if clipping renderers get reordered.
    if( clippingId == 1 )
    {
      // We are enabling the stencil-buffer for the first time within this render list.
      // Clear the buffer at this point.
      if( CLIP_DEBUG ) std::cout << "todor: ### Erasing ALL bitplanes of the stencil buffer ###" << std::endl;
      context.StencilMask( 0xff );
      context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );
    }
    else if( currentStencilDepth < lastStencilDepth )
    {
      // Note: If we traverse up the scene-graph to a previous stencil,
      // we need to clear some of the stencil-buffers bitplanes.
      // This calculation takes the new depth to move to, and creates an inverse-mask of that number of consecutive bits.
      // This has the effect of clearing everything except the bitplates up to our current depth.
      int stencilClearMask = ( ( 1 << ( currentStencilDepth - 1 ) ) - 1 ) ^ 0xff;
      if( CLIP_DEBUG ) std::cout << "todor: Reverting stencil depth to: " << currentStencilDepth << " with mask: " << std::hex << stencilClearMask << std::endl;
      context.StencilMask( stencilClearMask );
      context.Clear( GL_STENCIL_BUFFER_BIT, Context::CHECK_CACHED_VALUES );
    }
    lastStencilDepth = currentStencilDepth;

    // todor: comment calculation
    int stencilMaskNew = 1 << ( currentStencilDepth - 1 );
    context.StencilFunc( GL_ALWAYS, stencilMaskNew, 0u );
    if( CLIP_DEBUG ) std::cout << std::hex << "todor: drawing to stencil buf, mask: " << stencilMaskNew << std::endl;
    context.StencilMask( stencilMaskNew );
    context.StencilOp( GL_KEEP, GL_REPLACE, GL_REPLACE );
  }
  else
  {
    // todor: comment calculation
    int drawMaskCurrent = ( 1 << currentStencilDepth ) - 1;
    if( CLIP_DEBUG ) std::cout << std::hex << "todor: reading from stencil buf, mask: " << drawMaskCurrent << std::endl;
    context.StencilFunc( GL_EQUAL, drawMaskCurrent, 0xff );
    context.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
  }
}
#endif

/**
 * Process a render-list.
 * @param[in] renderList The render-list to process.
 * @param[in] context The GL context.
 * @param[in] defaultShader The default shader to use.
 * @param[in] buffer The current render buffer index (previous update buffer)
 * @param[in] viewMatrix The view matrix from the appropriate camera.
 * @param[in] projectionMatrix The projection matrix from the appropriate camera.
 */
inline void ProcessRenderList(
  const RenderList& renderList,
  Context& context,
  SceneGraph::TextureCache& textureCache,
  SceneGraph::Shader& defaultShader,
  BufferIndex bufferIndex,
  const Matrix& viewMatrix,
  const Matrix& projectionMatrix )
{
  if( CLIP_DEBUG ) std::cout << "todor: ProcessRenderList:" << renderList.GetName() << std::endl;
  DALI_PRINT_RENDER_LIST( renderList );

  SetRenderFlags( renderList, context );

#ifdef ENABLED_NEW_CLIPPING
  int lastStencilDepth = 0;
#endif

  if( renderList.HasColorRenderItems() )
  {
    bool depthBufferEnabled = ( ( renderList.GetFlags() & RenderList::DEPTH_BUFFER_ENABLED ) != 0u );
    size_t count = renderList.Count();

    for ( size_t index = 0; index < count; ++index )
    {
      const RenderItem& item = renderList.GetItem( index );
      DALI_PRINT_RENDER_ITEM( item );

#ifdef ENABLED_NEW_CLIPPING
      SetupStencilBuffer( item, context, lastStencilDepth );
#endif

      //Enable depth writes if depth buffer is enabled and item is opaque
      context.DepthMask( depthBufferEnabled && ( item.IsOpaque() || item.GetRenderer().RequiresDepthTest() ) );

      item.GetRenderer().Render( context, textureCache, bufferIndex, item.GetNode(), defaultShader, item.GetModelViewMatrix(), viewMatrix, projectionMatrix, !item.IsOpaque() );
    }
  }
  else
  {
    size_t count = renderList.Count();
    if( CLIP_DEBUG ) std::cout << "todor: ProcessRenderList: NOT HasColorRenderItems: " << (int)count << std::endl;
    for ( size_t index = 0; index < count; ++index )
    {
      const RenderItem& item = renderList.GetItem( index );
      DALI_PRINT_RENDER_ITEM( item );

      item.GetRenderer().Render( context, textureCache, bufferIndex, item.GetNode(), defaultShader, item.GetModelViewMatrix(), viewMatrix, projectionMatrix, !item.IsOpaque() );
    }
  }
}

void ProcessRenderInstruction( const RenderInstruction& instruction,
                               Context& context,
                               SceneGraph::TextureCache& textureCache,
                               SceneGraph::Shader& defaultShader,
                               BufferIndex bufferIndex )
{
  DALI_PRINT_RENDER_INSTRUCTION( instruction, bufferIndex );

  const Matrix* viewMatrix       = instruction.GetViewMatrix( bufferIndex );
  const Matrix* projectionMatrix = instruction.GetProjectionMatrix( bufferIndex );

  DALI_ASSERT_DEBUG( NULL != viewMatrix );
  DALI_ASSERT_DEBUG( NULL != projectionMatrix );

  if( NULL != viewMatrix &&
      NULL != projectionMatrix )
  {
    const RenderListContainer::SizeType count = instruction.RenderListCount();

    // Iterate through each render list in order. If a pair of render lists
    // are marked as interleaved, then process them together.
    for( RenderListContainer::SizeType index = 0; index < count; ++index )
    {
      const RenderList* renderList = instruction.GetRenderList( index );

      if(  renderList &&
          !renderList->IsEmpty() )
      {
        ProcessRenderList( *renderList, context, textureCache, defaultShader, bufferIndex, *viewMatrix, *projectionMatrix );
      }
    }
  }
}

} // namespace Render

} // namespace Internal

} // namespace Dali
