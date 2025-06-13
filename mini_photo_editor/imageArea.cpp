#include "imageArea.h"
#include <QPainter>

ImageArea::ImageArea(QWidget *parent) : QWidget(parent) {}

void ImageArea::setImage(const QImage &image) {
    currentImage = image;
    if (!currentImage.isNull()) {
        update(); // triggers paintEvent
    }
}


QImage ImageArea::getImage() const {
    return currentImage;
}

void ImageArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    if (!currentImage.isNull()) {
        painter.drawImage(rect(), currentImage);
    }
}
