/*
  Copyright (C) 1970-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

  File:	      YQReplacePoint.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qwidget.h>
#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include "YQReplacePoint.h"


YQReplacePoint::YQReplacePoint( YWidget * parent )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YReplacePoint( parent )
{
    setWidgetRep( this );
}


void YQReplacePoint::showChild()
{
    YWidget * child = firstChild();

    if ( child )
    {
	QWidget * qChild = (QWidget *) child->widgetRep();
	qChild->show();
    }
}


void YQReplacePoint::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


void YQReplacePoint::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
    YReplacePoint::setSize( newWidth, newHeight );
}

#include "YQReplacePoint.moc"
