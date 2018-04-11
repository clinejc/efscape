// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelType.ipp
// Copyright (C) 2006-2018 Jon C. Cline
// 
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
// __COPYRIGHT_END__

#include <efscape/impl/ModelType.hpp>
#include <efscape/utils/type.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/access.hpp>

#include <cereal/archives/json.hpp>

#include <sstream>

namespace efscape {
  namespace impl {
    
    template <class DataType>
    Json::Value exportDataTypeToJSON(DataType value) {
      std::stringstream lC_buffer_out;
      cereal::JSONOutputArchive oa( lC_buffer_out );
      oa( cereal::make_nvp(efscape::utils::type<DataType>(), value) );
      lC_buffer_out << "}";	// bug?: need a closing "}"

      Json::Value lC_jsonValue;
      
      lC_buffer_out >> lC_jsonValue;

      return lC_jsonValue;
    }

  } // namespace impl
} // namespace efscape
