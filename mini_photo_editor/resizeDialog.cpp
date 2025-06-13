#include "resizeDialog.h"
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QPushButton>

ResizeDialog::ResizeDialog(int currentWidth, int currentHeight, QWidget *parent)
    : QDialog(parent),
    aspectRatio(double(currentWidth) / currentHeight)
{
    widthLineEdit = new QLineEdit(QString::number(currentWidth), this);
    heightLineEdit = new QLineEdit(QString::number(currentHeight), this);
    aspectRatioCheckBox = new QCheckBox("Lock Aspect Ratio", this);
    aspectRatioCheckBox->setChecked(true);

    widthLineEdit->setValidator(new QIntValidator(1, 10000, this));
    heightLineEdit->setValidator(new QIntValidator(1, 10000, this));

    // Layout
    auto layout = new QVBoxLayout(this);
    auto wLayout = new QHBoxLayout();
    wLayout->addWidget(new QLabel("Width:", this));
    wLayout->addWidget(widthLineEdit);
    layout->addLayout(wLayout);

    auto hLayout = new QHBoxLayout();
    hLayout->addWidget(new QLabel("Height:", this));
    hLayout->addWidget(heightLineEdit);
    layout->addLayout(hLayout);

    layout->addWidget(aspectRatioCheckBox);

    // Buttons
    auto buttons = new QHBoxLayout();
    QPushButton *okBtn = new QPushButton("OK", this);
    QPushButton *cancelBtn = new QPushButton("Cancel", this);
    buttons->addWidget(okBtn);
    buttons->addWidget(cancelBtn);
    layout->addLayout(buttons);

    connect(okBtn, &QPushButton::clicked, this, &ResizeDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &ResizeDialog::reject);

    connect(widthLineEdit, &QLineEdit::textChanged, this, &ResizeDialog::onWidthChanged);
    connect(heightLineEdit, &QLineEdit::textChanged, this, &ResizeDialog::onHeightChanged);
}

void ResizeDialog::onWidthChanged(const QString &text) {
    if (isAdjusting) return;
    if (!aspectRatioCheckBox->isChecked()) return;

    bool ok;
    int width = text.toInt(&ok);
    if (!ok || width <= 0) return;

    isAdjusting = true;
    int newHeight = static_cast<int>(width / aspectRatio);
    heightLineEdit->setText(QString::number(newHeight));
    isAdjusting = false;
}

void ResizeDialog::onHeightChanged(const QString &text) {
    if (isAdjusting) return;
    if (!aspectRatioCheckBox->isChecked()) return;

    bool ok;
    int height = text.toInt(&ok);
    if (!ok || height <= 0) return;

    isAdjusting = true;
    int newWidth = static_cast<int>(height * aspectRatio);
    widthLineEdit->setText(QString::number(newWidth));
    isAdjusting = false;
}

int ResizeDialog::getNewWidth() const {
    return widthLineEdit->text().toInt();
}

int ResizeDialog::getNewHeight() const {
    return heightLineEdit->text().toInt();
}
