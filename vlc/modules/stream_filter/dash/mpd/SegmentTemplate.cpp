/*****************************************************************************
 * SegmentTemplate.cpp: Implement the UrlTemplate element.
 *****************************************************************************
 * Copyright (C) 1998-2007 VLC authors and VideoLAN
 * $Id: 507b5438472e68d764e0e3f3f19452246e061db1 $
 *
 * Authors: Hugo Beauzée-Luyssen <beauze.h@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "SegmentTemplate.h"
#include "SegmentTimeline.h"
#include "Representation.h"
#include "Group.h"
#include "SegmentInfoDefault.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>

using namespace dash::mpd;

SegmentTemplate::SegmentTemplate( bool containRuntimeIdentifier,
                                  Representation* representation ) :
    containRuntimeIdentifier( containRuntimeIdentifier ),
    representation( representation ),
    beginTime( std::string::npos ),
    beginIndex( std::string::npos ),
    currentSegmentIndex( 0 )
{
}

std::string     SegmentTemplate::getSourceUrl() const
{
    std::string     res = this->sourceUrl;

    if ( this->containRuntimeIdentifier == false )
        return Segment::getSourceUrl();

    if ( this->beginIndex != std::string::npos )
        std::cerr << "Unhandled identifier \"$Index$\"" << std::endl;
    if ( this->beginTime != std::string::npos )
    {
        //FIXME: This should use the current representation SegmentInfo
        //which "inherits" the SegmentInfoDefault values.
        if ( this->representation->getParentGroup()->getSegmentInfoDefault() != NULL &&
             this->representation->getParentGroup()->getSegmentInfoDefault()->getSegmentTimeline() != NULL )
        {
            const SegmentTimeline::Element  *el = this->representation->getParentGroup()->
                    getSegmentInfoDefault()->getSegmentTimeline()->getElement( this->currentSegmentIndex );
            if ( el != NULL )
            {
                std::ostringstream  oss;
                oss << el->t;
                res.replace( this->beginTime, strlen("$Time$"), oss.str() );
            }
        }
    }
    return res;
}

void    SegmentTemplate::setSourceUrl( const std::string &url )
{
    if ( this->containRuntimeIdentifier == true )
    {
        this->beginTime = url.find( "$Time$" );
        this->beginIndex = url.find( "$Index$" );
    }
    Segment::setSourceUrl( url );
}

bool            SegmentTemplate::isSingleShot() const
{
    return false;
}

void SegmentTemplate::done()
{
    this->currentSegmentIndex++;
}

