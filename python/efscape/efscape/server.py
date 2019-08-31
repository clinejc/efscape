# -*- coding: utf-8 -*-

"""Efscape Ice Server module."""
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

class ModelI(efscape.Model):
    """Implementation of efscape.Model"""

    # interfaces from Entity
    def getName(self, current=None):
        return "testmodel"

    # interfaces from DEVS_Component::Basic
    def initialize(self, current=None):
        return True

    def timeAdvance(self, current=None):
        return 0.

    def internalTransition(self, current=None):
        return True

    def externalTransition(self, elapsedTime, msg, current=None):
        return True

    def confluentTransition(self, msg, current=None):
        return True

    def outputFunction(self, current=None):
        return []

    # accessor/mutator methods
    def getType(self, current=None):
        return "dummy"

    def setName(self, name, current=None):
        pass

    def saveJSON(self, current=None):
        return {}

    def destroy(self, current=None):
        pass

class ModelHomeI(efscape.ModelHome):
    models = {}  # collection of models

    def addModel(name, model):
        ModelHomeI.models[name] = model

    """Implementation of efscape.ModelHome interface"""
    def create(self, name, current=None):
        self.model = None

        if name in self.models:
            return self.models[name]()

        return None

    def createFromXML(self, parameters, current=None):
        return None

    def createFromJSON(self, configuration, current=None):
        return None

    def createFromParameters(self, parameters, current=None):
        return None
        
    def getModelList(self, current=None):
        return []
    
    def getModelInfo(self, name, current= None):
        return {}

    def createSimulator(self, model, current=None):
        return None
