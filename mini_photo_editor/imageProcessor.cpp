#include "imageProcessor.h"

QImage ImageProcessor::toGrayscale(const QImage &image) {
    return image.convertToFormat(QImage::Format_Grayscale8);
}

QImage ImageProcessor::invert(const QImage &image) {
    QImage inverted = image;
    inverted.invertPixels();
    return inverted;
}

QImage ImageProcessor::adjustBrightness(const QImage &image, int delta) {
    QImage result = image;
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor c = result.pixelColor(x, y);
            c.setRed(qBound(0, c.red() + delta, 255));
            c.setGreen(qBound(0, c.green() + delta, 255));
            c.setBlue(qBound(0, c.blue() + delta, 255));
            result.setPixelColor(x, y, c);
        }
    }
    return result;
}
