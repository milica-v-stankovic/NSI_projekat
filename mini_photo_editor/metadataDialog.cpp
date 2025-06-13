#include "metadataDialog.h"
#include <QVBoxLayout>
#include <QLabel>

MetadataDialog::MetadataDialog(const QImage &image, QWidget *parent)
    : QDialog(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    QString info = QString("Size: %1x%2\nDepth: %3")
                       .arg(image.width())
                       .arg(image.height())
                       .arg(image.depth());

    layout->addWidget(new QLabel(info));
    setLayout(layout);
    setWindowTitle("Image Metadata");
}
