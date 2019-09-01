// __COPYRIGHT_START__
// Package Name : efscape
// File Name : ModelHome.ice
// Copyright (C) 2006-2019 Jon C. Cline
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
#include <efscape/Simulator.ice>

/**
 * Defines the efscape interface, a interface for a DEVS-based simulation
 * services, for an Internet Connection Environment (ICE)-based framework.
 *
 * @author Jon C. Cline <jon.c.cline@gmail.com>
 * @version 1.1.0 created 06 Dec 2006, revised 31 Aug 2019
 */
[["js:es6-module"]]
module efscape {

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

    void shutdown();
  };

};
