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

/**
 * Defines the efscape interface, a interface for a DEVS-based simulation
 * services, for an Internet Connection Environment (ICE)-based framework.
 *
 * @author Jon C. Cline <clinej@stanfordalumni.org>
 * @version 0.2.0 created 06 Dec 2006, revised 20 Aug 2017
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
   * class Content
   */
  struct /*class*/ Content {
    string port;
    string valueToJson;
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
    idempotent void setName(string name);
    idempotent string saveJSON();

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

  sequence<string> ModelNameList;

  /**
   * interface ModelHome -- a model factory that creates a Model object given
   *  - 1) model name,
   *  - 2) XML configuration embedded in a string
   *  - 3) JSON configuration embedded in a string
   *  - 4) Parameters in JSON format embedded in a string
   */
  interface ModelHome {
    Model* create(string name);
    Model* createFromXML(["cpp:type:wstring"] string parameters);
    Model* createFromJSON(["cpp:type:string"] string configuration);
    Model* createFromParameters(["cpp:type:string"] string parameters);

    ModelNameList getModelList();
    string getModelInfo(string name);

    Simulator* createSim(Model* rootModel);
  };

};

#endif
