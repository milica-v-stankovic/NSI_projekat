#include "mainwindow.h"
#include "imageProcessor.h"
#include "cropDialog.h"
#include "resizeDialog.h"
#include "metadataDialog.h"

#include <QFileDialog>
#include <QScrollBar>
#include <QMessageBox>
#include <QInputDialog>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QShortcut>
#include <QTransform>
#include <QMenuBar>
#include <QMenu>
#include <QAction>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Existing UI setup code here...

    // Create the menus
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    // File menu actions
    QAction *loadAction = new QAction(tr("&Load"), this);
    QAction *saveAction = new QAction(tr("&Save"), this);
    QAction *saveAsAction = new QAction(tr("Save &As"), this);
    QAction *exitAction = new QAction(tr("E&xit"), this);

    imageArea = new ImageArea(this);
    setCentralWidget(imageArea); // or add to layout


    fileMenu->addAction(loadAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    // Edit menu actions
    QAction *resizeAction = new QAction(tr("&Resize"), this);
    QAction *cropAction = new QAction(tr("&Crop"), this);
    QAction *rotateAction = new QAction(tr("&Rotate 90 degrees"), this);
    QAction *rotateCustomAction = new QAction(tr("Rotate custom angle"), this);
    QAction *undoAction = new QAction(tr("&Undo"), this);
    QAction *redoAction = new QAction(tr("&Redo"), this);

    editMenu->addAction(resizeAction);
    editMenu->addAction(cropAction);
    editMenu->addAction(rotateAction);
    editMenu->addAction(rotateCustomAction);
    editMenu->addSeparator();
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);

    QAction *grayscaleAction = new QAction("Grayscale", this);
    grayscaleAction->setShortcut(QKeySequence("Ctrl+G"));
    connect(grayscaleAction, &QAction::triggered, this, &MainWindow::applyGrayscale);

    QMenu *filterMenu = menuBar()->addMenu("Filters");
    filterMenu->addAction(grayscaleAction);

    QAction *invertAction = new QAction("Invert Colors", this);
    invertAction->setShortcut(QKeySequence("Ctrl+I"));
    connect(invertAction, &QAction::triggered, this, &MainWindow::applyInvert);
    filterMenu->addAction(invertAction);

    QAction *brightnessAction = new QAction("Adjust Brightness", this);
    brightnessAction->setShortcut(QKeySequence("Ctrl+B"));
    connect(brightnessAction, &QAction::triggered, this, &MainWindow::adjustBrightness);
    filterMenu->addAction(brightnessAction);


    // Connect actions to slots (implement these slots in MainWindow)
    connect(loadAction, &QAction::triggered, this, &MainWindow::openImage);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveImage);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveImageAs);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    connect(resizeAction, &QAction::triggered, this, &MainWindow::resizeImage);
    connect(cropAction, &QAction::triggered, this, &MainWindow::cropImage);
    connect(rotateAction, &QAction::triggered, this, &MainWindow::rotate90);
    connect(rotateCustomAction, &QAction::triggered, this, &MainWindow::rotateCustom);
    connect(undoAction, &QAction::triggered, this, &MainWindow::undo);
    connect(redoAction, &QAction::triggered, this, &MainWindow::redo);

    resize(1200, 800);
}

void MainWindow::setupShortcuts() {
    new QShortcut(QKeySequence("Ctrl+O"), this, SLOT(openImage()));
    new QShortcut(QKeySequence("Ctrl+S"), this, SLOT(saveImage()));
    new QShortcut(QKeySequence("Ctrl+Shift+S"), this, SLOT(saveImageAs()));
    new QShortcut(QKeySequence("Ctrl+Z"), this, SLOT(undo()));
    new QShortcut(QKeySequence("Ctrl+Y"), this, SLOT(redo()));
    new QShortcut(QKeySequence("Ctrl++"), this, SLOT(zoomIn()));
    new QShortcut(QKeySequence("Ctrl+-"), this, SLOT(zoomOut()));
    new QShortcut(QKeySequence("Ctrl+F"), this, SLOT(fitToWindow()));
}

void MainWindow::openImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", QString(),
                                                    "Images (*.png *.jpg *.bmp *.jpeg *.gif *.tiff)");

    if (fileName.isEmpty()) return;

    QImage loaded(fileName);
    if (loaded.isNull()) {
        QMessageBox::warning(this, "Open Image", "Failed to load image.");
        return;
    }
    updateImage(loaded);
    updateWindowTitle();
}

void MainWindow::saveImage() {
    if (currentImage.isNull()) return;
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", QString(),
                                                    "PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp)");
    if (fileName.isEmpty()) return;

    if (!currentImage.save(fileName)) {
        QMessageBox::warning(this, "Save Image", "Failed to save image.");
    }
}

void MainWindow::saveImageAs() {
    saveImage();
}

void MainWindow::pushUndo(const QImage &image) {
    undoStack.push(image);
    redoStack.clear();
}

void MainWindow::updateImage(const QImage &image) {
    if (!currentImage.isNull())
        pushUndo(currentImage);

    currentImage = image;
    imageArea->setImage(currentImage);
    scaleFactor = 1.0;
    imageArea->resize(currentImage.size());
    updateWindowTitle();
}

void MainWindow::undo() {
    if (undoStack.isEmpty()) return;
    redoStack.push(currentImage);
    currentImage = undoStack.pop();
    imageArea->setImage(currentImage);
    imageArea->resize(currentImage.size());
    updateWindowTitle();
}

void MainWindow::redo() {
    if (redoStack.isEmpty()) return;
    undoStack.push(currentImage);
    currentImage = redoStack.pop();
    imageArea->setImage(currentImage);
    imageArea->resize(currentImage.size());
    updateWindowTitle();
}

void MainWindow::cropImage() {
    if (currentImage.isNull()) return;
    CropDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        QRect r = dlg.getCropRect();
        if (r.isValid() && r.intersects(currentImage.rect())) {
            QImage cropped = currentImage.copy(r.intersected(currentImage.rect()));
            updateImage(cropped);
        }
    }
}

void MainWindow::resizeImage() {
    if (currentImage.isNull()) return;

    ResizeDialog dlg(currentImage.width(), currentImage.height(), this);
    if (dlg.exec() == QDialog::Accepted) {
        QImage resized = currentImage.scaled(dlg.getNewWidth(), dlg.getNewHeight(),
                                             Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        updateImage(resized);
    }
}

void MainWindow::rotate90() {
    if (currentImage.isNull()) return;

    QTransform transform;
    transform.rotate(90);
    QImage rotated = currentImage.transformed(transform);
    updateImage(rotated);
}

void MainWindow::rotateCustom() {
    if (currentImage.isNull()) return;

    bool ok;
    double angle = QInputDialog::getDouble(this, "Rotate", "Enter angle (degrees):", 0, -360, 360, 1, &ok);
    if (ok) {
        QTransform transform;
        transform.rotate(angle);
        QImage rotated = currentImage.transformed(transform, Qt::SmoothTransformation);
        updateImage(rotated);
    }
}

void MainWindow::applyGrayscale() {
    if (currentImage.isNull()) return;
    QImage gray = ImageProcessor::toGrayscale(currentImage);
    updateImage(gray);
}

void MainWindow::applyInvert() {
    if (currentImage.isNull()) return;
    QImage inv = ImageProcessor::invert(currentImage);
    updateImage(inv);
}

void MainWindow::adjustBrightness() {
    if (currentImage.isNull()) return;

    bool ok;
    int delta = QInputDialog::getInt(this, "Brightness", "Brightness adjustment (-255 to 255):", 0, -255, 255, 1, &ok);
    if (ok) {
        QImage bright = ImageProcessor::adjustBrightness(currentImage, delta);
        updateImage(bright);
    }
}

void MainWindow::zoomIn() {
    scaleFactor *= 1.25;
    imageArea->resize(scaleFactor * currentImage.size());
}

void MainWindow::zoomOut() {
    scaleFactor /= 1.25;
    imageArea->resize(scaleFactor * currentImage.size());
}

void MainWindow::fitToWindow() {
    QSize areaSize = scrollArea->size();
    QSize imgSize = currentImage.size();
    double factorW = double(areaSize.width()) / imgSize.width();
    double factorH = double(areaSize.height()) / imgSize.height();
    scaleFactor = qMin(factorW, factorH);

    imageArea->resize(scaleFactor * imgSize);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    auto urls = event->mimeData()->urls();
    if (!urls.isEmpty()) {
        QString filePath = urls.first().toLocalFile();
        QImage loaded(filePath);
        if (!loaded.isNull()) {
            updateImage(loaded);
        }
    }
}

void MainWindow::showMetadata() {
    if (currentImage.isNull()) return;
    MetadataDialog dlg(currentImage, this);
    dlg.exec();
}

void MainWindow::updateWindowTitle() {
    QString title = currentImage.isNull() ? "Untitled - Qt Photo Editor" : "Qt Photo Editor";
    setWindowTitle(title);
}
