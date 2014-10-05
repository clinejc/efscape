// __COPYRIGHT_START__
// __COPYRIGHT_END__
#include "ogis/impl/ogis_utils.hpp"

namespace ogis {
  namespace impl {

    /**
     * Utility function for converting an array of gdal strings to a std vector
     * of std strings.
     *
     * @param papszString array of gdal strings
     * @returns vector of std strings
     */
    std::vector<std::string> getStdStrings(char** papszStrings) {
      std::vector<std::string> lC1_strings;

      int li_length = CSLCount(papszStrings);

      for (int i = 0; i < li_length; i++) {
	std::string lC_field = CSLGetField(papszStrings, i);

 	lC1_strings.push_back(lC_field);

      }	// end of for (int i = 0...

      CSLDestroy(papszStrings);	// destroy the string list

      return lC1_strings;
    }

    /**
     * Utility function for converting an std vector of std strings to an array
     * of gdal strings.
     *
     * @param aC1_string vector of string
     * @returns array of gdal string
     */
    char** getCSLStrings(const std::vector<std::string>& aC1_strings) {
      char** papszStrings;

      for (int i = 0; i < aC1_strings.size(); i++)
	papszStrings = CSLAddString(papszStrings, aC1_strings[i].c_str());

      return papszStrings;
    }

    /**
     * utility function for copying metadata names from a GDAMajorObject
     * to a string sequence
     *
     * @param aCp_GDALObject pointer to GDALObject
     * @returns string vector containing array of metadata names
     */
    std::vector<std::string> getMetadataNames(GDALMajorObject* aCp_GDALObject)
    {
      // access GDALMajorObject metadata
      char **papszMetadata = aCp_GDALObject->GetMetadata();

      // create a string sequence to hold metadata names
      std::vector<std::string> lC1_MetadataNames;

      int li_length = CSLCount(papszMetadata);

      //-----------------------------------------------------------------------
      // copy each metadata name to the sequence
      //
      // Each GDAL string field (see "cpl_string.h") consists of a name/value
      // pair separated by either a ":" or a "=". If neither separator is
      // found, the name is set to the entire field. (2006-02-06)
      //-----------------------------------------------------------------------
      for (int i = 0; i < li_length; i++) {
	std::string lC_field = CSLGetField(papszMetadata, i);

	// split name from <name>:<value> pair
	std::string lC_name;
	std::string::size_type pos = 0;
	std::string::size_type endpos = lC_field.size();

	// find name/pair separator
	if ( ( pos = lC_field.find(":") ) != std::string::npos )
	  endpos = pos;
	else if ( ( pos = lC_field.find("=") ) != std::string::npos )
	  endpos = pos;

	lC_name = lC_field.substr(0, endpos); // set metadata name

 	lC1_MetadataNames.push_back(lC_name);

      }	// end of for (int i = 0...

      CSLDestroy(papszMetadata);	// destroy the string list

      return lC1_MetadataNames;

    } // metadataNames(...)

  } // namespace impl
}   // namespace ogis
