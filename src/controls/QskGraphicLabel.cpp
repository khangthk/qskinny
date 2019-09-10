/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGraphicLabel.h"
#include "QskAspect.h"
#include "QskColorFilter.h"
#include "QskGraphic.h"
#include "QskGraphicProvider.h"
#include "QskSetup.h"
#include "QskSkin.h"

QSK_SUBCONTROL( QskGraphicLabel, Graphic )

class QskGraphicLabel::PrivateData
{
  public:
    PrivateData( const QUrl& sourceUrl )
        : source( sourceUrl )
        , sourceSize( -1, -1 )
        , alignment( Qt::AlignLeft | Qt::AlignVCenter )
        , fillMode( QskGraphicLabel::PreserveAspectFit )
        , mirror( false )
        , isSourceDirty( !sourceUrl.isEmpty() )
    {
    }

    QUrl source;
    QSize sourceSize;

    QskGraphic graphic;

    Qt::Alignment alignment;

    uint fillMode : 2;
    bool mirror : 1;
    bool isSourceDirty : 1;
};

QskGraphicLabel::QskGraphicLabel( const QUrl& source, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData( source ) )
{
    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );

    if ( !m_data->source.isEmpty() )
        polish();
}

QskGraphicLabel::QskGraphicLabel( QQuickItem* parent )
    : QskGraphicLabel( QUrl(), parent )
{
}

QskGraphicLabel::QskGraphicLabel( const QString& source, QQuickItem* parent )
    : QskGraphicLabel( QUrl( source ), parent )
{
}

QskGraphicLabel::QskGraphicLabel( const QskGraphic& graphic, QQuickItem* parent )
    : QskGraphicLabel( parent )
{
    m_data->graphic = graphic;
}

QskGraphicLabel::~QskGraphicLabel()
{
}

bool QskGraphicLabel::isEmpty() const
{
    return m_data->graphic.isNull() && m_data->source.isEmpty();
}

QUrl QskGraphicLabel::source() const
{
    return m_data->source;
}

void QskGraphicLabel::setSource( const QString& source )
{
    setSource( QUrl( source ) );
}

void QskGraphicLabel::setSource( const QUrl& url )
{
    if ( url == m_data->source )
        return;

    m_data->graphic.reset();
    m_data->isSourceDirty = true;
    m_data->source = url;

    resetImplicitSize();
    polish();
    update();

    Q_EMIT sourceChanged();
}

QskGraphic QskGraphicLabel::graphic() const
{
    return m_data->graphic;
}

void QskGraphicLabel::setGraphic( const QskGraphic& graphic )
{
    if ( m_data->graphic != graphic )
    {
        m_data->graphic = graphic;

        resetImplicitSize();
        update();
    }

    // in case we have a sequence setting a source and a graphic later
    m_data->isSourceDirty = false;

    if ( !m_data->source.isEmpty() )
    {
        m_data->source.clear();
        Q_EMIT sourceChanged();
    }
}

void QskGraphicLabel::setGraphicRole( int role )
{
    const int oldRole = graphicRole();

    QskSkinnable::setGraphicRole( effectiveSubcontrol( Graphic ), role );

    if ( role != oldRole )
    {
        update();
        Q_EMIT graphicRoleChanged();
    }
}

int QskGraphicLabel::graphicRole() const
{
    return QskSkinnable::graphicRole( Graphic );
}

QskColorFilter QskGraphicLabel::graphicFilter() const
{
    // can be removed once we can store a filter inidividually
    // for a skinnable

    return effectiveGraphicFilter( QskGraphicLabel::Graphic );
}

void QskGraphicLabel::setMirror( bool on )
{
    if ( on != m_data->mirror )
    {
        m_data->mirror = on;

        if ( !( m_data->sourceSize.isEmpty() || m_data->graphic.isEmpty() ) )
            update();

        Q_EMIT mirrorChanged();
    }
}

bool QskGraphicLabel::mirror() const
{
    return m_data->mirror;
}

void QskGraphicLabel::setSourceSize( const QSize& size )
{
    QSize sz = size;

    if ( sz.width() < 0 )
        sz.setWidth( -1 );

    if ( sz.height() < 0 )
        sz.setHeight( -1 );

    if ( m_data->sourceSize != sz )
    {
        m_data->sourceSize = sz;

        resetImplicitSize();
        update();

        Q_EMIT sourceSizeChanged();
    }
}

QSize QskGraphicLabel::sourceSize() const
{
    return m_data->sourceSize;
}

void QskGraphicLabel::resetSourceSize()
{
    setSourceSize( QSize( -1, -1 ) );
}

void QskGraphicLabel::setFillMode( FillMode mode )
{
    if ( mode != m_data->fillMode )
    {
        m_data->fillMode = mode;

        if ( !m_data->graphic.isEmpty() )
            update();

        Q_EMIT fillModeChanged();
    }
}

QskGraphicLabel::FillMode QskGraphicLabel::fillMode() const
{
    return static_cast< QskGraphicLabel::FillMode >( m_data->fillMode );
}

Qt::Alignment QskGraphicLabel::alignment() const
{
    return m_data->alignment;
}

void QskGraphicLabel::setAlignment( Qt::Alignment alignment )
{
    if ( alignment != m_data->alignment )
    {
        m_data->alignment = alignment;

        if ( !( m_data->sourceSize.isEmpty() || m_data->graphic.isEmpty() ) )
            update();

        Q_EMIT alignmentChanged();
    }
}

QskGraphic QskGraphicLabel::loadSource( const QUrl& url ) const
{
    return Qsk::loadGraphic( url );
}

void QskGraphicLabel::updateLayout()
{
    if ( !m_data->source.isEmpty() && m_data->isSourceDirty )
        m_data->graphic = loadSource( m_data->source );

    m_data->isSourceDirty = false;
}

QSizeF QskGraphicLabel::contentsSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    auto sz = effectiveSourceSize();

    if ( !sz.isEmpty() )
    {
        if ( constraint.width() >= 0.0 )
        {
            sz.setHeight( sz.height() * constraint.width() / sz.width() );
            sz.setWidth( constraint.width() );
        }
        else if ( constraint.height() >= 0.0 )
        {
            sz.setWidth( sz.width() * constraint.height() / sz.height() );
            sz.setHeight( constraint.height() );
        }
    }

    return sz;
}

QSizeF QskGraphicLabel::effectiveSourceSize() const
{
    const QSizeF& sourceSize = m_data->sourceSize;

    if ( sourceSize.width() >= 0 && sourceSize.height() >= 0 )
    {
        // the size has been explicitly set
        return sourceSize;
    }

    if ( !m_data->source.isEmpty() && m_data->isSourceDirty )
    {
        // we have to load to know about the geometry
        m_data->graphic = loadSource( m_data->source );
        m_data->isSourceDirty = false;
    }

    QSizeF sz( 0, 0 );
    if ( !m_data->graphic.isEmpty() )
    {
        const QSizeF defaultSize = m_data->graphic.defaultSize();

        if ( sourceSize.width() <= 0 && sourceSize.height() <= 0 )
        {
            // size is derived from the default size
            sz = defaultSize;
        }
        else if ( sourceSize.width() <= 0 )
        {
            // only the height has been given
            const qreal f = sourceSize.height() / defaultSize.height();
            sz.setWidth( f * defaultSize.width() );
            sz.setHeight( sourceSize.height() );
        }
        else
        {
            // only the width has been given
            const qreal f = sourceSize.width() / defaultSize.width();
            sz.setWidth( sourceSize.width() );
            sz.setHeight( f * defaultSize.height() );
        }
    }

    return sz;
}

void QskGraphicLabel::changeEvent( QEvent* event )
{
    if ( event->type() == QEvent::StyleChange )
    {
        if ( !m_data->source.isEmpty() && qskSetup->skin()->hasGraphicProvider() )
        {
            // we might need to reload from a different skin
            m_data->isSourceDirty = true;
        }
    }

    Inherited::changeEvent( event );
}

#include "moc_QskGraphicLabel.cpp"
