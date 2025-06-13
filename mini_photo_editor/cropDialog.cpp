#include "cropDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QIntValidator>

CropDialog::CropDialog(QWidget *parent) : QDialog(parent), cropRect(0,0,0,0) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Enter crop rectangle (x, y, width, height):"));

    QLineEdit *xEdit = new QLineEdit(this);
    QLineEdit *yEdit = new QLineEdit(this);
    QLineEdit *wEdit = new QLineEdit(this);
    QLineEdit *hEdit = new QLineEdit(this);

    xEdit->setValidator(new QIntValidator(0, 10000, this));
    yEdit->setValidator(new QIntValidator(0, 10000, this));
    wEdit->setValidator(new QIntValidator(1, 10000, this));
    hEdit->setValidator(new QIntValidator(1, 10000, this));

    layout->addWidget(xEdit);
    layout->addWidget(yEdit);
    layout->addWidget(wEdit);
    layout->addWidget(hEdit);

    QPushButton *okBtn = new QPushButton("Crop", this);
    QPushButton *cancelBtn = new QPushButton("Cancel", this);
    layout->addWidget(okBtn);
    layout->addWidget(cancelBtn);

    connect(okBtn, &QPushButton::clicked, this, [=]() {
        cropRect.setX(xEdit->text().toInt());
        cropRect.setY(yEdit->text().toInt());
        cropRect.setWidth(wEdit->text().toInt());
        cropRect.setHeight(hEdit->text().toInt());
        accept();
    });
    connect(cancelBtn, &QPushButton::clicked, this, &CropDialog::reject);
}

QRect CropDialog::getCropRect() const {
    return cropRect;
}
