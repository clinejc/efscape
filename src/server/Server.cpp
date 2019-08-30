// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Server.cpp
// Copyright (C) 2006-2019 Jon C. Cline
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
// RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
// CONTRACT, NEGLIGENCE OR// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
// CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
// __COPYRIGHT_END__
//=============================================================================

#include <Ice/Ice.h>

// ModelHome servant definition
#include "ModelHomeI.hpp"

// definitions for accessing the model factory
#include <efscape/impl/ModelHomeI.hpp>
#include <efscape/impl/ModelHomeSingleton.hpp>

using namespace std;

/**
 * Implements an ICE (Internet Connection Engine) server simple interactive
 * model for the efscape modeling framework. It provides a command-line
 * interface derived from the CommandOpt class.
 *
 * @author Jon Cline <clinej@stanfordalumni.org>
 * @version 2.0.0 created 23 Sep 2014, revised 18 Aug 2019
 * @param argc argument count
 * @param argv array of arguments
 * @return exit status
 *
 * ChangeLog:
 *   - 2019-08-18 Updated to use IceGrid (forked from original RunServer)
 */
int main(int argc, char* argv[])
{
    int status = 0;

    //
    // hard-coded setting of logging level
    //
    efscape::impl::ModelHomeI::getLogger()->setLevel(log4cxx::Level::getDebug());

    LOG4CXX_DEBUG(efscape::impl::ModelHomeI::getLogger(), "Loading libraries...");
    efscape::impl::Singleton<efscape::impl::ModelHomeI>::Instance().LoadLibraries();

    try
    {
        //
        // CtrlCHandler must be created before the communicator or any other threads are started
        //
        Ice::CtrlCHandler ctrlCHandler;

        //
        // Set the 'BuildId' property displayed in the IceGridGUI
        //
        Ice::InitializationData initData;
        initData.properties = Ice::createProperties();
        initData.properties->setProperty("BuildId", string("Ice ") + ICE_STRING_VERSION);

        //
        // CommunicatorHolder's ctor initializes an Ice communicator,
        // and its dtor destroys this communicator.
        //
        Ice::CommunicatorHolder ich(argc, argv, initData);
        auto communicator = ich.communicator();

        ctrlCHandler.setCallback(
            [communicator](int)
            {
                communicator->shutdown();
            });

        //
        // The communicator initialization removes all Ice-related arguments from argc/argv
        //
        if(argc > 1)
        {
            cerr << argv[0] << ": too many arguments" << endl;
            status = 1;
        }
        else
        {
            auto properties = communicator->getProperties();
            auto adapter = communicator->createObjectAdapter("ModelHome");
            auto id = Ice::stringToIdentity(properties->getProperty("Identity"));
            auto modelhome = make_shared<ModelHomeI>();//properties->getProperty("Ice.ProgramName"));
            adapter->add(modelhome, id);
            adapter->activate();

            communicator->waitForShutdown();
        }
    }
    catch(const std::exception& ex)
    {
        cerr << ex.what() << endl;
        status = 1;
    }

    return status;
}
