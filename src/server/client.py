import sys
import os
import Ice
from pathlib import Path

efscape_slice_dir = Path(os.environ['EFSCAPE_PATH']) / 'src/slice'
Ice.loadSlice('-I' + str(efscape_slice_dir) + ' --all ' + str(efscape_slice_dir / 'efscape/ModelHome.ice'))

import efscape

def run(communicator):
    """ """

    modelHome = efscape.ModelHomePrx.checkedCast(
        communicator.propertyToProxy('ModelHome.Proxy').ice_twoway().ice_secure(False))
    if not modelHome:
        print("invalid proxy")
        sys.exit(1)

    print("ModelHome accessed successfully!")

    if len(sys.argv) == 1:
        modelList = modelHome.getModelList()

        idx = 0
        for x in modelList:
            print(str(idx) + ': ' + x)
            idx = idx + 1

        while True:
            result = input('\nEnter the number of the model (0 to exit)==> ')
            if int(result) > 0 and int(result) < len(modelList):
                print('selected ' + modelList[int(result)])
            elif int(result) == 0:
                break

        print('done')
        sys.exit(0)
            
    print('Running simulation...')
    print('argc=' + str(len(sys.argv)))
    
#
# Ice.initialize returns an initialized Ice communicator,
# the communicator is destroyed once it goes out of scope.
#
with Ice.initialize(sys.argv, "config.client") as communicator:

    #
    # The communicator initialization removes all Ice-related arguments from argv
    #
    if len(sys.argv) > 2:
        print(sys.argv[0] + ": too many arguments")
        sys.exit(1)

    run(communicator)
