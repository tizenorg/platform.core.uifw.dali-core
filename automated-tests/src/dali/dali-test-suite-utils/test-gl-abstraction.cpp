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

#include "test-gl-abstraction.h"

namespace Dali
{

TestGlAbstraction::TestGlAbstraction()
{
  Initialize();
}

TestGlAbstraction::~TestGlAbstraction() {}

void TestGlAbstraction::Initialize()
{
  mCurrentProgram = 0;
  mCompileStatus = GL_TRUE;
  mLinkStatus = GL_TRUE;

  mGetAttribLocationResult = 0;
  mGetErrorResult = 0;
  mGetStringResult = NULL;
  mIsBufferResult = 0;
  mIsEnabledResult = 0;
  mIsFramebufferResult = 0;
  mIsProgramResult = 0;
  mIsRenderbufferResult = 0;
  mIsShaderResult = 0;
  mIsTextureResult = 0;
  mVertexAttribArrayChanged = false;

  mCheckFramebufferStatusResult = 0;
  mFramebufferStatus = 0;
  mNumBinaryFormats = 0;
  mBinaryFormats = 0;
  mProgramBinaryLength = 0;
  mGetProgramBinaryCalled = false;

  mLastAutoTextureIdUsed = 0;

  mLastShaderIdUsed = 0;
  mLastProgramIdUsed = 0;
  mLastUniformIdUsed = 0;
  mLastShaderCompiled = 0;
  mLastClearBitMask = 0;
  mClearCount = 0;

  mLastBlendEquationRgb   = 0;
  mLastBlendEquationAlpha = 0;
  mLastBlendFuncSrcRgb    = 0;
  mLastBlendFuncDstRgb    = 0;
  mLastBlendFuncSrcAlpha  = 0;
  mLastBlendFuncDstAlpha  = 0;

  mUniforms.clear();
  mProgramUniforms1i.clear();
  mProgramUniforms1f.clear();
  mProgramUniforms2f.clear();
  mProgramUniforms3f.clear();
  mProgramUniforms4f.clear();
}

void TestGlAbstraction::PreRender()
{
}

void TestGlAbstraction::PostRender(unsigned int timeDelta)
{
}

} // Namespace dali

bool BlendEnabled(const Dali::TraceCallStack& callStack)
{
  std::stringstream out;
  out << GL_BLEND;
  bool blendEnabled = callStack.FindMethodAndParams("Enable", out.str());
  return blendEnabled;
}

bool BlendDisabled(const Dali::TraceCallStack& callStack)
{
  std::stringstream out;
  out << GL_BLEND;
  bool blendEnabled = callStack.FindMethodAndParams("Disable", out.str());
  return blendEnabled;
}
