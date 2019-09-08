# -*- coding: utf-8 -*-

"""Console script for efscape."""
import sys
import os
import click
import logging
import click_log
from pathlib import Path
from efscape.efscape import (
    ModelHomeI,
    run_server,
    run_server_with_icegrid,
    test_devs,
    createGPT,
)

# configure loggings
logger = logging.getLogger()  # __name__)
click_log.basic_config(logger)
logger.setLevel(logging.INFO)


@click.command()
@click.option('--Ice.Config', 'ice_config')
def main(ice_config):
    """Console script for efscape."""
    click.echo("Running " "efscape.cli.main...")
    if ice_config:
        click.echo("ice_config=<" + ice_config + ">")

    # test_devs()

    # load the class DEVS GPT model for testing
    modelInfo = {
        "modelName": "GPT",
        "description": "Classic DEVS GPT model",
        "ports": {"output_port": 2, "test_input_port": 3},
        "output_producer": "Observer",
    }
    ModelHomeI.addModel("GPT", createGPT, modelInfo)

    # Need to change to the server direction to run server
    efscape_server_dir = Path(os.environ["EFSCAPE_PATH"]) / "src" / "server"
    os.chdir(str(efscape_server_dir))

    run_server_with_icegrid(sys.argv)

    return 0


if __name__ == "__main__":
    sys.exit(main())  # pragma: no cover
