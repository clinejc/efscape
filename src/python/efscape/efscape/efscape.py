# -*- coding: utf-8 -*-

"""Main module."""


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

    ports = {}  # model ports

    def __init__(self, wrapped_model=None, modelInfo={}):
        super().__init__()
        self.wrapped_model = wrapped_model  # set the wrappped model
        self.name = "model"
        if wrapped_model is not None:
            self.name = wrapped_model.__class__.__name__

        self.modelInfo = modelInfo  # set model metadata

        # get information about model ports
        self.ports = {}
        if "ports" in self.modelInfo:
            # Add 2-way mapping of external ports to internal ports
            for key, value in self.modelInfo["ports"].items():
                self.ports[key] = value
                self.ports[value] = key

        #
        # get information about model output generator from model metadata
        # * The output producer should be the name of an atomic model
        # * The output producer is also tagged as the input consumer (for now)
        self.output_producer = None
        self.input_consumer = None
        if "output_producer" in self.modelInfo:
            print(self.modelInfo["output_producer"])
            if (
                self.wrapped_model.__class__.__name__
                == self.modelInfo["output_producer"]
            ):
                self.output_producer = self.wrapped_model
                logger.info(
                    "Added root model<" + self.wrapped_model.__class__.__name__ + ">"
                )
            else:
                for submodel in self.wrapped_model:
                    if submodel.__class__.__name__ == self.modelInfo["output_producer"]:
                        self.output_producer = submodel
                        logger.info(
                            "Added submodel<" + submodel.__class__.__name__ + ">"
                        )
            if not isinstance(self.output_producer, devs.AtomicBase):
                self.output_producer = None
            else:
                self.input_consumer = self.output_producer

    # interfaces from Entity
    def getName(self, current=None):
        return self.name

    # interfaces from DEVS_Component::Basic
    def initialize(self, current=None):
        status = False
        if self.wrapped_model is not None:
            # initialize the model if operation is available
            initialize_op = getattr(self.wrapped_model, "initialize", None)
            if callable(initialize_op):
                initialize_op(self.wrapped_model.path.parent_op)
                status = True

            # add simulator for the wrapped model
            self.simulator = devs.Simulator(self.wrapped_model)

            status = True  # 2019-09-03 ignoring initialization failure for now

        return status

    def timeAdvance(self, current=None):
        if self.wrapped_model is None:
            return sys.float_info.max

        return self.simulator.next_event_time()

    def internalTransition(self, current=None):
        if self.wrapped_model is None:
            return False

        self.simulator.execute_next_event()

        return True

    def externalTransition(self, elapsedTime, msg, current=None):
        if self.wrapped_model is None:
            return False
        print("ModelI.externalTransition(...)...")
        print("\ttime=" + str(elapsedTime))
        print("\tmessage=" + str(msg))

        # convert input message to pydevs input format
        xb = []
        for content in msg:
            if content.port in self.ports:
                try:
                    xb.append(
                        (self.ports[content.port], json.loads(content.valueToJson))
                    )
                except ValueError as ex:
                    logger.error(str(ex))
                    logger.error(
                        "Decoding JSON has failed for <" + content.valueToJson + ">"
                    )
            else:
                logger.info("content port not found")
                logger.info(self.ports)
        # self.simulator.compute_next_state(xb, elapsedTime)
        logger.info(xb)
        if self.input_consumer is not None:
            self.input_consumer.delta_ext(elapsedTime, xb)

        return True

    def confluentTransition(self, msg, current=None):
        if self.wrapped_model is None:
            return False

        # process external events first
        self.externalTransition(self.timeAdvance(), msg, current)
        self.internalTransition(current)

        return True

    def outputFunction(self, current=None):
        logger.info("ModelI.outputFunction(): entering...")
        message = []
        if self.output_producer is None:
            logger.info("efscape.ModelI.outputFunction(): missing output producer")
            return message

        logger.info("ModelI.outputFunction(): potential output...")

        yb = self.output_producer.output_func()

        if yb is None:
            print("ModelI.outputFunction(): nothing to output")
            return

        print(yb)

        # convert
        message = []
        for port, value in yb:
            print(port)
            if port in self.ports:
                try:
                    message.append(efscape.Content(self.ports[port], json.dumps(value)))
                except TypeError as ex:
                    logger.error(str(ex))
                    logger.error("Unable to serialize message <" + str(value) + ">")

        return message

    # accessor/mutator methods
    def getType(self, current=None):
        if self.wrapped_model is None:
            return "None"

        return str(self.wrapped_model.__class__.__name__)

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
        
        # need to ensure the wrapped model is deleted now
        del self.wrapped_model


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
            modelInfo = self.models[name][1]
            modelI = ModelI(wrapped_model, modelInfo)
            if current is not None:
                model = efscape.ModelPrx.uncheckedCast(
                    current.adapter.addWithUUID(modelI)
                )

        return model

    def createFromXML(self, parameters, current=None):
        return None

    def createFromJSON(self, configuration, current=None):
        return None

    def createFromParameters(self, parametersString, current=None):
        logger.info("Entering efscape.ModelHomeI.createFromParameters(...)...")
        model = None
        parameters = json.loads(parametersString)
        if "modelName" in parameters:
            modelName = parameters["modelName"]
            model = self.create(modelName, current)
            if not model:
                logger.error("Unable to create model <"+ + modelName +">!")
        else:
            logger.info("efscape.Model: <modelName> not found!")

        return model

    def getModelList(self, current=None):
        return list(self.models.keys())

    def getModelInfo(self, name, current=None):
        if name not in self.models:
            return {}

        return json.dumps(self.models[name][1])

    def createSim(self, model, current=None):
        simulator = None

        simulatorI = SimulatorI(model)
        simulator = efscape.SimulatorPrx.uncheckedCast(
            current.adapter.addWithUUID(simulatorI)
        )

        return simulator

    def shutdown(self, current=None):
        if current is not None:
            logger.info("Shutting down efscape.ModelHome")
            current.adapter.getCommunicator().shutdown()
        else:
            logger.info("Not able to shutdown efscape.ModelHome")


# for testing
import collections
import random


class Source(devs.AtomicBase):
    arrival_port = 0

    def __init__(self, arrival_rate=1.0, **kwds):
        super().__init__(**kwds)
        self.logger = logging.getLogger("quickstart.Source")
        self.logger.info("Initialize source with arrival rate {}".format(arrival_rate))
        self.arrival_rate = arrival_rate
        self.inter_arrival_time = random.expovariate(self.arrival_rate)
        self.job_id = 0

    def ta(self):
        self.logger.debug(
            "Next arrival in {} time units".format(self.inter_arrival_time)
        )
        return self.inter_arrival_time

    def delta_int(self):
        self.job_id += 1
        self.inter_arrival_time = random.expovariate(self.arrival_rate)

    def output_func(self):
        self.logger.info("Generate job {}".format(self.job_id))
        return self.arrival_port, self.job_id


class Server(devs.AtomicBase):
    arrival_port = 0
    departure_port = 1

    def __init__(self, service_rate=1.0, **kwds):
        super().__init__(**kwds)
        self.logger = logging.getLogger("quickstart.Server")
        self.logger.info("Initialize server with service rate {}".format(service_rate))
        self.service_rate = service_rate
        self.remaining_service_time = devs.infinity
        self.queue = collections.deque()
        self.job_in_service = None

    def ta(self):
        if self.job_in_service is None:
            self.logger.debug("Server is idle")
            return devs.infinity

        return self.remaining_service_time

    def start_next_job(self):
        self.job_in_service = self.queue.popleft()
        self.remaining_service_time = random.expovariate(self.service_rate)
        self.logger.info(
            "Start processing job {} with service time {}".format(
                self.job_in_service, self.remaining_service_time
            )
        )

    def delta_int(self):
        # service finished
        self.logger.info("Finished processing job {}".format(self.job_in_service))
        if len(self.queue):
            # jobs waiting, start to process immediately
            self.start_next_job()
        else:
            # no more jobs, switch to idle
            self.logger.info("Queue empty, server turns idle")
            self.job_in_service = None

    def delta_ext(self, e, xb):
        if self.job_in_service is not None:
            self.remaining_service_time -= e

        # new job(s) arriving
        for port, job_id in xb:
            self.logger.info("New job {} arrives".format(job_id))
            self.queue.append(job_id)
            if self.job_in_service is None:
                # queue empty, start immediately
                self.start_next_job()
            else:
                # server busy
                self.logger.debug("Server busy, enqueueing job {}".format(job_id))

        self.logger.debug(
            "Remaining service time for job {}: {} time units".format(
                self.job_in_service, self.remaining_service_time
            )
        )

    def delta_conf(self, xb):
        # treat incoming jobs first
        self.delta_ext(self.ta(), xb)
        self.delta_int()

    def output_func(self):
        # service finished
        return self.departure_port, self.job_in_service


class Observer(devs.AtomicBase):
    arrival_port = 0
    departure_port = 1
    output_port = 2
    test_input_port = 3

    def __init__(self, time=0.0, **kwds):
        super().__init__(**kwds)
        self.logger = logging.getLogger("quickstart.Observer")
        self.logger.info("Initialize observer at time {}".format(time))
        self.time = time
        self.arrivals = list()
        self.departures = list()

        # 03 Sep 2019: add output
        self.is_active = False
        self.output = None

    def delta_ext(self, e, xb):
        self.time += e
        for port, job_id in xb:
            if port == self.arrival_port:
                self.logger.info("Job {} arrives at time {}".format(job_id, self.time))
                self.arrivals.append(self.time)
                self.output = {"job_id": job_id, "action": "arrival", "time": self.time}
            elif port == self.departure_port:
                self.logger.info("Job {} departs at time {}".format(job_id, self.time))
                self.departures.append(self.time)
                self.output = {
                    "job_id": job_id,
                    "action": "departure",
                    "time": self.time,
                }
            elif port == self.test_input_port:
                self.logger.info("test input: {}".format(job_id))

    def delta_int(self):
        self.output = None

    def ta(self):
        if self.output is not None:
            return 0.0

        return devs.infinity

    def output_func(self):
        # output ready
        yb = []
        if self.output is not None:
            yb.append((self.output_port, self.output))
            print("yb=" + str(yb))
            return yb


def createGPT():
    source = Source(1.0)
    server = Server(1.0)
    observer = Observer()
    digraph = devs.Digraph()
    digraph.add(source)
    digraph.add(server)
    digraph.add(observer)
    digraph.couple(source, source.arrival_port, server, server.arrival_port)
    digraph.couple(source, source.arrival_port, observer, observer.arrival_port)
    digraph.couple(server, server.departure_port, observer, observer.departure_port)

    return digraph


def test_devs():
    """
    Test the GPT model
    """
    model = createGPT()

    simulator = devs.Simulator(model)
    simulator.execute_until(5.0)


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

        modelHome = ModelHomeI()
        adapter = communicator.createObjectAdapter("ModelHome")
        adapter.add(modelHome, Ice.stringToIdentity("ModelHome"))
        adapter.activate()
        communicator.waitForShutdown()

def run_server_with_icegrid(args):
    #
    # Ice.initialize returns an initialized Ice communicator,
    # the communicator is destroyed once it goes out of scope.
    #
    with Ice.initialize(args) as communicator:

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


        properties = communicator.getProperties()
        adapter = communicator.createObjectAdapter("ModelHome")
        id = Ice.stringToIdentity(properties.getProperty("Identity"))
        adapter.add(ModelHomeI(), id)
        adapter.activate()
        communicator.waitForShutdown()
