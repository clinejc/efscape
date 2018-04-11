// __COPYRIGHT_START__
// Package Name : efscape
// File Name : adevs_decorator.h
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
#ifndef __adevs_decorator_h_
#define __adevs_decorator_h_

// parent class definitions
#include <adevs_models.h>

namespace adevs {

  /**
   * This class implements the Decorator pattern to wrap a Network or Atomic
   * model and add methods and data to an existing model.\n
   * \n
   * Note: the model decorator does not own the wrapped model (i.e. is not
   * responsible for its ultimate destruction).
   *
   * @author Jon Cline <clinej@stanfordalumni.org>
   * @version 1.0.1 created 15 April 2008, revised 23 Apr 2017
   */
  template <class X>
  class ModelDecorator : public adevs::Network<X>
  {
  public:

    // constructor/destructor
    ModelDecorator();
    ~ModelDecorator();

    /**
     * Sets the wrapped model.
     *
     * @param aCp_model handle to model to be wrapped
     */
    void setWrappedModel(Devs<X>* aCp_model) {
      if (aCp_model) {
	mCp_model = aCp_model;
	mCp_model->setParent(this);
      }
    }

    /** @returns handle to wrapped model */
    Devs<X>* getWrappedModel() { return mCp_model; }

    /** @returns const handle to wrapped model */
    const Devs<X>* getWrappedModel() const { return mCp_model; }

    //----------------------------
    // adevs Network model methods
    //----------------------------
    void getComponents(Set< Devs<X>* >& c);
    void route(const X& value, Devs<X>* model,
	       Bag<Event<X> >& r);

  private:

    /** handle to wrapped model */
    Devs<X>* mCp_model;

  }; // end of template class<> ModelDecorator<X> definition

  //-------------------------------------------------------------------------
  // beginning of template class<> ModelDecorator<libname> implementation
  //-------------------------------------------------------------------------
  /** constructor */
  template <class X>
  ModelDecorator<X>::ModelDecorator() :
    adevs::Network<X>(),
    mCp_model(0)
  {
  }

  /** destructor */
  template <class X>
  ModelDecorator<X>::~ModelDecorator()
  {
  }

  /**
   * Fills set c with all component models.
   *
   * @param c set of component models
   */
  template <class X>
  void ModelDecorator<X>::getComponents(Set< Devs<X>* >& c)
  {
    if (mCp_model == NULL)
      return;

    c.insert(mCp_model);
  }

  /**
   * Routes events to target models. In this implementation, events are
   * routed between the parent model and the single component model.
   * 
   * @param x value to be delivered
   * @param model source model
   * @param r event receiver bag
   */
  template <class X>
  void ModelDecorator<X>::route(const X& x, Devs<X>* model,
				Bag<Event<X> >& r)
  {
    Event<X> lC_event;

    if (mCp_model == this) {
      if (mCp_model == NULL)
	return;

      lC_event.model = mCp_model;
      lC_event.value.port = x.port;
      lC_event.value.value = x.value;
    }
    else {
      lC_event.model = this;
      lC_event.value.port = x.port;
      lC_event.value.value = x.value;
    }

    r.insert(lC_event);

  } // ModelDecorator<X>::route(...)
  
} // namespace adevs

#endif	// #ifndef __adevs_decorator_h_
