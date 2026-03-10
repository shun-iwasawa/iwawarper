//---------------------------------------------------
// RenderSettings
// モーフィング、レンダリングの設定
//---------------------------------------------------

#include "rendersettings.h"

#include <QStringList>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

namespace {
QStringList precisionStrList = (QStringList() << "Linear"
                                              << "Low"
                                              << "Medium"
                                              << "High"
                                              << "Very High"
                                              << "Super High");

};

RenderSettings::RenderSettings()
    : m_warpPrecision(4)
    , m_alphaMode(SourceAlpha)
    , m_resampleMode(AreaAverage)
    , m_faceSizeThreshold(25.0)
    , m_imageShrink(1)
    //, m_antialias(true)  // obsoleted
    , m_keepSemiTransparent(true)
    , m_maskWithParentShape(true)
    , m_matteDilate(0) {}

//---------------------------------------------------

void RenderSettings::setWarpPrecision(int prec) {
  if (prec < 0)
    prec = 0;
  else if (prec > 5)
    prec = 5;
  m_warpPrecision = prec;
}

//---------------------------------------------------
// 保存/ロード
void RenderSettings::saveData(QXmlStreamWriter& writer) {
  writer.writeStartElement("WarpOptions");
  // メッシュ再帰分割の回数
  writer.writeTextElement("precision", precisionStrList.at(m_warpPrecision));
  writer.writeTextElement("faceSizeThreshold",
                          QString::number(m_faceSizeThreshold));
  writer.writeTextElement("alphaMode", QString::number((int)m_alphaMode));
  writer.writeTextElement("resampleMode", QString::number((int)m_resampleMode));
  writer.writeTextElement("imageShrink", QString::number((int)m_imageShrink));

  writer.writeTextElement(
      "antialias",
      (m_maskWithParentShape) ? "1" : "0");  // to keep backward compatibility
  writer.writeTextElement("keepSemiTransparent",
                          (m_keepSemiTransparent) ? "1" : "0");
  writer.writeTextElement("maskWithParentShape",
                          (m_maskWithParentShape) ? "1" : "0");

  writer.writeTextElement("matteDilate", QString::number(m_matteDilate));
  writer.writeEndElement();
}

//---------------------------------------------------
// Project file format versions history:
// 0.1.0 : initial version
// 0.2.0 : Changed behavior of SaveRange and InitialFrameNumber in
// OutputSettings
// 0.3.0 : render settings -> antialias is breaked into 2 separate options
void RenderSettings::loadData(QXmlStreamReader& reader,
                              const Version& loadedVersion) {
  while (reader.readNextStartElement()) {
    if (reader.name() == "WarpOptions") {
      while (reader.readNextStartElement()) {
        if (reader.name() == "precision")
          m_warpPrecision = precisionStrList.indexOf(reader.readElementText());
        else if (reader.name() == "faceSizeThreshold")
          m_faceSizeThreshold = reader.readElementText().toDouble();
        else if (reader.name() == "alphaMode")
          m_alphaMode = (AlphaMode)(reader.readElementText().toInt());
        else if (reader.name() == "resampleMode")
          m_resampleMode = (ResampleMode)(reader.readElementText().toInt());
        else if (reader.name() == "imageShrink")
          m_imageShrink = (AlphaMode)(reader.readElementText().toInt());

        else if (reader.name() == "antialias") {
          if (loadedVersion <
              Version(0, 3, 0)) {  // convert conventional parameter
            bool antialias        = (reader.readElementText().toInt() != 0);
            m_keepSemiTransparent = antialias;
            m_maskWithParentShape = antialias;
          } else
            reader.skipCurrentElement();
        } else if (reader.name() == "keepSemiTransparent")
          m_keepSemiTransparent = (reader.readElementText().toInt() != 0);
        else if (reader.name() == "maskWithParentShape")
          m_maskWithParentShape = (reader.readElementText().toInt() != 0);

        else if (reader.name() == "matteDilate")
          m_matteDilate = reader.readElementText().toInt();
        else
          reader.skipCurrentElement();
      }
    } else
      reader.skipCurrentElement();
  }
}