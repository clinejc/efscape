//
//
var Ice = require('zeroc-icejs').Ice;
var efscape = require("./lib/efscape").efscape;

var communicator;

var time = {
    current: 0.,
    max: 0.
};

var model = {
    parmName: "",
    proxy: null,
    name: ""
};

var modelName;		// class name of model
    

var modelHomePrx;		// ModelHome proxy

var simulatorPrx;		// Simulator proxy

var messageBuffer;		// current message buffer

//
// Asynchronous loop, each call to the given function returns a
// promise that when fulfilled runs the next iteration.
//    
function loop(fn, repetitions)
{
    var i = 0;
    var next = function()
    {
	if (i++ < repetitions)
	{
	    return fn.call().then(next);
	}
    };
    return next();
}


//
function run(fn, timeMax)
{
    var next = function()
    {
	if (time.current <= timeMax)
	{
	    return fn.call().then(next);
	}
    }
    return next();
}


function timeNextEvent(timeEnd)
{
    return simulatorPrx.timeNextEvent().then(
	function(t, asyncResult)
	{
	    return (t < timeEnd);
	}
    );
}
    

//
//
function getModelInfo(model)
{
    return model.getName().then(
	function(name)
	{
	    console.log("Model");
	    console.log("\tname=<" + name + ">");
	    return model.ice_ids().then(
		function(ids)
		{
		    return ids;
		}
	    );
	},
	function()
	{
	    console.log("model.getName() failed!");
	}
    ).then(
	function(ids)
	{
	    var idsToString = "";
	    for (var i = 0; i < ids.length; i++) {
		idsToString += "," + ids[i];
	    }
	    console.log("\tids=["
			+ idsToString.slice(1,idsToString.length)
			+ "]");
	}
    ).exception(
	function(ex)
	{
	    console.log("getModelInfo(): caught <"
			+ ex.message + ">");
	}
    );
}
    
//
// Processes message content
//
function getMessageContent(model)
{
    return model.outputFunction().then(
	function(message)
	{
	    console.log("Invoking outputFunction");
	    return message;
	}
    ).then(
	function(message)
	{
	    if (message == undefined) {
		throw new Error("output message undefined");
	    }
	    messageBuffer = message;
	    return message.length;
	}
    ).then(
	function(size)
	{
	    console.log("message content:");
	    for (var i = 0; i < size; i++) {
		console.log("\tmessage[" + i + "] on port <"
			    + messageBuffer[i].port
			    + ">="
			    + messageBuffer[i].valueToJson);
		var result = JSON.parse(messageBuffer[i].valueToJson);
		
		// extract data from messages
		if (result != undefined) {
		    if (result.clock != undefined) {
			if (result.clock.time_max != undefined) {
			    time.max = result.clock.time_max;
			    console.log("time max = " + time.max);
			}
		    }
		}
	    }
	}
    ).exception(
	function(ex)
	{
	    console.log("Encountered exception <"
			+ ex.message + ">"
			+ " invoking Model.outputFunction()");
	}
    );
}
    
//
// print process.argv
//
process.argv.forEach(function (val, index, array) {
    console.log(index + ': ' + val);
    if (index == 2) {
	model.parmName = val;
    }
    else if (index == 3) {
	model.name = model.parmName;
	model.parmName = val;
    }
});

if (!model.parmName) {
    console.log("usage: node Client.js [modelName] <paramFileName>");
    process.exit(1);
}

Ice.Promise.try(
    function()
    {
	communicator = Ice.initialize();
	var proxy = communicator.stringToProxy("ModelHome:default -p 10000").ice_twoway().ice_secure(false);

	if (!communicator) {
	    throw new Error("Invalid proxy");
	}
	console.log("Successfully connected to proxy <ModelHome>");

	var fs = require("fs");
	var contents;
	if (fs.existsSync(model.parmName)) {
	    contents = fs.readFileSync(model.parmName).toString();
	    console.log(contents);
	}
	else {
	    throw new Error("Input parameter file does not exist");
	}

	return efscape.ModelHomePrx.checkedCast(proxy).then(
	    function(modelHome)
	    {
		// Promise #1: access ModelHome, create the model
		console.log("Successfully cast proxy as <ModelHome>");
		modelHomePrx = modelHome;
		    
		if (model.name) {
		    console.log("Attempting to retrieve model <"
				+ model.name + ">");
		    return modelHome.createWithConfig(model.name,contents).then(
			function(modelPrx)
			{
			    getModelInfo(modelPrx);
			    return modelPrx;
			}
		    );
		}
		else {
		    console.log("Attempting to retrieve model from parameter file <"
				+ model.parmName + ">");
		    return modelHome.createFromXML(contents).then(
			function(modelPrx)
			{
			    getModelInfo(modelPrx);
			    return modelPrx;
			}
		    );
		}
	    },
	    function()
	    {
		throw new Error("Promise #1: ModelHome <create model> callback failed");
	    }
	).then(
	    // Promise #2: create a simulator for the model
	    function(modelPrx)
	    {
		model.proxy = modelPrx; // proxy should be resolved at this point
		
		console.log("Next: create simulator");
		simulator = modelHomePrx.createSim(modelPrx);

		if (!simulator) {
		    throw new Error("Invalid simulator proxy");
		}

		return simulator;
	    },
	    function()
	    {
		throw new("Promise #2: Simulator.createSim(...) callback failed");
	    }
	).then(
	    // Promise #3: start simulator 
	    function(simulator)
	    {
		simulatorPrx = simulator; // proxy should be resolved at this point
		return simulator.start().then(
		    function(started)
		    {
			if (!started) {
			    throw new Error("Unable to start simulator");
			}
			getMessageContent(model.proxy);
		    }
		);
	    },
	    function()
	    {
		throw new Error("Promise #3: failed callback for starting the simulator");
	    }
	).then(
	    function()
	    {
		return run(
		    function()
		    {
			return simulatorPrx.nextEventTime().then(
			    function(t, asyncResult)
			    {
				time.current = t;
				console.log("time = " + time.current);
				return simulatorPrx.execNextEvent();
			    }
			).then(
			    function()
			    {
				return getMessageContent(model.proxy);
			    }
			);
		    },
		    time.max
		);
	    }
	).exception(
	    function(ex)
	    {
		console.log("Encountered exception <" + ex.message + ">");
	    }
	).finally(
	    function()
	    {
		console.log("Done");
	    }
	);
    }
).finally(
    function()
    {
        if(communicator)
        {
	    return communicator.destroy();
        }
    }
).then(
    function()
    {
        process.exit(0);
    },
    function(ex)
    {
        console.log(ex.toString());
        process.exit(1);
    });
