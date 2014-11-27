/****************************************************************************
** 
** This file is part of HiPNOS.
** 
** Copyright 2012 Helmholtz-Zentrum Dresden-Rossendorf
** 
** HiPNOS is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
** 
** HiPNOS is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with HiPNOS.  If not, see <http://www.gnu.org/licenses/>.
** 
****************************************************************************/

#ifndef SLIDERSCENEITEM_H
#define SLIDERSCENEITEM_H

#include <QGraphicsItem>
#include <QPainter>

/**
 * @brief QGraphicsItem implementation that draws the slider to the AnalysisSliderScene
 *
 */
class SliderSceneItem : public QGraphicsItem
{
public:
    explicit SliderSceneItem(qreal s);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void draggingStart();
    void draggingEnd();

private:
    qreal size;  
};

#endif // SLIDERSCENEITEM_H
