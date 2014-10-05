// __COPYRIGHT_START__
// Package Name : efscape
// File Name : main.cc
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
//
// This program tests the features of the efscape::gis library.
//
// ChangeLog:

#include <gdal_priv.h>
#include <iostream>
#include <vector>
#include <sstream>

// from efscape/utils
//#include <string_utils.hpp>

#include <TestCode.hh>

/**
 * main body of test program
 *
 * @author Jon Cline <clinej@stanfordalumni.org>
 * @version 0.2 revised 16 Dec 2006
 */
int main(int argc, char** argv) {

  GDALAllRegister();		// register GDAL drivers

  //----------------------
  // set-up test functions
  //----------------------
  std::vector<std::string> lC_args;
  std::vector<gistest::TestCode*> lC_tests; //  vector of test functions

  lC_tests.push_back((gistest::TestCode*) new gistest::OpenGDAL);
  lC_tests.push_back((gistest::TestCode*) new gistest::CreateGDAL);
  lC_tests.push_back((gistest::TestCode*) new gistest::CreateGeoNetCDF);

  while (true) {
    // prompt for user input
    for (int i = 0; i < lC_tests.size(); i++)
      std::cout << i << ": " <<lC_tests[i]->name()
		<< " (" << lC_tests[i]->usage() << ")\n";

    std::cout << "Enter test case (# or q to quit): ";

    std::string lC_input;
    std::cin >> lC_input;

    if (lC_input == "q")
      break;

    // process user input (test #)
    int li_selection = -1;
    std::istringstream lC_buffer(lC_input.c_str());
    lC_buffer >> li_selection;

    if (li_selection > -1 && li_selection < lC_tests.size()) {
      std::cout << "Enter options for <" << lC_tests[li_selection]->name()
		<< "> (terminate with \".\"): ";

      // process user input (test options)
      std::vector<std::string> lC_args;
      while (std::cin >> lC_input) {
	if (lC_input == ".")
	  break;

	std::cout << "option #" << lC_args.size() << "=<" << lC_input <<">\n";
	lC_args.push_back(lC_input);
      }

      // initialize and run test
      if (lC_tests[li_selection]->init(lC_args) == EXIT_FAILURE)
	std::cout << lC_tests[li_selection]->errorMsg();
      else {
	// execute test
	if ( (*lC_tests[li_selection])() == EXIT_FAILURE )
	  std::cout << lC_tests[li_selection]->errorMsg() << std::endl;
	else
	  std::cout << "test <" << lC_tests[li_selection]->name()
		    << "> was successful\n...\n";
      }
    }
    else
      std::cout << "error: invalid selection <" << li_selection << ">\n";

  } // end of while (true)

  // clean up (should use smart pointers)
  for (int i = 0; i < lC_tests.size(); i++)
    delete lC_tests[i];

  return (EXIT_SUCCESS);

} // end of main(...)

