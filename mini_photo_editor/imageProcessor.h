#pragma once
#include <QImage>

namespace ImageProcessor {
QImage toGrayscale(const QImage &image);
QImage invert(const QImage &image);
QImage adjustBrightness(const QImage &image, int delta);
}
