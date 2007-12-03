// Copyright 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include "actions.h"
#include "debug.h"

#include <qtoolbutton.h>
//Added by qt3to4:
#include <Q3CString>

#include <klocale.h>

#include "videoWindow.h"

namespace Codeine
{
    PlayAction::PlayAction( QObject *receiver, const char *slot, KActionCollection *ac )
            : KToggleAction( i18n("Play"), ac )
     {
          setObjectName( "play" );
          setIcon( KIcon( "media-playback-start" ) );
          setShortcut( Qt::Key_Space );
          ac->addAction( objectName(), this );
          connect( this, SIGNAL( toggled( bool ) ), receiver, slot );
     }

     void PlayAction::setPlaying( bool playing )
     {
        if( playing )
        {
            setIcon( KIcon( "media-playback-pause" ) );
            setText( i18n("&Pause") );
        }
        else 
        {
            setIcon( KIcon( "media-playback-start" ) );
            setText( i18n("&Play") );
        }
     }

    void
    PlayAction::setChecked( bool b )
    {
        if( videoWindow()->state() == Engine::Empty && sender() && Q3CString(sender()->className()) == "KToolBarButton" ) {
            // clicking play when empty means open PlayMediaDialog, but we have to uncheck the toolbar button
            // as KDElibs sets that checked automatically..
            ((QToolButton*)sender())->setOn( false );
        }
        else
            KToggleAction::setChecked( b );
    }
}

#include "actions.moc"
