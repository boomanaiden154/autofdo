// Copyright 2014 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "symbolize/bytereader.h"
#include "symbolize/bytereader-inl.h"

#include "base/common.h"

namespace devtools_crosstool_autofdo {

ByteReader::ByteReader(enum Endianness endian)
    :offset_reader_(NULL), address_reader_(NULL), endian_(endian),
     address_size_(0), offset_size_(0)
{ }

ByteReader::~ByteReader() { }

void ByteReader::SetOffsetSize(uint8 size) {
  offset_size_ = size;
  CHECK(size == 4 || size == 8);
  if (size == 4) {
    this->offset_reader_ = &ByteReader::ReadFourBytes;
  } else {
    this->offset_reader_ = &ByteReader::ReadEightBytes;
  }
}

void ByteReader::SetAddressSize(uint8 size) {
  address_size_ = size;
  CHECK(size == 4 || size == 8);
  if (size == 4) {
    this->address_reader_ = &ByteReader::ReadFourBytes;
  } else {
    this->address_reader_ = &ByteReader::ReadEightBytes;
  }
}

uint64 ByteReader::ReadInitialLength(const char* start, size_t* len) {
    const uint64 initial_length = ReadFourBytes(start);
    start += 4;

    // In DWARF2/3, if the initial length is all 1 bits, then the offset
    // size is 8 and we need to read the next 8 bytes for the real length.
    if (initial_length == 0xffffffff) {
        SetOffsetSize(8);
        *len = 12;
        return ReadOffset(start);
    }
    else {
        SetOffsetSize(4);
        *len = 4;
    }
    return initial_length;
}

}  // namespace devtools_crosstool_autofdo
