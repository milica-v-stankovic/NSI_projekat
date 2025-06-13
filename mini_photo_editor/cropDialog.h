#pragma once
#include <QDialog>
#include <QRect>

class CropDialog : public QDialog {
    Q_OBJECT
public:
    explicit CropDialog(QWidget *parent = nullptr);
    QRect getCropRect() const;

private:
    QRect cropRect;
};
