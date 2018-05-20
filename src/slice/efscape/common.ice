// __COPYRIGHT_START__
// Package Name : efscape
// File Name : common.ice
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

/**
 * Defines the efscape interface, a interface for a DEVS-based simulation
 * services, for an Internet Connection Environment (ICE)-based framework.
 *
 * @author Jon C. Cline <jon.c.cline@gmail.com>
 * @version 1.0.0 created 06 Dec 2006, revised 19 May 2018
 */
[["js:es6-module"]]
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
  struct Content {
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

};
