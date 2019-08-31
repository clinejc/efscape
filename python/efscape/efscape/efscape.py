# -*- coding: utf-8 -*-

"""Main module."""
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

class ModelHomeI(efscape.ModelHome):
    models = {}  # collection of models

    def addModel(name, model):
        models[name] = model

    """Implementation of efscape.ModelHome interface"""
    def create(self, name, current=None):
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
