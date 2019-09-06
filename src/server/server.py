#!/usr/bin/env python
#
# Copyright (c) ZeroC, Inc. All rights reserved.
#

import signal
import sys
import Ice

from efscape.server import ModelHomeI, run_server, test_devs, createGPT
#
# Ice.initialize returns an initialized Ice communicator,
# the communicator is destroyed once it goes out of scope.
#
with Ice.initialize(sys.argv) as communicator:

    #
    # Install a signal handler to shutdown the communicator on Ctrl-C
    #
    signal.signal(signal.SIGINT, lambda signum, frame: communicator.shutdown())

    #
    # The communicator initialization removes all Ice-related arguments from argv
    #
    if len(sys.argv) > 1:
        print(sys.argv[0] + ": too many arguments")
        sys.exit(1)

    modelInfo = {
        "modelName": "GPT",
        "description": "Classic DEVS GPT model",
        "ports": {
            "output_port": 2,
            "test_input_port": 3
        },
        "output_producer": "Observer"
    }
    ModelHomeI.addModel("GPT", createGPT, modelInfo)

    properties = communicator.getProperties()
    adapter = communicator.createObjectAdapter("ModelHome")
    id = Ice.stringToIdentity(properties.getProperty("Identity"))
    adapter.add(ModelHomeI(), id)
    adapter.activate()
    communicator.waitForShutdown()
