#ifndef __DALI_SERIALIZABLE__
#define __DALI_SERIALIZABLE__

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

namespace Dali
{

/**
 * An abstract class that other classes can inherit from if their data is required to be serialized into a stream.
 *
 */

class Serializable
{
 public:

  /**
   * @brief Constructor
   */
  virtual Serializable(){};

  /**
   * @brief destructor
   */
  virtual ~Serializable(){};

  /**
   * @brief Method to serialize data of class using this interface
   * @param[in, out] os The ostream to hold the serialised data

   */
  virtual void serialize(std::ostream& os) const = 0;

};

namespace Serialize
{

std::ostream& operator<< (std::ostream& os, const Serializable& s)
{
  s.serialize(os);
  return os;
}

} // Serialize

} // namespace Dali

#endif // __DALI_SERIALIZABLE__
