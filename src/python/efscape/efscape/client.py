# -*- coding: utf-8 -*-

import sys
import os
import click
import logging
import click_log
import Ice
import IceGrid
from pathlib import Path
import json

# configure loggings
logger = logging.getLogger()  #__name__)
click_log.basic_config(logger)
logger.setLevel(logging.DEBUG)

# 1. set the path of the 'efscape' slice defitions
efscape_slice_dir = Path(os.environ['EFSCAPE_PATH']) / 'src/slice'

# 2. generate the python stubs for 'efscape'
Ice.loadSlice('-I' + str(efscape_slice_dir) + ' --all ' + str(efscape_slice_dir / 'efscape/ModelHome.ice'))

# 3. import efscape python API
import efscape

def view_available_models(modelHome):
    """
    Lists all models currently loaded in the ModelHome

    Parameters
    ----------
    modelHome efscape.ModelHomePrx
        handle to model home

    Returns
    -------
    int status
    """
    modelList = modelHome.getModelList()

    idx = 0
    for x in modelList:
        idx = idx + 1
        print(str(idx) + ': ' + x)

    modelName = None
    while True:
        # prompt user for input: number of the model
        response = input('\nEnter the number of the model (0 to exit)==> ')
        if int(response) > 0 and int(response) <= len(modelList):
            modelName = modelList[int(response) - 1]
            print('selected ' + modelName)
        elif int(response) == 0:
            if modelName is None:
                break
                
            parmString = modelHome.getModelInfo(modelName)
            parameters = json.loads(parmString)
            print(json.dumps(parameters, indent=4, separators=(',', ': '), sort_keys=True))

            # if the parameter 'modelName' exists, attempt to write
            # the default parameters to file [modelName].json
            if 'modelName' in parameters:
                parmName = parameters['modelName']

                with open(parmName + '.json', 'w') as f:
                    json.dump(parameters, f, indent=4)  #, separators=(',', ': '), sort_keys=True)
                    #add trailing newline for POSIX compatibility
                    f.write('\n')

                parmFile = open(parmName + '.json', 'w')
                parmFile.write(parmString)
                parmFile.close()
            
            break

    print('done')
    
    return 0

def run_simulation(simulator, timeMax=100.):
    """
    Run the efscape simulation

    Parameters
    ----------
    simulator efscape.SimulatorPrx
        simulator proxy
    timeMax double
        simulation duration
    """
    model = simulator.getModel()
    if not model:
        logger.error("run_simulation: model not found!")
        return 1

    t = 0
    idx = 0
    if simulator.start():
        logger.debug('simulator started!')
        message = model.outputFunction()
        logger.info('message size = ' + str(len(message)))
        for x in message:
            logger.debug('message ' + str(idx) + ': value on port <' + \
                      x.port + '> = ' +  x.valueToJson)

        while not simulator.halt():
            t = simulator.nextEventTime()
            if t > timeMax:
                break
            logger.info('Next event time = ' + str(t))
            simulator.execNextEvent()
            message = model.outputFunction()
            for x in message:
                logger.info('message ' + str(idx) + ': value on port <' + \
                          x.port + '> = ' +  x.valueToJson)
        
        #
        message = [efscape.Content("stop_input_port", json.dumps({}))]
        model.externalTransition(timeMax, message)

    # 7. Wrap-up
    simulator.destroy()
    model.destroy()

    return 0


def run_input_test(simulator):
    """
    Tests injection of input events
    """
    model = simulator.getModel()
    if not model:
        logger.error("run_simulation: model not found!")
        return 1

    # run simulation
    t = 0
    idx = 0
    event_count = 0
    if simulator.start():
        print('simulator started!')
        message = model.outputFunction()
        print('message size = ' + str(len(message)))
        for x in message:
            print('message ' + str(idx) + ': value on port <' + \
                      x.port + '> = ' +  x.valueToJson)
        # test input
        content = efscape.Content("test_input_port", json.dumps({"houseA1": {"cooling_setpoint": 60}}))
        print(content)
        message.append(content)
        model.externalTransition(0., message)

        while not simulator.halt():
            t = simulator.nextEventTime()
            print('Next event time = ' + str(t))
            simulator.execNextEvent()
            message = model.outputFunction()
            for x in message:
                print('message ' + str(idx) + ': value on port <' + \
                          x.port + '> = ' +  x.valueToJson)
            event_count += 1
            if event_count > 2:
                break

    else:
        print('simulator failed to start')

    simulator.destroy()
    model.destroy()
    return 0


def run(communicator, filename, icegrid):
    """
    Run the efscape client

    Parameters
    ----------
    communicator Ice Communicator
    filename str
        json file name
    """
    status = 0

    # 4. attempt to access the efscape.ModelHome proxy
    modelHome = None
    if not icegrid:
        modelHome = efscape.ModelHomePrx.checkedCast(
            communicator.propertyToProxy('ModelHome.Proxy').ice_twoway().ice_secure(False))
    else:
        try:
            modelHome = efscape.ModelHomePrx.checkedCast(communicator.stringToProxy('modelhome'))
        except Ice.NotRegisteredException:
            query = IceGrid.QueryPrx.checkedCast(communicator.stringToProxy("EfscapeIceGrid/Query"))
            modelHome = efscape.ModelHomePrx.checkedCast(query.findObjectByType("::efscape::ModelHome"))

    if not modelHome:
        logger.error("invalid proxy")
        sys.exit(1)

    logger.info("ModelHome accessed successfully!")

   # 4a. If no parameter file has been specified, prompt the user to select
    #     a model and its corresponding default parameter file
    if not filename:
        status = view_available_models(modelHome)
        modelHome.shutdown()
        return status

    # 4b. Else, attempt to run the simulation with assuming argv[1] is either:
    #     1. name of a parameter file in JSON format, or
    #     2. name of a model
    model = None
    parmName = Path(filename)
    parameters = None
    if parmName.exists():
        logger.info('Running simulation using input from <' + str(parmName) + '>...')
        parmString = None
        with parmName.open() as f:
            parmString = f.read()
            parameters = json.loads(parmString)  # will need later

            # 5a. create model from parameter file
            model = modelHome.createFromParameters(parmString)

    else:
        logger.error("parameter file <" + str(parmName) + "> not found!")

    if not model:
        logger.error('Invalid mode proxy!')
        modelHome.shutdown()
        return status

    logger.info('model successfully created!')
    simulator = modelHome.createSim(model)

    if not simulator:
        logger.info('Invalid simulator proxy!')
        modelHome.shutdown()
        return status

    logger.info('simulator created!')

    # 6. run simulation
    timeMax = 100.  # default
    if "timeMax" in parameters:
        timeMax = parameters["timeMax"]

    status = run_simulation(simulator, timeMax)

    #status = run_input_test(simulator)

    logger.info("efscape.ModelHome client shutting down server ..")
    modelHome.shutdown()

    return status

def run_client(argv, filename):
    """ """

    config_filename = "config.client"
    icegrid = False
    if "ICEGRID" in os.environ:
        config_filename = "config.client2"
        icegrid = True

        click.echo("Using icegrid...")
    #
    # Ice.initialize returns an initialized Ice communicator,
    # the communicator is destroyed once it goes out of scope.
    #
    with Ice.initialize(argv,
            str(Path(os.environ['EFSCAPE_PATH']) / "src/server" / config_filename)) as communicator:

        #
        # The communicator initialization removes all Ice-related arguments from argv
        #
        # if len(sys.argv) > 2:
        #     print(sys.argv[0] + ": too many arguments")
        #     sys.exit(1)

        run(communicator, filename, icegrid)

@click.command()
@click.argument('filename', required=False)
def main(filename):
    """Console script for efscape.client."""
    click.echo("Running " "efscape.client.main...")

    argv = [sys.argv[0]]  # ignoring Ice-related arguments for now
    run_client(argv, filename)
