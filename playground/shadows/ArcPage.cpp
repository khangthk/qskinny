/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "ArcPage.h"
#include "ShadowedArc.h"
#include "Slider.h"

#include <QskGridBox.h>

namespace
{
    class ControlPanel : public QskGridBox
    {
        Q_OBJECT

      public:
        ControlPanel( ShadowedArc* arc, QQuickItem* parent = nullptr )
            : QskGridBox( parent )
        {
            setMargins( 5 );
            setSpacing( 10 );

            {
                auto slider = new Slider( "Start", 0, 360, 10, arc->startAngle() );
                connect( slider, &Slider::valueChanged, arc, &ShadowedArc::setStartAngle );

                addItem( slider, 0, 0 );
            }
            {
                auto slider = new Slider( "Span", -360, 360, 10, arc->spanAngle() );
                connect( slider, &Slider::valueChanged, arc, &ShadowedArc::setSpanAngle );

                addItem( slider, 0, 1 );
            }
            {
                auto slider = new Slider( "Extent", 0, 100, 1, arc->thickness() );
                connect( slider, &Slider::valueChanged, arc, &ShadowedArc::setThickness );

                addItem( slider, 1, 0 );
            }
            {
                auto slider = new Slider( "Border", 0, 10, 1, arc->borderWidth() );
                connect( slider, &Slider::valueChanged, arc, &ShadowedArc::setBorderWidth );

                addItem( slider, 1, 1);
            }
            {
                auto slider = new Slider( "Spread Radius", -10, 50, 1, arc->spreadRadius() );
                connect( slider, &Slider::valueChanged, arc, &ShadowedArc::setSpreadRadius );

                addItem( slider, 2, 0 );
            }
            {
                auto slider = new Slider( "Blur Radius", 0, 50, 1, arc->blurRadius() );
                connect( slider, &Slider::valueChanged, arc, &ShadowedArc::setBlurRadius );

                addItem( slider, 2, 1 );
            }
            {
                auto slider = new Slider( "Offset X", -50, 50, 1, arc->offsetX() );
                connect( slider, &Slider::valueChanged, arc, &ShadowedArc::setOffsetX );

                addItem( slider, 3, 0 );

            }
            {
                auto slider = new Slider( "Offset Y", -50, 50, 1, arc->offsetY() );
                connect( slider, &Slider::valueChanged, arc, &ShadowedArc::setOffsetY );

                addItem( slider, 3, 1 );
            }
        }
    };

    class Arc : public ShadowedArc
    {
      public:
        Arc()
        {
            setStartAngle( 45.0 );
            setSpanAngle( 270.0 );
            setThickness( 10.0 );

            setFillGradient( Qt::darkRed );

            setBorderWidth( 0 );
            setBorderColor( Qt::darkYellow );

            setShadowColor( Qt::black );
            setSpreadRadius( 0.0 );
            setBlurRadius( 4.0 );
            setOffsetX( 2.0 );
            setOffsetY( 2.0 );
        }
    };
}

ArcPage::ArcPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    auto arc = new Arc();
    arc->setMargins( 40 ); // some extra space for testing the offsets

    auto panel = new ControlPanel( arc );
    panel->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );

    addItem( panel );
    addItem( arc );
}

#include "ArcPage.moc"
