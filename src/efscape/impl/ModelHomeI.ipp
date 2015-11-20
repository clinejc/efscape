// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHomeI.ipp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#ifndef EFSCAPE_IMPL_MODELHOMEI_IPP
#define EFSCAPE_IMPL_MODELHOMEI_IPP

#include <efscape/impl/ModelHomeI.hh>

#include <boost/functional/factory.hpp>

namespace efscape {
  namespace impl {

    /**
     * Registers a command object (derived class of efscape::utils::CommandOpt)
     * in the CommandFactory.
     *
     * @tparam DerivedType derived command type
     * @param aC_cmd_name name of derived command
     * @returns whether the registration was successful
     */
    template <class DerivedType>
    bool ModelHomeI::registerCommand(std::string aC_cmd_name)
    {
      LOG4CXX_DEBUG(ModelHomeI::getLogger(),
		    "Registering CommandOpt class <" << aC_cmd_name << ">");

      if (mCpF_CommandFactory.get() == NULL)
        mCpF_CommandFactory.reset( new command_factory_map );
  
      (*mCpF_CommandFactory)[aC_cmd_name] = boost::factory<DerivedType*>();

      return true;		// todo: check if successful
    }

  } // namespace impl
} // namespace efscape

#endif	// #ifndef EFSCAPE_IMPL_MODELHOMEI_IPP
