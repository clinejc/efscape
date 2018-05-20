// __COPYRIGHT_START__
// Package Name : efscape
// File Name : Simulator.ice
// Copyright (C) 2006-2018 Jon C. Cline
// 
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
// __COPYRIGHT_END__
//=============================================================================
//
#pragma once

#include <efscape/common.ice>
#include <efscape/Model.ice>

/**
 * Defines the efscape interface, a interface for a DEVS-based simulation
 * services, for an Internet Connection Environment (ICE)-based framework.
 *
 * @author Jon C. Cline <jon.c.cline@gmail.com>
 * @version 1.0.0 created 06 Dec 2006, revised 02 Apr 2018
 */
[["js:es6-module"]]
module efscape {

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

    /**
     * Returns the model associated with this simulator
     *
     * @returns model proxy
     */
    Model* getModel();

    void destroy();
  };

};
