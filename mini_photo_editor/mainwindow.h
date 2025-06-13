#pragma once
#include <QMainWindow>
#include <QStack>
#include <QImage>
#include <QScrollArea>
#include <QLabel>
#include "imageArea.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void openImage();
    void saveImage();
    void saveImageAs();

    void undo();
    void redo();

    void cropImage();
    void resizeImage();
    void rotate90();
    void rotateCustom();

    void applyGrayscale();
    void applyInvert();
    void adjustBrightness();

    void zoomIn();
    void zoomOut();
    void fitToWindow();

    void showMetadata();

private:
    ImageArea *imageArea;
    QScrollArea *scrollArea;

    QImage currentImage;
    QStack<QImage> undoStack;
    QStack<QImage> redoStack;

    double scaleFactor = 1.0;

    void pushUndo(const QImage &image);
    void updateImage(const QImage &image);

    void setupShortcuts();
    void updateWindowTitle();
};
