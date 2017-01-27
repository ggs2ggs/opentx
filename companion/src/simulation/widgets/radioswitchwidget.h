/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _RADIOSWITCHWIDGET_H_
#define _RADIOSWITCHWIDGET_H_

#include "radiowidget.h"
#include "boards.h"

#include <QSlider>
#include <QTimer>
#include <QToolButton>

class RadioSwitchWidget : public RadioWidget
{
  Q_OBJECT

  public:

    explicit RadioSwitchWidget(Board::SwitchType type = Board::SWITCH_3POS, QWidget * parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) :
      RadioWidget(parent, f),
      swType(type)
    {
      init();
    }
    explicit RadioSwitchWidget(Board::SwitchType type, const QString & labelText, int value = 0, QWidget * parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) :
      RadioWidget(labelText, value, parent, f),
      swType(type)
    {
      init();
    }

    void init()
    {
      m_type = RADIO_WIDGET_SWITCH;

      QSlider * sl = new QSlider();
      sl->setOrientation(Qt::Vertical);
      sl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
      sl->setInvertedAppearance(true);
      sl->setInvertedControls(true);
      sl->setTickPosition(QSlider::TicksBothSides);
      sl->setPageStep(1);
      sl->setMinimum((swType == Board::SWITCH_3POS ? -1 : 0));
      sl->setMaximum(1);
      sl->setTickInterval(1);
      sl->setValue(m_value);

      if (swType == Board::SWITCH_TOGGLE) {
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/simulator/icons/8/lock-locked.png"), QSize(8, 8), QIcon::Normal, QIcon::On);
        icon.addFile(QStringLiteral(":/images/simulator/icons/8/lock-unlocked.png"), QSize(8, 8), QIcon::Normal, QIcon::Off);
        QToolButton * lockBtn = new QToolButton(this);
        lockBtn->setIcon(icon);
        lockBtn->setIconSize(QSize(8, 8));
        lockBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        lockBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        lockBtn->setAutoRaise(true);
        lockBtn->setCheckable(true);
        lockBtn->setToolTip(tr("Latch/unlatch the momentary switch."));

        QWidget * container = new QWidget(this);
        container->setFixedHeight(50);
        container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        QVBoxLayout * cl = new QVBoxLayout(container);
        cl->setContentsMargins(0, 0, 0, 0);
        cl->setSpacing(1);
        cl->addWidget(sl, 1, Qt::AlignHCenter);
        cl->addWidget(lockBtn, 0, Qt::AlignHCenter);

        setWidget(container);

        connect(lockBtn, &QToolButton::toggled, this, &RadioSwitchWidget::setToggleLocked);
        connect(sl, &QSlider::valueChanged, this, &RadioSwitchWidget::onValueChanged);
        connect(this, &RadioWidget::flagsChanged, lockBtn, &QToolButton::setChecked);
      }
      else {
        sl->setFixedHeight(50);
        setWidget(sl);
      }

      connect(sl, &QSlider::valueChanged, this, &RadioWidget::setValue);
      connect(this, &RadioWidget::valueChanged, sl, &QSlider::setValue);

    }

    void setToggleLocked(bool lock)
    {
      if (m_flags != lock) {
        m_flags = lock;
        setValue((int)lock);
        emit flagsChanged(m_flags);
      }
    }

    void resetToggle()
    {
      setValue(0);
    }

  private slots:

    void onValueChanged(int value)
    {
      if (swType == Board::SWITCH_TOGGLE && !m_flags)
        QTimer::singleShot(500, this, SLOT(onMomentaryTimeout()));
    }

    void onMomentaryTimeout()
    {
      if (!m_flags)
        resetToggle();
    }

  private:
    Board::SwitchType swType;
};




#endif // _RADIOSWITCHWIDGET_H_
