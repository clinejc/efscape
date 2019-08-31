# -*- coding: utf-8 -*-

"""Console script for efscape."""
import sys
import click
from efscape.server import ModelHomeI, ModelI


@click.command()
def main(args=None):
    """Console script for efscape."""
    click.echo("Replace this message by putting your code into "
               "efscape.cli.main")
    click.echo("See click documentation at http://click.pocoo.org/")

    ModelHomeI.addModel("test", ModelI)

    modelHome = ModelHomeI()

    model = modelHome.create("test")

    if model is None:
        print("model <test> not found")
    else:
        print("model <test> found")
        print("model name = " + model.getName())

    return 0


if __name__ == "__main__":
    sys.exit(main())  # pragma: no cover
