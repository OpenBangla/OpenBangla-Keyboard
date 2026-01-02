/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2016-2020 Muhammad Mominul Huque <mominul2082@gmail.com>
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

#include <QMap>
#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QJsonDocument>
#include <QJsonObject>
#include <zstd.h>
#include "LayoutConverter.h"
#include "FileSystem.h"
#include "base.hpp"

// This map is used for when converting from previous layout format(version 1) or Avro keyboard layout.
QMap<QString, QString> keyConversionMap = {
    {"Key_0_AltGr", "Key_0_AltGr"},
    {"Key_0_Normal", "Key_0_Normal"},
    {"Key_0_Shift", "Key_ParenRight_Normal"},
    {"Key_0_ShiftAltGr", "Key_ParenRight_AltGr"},
    {"Key_1_AltGr", "Key_1_AltGr"},
    {"Key_1_Normal", "Key_1_Normal"},
    {"Key_1_Shift", "Key_Exclaim_Normal"},
    {"Key_1_ShiftAltGr", "Key_Exclaim_AltGr"},
    {"Key_2_AltGr", "Key_2_AltGr"},
    {"Key_2_Normal", "Key_2_Normal"},
    {"Key_2_Shift", "Key_At_Normal"},
    {"Key_2_ShiftAltGr", "Key_At_AltGr"},
    {"Key_3_AltGr", "Key_3_AltGr"},
    {"Key_3_Normal", "Key_3_Normal"},
    {"Key_3_Shift", "Key_Hash_Normal"},
    {"Key_3_ShiftAltGr", "Key_Hash_AltGr"},
    {"Key_4_AltGr", "Key_4_AltGr"},
    {"Key_4_Normal", "Key_4_Normal"},
    {"Key_4_Shift", "Key_Dollar_Normal"},
    {"Key_4_ShiftAltGr", "Key_Dollar_AltGr"},
    {"Key_5_AltGr", "Key_5_AltGr"},
    {"Key_5_Normal", "Key_5_Normal"},
    {"Key_5_Shift", "Key_Percent_Normal"},
    {"Key_5_ShiftAltGr", "Key_Percent_AltGr"},
    {"Key_6_AltGr", "Key_6_AltGr"},
    {"Key_6_Normal", "Key_6_Normal"},
    {"Key_6_Shift", "Key_Circum_Normal"},
    {"Key_6_ShiftAltGr", "Key_Circum_AltGr"},
    {"Key_7_AltGr", "Key_7_AltGr"},
    {"Key_7_Normal", "Key_7_Normal"},
    {"Key_7_Shift", "Key_Ampersand_Normal"},
    {"Key_7_ShiftAltGr", "Key_Ampersand_AltGr"},
    {"Key_8_AltGr", "Key_8_AltGr"},
    {"Key_8_Normal", "Key_8_Normal"},
    {"Key_8_Shift", "Key_Asterisk_Normal"},
    {"Key_8_ShiftAltGr", "Key_Asterisk_AltGr"},
    {"Key_9_AltGr", "Key_9_AltGr"},
    {"Key_9_Normal", "Key_9_Normal"},
    {"Key_9_Shift", "Key_ParenLeft_Normal"},
    {"Key_9_ShiftAltGr", "Key_ParenLeft_AltGr"},
    {"Key_A_AltGr", "Key_a_AltGr"},
    {"Key_A_Normal", "Key_a_Normal"},
    {"Key_A_Shift", "Key_A_Normal"},
    {"Key_A_ShiftAltGr", "Key_A_AltGr"},
    {"Key_B_AltGr", "Key_b_AltGr"},
    {"Key_B_Normal", "Key_b_Normal"},
    {"Key_B_Shift", "Key_B_Normal"},
    {"Key_B_ShiftAltGr", "Key_B_AltGr"},
    {"Key_BackQuote_AltGr", "Key_Grave_AltGr"},
    {"Key_BackQuote_Normal", "Key_Grave_Normal"},
    {"Key_BackQuote_Shift", "Key_Tilde_Normal"},
    {"Key_BackQuote_ShiftAltGr", "Key_Tilde_AltGr"},
    {"Key_BackSlash_AltGr", "Key_BackSlash_AltGr"},
    {"Key_BackSlash_Normal", "Key_BackSlash_Normal"},
    {"Key_BackSlash_Shift", "Key_Bar_Normal"},
    {"Key_BackSlash_ShiftAltGr", "Key_Bar_AltGr"},
    {"Key_C_AltGr", "Key_c_AltGr"},
    {"Key_C_Normal", "Key_c_Normal"},
    {"Key_C_Shift", "Key_C_Normal"},
    {"Key_C_ShiftAltGr", "Key_C_AltGr"},
    {"Key_CloseBracket_AltGr", "Key_BracketRight_AltGr"},
    {"Key_CloseBracket_Normal", "Key_BracketRight_Normal"},
    {"Key_CloseBracket_Shift", "Key_BraceRight_Normal"},
    {"Key_CloseBracket_ShiftAltGr", "Key_BraceRight_AltGr"},
    {"Key_Comma_AltGr", "Key_Comma_AltGr"},
    {"Key_Comma_Normal", "Key_Comma_Normal"},
    {"Key_Comma_Shift", "Key_Less_Normal"},
    {"Key_Comma_ShiftAltGr", "Key_Less_AltGr"},
    {"Key_D_AltGr", "Key_d_AltGr"},
    {"Key_D_Normal", "Key_d_Normal"},
    {"Key_D_Shift", "Key_D_Normal"},
    {"Key_D_ShiftAltGr", "Key_D_AltGr"},
    {"Key_E_AltGr", "Key_e_AltGr"},
    {"Key_E_Normal", "Key_e_Normal"},
    {"Key_E_Shift", "Key_E_Normal"},
    {"Key_E_ShiftAltGr", "Key_E_AltGr"},
    {"Key_Equals_AltGr", "Key_Equals_AltGr"},
    {"Key_Equals_Normal", "Key_Equals_Normal"},
    {"Key_Equals_Shift", "Key_Plus_Normal"},
    {"Key_Equals_ShiftAltGr", "Key_Plus_AltGr"},
    {"Key_F_AltGr", "Key_f_AltGr"},
    {"Key_F_Normal", "Key_f_Normal"},
    {"Key_F_Shift", "Key_F_Normal"},
    {"Key_F_ShiftAltGr", "Key_F_AltGr"},
    {"Key_G_AltGr", "Key_g_AltGr"},
    {"Key_G_Normal", "Key_g_Normal"},
    {"Key_G_Shift", "Key_G_Normal"},
    {"Key_G_ShiftAltGr", "Key_G_AltGr"},
    {"Key_H_AltGr", "Key_h_AltGr"},
    {"Key_H_Normal", "Key_h_Normal"},
    {"Key_H_Shift", "Key_H_Normal"},
    {"Key_H_ShiftAltGr", "Key_H_AltGr"},
    {"Key_I_AltGr", "Key_i_AltGr"},
    {"Key_I_Normal", "Key_i_Normal"},
    {"Key_I_Shift", "Key_I_Normal"},
    {"Key_I_ShiftAltGr", "Key_I_AltGr"},
    {"Key_J_AltGr", "Key_j_AltGr"},
    {"Key_J_Normal", "Key_j_Normal"},
    {"Key_J_Shift", "Key_J_Normal"},
    {"Key_J_ShiftAltGr", "Key_J_AltGr"},
    {"Key_K_AltGr", "Key_k_AltGr"},
    {"Key_K_Normal", "Key_k_Normal"},
    {"Key_K_Shift", "Key_K_Normal"},
    {"Key_K_ShiftAltGr", "Key_K_AltGr"},
    {"Key_L_AltGr", "Key_l_AltGr"},
    {"Key_L_Normal", "Key_l_Normal"},
    {"Key_L_Shift", "Key_L_Normal"},
    {"Key_L_ShiftAltGr", "Key_L_AltGr"},
    {"Key_M_AltGr", "Key_m_AltGr"},
    {"Key_M_Normal", "Key_m_Normal"},
    {"Key_M_Shift", "Key_M_Normal"},
    {"Key_M_ShiftAltGr", "Key_M_AltGr"},
    {"Key_Minus_AltGr", "Key_Minus_AltGr"},
    {"Key_Minus_Normal", "Key_Minus_Normal"},
    {"Key_Minus_Shift", "Key_UnderScore_Normal"},
    {"Key_Minus_ShiftAltGr", "Key_UnderScore_AltGr"},
    {"Key_N_AltGr", "Key_n_AltGr"},
    {"Key_N_Normal", "Key_n_Normal"},
    {"Key_N_Shift", "Key_N_Normal"},
    {"Key_N_ShiftAltGr", "Key_N_AltGr"},
    {"Key_O_AltGr", "Key_o_AltGr"},
    {"Key_O_Normal", "Key_o_Normal"},
    {"Key_O_Shift", "Key_O_Normal"},
    {"Key_O_ShiftAltGr", "Key_O_AltGr"},
    {"Key_OpenBracket_AltGr", "Key_BracketLeft_AltGr"},
    {"Key_OpenBracket_Normal", "Key_BracketLeft_Normal"},
    {"Key_OpenBracket_Shift", "Key_BraceLeft_Normal"},
    {"Key_OpenBracket_ShiftAltGr", "Key_BraceLeft_AltGr"},
    {"Key_P_AltGr", "Key_p_AltGr"},
    {"Key_P_Normal", "Key_p_Normal"},
    {"Key_P_Shift", "Key_P_Normal"},
    {"Key_P_ShiftAltGr", "Key_P_AltGr"},
    {"Key_Period_AltGr", "Key_Period_AltGr"},
    {"Key_Period_Normal", "Key_Period_Normal"},
    {"Key_Period_Shift", "Key_Greater_Normal"},
    {"Key_Period_ShiftAltGr", "Key_Greater_AltGr"},
    {"Key_Q_AltGr", "Key_q_AltGr"},
    {"Key_Q_Normal", "Key_q_Normal"},
    {"Key_Q_Shift", "Key_Q_Normal"},
    {"Key_Q_ShiftAltGr", "Key_Q_AltGr"},
    {"Key_Quote_AltGr", "Key_Apostrophe_AltGr"},
    {"Key_Quote_Normal", "Key_Apostrophe_Normal"},
    {"Key_Quote_Shift", "Key_Quote_Normal"},
    {"Key_Quote_ShiftAltGr", "Key_Quote_AltGr"},
    {"Key_R_AltGr", "Key_r_AltGr"},
    {"Key_R_Normal", "Key_r_Normal"},
    {"Key_R_Shift", "Key_R_Normal"},
    {"Key_R_ShiftAltGr", "Key_R_AltGr"},
    {"Key_S_AltGr", "Key_s_AltGr"},
    {"Key_S_Normal", "Key_s_Normal"},
    {"Key_S_Shift", "Key_S_Normal"},
    {"Key_S_ShiftAltGr", "Key_S_AltGr"},
    {"Key_Semicolon_AltGr", "Key_Semicolon_AltGr"},
    {"Key_Semicolon_Normal", "Key_Semicolon_Normal"},
    {"Key_Semicolon_Shift", "Key_Colon_Normal"},
    {"Key_Semicolon_ShiftAltGr", "Key_Colon_AltGr"},
    {"Key_Slash_AltGr", "Key_Slash_AltGr"},
    {"Key_Slash_Normal", "Key_Slash_Normal"},
    {"Key_Slash_Shift", "Key_Question_Normal"},
    {"Key_Slash_ShiftAltGr", "Key_Question_AltGr"},
    {"Key_T_AltGr", "Key_t_AltGr"},
    {"Key_T_Normal", "Key_t_Normal"},
    {"Key_T_Shift", "Key_T_Normal"},
    {"Key_T_ShiftAltGr", "Key_T_AltGr"},
    {"Key_U_AltGr", "Key_u_AltGr"},
    {"Key_U_Normal", "Key_u_Normal"},
    {"Key_U_Shift", "Key_U_Normal"},
    {"Key_U_ShiftAltGr", "Key_U_AltGr"},
    {"Key_V_AltGr", "Key_v_AltGr"},
    {"Key_V_Normal", "Key_v_Normal"},
    {"Key_V_Shift", "Key_V_Normal"},
    {"Key_V_ShiftAltGr", "Key_V_AltGr"},
    {"Key_W_AltGr", "Key_w_AltGr"},
    {"Key_W_Normal", "Key_w_Normal"},
    {"Key_W_Shift", "Key_W_Normal"},
    {"Key_W_ShiftAltGr", "Key_W_AltGr"},
    {"Key_X_AltGr", "Key_x_AltGr"},
    {"Key_X_Normal", "Key_x_Normal"},
    {"Key_X_Shift", "Key_X_Normal"},
    {"Key_X_ShiftAltGr", "Key_X_AltGr"},
    {"Key_Y_AltGr", "Key_y_AltGr"},
    {"Key_Y_Normal", "Key_y_Normal"},
    {"Key_Y_Shift", "Key_Y_Normal"},
    {"Key_Y_ShiftAltGr", "Key_Y_AltGr"},
    {"Key_Z_AltGr", "Key_z_AltGr"},
    {"Key_Z_Normal", "Key_z_Normal"},
    {"Key_Z_Shift", "Key_Z_Normal"},
    {"Key_Z_ShiftAltGr", "Key_Z_AltGr"},
    {"Num0", "Num0"},
    {"Num1", "Num1"},
    {"Num2", "Num2"},
    {"Num3", "Num3"},
    {"Num4", "Num4"},
    {"Num5", "Num5"},
    {"Num6", "Num6"},
    {"Num7", "Num7"},
    {"Num8", "Num8"},
    {"Num9", "Num9"},
    {"NumAdd", "NumAdd"},
    {"NumDecimal", "NumDecimal"},
    {"NumDivide", "NumDivide"},
    {"NumMultiply", "NumMultiply"},
    {"NumSubtract", "NumSubtract"},
    /// Fix the previous layout format bug. Issue #98
    {"Key_BackBracket_AltGr", "Key_BackSlash_AltGr"},
    {"Key_BackBracket_Normal", "Key_BackSlash_Normal"},
    {"Key_BackBracket_Shift", "Key_Bar_Normal"},
    {"Key_BackBracket_ShiftAltGr", "Key_Bar_AltGr"}
};

/** Convert Layout format from version 1 into version 2 **/
ConversionResult LayoutConverter::convertLayoutFormat(QString path) {
  QFile oldFile(path);
  if (!oldFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    return OpenError;
  }

  QJsonObject oldLayout = QJsonDocument::fromJson(oldFile.readAll()).object();

  // Key conversion
  QJsonObject newKeys;
  QJsonObject oldKeys = oldLayout.value("layout").toObject();
  QJsonObject::const_iterator iter = oldKeys.constBegin();
  while (iter != oldKeys.constEnd())
  {
    newKeys.insert(keyConversionMap[iter.key()], iter.value());

     ++iter;
  }

  // Image conversion
  QJsonObject infoLayout;
  QJsonValue image0 = oldLayout.value("info").toObject().value("layout").toObject().value("image0");
  QJsonValue image1 = oldLayout.value("info").toObject().value("layout").toObject().value("image1");

  // Image 0 or Normal View
  if(!image0.isUndefined()) {
    QString data = image0.toString();
    infoLayout["image0"] = decodeCompressAndEncode(data);
  }

  // Image 1 or AltGr View
  if(!image1.isUndefined()) {
    QString data = image1.toString();
    infoLayout["image1"] = decodeCompressAndEncode(data);
  }

  QJsonObject layout, info;

  infoLayout["developer"] = oldLayout.value("info").toObject().value("layout").toObject().value("developer").toObject();
  infoLayout["name"] = oldLayout.value("info").toObject().value("layout").toObject().value("name");
  infoLayout["version"] = oldLayout.value("info").toObject().value("layout").toObject().value("version");

  info["layout"] = infoLayout;
  info["type"] = "fixed";
  info["version"] = "2";
    
  layout["info"] = info;
  layout["layout"] = newKeys;

  oldFile.close();

  QFileInfo fileInfo(oldFile);
  QString savePath = gUserFolders->getUserLayoutPath() + fileInfo.baseName() + ".json";
  return saveLayout(layout, savePath);
}

QString LayoutConverter::unescapeXML(QString escaped) {
  escaped.replace("&amp;", "&");
  escaped.replace("&lt;", "<");
  escaped.replace("&gt;", ">");
  escaped.replace("&apos;", "'");
  escaped.replace("&quot;", "\"");
  return escaped;
}

/** Convert Avro layout format into OBK's layout format. **/
ConversionResult LayoutConverter::convertAvroLayout(QString path) {
  QFile xmlFile(path);
  if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return OpenError;
  }
  QByteArray xmlData = xmlFile.readAll();
  QXmlStreamReader *xmlReader = new QXmlStreamReader(xmlData);
  QJsonObject layoutDev, infoLayout, info, keys;

  while (!xmlReader->atEnd() && !xmlReader->hasError()) {
    xmlReader->readNext();
    if (xmlReader->isStartDocument()) {
      continue;
    }

    if (xmlReader->isStartElement()) {
      QStringRef name = xmlReader->name();
      if (name == "Layout" || name == "KeyData") {
        continue;
      }

      QString data = xmlReader->readElementText().trimmed();

      // Check layout version, we only support Avro Keyboard 5 layout.
      if (name == "AvroKeyboardVersion") {
        if (data != "5") {
          xmlReader->clear();
          xmlFile.close();
          delete xmlReader;
          return UnsupportedLayout;
        }
      }

      if (!name.contains("Key") && !name.contains("Num")) {
        // Meta data conversion
        if (name == "LayoutName") {
          infoLayout["name"] = data;
        } else if (name == "LayoutVersion") {
          infoLayout["version"] = data;
        } else if (name == "DeveloperName") {
          layoutDev["name"] = data;
        } else if (name == "DeveloperComment") {
          layoutDev["comment"] = unescapeXML(data);
        } else if (name == "ImageNormalShift") {
          infoLayout["image0"] = decodeCompressAndEncode(data.simplified().replace(" ", ""));
        } else if (name == "ImageAltGrShift") {
          infoLayout["image1"] = decodeCompressAndEncode(data.simplified().replace(" ", ""));
        }
      } else {
        // Key conversion
        QString key = name.toString();
        if (key.contains("OEM1"))
          key.replace("OEM1", "Semicolon");
        if (key.contains("OEM2"))
          key.replace("OEM2", "Slash");
        if (key.contains("OEM3"))
          key.replace("OEM3", "BackQuote");
        if (key.contains("OEM4"))
          key.replace("OEM4", "OpenBracket");
        if (key.contains("OEM5"))
          key.replace("OEM5", "BackSlash");
        if (key.contains("OEM6"))
          key.replace("OEM6", "CloseBracket");
        if (key.contains("OEM7"))
          key.replace("OEM7", "Quote");
        if (key.contains("MINUS"))
          key.replace("MINUS", "Minus");
        if (key.contains("PLUS"))
          key.replace("PLUS", "Equals");
        if (key.contains("PERIOD"))
          key.replace("PERIOD", "Period");
        if (key.contains("COMMA"))
          key.replace("COMMA", "Comma");

        keys[keyConversionMap[key]] = unescapeXML(data);
      }
    }
  }

  info["type"] = "fixed";
  info["version"] = "2";
  infoLayout["developer"] = layoutDev;
  info["layout"] = infoLayout;

  QJsonObject layout;
  layout["info"] = info;
  layout["layout"] = keys;

  xmlReader->clear();
  xmlFile.close();
  delete xmlReader;

  QFileInfo fileInfo(xmlFile);
  QString savePath = gUserFolders->getUserLayoutPath() + fileInfo.baseName() + ".json";
  return saveLayout(layout, savePath);
}

ConversionResult LayoutConverter::saveLayout(QString path) {
  QFile jsonFile(path);
  if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return OpenError;
  }
  QString version = QJsonDocument::fromJson(jsonFile.readAll()).object()
                    .value("info").toObject()
                    .value("version").toString();
  if (version == "1") return convertLayoutFormat(path);
  if (version != "2") return UnsupportedLayout;

  QFileInfo fileInfo(jsonFile);
  QString savePath = gUserFolders->getUserLayoutPath() + fileInfo.fileName();
  return jsonFile.copy(savePath) ? Ok : SaveError;
}

ConversionResult LayoutConverter::saveLayout(QJsonObject obj, QString path) {
  QFile saveFile(path);
  if (!saveFile.open(QIODevice::WriteOnly)) {
    return SaveError;
  }

  QJsonDocument json(obj);
  saveFile.write(json.toJson());
  saveFile.close();

  return Ok;
}

/** 
 * Decodes `data` using the Base64 algorithm, compresses it using zstd algorithm
 * and encodes the compressed bytes using a custom base91 algorithm.
 **/
QString LayoutConverter::decodeCompressAndEncode(QString &data) {
    QByteArray image = QByteArray::fromBase64(data.toUtf8());
    size_t cap = ZSTD_compressBound(image.size());
    char *dst = (char *)malloc(cap);

    size_t ret = ZSTD_compress(dst, cap, image.data(), image.size(), 20);

    std::string imgCompressed = std::string(dst, ret);
    QString imgEncoded = QString::fromStdString(base91::encode(imgCompressed));
    free(dst);

    return imgEncoded;
}
