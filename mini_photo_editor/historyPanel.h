#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QImage>

class HistoryPanel : public QWidget {
    Q_OBJECT
public:
    explicit HistoryPanel(QWidget *parent = nullptr);
    void addThumbnail(const QImage &image);

private:
    QVBoxLayout *layout;
};
