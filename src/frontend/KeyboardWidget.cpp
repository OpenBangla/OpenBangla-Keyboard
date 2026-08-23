/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2016 Muhammad Mominul Huque <mominul2082@gmail.com>
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

#include <QFontDatabase>
#include <QPainter>
#include <QPaintEvent>
#include "KeyboardWidget.h"

// Space between the bezel edge and the outermost keycaps.
static const double kBezelPad = 14.0;
// Half-gap kept clear around every keycap so adjacent caps don't touch.
static const double kKeyGap = 3.0;
// Base unit (a 1.0-wide keycap) used for the natural size.
static const double kBaseUnit = 56.0;
static const int kRows = 5;

KeyboardWidget::KeyboardWidget(QWidget *parent) : QWidget(parent) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  // Render the Bangla glyphs with the bundled Kalpurush font so complex
  // conjuncts/matras display correctly regardless of the system fonts. Register
  // it only once across all instances.
  static QString registeredFamily = [] {
    int id = QFontDatabase::addApplicationFont(":/fonts/kalpurush.ttf");
    QStringList families = (id != -1) ? QFontDatabase::applicationFontFamilies(id) : QStringList();
    return families.isEmpty() ? QStringLiteral("Kalpurush") : families.first();
  }();
  m_banglaFamily = registeredFamily;

  buildRows();
}

void KeyboardWidget::buildRows() {
  auto letter = [](QChar c) -> Key {
    QString up(c.toUpper());
    QString lo(c.toLower());
    return Key{1.0, QString(), up, QString(), "Key_" + up, "Key_" + lo};
  };
  auto sym = [](const QString &bottom, const QString &top,
                const QString &keyBottom, const QString &keyTop,
                double width = 1.0) -> Key {
    return Key{width, QString(), top, bottom, keyTop, keyBottom};
  };
  auto mod = [](double width, const QString &label) -> Key {
    return Key{width, label, QString(), QString(), QString(), QString()};
  };

  m_rows.clear();

  // Number row.
  m_rows.append({
      sym("`", "~", "Key_Grave", "Key_Tilde"),
      sym("1", "!", "Key_1", "Key_Exclaim"),
      sym("2", "@", "Key_2", "Key_At"),
      sym("3", "#", "Key_3", "Key_Hash"),
      sym("4", "$", "Key_4", "Key_Dollar"),
      sym("5", "%", "Key_5", "Key_Percent"),
      sym("6", "^", "Key_6", "Key_Circum"),
      sym("7", "&", "Key_7", "Key_Ampersand"),
      sym("8", "*", "Key_8", "Key_Asterisk"),
      sym("9", "(", "Key_9", "Key_ParenLeft"),
      sym("0", ")", "Key_0", "Key_ParenRight"),
      sym("-", "_", "Key_Minus", "Key_UnderScore"),
      sym("=", "+", "Key_Equals", "Key_Plus"),
      mod(2.0, "Backspace"),
  });

  // QWERTY row.
  m_rows.append({
      mod(1.5, "Tab"),
      letter('Q'), letter('W'), letter('E'), letter('R'), letter('T'),
      letter('Y'), letter('U'), letter('I'), letter('O'), letter('P'),
      sym("[", "{", "Key_BracketLeft", "Key_BraceLeft"),
      sym("]", "}", "Key_BracketRight", "Key_BraceRight"),
      sym("\\", "|", "Key_BackSlash", "Key_Bar", 1.5),
  });

  // Home row.
  m_rows.append({
      mod(1.75, "Caps Lock"),
      letter('A'), letter('S'), letter('D'), letter('F'), letter('G'),
      letter('H'), letter('J'), letter('K'), letter('L'),
      sym(";", ":", "Key_Semicolon", "Key_Colon"),
      sym("'", "\"", "Key_Apostrophe", "Key_Quote"),
      mod(2.25, "Enter"),
  });

  // Bottom letter row.
  m_rows.append({
      mod(2.25, "Shift"),
      letter('Z'), letter('X'), letter('C'), letter('V'), letter('B'),
      letter('N'), letter('M'),
      sym(",", "<", "Key_Comma", "Key_Less"),
      sym(".", ">", "Key_Period", "Key_Greater"),
      sym("/", "?", "Key_Slash", "Key_Question"),
      mod(2.75, "Shift"),
  });

  // Modifier row. macOS uses the Mac symbols (⌃ Control, ⌥ Option, ⌘ Command);
  // other platforms use the PC layout.
#if defined(Q_OS_MACOS)
  const QString ctrlSym(QChar(0x2303));
  const QString optSym(QChar(0x2325));
  const QString cmdSym(QChar(0x2318));
  m_rows.append({
      mod(1.25, ctrlSym),
      mod(1.25, optSym),
      mod(1.25, cmdSym),
      mod(6.25, QString()),
      mod(1.25, cmdSym),
      mod(1.25, optSym),
      mod(1.25, ctrlSym),
  });
#else
  m_rows.append({
      mod(1.25, "Ctrl"),
      mod(1.25, "Win"),
      mod(1.25, "Alt"),
      mod(6.25, QString()),
      mod(1.25, "Alt"),
      mod(1.25, "Menu"),
      mod(1.25, "Ctrl"),
  });
#endif

  m_totalUnits = 0.0;
  for (const auto &row : m_rows) {
    double rowUnits = 0.0;
    for (const auto &key : row) {
      rowUnits += key.width;
    }
    if (rowUnits > m_totalUnits) {
      m_totalUnits = rowUnits;
    }
  }
}

void KeyboardWidget::setKeyMap(const QMap<QString, QString> &map) {
  m_map = map;
  update();
}

void KeyboardWidget::setMode(Mode mode) {
  m_mode = mode;
  update();
}

bool KeyboardWidget::hasAltGr() const {
  const QString altSuffix = "_AltGr";
  for (auto it = m_map.constBegin(); it != m_map.constEnd(); ++it) {
    if (!it.key().endsWith(altSuffix)) {
      continue;
    }
    if (it.value().isEmpty()) {
      continue;
    }
    QString normalKey = it.key();
    normalKey.chop(altSuffix.size());
    normalKey += "_Normal";
    if (it.value() != m_map.value(normalKey)) {
      return true;
    }
  }
  return false;
}

QString KeyboardWidget::glyph(const QString &keyName) const {
  if (keyName.isEmpty()) {
    return QString();
  }
  return m_map.value(keyName + (m_mode == Normal ? "_Normal" : "_AltGr"));
}

bool KeyboardWidget::isAltGrTrigger(const QString &label) const {
#if defined(Q_OS_MACOS)
  // macOS: the Option (⌥) key alone acts as AltGr.
  return label == QString(QChar(0x2325));
#else
  // Linux (right Alt / AltGr) and Windows (Ctrl+Alt): the Ctrl and Alt keys
  // together form the AltGr combination.
  return label == "Ctrl" || label == "Alt";
#endif
}

QSize KeyboardWidget::sizeHint() const {
  int w = qRound(m_totalUnits * kBaseUnit + 2 * kBezelPad);
  int h = qRound(kRows * kBaseUnit + 2 * kBezelPad);
  return QSize(w, h);
}

QSize KeyboardWidget::minimumSizeHint() const {
  const double unit = kBaseUnit * 0.6;
  int w = qRound(m_totalUnits * unit + 2 * kBezelPad);
  int h = qRound(kRows * unit + 2 * kBezelPad);
  return QSize(w, h);
}

void KeyboardWidget::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);

  // Bezel fills the whole widget with rounded corners.
  QRectF full(0, 0, width(), height());
  p.setPen(Qt::NoPen);
  p.setBrush(m_bezel);
  p.drawRoundedRect(full, 14, 14);

  // Area available for the keys, and the unit size that fits it while keeping
  // the keyboard's aspect ratio.
  QRectF area = full.adjusted(kBezelPad, kBezelPad, -kBezelPad, -kBezelPad);
  double unit = qMin(area.width() / m_totalUnits, area.height() / kRows);
  double boardW = unit * m_totalUnits;
  double boardH = unit * kRows;
  double originX = area.left() + (area.width() - boardW) / 2.0;
  double originY = area.top() + (area.height() - boardH) / 2.0;

  QFont asciiFont = font();
  asciiFont.setPixelSize(qMax(7, qRound(unit * 0.24)));
  QFont banglaFont(m_banglaFamily);
  banglaFont.setPixelSize(qMax(9, qRound(unit * 0.34)));
  QFont modFont = font();
  modFont.setPixelSize(qMax(7, qRound(unit * 0.22)));

  const double tp = unit * 0.11; // inner text padding

  for (int r = 0; r < m_rows.size(); ++r) {
    const auto &row = m_rows[r];
    double rowUnits = 0.0;
    for (const auto &key : row) {
      rowUnits += key.width;
    }
    // Center rows narrower than the widest row (the modifier row).
    double x = originX + (boardW - rowUnits * unit) / 2.0;
    double y = originY + r * unit;

    for (const auto &key : row) {
      QRectF cell(x, y, key.width * unit, unit);
      QRectF cap = cell.adjusted(kKeyGap, kKeyGap, -kKeyGap, -kKeyGap);
      x += key.width * unit;

      // In AltGr view, highlight the modifier caps that trigger the AltGr layer
      // on this platform. Character keys carry an empty label and never match.
      bool highlight = m_mode == AltGr && isAltGrTrigger(key.label);

      double radius = qMax(3.0, unit * 0.12);
      p.setPen(QPen(m_keyBorder, 1.0));
      p.setBrush(highlight ? m_altGrHighlight : m_keycap);
      p.drawRoundedRect(cap, radius, radius);

      QRectF inner = cap.adjusted(tp, tp, -tp, -tp);

      bool isModifier = key.keyBottom.isEmpty() && key.keyTop.isEmpty();
      if (isModifier) {
        if (!key.label.isEmpty()) {
          p.setPen(highlight ? m_altGrHighlightText : m_modifierText);
          p.setFont(modFont);
          p.drawText(inner, Qt::AlignLeft | Qt::AlignTop, key.label);
        }
        continue;
      }

      // ASCII legends live in a narrow left column; the Bangla glyphs get the
      // full inner height on the right so tall matras/conjuncts aren't clipped.
      QRectF topLeft(inner.left(), inner.top(), inner.width() * 0.45, inner.height() * 0.5);
      QRectF bottomLeft(inner.left(), inner.center().y(), inner.width() * 0.45, inner.height() * 0.5);
      QRectF rightBand(inner.center().x(), inner.top(), inner.width() * 0.5, inner.height());

      // Stacked shift/normal glyphs get a taller band than the padded inner rect
      // so the two layers sit farther apart vertically.
      const double gp = unit * 0.03; // reduced vertical pad for the glyph column
      QRectF glyphBand(inner.center().x(), cap.top() + gp,
                       inner.width() * 0.5, cap.height() - 2 * gp);

      // ASCII legends. Qt::TextDontClip keeps glyphs from being hard-cut.
      p.setFont(asciiFont);
      p.setPen(m_asciiText);
      if (!key.asciiTop.isEmpty()) {
        p.drawText(topLeft, Qt::AlignLeft | Qt::AlignTop | Qt::TextDontClip, key.asciiTop);
      }
      if (!key.asciiBottom.isEmpty()) {
        p.drawText(bottomLeft, Qt::AlignLeft | Qt::AlignBottom | Qt::TextDontClip, key.asciiBottom);
      }

      // Bangla legends — skip ones that merely repeat the ASCII legend.
      p.setFont(banglaFont);
      p.setPen(m_banglaText);
      QString bTop = glyph(key.keyTop);
      QString bBottom = glyph(key.keyBottom);
      bool showTop = !bTop.isEmpty() && bTop != key.asciiTop;
      bool showBottom = !bBottom.isEmpty() && bBottom != key.asciiBottom && bBottom != key.asciiTop;
      if (showTop && showBottom) {
        // Two layers: shift anchored to the top, normal to the bottom.
        p.drawText(glyphBand, Qt::AlignRight | Qt::AlignTop | Qt::TextDontClip, bTop);
        p.drawText(glyphBand, Qt::AlignRight | Qt::AlignBottom | Qt::TextDontClip, bBottom);
      } else if (showTop) {
        p.drawText(rightBand, Qt::AlignRight | Qt::AlignVCenter | Qt::TextDontClip, bTop);
      } else if (showBottom) {
        p.drawText(glyphBand, Qt::AlignRight | Qt::AlignBottom | Qt::TextDontClip, bBottom);
      }
    }
  }
}
