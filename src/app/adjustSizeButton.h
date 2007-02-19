// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINE_ADJUST_SIZE_BUTTON_H
#define CODEINE_ADJUST_SIZE_BUTTON_H

#include <q3frame.h>
//Added by qt3to4:
#include <QEvent>
#include <QTimerEvent>

namespace Codeine
{
   class AdjustSizeButton : public Q3Frame
   {
      int m_counter;
      int m_stage;
      int m_offset;
      int m_timerId;

      QWidget *m_preferred;
      QWidget *m_oneToOne;

      Q3Frame *m_thingy;

   public:
      AdjustSizeButton( QWidget *parent );

   private:
      virtual void timerEvent( QTimerEvent* );
      virtual bool eventFilter( QObject*, QEvent* );

      inline void move()
      {
         QWidget::move( parentWidget()->width() - width(), parentWidget()->height() - m_offset );
      }
   };
}

#endif
