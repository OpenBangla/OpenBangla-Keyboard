/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2016-2022 Muhammad Mominul Huque <mominul2082@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TOGGLESWITCH_H
#define TOGGLESWITCH_H

#include <QAbstractButton>
#include <QColor>

class QPropertyAnimation;

/**
 * A modern sliding pill toggle switch.
 *
 * Subclasses QAbstractButton so that the existing isChecked()/setChecked()/
 * toggled() API keeps working exactly like the QPushButton toggles it replaces.
 *
 * The track/handle colors are exposed as Q_PROPERTYs so they can be driven
 * entirely from the theme stylesheets, e.g.:
 *   ToggleSwitch { qproperty-trackOn: #16a394; qproperty-trackOff: #cfd3d8; }
 */
class ToggleSwitch : public QAbstractButton {
Q_OBJECT
  Q_PROPERTY(QColor trackOn READ trackOn WRITE setTrackOn)
  Q_PROPERTY(QColor trackOff READ trackOff WRITE setTrackOff)
  Q_PROPERTY(QColor handle READ handle WRITE setHandle)
  // Internal animated handle offset (not meant to be set from QSS).
  Q_PROPERTY(int offset READ offset WRITE setOffset)

public:
  explicit ToggleSwitch(QWidget *parent = nullptr);

  QSize sizeHint() const override;

  QColor trackOn() const { return m_trackOn; }
  void setTrackOn(const QColor &c);

  QColor trackOff() const { return m_trackOff; }
  void setTrackOff(const QColor &c);

  QColor handle() const { return m_handle; }
  void setHandle(const QColor &c);

  int offset() const { return m_offset; }
  void setOffset(int o);

protected:
  void paintEvent(QPaintEvent *) override;
  void resizeEvent(QResizeEvent *) override;  // re-snap handle to final size
  void checkStateSet() override;   // snap (programmatic setChecked)
  void nextCheckState() override;  // animate (user click)

private:
  int offForState(bool checked) const;
  void animateTo(bool checked);

  QColor m_trackOn;
  QColor m_trackOff;
  QColor m_handle;
  int m_offset;
  QPropertyAnimation *m_anim;
};

#endif // TOGGLESWITCH_H
