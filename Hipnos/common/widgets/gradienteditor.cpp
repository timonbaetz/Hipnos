/*
* KmPlot - a math. function plotter for the KDE-Desktop
*
* Copyright (C)      2006  David Saxton <david@bluehaze.org>
*
* This file is part of the KDE Project.
* KmPlot is part of the KDE-EDU Project.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*/

#include "gradienteditor.h"

#include <QApplication>
#include <QLabel>
#include <QLinearGradient>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QtGlobal>
#include <QVBoxLayout>
#include <QWhatsThis>

const double SQRT_3 = 1.732050808;
const double ArrowLength = 8;
const double ArrowHalfWidth = ArrowLength/SQRT_3;


//BEGIN class KGradientEditor
GradientEditor::GradientEditor( QWidget * parent )
: QWidget( parent )
{
    m_haveArrow = false;
    m_clickOffset = 0;
    m_orientation = Qt::Horizontal;
    findGradientStop();
}


GradientEditor::~GradientEditor()
{
}


void GradientEditor::setGradient( const QGradient & gradient )
{
    if ( m_gradient == gradient )
       return;
    setGradient( gradient.stops() );
    findGradientStop();
}


void GradientEditor::setColor( const QColor & color )
{
// Hmm...why doesn't qvector have some sortof search / replace functionality?
QGradientStops stops = m_gradient.stops();
for ( int i = 0; i < stops.size(); ++i )
{
   if ( stops[i] != m_currentStop )
         continue;

   if ( stops[i].second == color )
         return;

   m_currentStop.second = color;
   stops[i] = m_currentStop;
   break;
}

setGradient( stops );
}


QSize GradientEditor::minimumSizeHint() const
{
double w = 3 * ArrowHalfWidth;
double h = 12 + ArrowLength;

if ( m_orientation == Qt::Vertical )
   qSwap( w, h );

return QSizeF( w, h ).toSize();
}


void GradientEditor::paintEvent( QPaintEvent * )
{
QPainter painter( this );

//BEGIN draw gradient
QRectF r;
QLinearGradient lg;

if ( m_orientation == Qt::Horizontal )
{
   lg = QLinearGradient( 0, 0, width(), 0 );
   r = QRectF( ArrowHalfWidth-1, 0, width() - 2*ArrowHalfWidth + 1, height()-ArrowLength );
}
else
{
   lg = QLinearGradient( 0, 0, 0, height() );
   r = QRectF( 0, ArrowHalfWidth-1, width()-ArrowLength, height() - 2*ArrowHalfWidth + 1 );
}

lg.setStops( m_gradient.stops() );
painter.setBrush( lg );
painter.setPen( QPen( Qt::black, 1 ) );
painter.drawRect( r );
//END draw gradient


//BEGIN draw arrows
painter.setRenderHint( QPainter::Antialiasing, true );
QGradientStops stops = m_gradient.stops();
foreach ( const QGradientStop &stop, stops )
   drawArrow( & painter, stop );
//END draw arrows
}


void GradientEditor::drawArrow( QPainter * painter, const QGradientStop & stop )
{
QPolygonF arrow(3);

double mid = toArrowPos( stop.first );

if ( m_orientation == Qt::Horizontal )
{
   arrow[0] = QPointF( mid,                        height()-ArrowLength+0.5 );
   arrow[1] = QPointF( mid+ArrowHalfWidth,   height()-0.5 );
   arrow[2] = QPointF( mid-ArrowHalfWidth,   height()-0.5 );
}
else
{
   arrow[0] = QPointF( width()-ArrowLength+0.5,    mid );
   arrow[1] = QPointF( width()-0.5,                      mid+ArrowHalfWidth );
   arrow[2] = QPointF( width()-0.5,                      mid-ArrowHalfWidth );
}

bool selected = (stop == m_currentStop);
QColor color( selected ? palette().color( QPalette::Dark ) : Qt::black );

painter->setPen( color );
painter->setBrush( stop.second );
painter->drawPolygon( arrow );
}


void GradientEditor::contextMenuEvent( QContextMenuEvent * e )
{
// Prevent the "QWhatsThis" menu from popping up when right-clicking
e->accept();
}


void GradientEditor::removeStop()
{
QGradientStops stops = m_gradient.stops();
for ( int i = 0; i < stops.size(); ++i )
{
   if ( stops[i] != m_currentStop )
         continue;

   stops.remove( i );
   break;
}

setGradient( stops );
findGradientStop();
}


void GradientEditor::mousePressEvent( QMouseEvent * e )
{
if ( !getGradientStop( e->pos() ) )
   return;
e->accept();

if ( e->button() == Qt::RightButton )
   removeStop();
else
   m_haveArrow = true;
}


bool GradientEditor::getGradientStop( const QPoint & point )
{
double dl; // the vertical (for horizontal layout) distance from the tip of the arrows
if ( m_orientation == Qt::Horizontal )
   dl = point.y() - (height() - ArrowLength);
else
   dl = point.x() - (width() - ArrowLength);

// Is the arrow in the strip?
if ( dl < 0 )
   return false;

QGradientStops stops = m_gradient.stops();

// Iterate over stops in reverse as the last stops are displayed on top of
// the first stops.
for ( int i = stops.size()-1; i >= 0; --i )
{
   QGradientStop stop = stops[i];

   double pos = toArrowPos( stop.first );

   // Is the click inside the arrow?
   double lower = pos - dl*(ArrowHalfWidth/ArrowLength);
   double upper = pos + dl*(ArrowHalfWidth/ArrowLength);

   double x = (m_orientation == Qt::Horizontal) ? point.x() : point.y();
   if ( x < lower || x > upper )
         continue;

   // Is inside arrow! :)
   m_clickOffset = x - pos;

   setCurrentStop( stop );
   return true;
}

return false;
}


void GradientEditor::mouseMoveEvent( QMouseEvent * e )
{
if ( !m_haveArrow )
   return;

e->accept();
QPoint point = e->pos();

// Hmm...why doesn't qvector have some sortof search / replace functionality?
QGradientStops stops = m_gradient.stops();
for ( int i = 0; i < stops.size(); ++i )
{
   if ( stops[i] != m_currentStop )
         continue;

   double x = (m_orientation == Qt::Horizontal) ? point.x() : point.y();

   m_currentStop.first = fromArrowPos( x-m_clickOffset );

   stops[i] = m_currentStop;
   break;
}

setGradient( stops );
}


void GradientEditor::mouseReleaseEvent( QMouseEvent * )
{
m_haveArrow = false;
}


void GradientEditor::mouseDoubleClickEvent( QMouseEvent * e )
{
e->accept();

if ( getGradientStop( e->pos() ) )
   return;

// Create new stop
QPoint point = e->pos();
double pos = fromArrowPos( (m_orientation == Qt::Horizontal) ? point.x() : point.y() );

QGradientStop stop;
stop.first = pos;
stop.second = Qt::red;

QGradientStops stops = m_gradient.stops();
stops << stop;

setGradient( stops );
setCurrentStop( stop );
}


void GradientEditor::setOrientation( Qt::Orientation orientation )
{
m_orientation = orientation;
update();
}


void GradientEditor::findGradientStop()
{
QGradientStops stops = m_gradient.stops();

// The QGradientStops should always have at least one stop in, since
// QGradient returns a Black->White gradient if its stops are empty.
Q_ASSERT( !stops.isEmpty() );

// Pick a stop in the center
setCurrentStop( stops[ stops.size()/2 ] );
}


void GradientEditor::setCurrentStop( const QGradientStop & stop )
{
if ( m_currentStop == stop )
   return;

bool colorChanged = stop.second != m_currentStop.second;

m_currentStop = stop;
update();

if ( colorChanged )
   emit colorSelected( stop.second );
}


void GradientEditor::setGradient( const QGradientStops & stops )
{
if ( stops == m_gradient.stops() )
   return;

m_gradient.setStops( stops );
update();
emit gradientChanged( m_gradient );
}


double GradientEditor::toArrowPos( double stop ) const
{
double l = (m_orientation == Qt::Horizontal) ? width() : height();
l -= 2*ArrowHalfWidth;
return stop*l + ArrowHalfWidth;
}


double GradientEditor::fromArrowPos( double pos ) const
{
double l = (m_orientation == Qt::Horizontal) ? width() : height();
l -= 2*ArrowHalfWidth;

double stop = (pos - ArrowHalfWidth) / l;

if ( stop < 0 )
   stop = 0;
else if ( stop > 1 )
   stop = 1;

return stop;
}
//END class KGradientEditor
