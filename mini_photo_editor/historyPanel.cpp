#include "historyPanel.h"
#include <QLabel>
#include <QPixmap>

HistoryPanel::HistoryPanel(QWidget *parent) : QWidget(parent), layout(new QVBoxLayout) {
    setLayout(layout);
}

void HistoryPanel::addThumbnail(const QImage &image) {
    QLabel *thumb = new QLabel;
    thumb->setPixmap(QPixmap::fromImage(image.scaled(64, 64, Qt::KeepAspectRatio)));
    layout->addWidget(thumb);
}
