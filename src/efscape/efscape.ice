// __COPYRIGHT_START__
// Package Name : efscape
// File Name : efscape.ice
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
//=============================================================================
//=============================================================================
//
#ifndef EFSCAPE_IDL
#define EFSCAPE_IDL

//#include <efscape/Data.ice>

/**
 * Defines the efscape interface, a interface for a DEVS-based simulation
 * services, for an Internet Connection Environment (ICE)-based framework.
 *
 * @author Jon C. Cline <clinej@stanfordalumni.org>
 * @version 0.0.11 created 06 Dec 2006, revised 02 Nov 2014
 *
 * ChangeLog:
 *   - 2014-11-02 Added method to ModelHome for creating JsonDataset objects
 *      - JsonDataset createJsonDataset(string, string)
 *   - 2014-10-28 Added Interface JsonDataset
 *   - 2014-09-24 Revised ModelHome interface
 *      - Model* createWithConfig(string name, string config)
 *   - 2014-02-06 removed use of Data module (deprecated)
 *   - 2010-06-26 Restored destroy methods.
 *   - 2007-12-01 Revised ModelHomeI interface
 *     - Added: Model createFromParameters(string parameters)
 *   - 2007-10-20 Removed destroy methods (using smart pointers)
 *   - 2007-10-18 Changed objects to pointers
 *    - Content Object to Object*
 *    - Clock to Clock*
 *   - 2007-08-30 revised Simulator interface
 *   - 2007-01-01 moved data interfaces to Data.ice
 *   - 2006-12-23 interface 'Devs' renamed 'Model'
 *   - 2006-12-21 added DataFrame and related interfaces
 *   - 2006-12-06 created efscape.ice
 */
module efscape {

  //-------------------------------------------------------------------------
  // module DEVS_Component
  //-------------------------------------------------------------------------
  /**
   * Base exception class
   */
  exception SimException {
    string what;
  };


  /**
   * struct PortDescription: description of a port interface
   */
  struct PortDescription {
    string port;
    string typeID;
  };

  // sequence of port descriptions
  sequence<PortDescription> PortDescriptions;

  /**
   * class Content
   */
  class Content {
    string port;
    Object* value;
  };

  /**
   * Message: a sequence of Content objects containing port/value info
   */
  sequence<Content> Message;

  /**
   * interface Entity -- base class for simulation objects
   **/
  interface Entity {
    ["cpp:const"]
    idempotent string getName();
  };

  /**
   * interface JsonDataset -- a simple interface for accessing JSON datasets.
   * The components of a JSON dataset are:
   *  - schema
   *  - data
   */
  interface JsonDataset {
    string getSchema();
    string getData();
    void setData(string data, string schema);
  };

  /**
   * interface Clock -- simulation clock
   **/
  interface Clock extends Entity {

    /**
     * Returns time units of the simulation.
     *
     * @return time units
     **/
    ["cpp:const"]
    idempotent string timeUnits();

    /**
     * Returns the minimum time step of the simulation.
     *
     * @return minimum time step
     **/
    ["cpp:const"]
    idempotent double timeDelta();

    /**
     * Returns the end time of the simulation.
     *
     * @return end time
     **/
    ["cpp:const"]
    idempotent double timeMax();

    /**
     * Returns the current time.
     *
     * @return current time
     **/
    ["cpp:const"]
    idempotent double timeCurrent();

  };

  /**
   * interface Model -- basic DEVS interface
   *
   * See Zeigler et al (1999) -- interface DEVS_Component::Basic
   */
  interface Model extends Entity {

    // interfaces from DEVS_Component::Basic
    bool initialize();
    double timeAdvance();
    bool internalTransition();
    bool externalTransition(double elapsedTime, Message msg);
    bool confluentTransition(Message msg);
    Message outputFunction();

    // accessor/mutator methods
    ["cpp:const"] idempotent string getType();
    ["cpp:const"] idempotent string getBaseType();
    void setName(string name);
    ["cpp:const"] idempotent PortDescriptions getInPorts();
    ["cpp:const"] idempotent PortDescriptions getOutPorts();

    ["cpp:type:wstring"] string saveXML();

    void destroy();
  };

  /**
   * interface Simulator -- basic DEVS simulator interface
   *
   * See Zeigler et al 1999 (interface Simulator::toCoordinator)
   *     bool start()
   *     double set_Global_and_SendOutput(in double tN);
   *     bool applyDeltaFunc(in message msg);
   */
  interface Simulator {
    /**
     * Invokes its components initialize method
     **/
    bool start();

    /**
     * Returns time of next event.
     *
     * Equivalent to: 
     *   - double tN_yet (Zeigler et al 1999)
     *   - double timeNext() (adevs 1.3.x/Simulator)
     *
     * @return time of next event
     **/
    double nextEventTime();

    /**
     * Executes the next event.
     **/
    void execNextEvent();

    /**
     * Compute the output of the imminent models
     **/
    void computeNextOutput();

    /**
     * Apply the specified inputs at the specified time and compute the
     * next model state.
     *
     * @param elapsedTime elapsed time
     * @param msg specified inputs
     **/
    void computeNextState(double elapsedTime, Message msg);

    // additional methods (see adevs C++ simulator library)
    bool halt();

    void destroy();
  };

  sequence<string> ModelList;

  /**
   * interface ModelHome -- a model factory that creates a Model object given
   *  - 1) model name,
   *  - 2) XML configuration string, or
   *  - 3) model name and configuration string
   */
  interface ModelHome {
    Model* create(string name);
    Model* createFromXML(["cpp:type:wstring"] string configuration);
    Model* createWithConfig(string name,
			    ["cpp:type:wstring"] string configuration);

    ModelList getModelList();

    Simulator* createSim(Model* rootModel);
    JsonDataset* createJsonDataset(string data, string schema);
  };

};
    
#endif
