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

#include <QPainter>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include "ToggleSwitch.h"

// Geometry constants of the pill.
static constexpr int kWidth = 46;
static constexpr int kHeight = 26;
static constexpr int kMargin = 3;  // gap between handle and track edge

ToggleSwitch::ToggleSwitch(QWidget *parent) :
    QAbstractButton(parent),
    m_trackOn(0x16, 0xa3, 0x94),
    m_trackOff(0xcf, 0xd3, 0xd8),
    m_handle(Qt::white),
    m_offset(kMargin),
    m_anim(new QPropertyAnimation(this, "offset", this)) {
  setCheckable(true);
  setCursor(Qt::PointingHandCursor);
  setFocusPolicy(Qt::TabFocus);
  m_anim->setDuration(120);
  m_anim->setEasingCurve(QEasingCurve::InOutCubic);
  m_offset = offForState(isChecked());
}

QSize ToggleSwitch::sizeHint() const {
  return QSize(kWidth, kHeight);
}

int ToggleSwitch::offForState(bool checked) const {
  int diameter = height() > 0 ? height() - 2 * kMargin : kHeight - 2 * kMargin;
  int travel = (width() > 0 ? width() : kWidth) - diameter - 2 * kMargin;
  return checked ? kMargin + travel : kMargin;
}

void ToggleSwitch::setOffset(int o) {
  m_offset = o;
  update();
}

void ToggleSwitch::setTrackOn(const QColor &c) { m_trackOn = c; update(); }
void ToggleSwitch::setTrackOff(const QColor &c) { m_trackOff = c; update(); }
void ToggleSwitch::setHandle(const QColor &c) { m_handle = c; update(); }

void ToggleSwitch::resizeEvent(QResizeEvent *e) {
  // The handle offset depends on the widget size, which isn't final until the
  // layout has sized us. Re-snap to the correct end once we know our real size
  // (skip while a click animation is in flight so we don't fight it).
  QAbstractButton::resizeEvent(e);
  if (m_anim->state() != QAbstractAnimation::Running) {
    setOffset(offForState(isChecked()));
  }
}

void ToggleSwitch::checkStateSet() {
  // Programmatic setChecked() (e.g. loading settings): snap, don't animate.
  QAbstractButton::checkStateSet();
  m_anim->stop();
  setOffset(offForState(isChecked()));
}

void ToggleSwitch::nextCheckState() {
  // User interaction: flip the state and animate the handle to the new end.
  QAbstractButton::nextCheckState();
  animateTo(isChecked());
}

void ToggleSwitch::animateTo(bool checked) {
  m_anim->stop();
  m_anim->setStartValue(m_offset);
  m_anim->setEndValue(offForState(checked));
  m_anim->start();
}

void ToggleSwitch::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);

  int diameter = height() - 2 * kMargin;
  qreal radius = height() / 2.0;

  // Track: interpolate between off/on color based on how far the handle
  // has travelled so the color cross-fades along with the slide.
  int start = offForState(false);
  int end = offForState(true);
  qreal t = (end != start)
                ? qBound(0.0, qreal(m_offset - start) / qreal(end - start), 1.0)
                : (isChecked() ? 1.0 : 0.0);
  QColor track(
      int(m_trackOff.red() + (m_trackOn.red() - m_trackOff.red()) * t),
      int(m_trackOff.green() + (m_trackOn.green() - m_trackOff.green()) * t),
      int(m_trackOff.blue() + (m_trackOn.blue() - m_trackOff.blue()) * t));

  if (!isEnabled()) {
    track.setAlpha(120);
  }

  p.setPen(Qt::NoPen);
  p.setBrush(track);
  p.drawRoundedRect(rect(), radius, radius);

  // Handle
  QColor handle = m_handle;
  if (!isEnabled()) {
    handle.setAlpha(200);
  }
  p.setBrush(handle);
  p.drawEllipse(QRectF(m_offset, kMargin, diameter, diameter));
}
