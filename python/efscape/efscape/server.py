# -*- coding: utf-8 -*-

"""Efscape Ice Server module."""
import signal
import sys
import os
import Ice
from pathlib import Path
import json
import devs
import logging

# configure logging
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)

# 1. set the path of the 'efscape' slice defitions
efscape_slice_dir = Path(os.environ["EFSCAPE_PATH"]) / "src/slice"

# 2. generate the python stubs for 'efscape'
Ice.loadSlice(
    "-I"
    + str(efscape_slice_dir)
    + " --all "
    + str(efscape_slice_dir / "efscape/ModelHome.ice")
)

# 3. import efscape python API
import efscape

def createContent(port, value):
    """
    Creates an efscape.Content object

    Parameters
    ----------
    port: str
       port name
    value: object
       value assigned to this port

    Returns
    -------
    efscape.Content object
    """
    return efscape.Content(port, json.dumps(value))

class ModelI(efscape.Model):
    """
    Implements interface Model -- basic DEVS interface
    """

    name = 'model'  # model name
    wrapped_model = None  # wrapped model
    ports = {}  # model ports

    def __init__(self, wrapped_model = None):
        super().__init__()
        self.wrapped_model = wrapped_model
        if wrapped_model is not None:
            self.name = wrapped_model.__class__
    
    def addPort(self, port_name, port_index):
        self.ports[port_name] = port_index
        self.ports[port_index] = port_name

    # interfaces from Entity
    def getName(self, current=None):
        return self.name

    # interfaces from DEVS_Component::Basic
    def initialize(self, current=None):
        status = False
        if self.wrapped_model is not None:
            initialize_op = getattr(self.wrapped_model, "initialize", None)
            if callable(initialize_op):
                initialize_op(self.wrapped_model.path.parent_op)
                status = True
        
        return status

    def timeAdvance(self, current=None):
        if self.wrapped_model is None:
            return sys.float_info.max

        return self.wrapped_model.ta()

    def internalTransition(self, current=None):
        if self.wrapped_model is None:
            return False
        
        self.wrapped_model.delta_int()

        return True

    def externalTransition(self, elapsedTime, msg, current=None):
        if self.wrapped_model is None:
            return False

        # convert input message to pydevs input format
        xb = []
        for content in msg:
            if content.port in self.ports:
                xb.append((self.ports[content.port],json.loads(content.valueToJson)))
        self.wrapped_model.delta_ext(elapsedTime, xb)
        
        return True

    def confluentTransition(self, msg, current=None):
        if self.wrapped_model is None:
            return False
        
        self.externalTransition(self.timeAdvance(), msg, current)
        self.internalTransition(current)

        return True

    def outputFunction(self, current=None):
        message = []
        if self.wrapped_model is None:
            return message
        
        yb = self.wrapped_model.output_func()

        # convert
        message = []
        for port, value in yb:
            print(port)
            if port in self.ports:
                message.append(efscape.Content(self.ports[port], json.dumps(value)))

        return message

    # accessor/mutator methods
    def getType(self, current=None):
        if self.wrapped_model is  None:
            return 'None'

        return str(self.wrapped_model.__class__)

    def setName(self, name, current=None):
        self.name = name

    def saveJSON(self, current=None):
        return {}

    def destroy(self, current=None):
        """
        Destroys the model
        """
        if current is not None:
            current.adapter.remove(current.id)


class SimulatorI(efscape.Simulator):
    """
    interface Simulator -- basic DEVS simulator interface
    
     See Zeigler et al 1999 (interface Simulator::toCoordinator)
         bool start()
         double set_Global_and_SendOutput(in double tN);
         bool applyDeltaFunc(in message msg);
    """
    def __init__(self, model=None):
        super().__init__()
        self.model = model

    def start(self, current=None):
        """
        Invokes its components initialize method

        Returns
        -------
        bool
            Whether initialization was successful
        """
        if self.model is None:
            return False
        
        return self.model.initialize()

    def nextEventTime(self, current=None):
        """
        Returns time of next event.
     
        Equivalent to:
         - double tN_yet (Zeigler et al 1999)
         - double timeNext() (adevs 1.3.x/Simulator)
     
        Returns
        -------
        double
            time of next event
        """
        if self.model is None:
            return sys.float_info.max
        
        return self.model.timeAdvance()

    def execNextEvent(self, current=None):
        """
        Executes the next event
        """
        if self.model is None:
            return
        
        self.model.internalTransition()

    def computeNextOutput(self, current=None):
        """
        Compute the output of the imminent models
        """
        pass

    def computeNextState(self, elapsedTime, msg):
        """
        Apply the specified inputs at the specified time and compute the
        next model state.

        Parameters
        ----------
        elapsedTime double
            elapsed time
        msg efscape.Message
            specified inputs (list of port/valueToJson pairs)
        """
        if self.model is None:
            return
        
        return
    
    def halt(self, current=None):
        if self.model is None:
            return True

        return self.model.timeAdvance() == sys.float_info.max

    def getModel(self, current=None):
        return self.model

    def destroy(self, current=None):
        """
        Destroys the simulator
        """
        if current is not None:
            current.adapter.remove(current.id)


class ModelHomeI(efscape.ModelHome):
    """
    Implements interface ModelHome -- a model factory that creates a Model
    object given:

    1. model name,
    2. XML configuration embedded in a string
    3. JSON configuration embedded in a string
    4. Parameters in JSON format embedded in a string
    """

    models = {}  # collection of models

    @classmethod
    def addModel(cls, name, model, info={}):
        """
        Adds a model to ModelHome

        Parameters
        ----------
        name : str
            identifier for model
        model : function
            constructor
        info : dict
            model metadata
        """
        cls.models[name] = (model, info)

    def create(self, name, current=None):
        model = None

        if name in self.models:
            wrapped_model = self.models[name][0]()
            modelI = ModelI(wrapped_model)
            model = efscape.ModelPrx.uncheckedCast(current.adapter.addWithUUID(modelI))

        return model

    def createFromXML(self, parameters, current=None):
        return None

    def createFromJSON(self, configuration, current=None):
        return None

    def createFromParameters(self, parameters, current=None):
        return None

    def getModelList(self, current=None):
        return list(self.models.keys())

    def getModelInfo(self, name, current=None):
        if name not in self.models:
            return {}
        return json.dumps(self.models[name][1])

    def createSim(self, model, current=None):
        simulator = None

        simulatorI = SimulatorI(model)
        simulator = efscape.SimulatorPrx.uncheckedCast(current.adapter.addWithUUID(simulatorI))
 
        return simulator


# for testing
import collections
import random

class Source(devs.AtomicBase):
    arrival_port = 0

    def __init__(self, arrival_rate=1.0, **kwds):
        super().__init__(**kwds)
        self.logger = logging.getLogger('quickstart.Source')
        self.logger.info('Initialize source with arrival rate {}'.format(arrival_rate))
        self.arrival_rate = arrival_rate
        self.inter_arrival_time = random.expovariate(self.arrival_rate)
        self.job_id = 0

    def ta(self):
        self.logger.debug('Next arrival in {} time units'.format(self.inter_arrival_time))
        return self.inter_arrival_time

    def delta_int(self):
        self.job_id += 1
        self.inter_arrival_time = random.expovariate(self.arrival_rate)

    def output_func(self):
        self.logger.info('Generate job {}'.format(self.job_id))
        return self.arrival_port, self.job_id


def run_server(args):
    #
    # Ice.initialize returns an initialized Ice communicator,
    # the communicator is destroyed once it goes out of scope.
    #
    with Ice.initialize(args, "config.server") as communicator:

        #
        # Install a signal handler to shutdown the communicator on Ctrl-C
        #
        signal.signal(signal.SIGINT, lambda signum, frame: communicator.shutdown())

        if len(args) > 1:
            print(args[0] + ": too many arguments")
            sys.exit(1)

        ModelHomeI.addModel("Source", Source, {"description": "Class DEVS model"})
        modelHome = ModelHomeI()

        adapter = communicator.createObjectAdapter("ModelHome")
        adapter.add(modelHome, Ice.stringToIdentity("ModelHome"))
        adapter.activate()
        communicator.waitForShutdown()