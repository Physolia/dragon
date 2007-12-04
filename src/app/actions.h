// (c) 2004 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINEACTIONS_H
#define CODEINEACTIONS_H

#include <KToggleAction>    //baseclass
#include <KActionCollection> //convenience

namespace Codeine
{
   KActionCollection *actionCollection(); ///defined in mainWindow.cpp
   QAction *action( const char* ); ///defined in mainWindow.cpp
   inline KToggleAction *toggleAction( const char *name ) { return (KToggleAction*)action( name ); }

   class PlayAction : public KToggleAction
   {
   Q_OBJECT
   public:
      PlayAction( QObject *receiver, const char *slot, KActionCollection* );
      void setPlaying( bool playing );
   protected:
      virtual void setChecked( bool );
   };
}

#endif

