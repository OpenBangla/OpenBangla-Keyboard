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

#ifndef KEYBOARDWIDGET_H
#define KEYBOARDWIDGET_H

#include <QColor>
#include <QMap>
#include <QString>
#include <QVector>
#include <QWidget>

/* A widget that draws a physical keyboard and paints, on each keycap, the
 * Bangla glyph a fixed layout assigns to it. The per-key data comes from a
 * layout's flat map (Key_<name>_Normal / Key_<name>_AltGr -> glyph), see
 * Layout::getLayoutMap(). Colors are driven from the QSS stylesheet through
 * the Q_PROPERTY declarations below. */
class KeyboardWidget : public QWidget {
Q_OBJECT
  Q_PROPERTY(QColor bezel READ bezel WRITE setBezel)
  Q_PROPERTY(QColor keycap READ keycap WRITE setKeycap)
  Q_PROPERTY(QColor keyBorder READ keyBorder WRITE setKeyBorder)
  Q_PROPERTY(QColor asciiText READ asciiText WRITE setAsciiText)
  Q_PROPERTY(QColor banglaText READ banglaText WRITE setBanglaText)
  Q_PROPERTY(QColor modifierText READ modifierText WRITE setModifierText)
  Q_PROPERTY(QColor altGrHighlight READ altGrHighlight WRITE setAltGrHighlight)
  Q_PROPERTY(QColor altGrHighlightText READ altGrHighlightText WRITE setAltGrHighlightText)

public:
  enum Mode { Normal, AltGr };

  explicit KeyboardWidget(QWidget *parent = nullptr);

  /* Feed the widget the currently selected layout's key map. */
  void setKeyMap(const QMap<QString, QString> &map);

  /* Switch between the Normal and AltGr layers. */
  void setMode(Mode mode);

  /* Whether the current map defines any AltGr glyph that differs from its
   * Normal counterpart (used to enable/disable the AltGr view button). */
  bool hasAltGr() const;

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

  QColor bezel() const { return m_bezel; }
  void setBezel(const QColor &c) { m_bezel = c; update(); }
  QColor keycap() const { return m_keycap; }
  void setKeycap(const QColor &c) { m_keycap = c; update(); }
  QColor keyBorder() const { return m_keyBorder; }
  void setKeyBorder(const QColor &c) { m_keyBorder = c; update(); }
  QColor asciiText() const { return m_asciiText; }
  void setAsciiText(const QColor &c) { m_asciiText = c; update(); }
  QColor banglaText() const { return m_banglaText; }
  void setBanglaText(const QColor &c) { m_banglaText = c; update(); }
  QColor modifierText() const { return m_modifierText; }
  void setModifierText(const QColor &c) { m_modifierText = c; update(); }
  QColor altGrHighlight() const { return m_altGrHighlight; }
  void setAltGrHighlight(const QColor &c) { m_altGrHighlight = c; update(); }
  QColor altGrHighlightText() const { return m_altGrHighlightText; }
  void setAltGrHighlightText(const QColor &c) { m_altGrHighlightText = c; update(); }

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  /* A single physical key. Modifier keys (Tab, Shift, ...) carry only `label`
   * and no key-names; character keys carry the ASCII legends plus the riti
   * key-names used to look the Bangla glyph up in `m_map`. */
  struct Key {
    double width;        // width in key units (1.0 == a standard alphanumeric key)
    QString label;       // modifier label; empty for character keys
    QString asciiTop;    // shifted ASCII legend (or the single letter)
    QString asciiBottom; // unshifted ASCII legend
    QString keyTop;      // riti key-name for the shifted glyph (e.g. Key_A)
    QString keyBottom;   // riti key-name for the unshifted glyph (e.g. Key_a)
  };

  void buildRows();
  QString glyph(const QString &keyName) const; // looks up keyName + current suffix
  // Whether a modifier with this label is part of the platform's AltGr trigger.
  bool isAltGrTrigger(const QString &label) const;

  QVector<QVector<Key>> m_rows;
  QMap<QString, QString> m_map;
  Mode m_mode = Normal;
  double m_totalUnits = 0.0; // widest row, in units
  QString m_banglaFamily;    // Kalpurush (bundled) family used for Bangla glyphs

  QColor m_bezel{"#3a3f47"};
  QColor m_keycap{"#f5f6f7"};
  QColor m_keyBorder{"#c9ccd1"};
  QColor m_asciiText{"#7c828c"};
  QColor m_banglaText{"#1b3a8f"};
  QColor m_modifierText{"#5a6069"};
  QColor m_altGrHighlight{"#16a394"};     // accent fill for AltGr trigger caps
  QColor m_altGrHighlightText{"#ffffff"}; // label color on highlighted caps
};

#endif // KEYBOARDWIDGET_H
