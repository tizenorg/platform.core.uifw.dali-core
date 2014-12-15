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

#include "test-platform-abstraction.h"
#include "dali-test-suite-utils.h"
#include <dali/integration-api/bitmap.h>

namespace Dali
{

/**
 * Constructor
 */
TestPlatformAbstraction::TestPlatformAbstraction()
: mRequest(0),
  mDynamicsFactory(NULL)
{
  Initialize();
}

/**
 * Destructor
 */
TestPlatformAbstraction::~TestPlatformAbstraction()
{
  delete mDynamicsFactory;
}

/**
 * @copydoc PlatformAbstraction::GetTimeMicroseconds()
 */
void TestPlatformAbstraction::GetTimeMicroseconds(unsigned int &seconds, unsigned int &microSeconds)
{
  seconds = mSeconds;
  microSeconds = mMicroSeconds;
  mTrace.PushCall("GetTimeMicroseconds", "");
}

/**
 * @copydoc PlatformAbstraction::Suspend()
 */
void TestPlatformAbstraction::Suspend()
{
  mTrace.PushCall("Suspend", "");
}

/**
 * @copydoc PlatformAbstraction::Resume()
 */
void TestPlatformAbstraction::Resume()
{
  mTrace.PushCall("Resume", "");
}

void TestPlatformAbstraction::GetClosestImageSize( const std::string& filename,
                                                   const ImageAttributes& attributes,
                                                   Vector2& closestSize)
{
  closestSize = mClosestSize;
  mTrace.PushCall("GetClosestImageSize", "");
}

void TestPlatformAbstraction::GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                                   const ImageAttributes& attributes,
                                                   Vector2& closestSize)
{
  closestSize = mClosestSize;
  mTrace.PushCall("GetClosestImageSize", "");
}


/**
 * @copydoc PlatformAbstraction::LoadResource()
 */
void TestPlatformAbstraction::LoadResource(const Integration::ResourceRequest& request)
{
  mTrace.PushCall("LoadResource", "");
  if(mRequest != NULL)
  {
    delete mRequest;
    tet_infoline ("Warning: multiple resource requests not handled by Test Suite. You may see unexpected errors");
  }
  mRequest = new Integration::ResourceRequest(request);
}

Integration::ResourcePointer TestPlatformAbstraction::LoadResourceSynchronously( const Integration::ResourceType& resourceType, const std::string& resourcePath)
{
  mTrace.PushCall("LoadResourceSynchronously", "");
  return mResources.loadedResource;
}

/**
 * @copydoc PlatformAbstraction::SaveResource()
 */
void TestPlatformAbstraction::SaveResource(const Integration::ResourceRequest& request)
{
  mTrace.PushCall("SaveResource", "");
  if(mRequest != NULL)
  {
    delete mRequest;
    tet_infoline ("Warning: multiple resource requests not handled by Test Suite. You may see unexpected errors");
  }
  mRequest = new Integration::ResourceRequest(request);
}

/**
 * @copydoc PlatformAbstraction::CancelLoad()
 */
void TestPlatformAbstraction::CancelLoad(Integration::ResourceId id, Integration::ResourceTypeId typeId)
{
  mTrace.PushCall("CancelLoad", "");
}

/**
 * @copydoc PlatformAbstraction::GetResources()
 */
void TestPlatformAbstraction::GetResources(Integration::ResourceCache& cache)
{
  mTrace.PushCall("GetResources", "");

  if(mResources.loaded)
  {
    cache.LoadResponse( mResources.loadedId, mResources.loadedType, mResources.loadedResource, Integration::RESOURCE_COMPLETELY_LOADED );
  }
  if(mResources.loadFailed)
  {
    cache.LoadFailed( mResources.loadFailedId, mResources.loadFailure );
  }
  if(mResources.saved)
  {
    cache.SaveComplete( mResources.savedId, mResources.savedType );
  }
  if(mResources.saveFailed)
  {
    cache.SaveFailed( mResources.saveFailedId, mResources.saveFailure );
  }
}

/**
 * @copydoc PlatformAbstraction::IsLoading()
 */
bool TestPlatformAbstraction::IsLoading()
{
  mTrace.PushCall("IsLoading", "");
  return mIsLoadingResult;
}

/**
 * @copydoc PlatformAbstraction::SetDpi()
 */
void TestPlatformAbstraction::SetDpi (unsigned int dpiHorizontal, unsigned int dpiVertical)
{
  mTrace.PushCall("SetDpi", "");
}

/**
 * @copydoc PlatformAbstraction::LoadFile()
 */
bool TestPlatformAbstraction::LoadFile( const std::string& filename, std::vector< unsigned char >& buffer ) const
{
  mTrace.PushCall("LoadFile", "");
  if( mLoadFileResult.loadResult )
  {
    buffer = mLoadFileResult.buffer;
  }

  return mLoadFileResult.loadResult;
}

/**
 * @copydoc PlatformAbstraction::SaveFile()
 */
bool TestPlatformAbstraction::SaveFile(const std::string& filename, std::vector< unsigned char >& buffer) const
{
  mTrace.PushCall("SaveFile", "");
  return false;
}

void TestPlatformAbstraction::JoinLoaderThreads()
{
  mTrace.PushCall("JoinLoaderThreads", "");
}

Integration::DynamicsFactory* TestPlatformAbstraction::GetDynamicsFactory()
{
  mTrace.PushCall("GetDynamicsFactory", "");
  if( mDynamicsFactory == NULL )
  {
    mDynamicsFactory = new TestDynamicsFactory( mTrace );
  }
  return mDynamicsFactory;
}

bool TestPlatformAbstraction::ReadGlobalMetricsFromCache( const std::string& fontFamily,
                                                          const std::string& fontStyle,
                                                          Integration::GlobalMetrics& globalMetrics )
{
  mTrace.PushCall("ReadGlobalMetricsFromCacheFile", "");
  globalMetrics = mReadGlobalMetrics; // Want to copy contents...
  return mReadGlobalMetricsResult; // Default false (will be set to true on subsequent write)
}

void TestPlatformAbstraction::WriteGlobalMetricsToCache( const std::string& fontFamily,
                                                         const std::string& fontStyle,
                                                         const Integration::GlobalMetrics& globalMetrics )
{
  // Copy so next read uses written values. TODO: Could add method
  // to turn this behaviour off for more extensive testing.
  mReadGlobalMetrics = globalMetrics;
  mReadGlobalMetricsResult = true;

  mTrace.PushCall("WriteGlobalMetricsToCacheFile", "");
}

bool TestPlatformAbstraction::ReadMetricsFromCache( const std::string& fontFamily,
                                                    const std::string& fontStyle,
                                                    std::vector<Integration::GlyphMetrics>& glyphMetricsContainer )
{
  mTrace.PushCall("ReadMetricsFromCacheFile", "");
  glyphMetricsContainer = mReadMetrics;
  return mReadMetricsResult; // Default false (will be set to true on subsequent write)
}

void TestPlatformAbstraction::WriteMetricsToCache( const std::string& fontFamily,
                                                   const std::string& fontStyle,
                                                   const Integration::GlyphSet& glyphSet )
{
  // Copy so next read uses written values. TODO: Could add method
  // to turn this behaviour off for more extensive testing.
  const Integration::GlyphSet::CharacterList& charList =  glyphSet.GetCharacterList();
  mReadMetrics.clear();
  for(std::size_t i=0, end=charList.size(); i<end; ++i)
  {
    mReadMetrics.push_back(charList[i].second);
  }
  mReadMetricsResult = true;

  mTrace.PushCall("WriteMetricsToCacheFile", "");
}

void TestPlatformAbstraction::GetFileNamesFromDirectory( const std::string& directoryName,
                                                         std::vector<std::string>& fileNames )
{
  fileNames.push_back( std::string( "u1f004.png" ) );
  fileNames.push_back( std::string( "u1f0cf.png" ) );
  fileNames.push_back( std::string( "u1f170.png" ) );
  fileNames.push_back( std::string( "u1f601.png" ) );
}

Integration::BitmapPtr TestPlatformAbstraction::GetGlyphImage( const std::string& fontFamily, const std::string& fontStyle, float fontSize, uint32_t character ) const
{
  Integration::BitmapPtr image = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
  image->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, 1, 1 );

  mTrace.PushCall("GetGlyphImage", "");

  return image;
}

/** Call this every test */
void TestPlatformAbstraction::Initialize()
{
  mTrace.Reset();
  mTrace.Enable(true);
  memset(&mResources, 0, sizeof(Resources));
  mSeconds=0;
  mMicroSeconds=0;
  mIsLoadingResult=false;

  if(mRequest)
  {
    delete mRequest;
    mRequest = 0;
  }
}

bool TestPlatformAbstraction::WasCalled(TestFuncEnum func)
{
  switch(func)
  {
    case GetTimeMicrosecondsFunc:             return mTrace.FindMethod("GetTimeMicroseconds");
    case SuspendFunc:                         return mTrace.FindMethod("Suspend");
    case ResumeFunc:                          return mTrace.FindMethod("Resume");
    case LoadResourceFunc:                    return mTrace.FindMethod("LoadResource");
    case SaveResourceFunc:                    return mTrace.FindMethod("SaveResource");
    case LoadFileFunc:                        return mTrace.FindMethod("LoadFile");
    case SaveFileFunc:                        return mTrace.FindMethod("SaveFile");
    case CancelLoadFunc:                      return mTrace.FindMethod("CancelLoad");
    case GetResourcesFunc:                    return mTrace.FindMethod("GetResources");
    case IsLoadingFunc:                       return mTrace.FindMethod("IsLoading");
    case JoinLoaderThreadsFunc:               return mTrace.FindMethod("JoinLoaderThreads");
    case GetDynamicsFactoryFunc:              return mTrace.FindMethod("GetDynamicsFactory");
  }
  return false;
}

void TestPlatformAbstraction::SetGetTimeMicrosecondsResult(size_t sec, size_t usec)
{
  mSeconds = sec;
  mMicroSeconds = usec;
}

void TestPlatformAbstraction::IncrementGetTimeResult(size_t milliseconds)
{
  mMicroSeconds += milliseconds * 1000u;
  unsigned int additionalSeconds = mMicroSeconds / 1000000u;

  mSeconds += additionalSeconds;
  mMicroSeconds -= additionalSeconds * 1000000u;
}

void TestPlatformAbstraction::SetIsLoadingResult(bool result)
{
  mIsLoadingResult = result;
}

void TestPlatformAbstraction::ClearReadyResources()
{
  memset(&mResources, 0, sizeof(Resources));
}

void TestPlatformAbstraction::SetResourceLoaded(Integration::ResourceId  loadedId,
                                                Integration::ResourceTypeId  loadedType,
                                                Integration::ResourcePointer loadedResource)
{
  mResources.loaded = true;
  mResources.loadedId = loadedId;
  mResources.loadedType = loadedType;
  mResources.loadedResource = loadedResource;
}

void TestPlatformAbstraction::SetResourceLoadFailed(Integration::ResourceId  id,
                                                    Integration::ResourceFailure failure)
{
  mResources.loadFailed = true;
  mResources.loadFailedId = id;
  mResources.loadFailure = failure;
}

void TestPlatformAbstraction::SetResourceSaved(Integration::ResourceId      savedId,
                                               Integration::ResourceTypeId  savedType)
{
  mResources.saved = true;
  mResources.savedId = savedId;
  mResources.savedType = savedType;
}

void TestPlatformAbstraction::SetResourceSaveFailed(Integration::ResourceId  id,
                                                    Integration::ResourceFailure failure)
{
  mResources.saveFailed = true;
  mResources.saveFailedId = id;
  mResources.saveFailure = failure;
}

Integration::ResourceRequest* TestPlatformAbstraction::GetRequest()
{
  return mRequest;
}

void TestPlatformAbstraction::DiscardRequest()
{
  delete mRequest;
  mRequest = NULL;
}

void TestPlatformAbstraction::SetClosestImageSize(const Vector2& size)
{
  mClosestSize = size;
}

void TestPlatformAbstraction::SetLoadFileResult( bool result, std::vector< unsigned char >& buffer )
{
  mLoadFileResult.loadResult = result;
  if( result )
  {
    mLoadFileResult.buffer = buffer;
  }
}

void TestPlatformAbstraction::SetSaveFileResult( bool result )
{
  mSaveFileResult = result;
}

} // namespace Dali
