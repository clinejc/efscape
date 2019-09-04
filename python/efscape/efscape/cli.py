# -*- coding: utf-8 -*-

"""Console script for efscape."""
import sys
import click
import logging
import click_log
from efscape.server import ModelHomeI, run_server, test_devs, createGPT

# configure loggings
logger = logging.getLogger()  #__name__)
click_log.basic_config(logger)
logger.setLevel(logging.INFO)

@click.command()
def main(args=sys.argv):
    """Console script for efscape."""
    click.echo("Running " "efscape.cli.main...")

    #test_devs()

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
    run_server(args)
    
    return 0


if __name__ == "__main__":
    sys.exit(main())  # pragma: no cover
