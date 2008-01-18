/***********************************************************************
 * Copyright 2004  Max Howell <max.howell@methylblue.com>
 *           2007  Ian Monroe <ian@monroe.nu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy 
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************/

#include "mainWindow.h"

#include <KConfig>
#include <KLocale>
#include <KGlobal>
#include <KNotificationRestrictions>
#include <KToolBar>
#include <KXMLGUIFactory>

#include <QContextMenuEvent>
#include <QEvent>
#include <QLabel>
#include <QToolButton>

#include "actions.h"
#include "adjustSizeButton.h"
#include "dbus/playerDbusHandler.h"
#include "debug.h"
#include "fullScreenAction.h"
#include "mxcl.library.h"
#include "theStream.h"
#include "videoWindow.h"


//TODO do in Sconstruct
#define QT_FATAL_ASSERT


//TODO make the XineEngine into xine::Stream and then make singleton and add functions like Stream::hasVideo() etc.
//TODO make convenience function to get fullscreen state


namespace Codeine {


void
MainWindow::engineStateChanged( Engine::State state )
{
    DEBUG_BLOCK
    if( state == Engine::Uninitialised )
    {
        warning() << "Engine Uninitialised!";
    }
    KUrl const &url = TheStream::url();
    bool const isFullScreen = toggleAction("fullscreen")->isChecked();
    QWidget *const toolbar = reinterpret_cast<QWidget*>(toolBar());

    Debug::Block block( state == Engine::Empty
            ? "State: Empty" : state == Engine::Loaded
            ? "State: Loaded" : state == Engine::Playing
            ? "State: Playing" : state == Engine::Paused
            ? "State: Paused" : state == Engine::TrackEnded
            ? "State: TrackEnded" : "State: Unknown" );


    /// update actions
    {
        using namespace Engine;

        #define enableIf( name, criteria ) action( name )->setEnabled( state & criteria );
        enableIf( "stop", (Playing | Paused) );
        enableIf( "fullscreen", (Playing | Paused) || isFullScreen );
        enableIf( "reset_zoom", ~Empty && !isFullScreen );
        enableIf( "video_settings", (Playing | Paused) );
        enableIf( "volume", (Playing | Paused) );
        #undef enableIf

        toggleAction( "play" )->setChecked( state == Playing );
    }

    debug() << "updated actions";

    /// update menus
    {
        using namespace Engine;

        // the toolbar play button is always enabled, but the menu item
        // is disabled if we are empty, this looks more sensible
        PlayAction* playAction = static_cast<PlayAction*>( actionCollection()->action("play") );
        playAction->setEnabled( state != Empty );
        playAction->setPlaying( state == Playing );
        actionCollection()->action("aspect_ratio_menu")->setEnabled( state & (Playing | Paused) && TheStream::hasVideo() );

        // set correct aspect ratio
        if( state == Loaded )
            TheStream::aspectRatioAction()->setChecked( true );
    }
    debug() << "updated menus";

    /// update statusBar
    {
        using namespace Engine;
        m_timeLabel->setVisible( state & (Playing | Paused) );
    }
    debug() << "updated statusbar";

    /// update position slider
    switch( state )
    {
        case Engine::Uninitialised:
        case Engine::Loaded:
        case Engine::TrackEnded:
        case Engine::Empty:
            m_positionSlider->setEnabled( false );
            if( m_volumeSlider )
                m_volumeSlider->setEnabled( false );
            break;
        case Engine::Playing:
        case Engine::Paused:
            m_positionSlider->setEnabled( TheStream::canSeek() );
            if( m_volumeSlider )
                m_volumeSlider->setEnabled( true );
            break;
    }
    debug() << "update position slider";

    /// update recent files list if necessary
    if( state == Engine::Loaded ) 
    {
        emit fileChanged( engine()->urlOrDisc() );
        // update recently played list
        debug() << " update recent files list ";
        #ifndef NO_SKIP_PR0N
        // ;-)
        const QString url_string = url.url();
        if( !(url_string.contains( "porn", Qt::CaseInsensitive ) || url_string.contains( "pr0n", Qt::CaseInsensitive )) )
        #endif
            if( url.protocol() != "dvd" && url.protocol() != "vcd" && url.prettyUrl()!="") {
                KConfigGroup config = KConfigGroup( KGlobal::config(), "General" );
                const QString prettyUrl = url.prettyUrl();

                QStringList urls = config.readPathEntry( "Recent Urls", QStringList() );
                urls.removeAll( prettyUrl );
                config.writePathEntry( "Recent Urls", urls << prettyUrl );
            }

        if( TheStream::hasVideo() && !isFullScreen && false )  //disable for now, it doesn't paint right
            new AdjustSizeButton( reinterpret_cast<QWidget*>(videoWindow()) );
    }

    /// turn off screensaver
    if( state == Engine::Playing )
    {
        if( !m_stopScreenSaver )
        {
            debug() << "screensaver off";
            m_stopScreenSaver = new KNotificationRestrictions( KNotificationRestrictions::NonCriticalServices, this );
        }
        else
            warning() << "m_stopScreenSaver not null";
    }
    else if( state & ( Engine::TrackEnded | Engine::Empty ) )
    {
        delete m_stopScreenSaver;
        m_stopScreenSaver = 0;
        debug() << "screensaver on";
    }

    /// set titles
    switch( state )
    {
        case Engine::Uninitialised:
        case Engine::Empty:
            m_titleLabel->setText( i18n("No media loaded") );
            break;
        case Engine::Paused:
            m_titleLabel->setText( i18n("Paused") );
            break;
        case Engine::Loaded:
        case Engine::Playing:
        case Engine::TrackEnded:
            m_titleLabel->setText( TheStream::prettyTitle() );
            break;
    }
    debug() << "set titles ";


    //enable/disbale DVD specific buttons
    QWidget *dvd_button = toolBar()->findChild< QWidget* >( "toolbutton_toggle_dvd_menu" );
    if(videoWindow()->isDVD())
    {
        if (dvd_button)
        {
            dvd_button->setVisible(true);
        }
        action("toggle_dvd_menu")->setEnabled( true );
    }
    else
    {
        if (dvd_button)
        {
            dvd_button->setVisible(false);
        }
        action("toggle_dvd_menu")->setEnabled( false );
    }
    if( isFullScreen && !toolbar->testAttribute( Qt::WA_UnderMouse ) ) 
    {
        switch( state ) {
        case Engine::TrackEnded:
            toolbar->show();

            if( videoWindow()->isActiveWindow() ) {
                //FIXME dual-screen this seems to still show
                QContextMenuEvent e( QContextMenuEvent::Other, QPoint() );
                QApplication::sendEvent( videoWindow(), &e );
            }
            break;
        case Engine::Empty:
        case Engine::Paused:
        case Engine::Uninitialised:
            toolBar()->show();
            break;
        case Engine::Playing:
            toolBar()->hide();
            break;
        case Engine::Loaded:
            break;
        }
    } 
    switch( state )
    {
        case Engine::TrackEnded:
        case Engine::Empty:
            emit dbusStatusChanged( PlayerDbusHandler::Stopped ), debug() << "dbus: stopped";
            break;
        case Engine::Paused:
            emit dbusStatusChanged( PlayerDbusHandler::Paused ), debug() << "dbus: paused";
            break;
        case Engine::Playing:
            emit dbusStatusChanged( PlayerDbusHandler::Playing ), debug() << "dbus: playing";
            break;
        break;
        default: break;
    }
    /*
    ///hide videoWindow if audio-only
    if( state == Engine::Playing )
    {
        bool hasVideo = TheStream::hasVideo();
        videoWindow()->setVisible( hasVideo );
        m_fullScreenAction->setEnabled( hasVideo );
    }*/
    /*
    videoWindow()->setVisible( hasVideo );
    toolBar()->dumpObjectTree();
    debug() << "boo";
    QList<QObject*> list = toolBar()->findChildren<QObject *>( "" );
    foreach( QObject* obj, list )
    {
        debug() << QString("*%1*").arg( obj->metaObject()->className() ) << (obj->metaObject()->className() == "QToolButton");
        if(  QString("*%1*").arg( obj->metaObject()->className() ) == "*QToolButton*" )
        {
            debug() << "its a tool button!";
            if( static_cast<QToolButton*>( obj )->defaultAction() == m_fullScreenAction )
            {
                debug() << "hiding? " << hasVideo;
                static_cast<QWidget*>( obj )->setVisible( hasVideo );
            }
        }
    }*/
}

}
