// __COPYRIGHT_START__
// Package Name : efscape
// File Name : LandscapeI.cpp
// Copyright (C) 2006-2014 by Jon C. Cline (clinej@alumni.stanford.edu)
// Distributed under the terms of the LGPLv3 or newer.
// __COPYRIGHT_END__
#include <efscape/gis/LandscapeI.hpp> // class definition

#include <iostream>

namespace efscape {

  namespace gis {

    /** constructor */
    LandscapeI::LandscapeI() {
      mCCCp_Geogrids.reset(new GeogridMap);
    }

    /** destructor */
    LandscapeI::~LandscapeI() {
    }

    /**
     * Add a geogrid layer.
     *
     * @param aCp_grid handle to geogrid
     * @returns handle to geogrid
     */
    Geogrid* LandscapeI::addGeogrid(Geogrid* aCp_grid)
    {
      if (aCp_grid == 0)
	return 0;

      GeogridPtr lCp_grid(aCp_grid);

      (*mCCCp_Geogrids)[aCp_grid->name()] = lCp_grid;
      // mCp1_geogrids.push_back(lCp_grid);
      
      return aCp_grid;
    }

    /**
     * Returns handle to geogrid layer <acp_name>
     *
     * @param acp_name geogrid name
     * @returns handle to geogrid
     */
    Geogrid* LandscapeI::getGeogrid(const char* acp_name)
    {
      GeogridMap::iterator iGrid = mCCCp_Geogrids->find(acp_name);

      if (iGrid == mCCCp_Geogrids->end()) {
      	return 0;
      }

      return iGrid->second.get();
    }

    /**
     * Returns handle to byte geogrid layer <acp_name>
     *
     * @param acp_name geogrid name
     * @returns handle to byte geogrid
     */
    ByteGeogrid* LandscapeI::getByteGeogrid(const char* acp_name)
    {
      return dynamic_cast<ByteGeogrid*>( getGeogrid(acp_name) );
    }

    /**
     * Returns handle to short geogrid layer <acp_name>
     *
     * @param acp_name geogrid name
     * @returns handle to short geogrid
     */
    ShortGeogrid* LandscapeI::getShortGeogrid(const char* acp_name)
    {
      return dynamic_cast<ShortGeogrid*>( getGeogrid(acp_name) );
    }

    /**
     * Returns handle to int geogrid layer <acp_name>
     *
     * @param acp_name geogrid name
     * @returns handle to int geogrid
     */
    IntGeogrid* LandscapeI::getIntGeogrid(const char* acp_name)
    {
      return dynamic_cast<IntGeogrid*>( getGeogrid(acp_name) );
    }

    /**
     * Returns handle to long geogrid layer <acp_name>
     *
     * @param acp_name geogrid name
     * @returns handle to long geogrid
     */
    LongGeogrid* LandscapeI::getLongGeogrid(const char* acp_name)
    {
      return dynamic_cast<LongGeogrid*>( getGeogrid(acp_name) );
    }

    /**
     * Returns handle to float geogrid layer <acp_name>
     *
     * @param acp_name geogrid name
     * @returns handle to float geogrid
     */
    FloatGeogrid* LandscapeI::getFloatGeogrid(const char* acp_name)
    {
      return dynamic_cast<FloatGeogrid*>( getGeogrid(acp_name) );
    }

    /**
     * Returns handle to double geogrid layer <acp_name>
     *
     * @param acp_name geogrid name
     * @returns handle to double geogrid
     */
    DoubleGeogrid* LandscapeI::getDoubleGeogrid(const char* acp_name)
    {
      return dynamic_cast<DoubleGeogrid*>( getGeogrid(acp_name) );
    }

    /**
     * Returns const handle to geogrid layer <acp_name>
     *
     * @param acp_name geogrid name
     * @returns const handle to geogrid
     */
    const Geogrid* LandscapeI::getGeogrid(const char* acp_name) const
    {
      GeogridMap::const_iterator iGrid = mCCCp_Geogrids->find(acp_name);
      if (iGrid == mCCCp_Geogrids->end()) {
	return 0;
      }

      return iGrid->second.get();
    }

    /**
     * Returns handle to const byte geogrid layer <acp_name>
     *
     * @param acp_name geogrid name
     * @returns handle to const byte geogrid
     */
    const ByteGeogrid* LandscapeI::getByteGeogrid(const char* acp_name) const
    {
      return dynamic_cast<const ByteGeogrid*>( getGeogrid(acp_name) );
    }

    /**
     * Returns handle to const short geogrid layer <acp_name>
     *
     * @param acp_name geogrid name
     * @returns handle to const short geogrid
     */
    const ShortGeogrid* LandscapeI::getShortGeogrid(const char* acp_name) const
    {
      return dynamic_cast<const ShortGeogrid*>( getGeogrid(acp_name) );
    }

    /**
     * Returns handle to const int geogrid layer <acp_name>
     *
     * @param acp_name geogrid name
     * @returns handle to const int geogrid
     */
    const IntGeogrid* LandscapeI::getIntGeogrid(const char* acp_name) const
    {
      return dynamic_cast<const IntGeogrid*>( getGeogrid(acp_name) );
    }

    /**
     * Returns handle to const long geogrid layer <acp_name>
     *
     * @param acp_name geogrid name
     * @returns handle to const long geogrid
     */
    const LongGeogrid* LandscapeI::getLongGeogrid(const char* acp_name) const
    {
      return dynamic_cast<const LongGeogrid*>( getGeogrid(acp_name) );
    }

    /**
     * Returns handle to const float geogrid layer <acp_name>
     *
     * @param acp_name geogrid name
     * @returns handle to const float geogrid
     */
    const FloatGeogrid* LandscapeI::getFloatGeogrid(const char* acp_name) const
    {
      return dynamic_cast<const FloatGeogrid*>( getGeogrid(acp_name) );
    }

    /**
     * Returns handle to const double geogrid layer <acp_name>
     *
     * @param acp_name geogrid name
     * @returns handle to const double geogrid
     */
    const DoubleGeogrid* LandscapeI::getDoubleGeogrid(const char* acp_name) const
    {
      return dynamic_cast<const DoubleGeogrid*>( getGeogrid(acp_name) );
    }

    /**
     * Returns the set of all geogrids.
     *
     * @param reference to set of geogrids
     */
    void LandscapeI::getGeogrids(GeogridSet& aCCpr_geogrids)
    {
      GeogridMap::iterator iGrid = mCCCp_Geogrids->begin();

      for ( ; iGrid != mCCCp_Geogrids->end(); iGrid++) {
	aCCpr_geogrids.insert(iGrid->second);
      }
    }

    /**
     * Add a GDALDataset.
     *
     * @param aCp_dataset smart handle to dataset
     */
    void LandscapeI::setDataset(const boost::shared_ptr<GDALDataset>& aCp_dataset)
    {
      mCp_dataset = aCp_dataset;
    }

  } // namespace gis

} // namespace efscape
