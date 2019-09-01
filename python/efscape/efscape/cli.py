# -*- coding: utf-8 -*-

"""Console script for efscape."""
import sys
import click
import logging
import click_log
from efscape.server import ModelHomeI, ModelI, run_server ## createContent

# configure loggings
logger = logging.getLogger()  #__name__)
click_log.basic_config(logger)
logger.setLevel(logging.INFO)

@click.command()
def main(args=sys.argv):
    """Console script for efscape."""
    click.echo("Running " "efscape.cli.main...")

    ModelHomeI.addModel("test", ModelI)
    run_server(args)

    # modelHome = ModelHomeI()

    # model = modelHome.create("test")

    # if model is None:
    #     print("model <test> not found")
    # else:
    #     print("model <test> found")
    #     print("model name = " + model.getName())
    #     print("model class = " + model.getType())
    #     model.addPort("injection", 0)
    #     model.addPort("step", 1)
    #     model.addPort("output", 2)

    #     message = []
    #     message.append(createContent("injection", {"houseA1": {"cooling_setpoint": 80}}))
    #     message.append(createContent("step", {}))
    #     model.externalTransition(0., message)

    #     print("getting output...")
    #     message_out = model.outputFunction()
    #     for content in message_out:
    #         print("port=" + content.port)
    #         print("valueToJson=" + content.valueToJson)

    #     simulator = modelHome.createSimulator(model)
    
    return 0


if __name__ == "__main__":
    sys.exit(main())  # pragma: no cover
