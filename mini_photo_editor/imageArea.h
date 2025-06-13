#pragma once
#include <QWidget>
#include <QImage>

class ImageArea : public QWidget {
    Q_OBJECT
public:
    explicit ImageArea(QWidget *parent = nullptr);
    void setImage(const QImage &image);
    QImage getImage() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage currentImage;
};
