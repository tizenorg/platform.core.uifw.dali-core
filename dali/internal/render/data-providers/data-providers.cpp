/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include "data-providers.h"

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

DataProviders::DataProviders()
{
}

DataProviders::~DataProviders()
{
}

void DataProviders::SetVertexBuffers( const DataProviders::VertexBuffers& vertexBuffers)
{
  mVertexBuffers = vertexBuffers;
}

const DataProviders::VertexBuffers& DataProviders::GetVertexBuffers() const
{
  return mVertexBuffers;
}

void DataProviders::SetIndexBuffer( const PropertyBuffer* indexBuffer )
{
  mIndexBuffer = indexBuffer;
}

const PropertyBuffer* DataProviders::GetIndexBuffer() const
{
  return mIndexBuffer;
}

void DataProviders::SetShader( Shader* shader )
{
  mShader = shader;
}

Shader* DataProviders::GetShader() const
{
  return mShader;
}

void DataProviders::SetSamplers( const DataProviders::Samplers& samplers )
{
  mSamplers = samplers;
}

const DataProviders::Samplers& DataProviders::GetSamplers() const
{
  return mSamplers;
}

void DataProviders::SetMaterial( const MaterialDataProvider* materialDataProvider )
{
  mMaterialDataProvider = materialDataProvider;
}

const MaterialDataProvider* DataProviders::GetMaterial() const
{
  return mMaterialDataProvider;
}

void DataProviders::SetGeometry( const GeometryDataProvider* geometryDataProvider )
{
  mGeometryDataProvider = geometryDataProvider;
}

const GeometryDataProvider* DataProviders::GetGeometry() const
{
  return mGeometryDataProvider;
}

void DataProviders::SetUniformMap(const UniformMapDataProvider* uniformMapDataProvider)
{
  mUniformMapDataProvider = uniformMapDataProvider;
}

const UniformMapDataProvider* DataProviders::GetUniformMap() const
{
  return mUniformMapDataProvider;
}

} // SceneGraph
} // Internal
} // Dali
