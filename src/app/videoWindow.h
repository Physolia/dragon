/***********************************************************************
 * Copyright 2005  Max Howell <max.howell@methylblue.com>
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

#ifndef CODEINE_VIDEOWINDOW_H
#define CODEINE_VIDEOWINDOW_H

#include "codeine.h"

#include <QMultiMap>
#include <QWidget>

#include <Phonon/Path>
#include <Phonon/ObjectDescription>
#include <Solid/Device>
#include <KUrl>

class QActionGroup;
class QTimer;

namespace Phonon {
     class VideoWidget;
     class AudioOutput;
     class MediaObject;
     class MediaController;
}

typedef struct xine_stream_s xine_stream_t;

#include "phonon/phononnamespace.h" //Phonon::State

namespace Codeine
{
    class VideoWindow : public QWidget
    {
    Q_OBJECT

        static VideoWindow *s_instance;

        VideoWindow( const VideoWindow& ); //disable
        VideoWindow &operator=( const VideoWindow& ); //disable
        void eject();

        QTimer* m_cursorTimer;
        bool m_justLoaded;
		bool m_adjustedSize;
        xine_stream_t* m_xineStream;
        QActionGroup* m_subLanguages;
        QActionGroup* m_audioLanguages;
        QWidget* m_logo;

        Phonon::VideoWidget *m_vWidget;
        Phonon::AudioOutput *m_aOutput;
        Phonon::MediaObject *m_media;
        Phonon::MediaController *m_controller;
        Phonon::Path m_audioPath;

        friend class TheStream;
        friend VideoWindow* const engine();
        friend VideoWindow* const videoWindow();

        template<class ChannelDescription>
        void updateActionGroup( QActionGroup* channelActions, const QList<ChannelDescription>& availableChannels
            , const char* actionSlot );

    public:
        VideoWindow( QWidget *parent );
       ~VideoWindow();

        bool init();

        bool load( const KUrl &url );
        bool play( qint64 = 0 );
        bool resume();
        bool playDvd();
        bool playDisc( const Solid::Device& );
        bool isMuted();
        bool mouseUnderWidget();
        void relativeSeek( qint64 );

        qint64 length() const;
        bool isDVD() const;

        ///stuff for dbus:
        //{
        void pause();
        qreal volume() const;
        void setVolume( qreal );
        QString urlOrDisc() const;
        QMultiMap<QString, QString> metaData() const;
        bool isSeekable() const;
        //}

        QWidget* newPositionSlider();
        QWidget* newVolumeSlider();
        void loadSettings();

        Engine::State state() const;

    /// Stuff to do with video and the video window/widget
        static const uint CURSOR_HIDE_TIMEOUT = 2000;

        void becomePreferredSize();

        enum { ExposeEvent = 3000 };

        qint64 currentTime() const;
        int videoSetting( const QString& );

        const xine_stream_t* xineStream() const { return m_xineStream; }

    public slots:
        void seek( qint64 );
        void stop();
        void stateChanged( Phonon::State, Phonon::State );
        void settingChanged( int );
        void mute( bool );

        void toggleDVDMenu();
        void showOSD( const QString& );
        void slotSetSubtitle();
        void slotSetAudio();
        void resetZoom();

        void prevChapter();
        void nextChapter();
	void tenBack();
	void tenForward();

    protected:
        virtual bool event( QEvent* e );
        virtual void contextMenuEvent( QContextMenuEvent * event );
        virtual void mouseDoubleClickEvent( QMouseEvent* );
        virtual QSize sizeHint() const;
        void refreshXineStream();
        Engine::State state( Phonon::State state ) const;
        void setSubtitle( int channel );
        void setAudioChannel( int channel );
    private slots:
        void updateChannels();
        void hideCursor();
    signals:
        void stateChanged( Engine::State );
        void statusMessage( const QString& );
        void titleChanged( const QString& );
        void subChannelsChanged( QList< QAction* > );
        void audioChannelsChanged( QList< QAction* > );
        void tick( qint64 );
        void totalTimeChanged( qint64 );
        void mutedChanged( bool );
        void seekableChanged( bool );
    };

    //global function for general use by Codeine
    //videoWindow() is const for Xlib-thread-safety reasons
    inline VideoWindow* const videoWindow() { return VideoWindow::s_instance; }
    inline VideoWindow* const engine() { return VideoWindow::s_instance; }
}

#endif
