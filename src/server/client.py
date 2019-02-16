import sys
import os
import Ice
from pathlib import Path
import json

# 1. set the path of the 'efscape' slice defitions
efscape_slice_dir = Path(os.environ['EFSCAPE_PATH']) / 'src/slice'

# 2. generate the python stubs for 'efscape'
Ice.loadSlice('-I' + str(efscape_slice_dir) + ' --all ' + str(efscape_slice_dir / 'efscape/ModelHome.ice'))

# 3. import efscape python API
import efscape

def run(communicator):
    """ """

    # 4. attempt to access the efscape.ModelHome proxy
    modelHome = efscape.ModelHomePrx.checkedCast(
        communicator.propertyToProxy('ModelHome.Proxy').ice_twoway().ice_secure(False))
    if not modelHome:
        print("invalid proxy")
        sys.exit(1)

    print("ModelHome accessed successfully!")

    # 4a. If no parameter file has been specified, prompt the user to select
    #     a model and its corresponding default parameter file
    if len(sys.argv) == 1:
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
        sys.exit(0)

    # 4b. Else, attempt to run the simulation with the parameter file specified
    parmName = sys.argv[1]
    print('Running simulation using input from <' + parmName + '>...')

    f = open(parmName, 'r')
    parmString = f.read()

    # 5. create model from parameter file
    model = modelHome.createFromParameters(parmString)

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
    t = 0
    idx = 0
    if simulator.start():
        print('simulator started!')
        message = model.outputFunction()
        print('message size = ' + str(len(message)))
        for x in message:
            print('message ' + str(idx) + ': value on port <' + \
                      x.port + '> = ' +  x.valueToJson)

        while not simulator.halt():
            t = simulator.nextEventTime()
            print('Next event time = ' + str(t))
            simulator.execNextEvent()
            message = model.outputFunction()
            for x in message:
                print('message ' + str(idx) + ': value on port <' + \
                          x.port + '> = ' +  x.valueToJson)            

    # 7. Wrap-up
    simulator.destroy()
    model.destroy()
    
#
# Ice.initialize returns an initialized Ice communicator,
# the communicator is destroyed once it goes out of scope.
#
with Ice.initialize(sys.argv,
        str(Path(os.environ['EFSCAPE_PATH']) / "src/server/config.client")) as communicator:

    #
    # The communicator initialization removes all Ice-related arguments from argv
    #
    if len(sys.argv) > 2:
        print(sys.argv[0] + ": too many arguments")
        sys.exit(1)

    run(communicator)
