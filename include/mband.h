
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.53
//  C++ Music Library
//
//  Copyright (c) 2012-2013 Arturo Cepeda Pérez
//
//  --------------------------------------------------------------------
//
//  This file is part of Modus. Permission is hereby granted, free 
//  of charge, to any person obtaining a copy of this software and 
//  associated documentation files (the "Software"), to deal in the
//  Software without restriction, including without limitation the 
//  rights to use, copy, modify, merge, publish, distribute, 
//  sublicense, and/or sell copies of the Software, and to permit 
//  persons to whom the Software is furnished to do so, subject to 
//  the following conditions:
//
//  The above copyright notice and this permission notice shall be 
//  included in all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY 
//  KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
//  WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
//  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
//  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
//  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
////////////////////////////////////////////////////////////////////////

///\file

#ifndef _MBAND_H_
#define _MBAND_H_

#include "minstrument.h"

/**
 *  @brief Represents a group of instruments
 */
class MCBand
{
private:
    std::vector<MCInstrument*> mInstruments;
    std::vector<MCBand*> mBands;

public:
    /**
     *  @brief Add an instrument to the band
     *  @param Instrument Instrument to be added to the band
     */
    void addInstrument(MCInstrument* Instrument);
    /**
     *  @brief Remove the instrument in the specified position from the band
     *  @param Position Position of the instrument in the band
     */
    void removeInstrument(unsigned int Position);
    /**
     *  @brief Remove the specified instrument from the band
     *  @param Instrument Instrument to be removed from the band
     */
    void removeInstrument(MCInstrument* Instrument);

    /**
     *  @brief Add a band to this band
     *  @param Band Band to be added to this band
     */
    void addBand(MCBand* Band);
    /**
     *  @brief Remove the band in the specified position from this band
     *  @param Position Position of the band in this band
     */
    void removeBand(unsigned int Position);
    /**
     *  @brief Remove the specified band from this band
     *  @param Band Band to be removed from this band
     */
    void removeBand(MCBand* Band);

    /**
     *  @brief Update all the instruments in the band
     *  @param TimePosition Current time position
     */
    void update(const MSTimePosition& TimePosition);
    /**
     *  @brief Remove all the instruments from the band
     */
    void clear();

    /**
     *  @brief Returns a pointer to the instrument in the specified position
     *  @param Position Position of the instrument in the band
     */
    MCInstrument* getInstrument(unsigned int Position);
    /**
     *  @brief Returns the number of instruments in the band
     */
    unsigned int getNumberOfInstruments();

    /**
     *  @brief Returns a pointer to the band in the specified position
     *  @param Position Position of the band in this band
     */
    MCBand* getBand(unsigned int Position);
    /**
     *  @brief Returns the number of bands in this band
     */
    unsigned int getNumberOfBands();
};

#endif
