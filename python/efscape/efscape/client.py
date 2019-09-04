import sys
import os
import click
import logging
import click_log
import Ice
from pathlib import Path
import json

# configure loggings
logger = logging.getLogger()  #__name__)
click_log.basic_config(logger)
logger.setLevel(logging.INFO)

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
                
            parmString = modelHome.getModelInfo(modelName);
            parameters = json.loads(parmString)
            print(parameters)

            # if the parameter 'modelName' exists, attempt to write
            # the default parameters to file [modelName].json
            if 'modelName' in parameters:
                parmName = parameters['modelName']

                parmFile = open(parmName + '.json', 'w')
                parmFile.write(parmString)
                parmFile.close()
            
            break

    print('done')
    
    return 0

def run_simulation(simulator, time_max=100.):
    """
    Run the efscape simulation

    Parameters
    ----------
    simulator efscape.SimulatorPrx
        simulator proxy
    time_max double
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
            if t > time_max:
                break
            logger.info('Next event time = ' + str(t))
            simulator.execNextEvent()
            message = model.outputFunction()
            for x in message:
                logger.info('message ' + str(idx) + ': value on port <' + \
                          x.port + '> = ' +  x.valueToJson)

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


def run(communicator, name, time_max):
    """
    Run the efscape client

    Parameters
    ----------
    communicator Ice Communicator
    name str
        model name or json file name
    time_max float
        maximum simulation time
    """
    status = 0

    # 4. attempt to access the efscape.ModelHome proxy
    modelHome = efscape.ModelHomePrx.checkedCast(
        communicator.propertyToProxy('ModelHome.Proxy').ice_twoway().ice_secure(False))
    if not modelHome:
        logger.error("invalid proxy")
        sys.exit(1)

    logger.info("ModelHome accessed successfully!")

   # 4a. If no parameter file has been specified, prompt the user to select
    #     a model and its corresponding default parameter file
    if not name:
        status = view_available_models(modelHome)
        sys.exit(status)

    # 4b. Else, attempt to run the simulation with assuming argv[1] is either:
    #     1. name of a parameter file in JSON format, or
    #     2. name of a model
    model = None
    parmName = Path(name)
    if parmName.exists():
        logger.info('Running simulation using input from <' + str(parmName) + '>...')
        parmString = None
        with parmName.open() as f:
            parmString = f.read()

            # 5a. create model from parameter file
            model = modelHome.createFromParameters(parmString)

    else:   # 5.b create model from model name
        modelName = name
        model = modelHome.create(modelName)

    if not model:
        print('Invalid mode proxy!')
        sys.exit(1)

    print('model successfully created!')
    simulator = modelHome.createSim(model)

    if not simulator:
        print('Invalid simulator proxy!')
        sys.exit(1)

    print('simulator created!')

    # 6. run simulation
    if not time_max:
        status = run_simulation(simulator)
    else:
        status = run_simulation(simulator, time_max)

    #status = run_input_test(simulator)

    return status

def run_client(argv, name, time_max):
    """ """
    #
    # Ice.initialize returns an initialized Ice communicator,
    # the communicator is destroyed once it goes out of scope.
    #
    with Ice.initialize(argv,
            str(Path(os.environ['EFSCAPE_PATH']) / "src/server/config.client")) as communicator:

        #
        # The communicator initialization removes all Ice-related arguments from argv
        #
        # if len(sys.argv) > 2:
        #     print(sys.argv[0] + ": too many arguments")
        #     sys.exit(1)

        run(communicator, name, time_max)

@click.command()
@click.argument('name', required=False)
@click.argument('time_max', required=False, type=float)
def main(name, time_max):
    """Console script for efscape.client."""
    click.echo("Running " "efscape.client.main...")

    argv = [sys.argv[0]]  # ignoring Ice-related arguments for now
    run_client(argv, name, time_max)
