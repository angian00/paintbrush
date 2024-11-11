#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QColor>

constexpr QColor defaultDrawColor = QColorConstants::Black;
//constexpr QColor bkgColor = { QColorConstants::White };
constexpr QColor bkgColor = QColorConstants::Transparent;
constexpr QColor cursorColor = QColorConstants::Black;

constexpr int defaultDrawWidth = 5;
constexpr int maxDrawWidth = 20;

constexpr int toolThumbnailSize = 32;

constexpr int bkgPatternSize = 8; //in px
constexpr QColor bkgPatternColor1 = QColorConstants::LightGray;
constexpr QColor bkgPatternColor2 = QColorConstants::DarkGray;

constexpr double maxZoomLevel = 10.0;
constexpr double minZoomLevel = 0.01;

constexpr int defaultScrollAmount = 20; //in pixels
constexpr int defaultZoomFactor = 2; //multiplicative factor

#endif // CONSTANTS_H
