#pragma once
#include <QDialog>
#include <QImage>

class MetadataDialog : public QDialog {
    Q_OBJECT
public:
    explicit MetadataDialog(const QImage &image, QWidget *parent = nullptr);
};
