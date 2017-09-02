// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelType.ipp
// Copyright (C) 2006-2017 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
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
