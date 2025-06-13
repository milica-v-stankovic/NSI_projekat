# Jednostavni editor slika u Qt-u
Ovaj projekat predstavlja primer jednostavnog editora slika napisanog u Qt famework-u.
Pokriva neke od osnovnih operacija, kao što su učitavanje i čuvanje slike, promena veličine slike, postavljanje jednostavnih filtera i slično.

Korisnički interfejs projekta je napisan u potpunosti u C++ kodu, bez korišćenja `.ui` fajla.

Ovim kratkim uputstvom ćemo pokriti:
- Postavljanje Qt projekta
- Glavni prozor i widget-i
- Učitavanje slika i alati za edit (promena veličine, sečenje, rotacija, grayscale, osvetljenje...)
- Undo/redo funkcionalnost
- Prečice na tastaturi

---

## 📦 Zahtevi sistema

- Qt 6.x ili 5.15+ (instalirati preko [Qt installer](https://www.qt.io/download))
- Qt Creator (IDE) ili neki drugi C++ IDE
- Osnovno znanje iz C++ programskog jezika
- Osnovno znanje iz Qt signala i widget-a

---

## 🧱 Kreiranje projekta

1. Otvorite **Qt Creator**.
2. File → New Project → Qt Widgets Application
3. Deselektujte opciju `.ui` (za kreiranje potpunog UI-a iz C++ koda).
4. Dajte proizvoljno ime i lokaciju za čuvanje projekta, kao i verziju Qt-a
5. Završite sve korake i otvorite `.pro` fajl.

---

## 📁 Struktura projekta

Za osnovnu strukturu jednostavnog projekta ćemo koristiti strukturu projekta priloženog u ovom repozitorijumu.

Potrebno je kreirati sledeće fajlove:

```
mini_photo_editor/
├── main.cpp
├── mainwindow.h/.cpp
├── imageArea.h/.cpp
├── resizeDialog.h/.cpp
├── cropDialog.h/.cpp
├── rotateDialog.h/.cpp
├── imageProcessor.h/.cpp
├── metadataDialog.h/.cpp
├── historyPanel.h/.cpp
└── mini_photo_editor.pro
```

Svaki par `.h/.cpp` predstavlja dialog za modularnost.

---

## 🖥️ Glavni prozor

U klasi `MainWindow`, napraviti:
- Menu bar (File, Edit, Filters, View, Help)
- Centralni `ImageArea` widget za prikaz slike
- Metadata info panel

**Jednostavni izgled:**

```cpp
setCentralWidget(imageArea);
addDockWidget(Qt::RightDockWidgetArea, historyPanel);
addDockWidget(Qt::BottomDockWidgetArea, metadataPanel);
```

Povezivanje `QAction`-a za:
- File → Open/Save
- Edit → Resize, Crop, Rotate
- Filters → Grayscale, Invert, Brightness

---

## 🖼️ Učitavanje i prikaz slike

Korišćenje `QImage` komponente:

```cpp
void MainWindow::openImage() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", 
                       tr("Image Files (*.png *.jpg *.bmp)"));
    if (fileName.isEmpty()) return;

    QImage image;
    if (!image.load(fileName)) {
        QMessageBox::warning(this, tr("Open Image"), tr("Failed to load image."));
        return;
    }
    imageArea->setImage(image);
    currentImage = image;
}
```

Omogućavanje drag-and-drop funkcionalnosti:

```cpp
setAcceptDrops(true);
void dragEnterEvent(QDragEnterEvent *event);
void dropEvent(QDropEvent *event);
```

---

## ✂️ Dodavanje jednoostavnih transformacija (promena veličine, sečenje i rotacija)

### ResizeDialog

Primer koda za promenu veličine slike:

```cpp
void ResizeDialog::onWidthChanged(const QString &text) {
    if (aspectRatioLock->isChecked() && !text.isEmpty()) {
        int width = text.toInt();
        int height = static_cast<int>(width / aspectRatio);
        heightLineEdit->setText(QString::number(height));
    }
}
```

### CropDialog
Primer koda za sečenje slike:

```cpp
QRect cropRect = rubberBand->geometry();
QImage cropped = currentImage.copy(cropRect);
imageArea->setImage(cropped);
pushUndoStack(cropped);
```

### RotateDialog
Primer koda za rotaciju slike:

```cpp
// Rotate 90 degrees
currentImage = currentImage.transformed(QTransform().rotate(90));
imageArea->setImage(currentImage);
pushUndoStack(currentImage);
```

---

## 🧠 Stack za undo/redo operacije

Undo/redo operacije se jednostavno mogu implementirati u Qt-u samo korišćenjem ugrađene strukture stack-a

```cpp
void MainWindow::undo() {
    if (undoStack.isEmpty()) return;
    redoStack.push(currentImage);
    currentImage = undoStack.pop();
    imageArea->setImage(currentImage);
}

void MainWindow::redo() {
    if (redoStack.isEmpty()) return;
    undoStack.push(currentImage);
    currentImage = redoStack.pop();
    imageArea->setImage(currentImage);
}
```

---

## 🎨 Dodavanje filtera na sliku

Zahvaljujući Qt komponenti `QImage`, promena njenih stanja i dodavanje filtera je jako jednostavna. Korišćenjem ugrađene funkcije članice `convertToFormat`, možemo dodati neke od osnovnih filtera.

### Grayscale:
```cpp
void MainWindow::applyGrayscale() {
    if (currentImage.isNull()) return;
    currentImage = currentImage.convertToFormat(QImage::Format_Grayscale8);
    imageArea->setImage(currentImage);
    pushUndoStack(currentImage);
}

```

### Invert:
Za invertovanje piksela slike, komponenta `QImage` takođe ima ugrađenu funkciju `invertPixels`, koja omogućava jednostavnu implementaciju ove funkcionalnosti.

```cpp
void MainWindow::applyInvert() {
    if (currentImage.isNull()) return;
    currentImage.invertPixels();
    imageArea->setImage(currentImage);
    pushUndoStack(currentImage);
}

```

### BrightnessDialog:
Promena osvetoljena slike se takođe postiže pozivom funkcije `convertToFormat`

```cpp
QImage adjustBrightness(const QImage &src, int brightness) {
    QImage img = src.convertToFormat(QImage::Format_ARGB32);
    for (int y = 0; y < img.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(img.scanLine(y));
        for (int x = 0; x < img.width(); ++x) {
            QColor color = QColor::fromRgb(line[x]);
            int r = qBound(0, color.red() + brightness, 255);
            int g = qBound(0, color.green() + brightness, 255);
            int b = qBound(0, color.blue() + brightness, 255);
            line[x] = qRgb(r, g, b);
        }
    }
    return img;
}
```

---

## 🔍 Zumiranje slike

Za promene oparacije kao što su zumiranje slike, Qt omogućava jednostavnost zahvaljujući ugrađenoj funkciji `resize` u okviru `QWidget` komponente.

```cpp
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
```

---


## 💾 Čuvanje slike

Qt komponenta `QFileDialog` nam omogućava otvaranje dijaloga za čuvanje fajlova, pomoću ugrađene funkcije `getSaveFileName`.

```cpp
void MainWindow::saveImage() {
    if (currentImage.isNull()) return;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", 
                       tr("PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp)"));
    if (fileName.isEmpty()) return;

    if (!currentImage.save(fileName)) {
        QMessageBox::warning(this, tr("Save Image"), tr("Failed to save image."));
    }
}
```

---

## ⌨️ Dodavanje prečica

Komponenta `QAction` ima ugrađenu funkciju `setShortcut` pomoću koje se može dodati prečica za rad preko tastature. Pored predefinisanih prečica, kao što su `Ctrl+O` ili `Ctrl+Z`, mogu se dodati i proizvoljne prečice za rad u aplikaciji.
```cpp
openAct->setShortcut(QKeySequence::Open);    // Ctrl+O
saveAct->setShortcut(QKeySequence::Save);    // Ctrl+S
undoAct->setShortcut(QKeySequence::Undo);    // Ctrl+Z
redoAct->setShortcut(QKeySequence::Redo);    // Ctrl+Y
zoomInAct->setShortcut(QKeySequence("Ctrl++"));
zoomOutAct->setShortcut(QKeySequence("Ctrl+-"));
grayscaleAct->setShortcut(QKeySequence("Ctrl+G"));
invertAct->setShortcut(QKeySequence("Ctrl+I"));
```

---

## 📜 .pro fajl

`.pro` fajl (skraćeno od project file) je fajl koji koristi qmake, Qt-jev bild sistem, za definisanje projekta koji se bilduje. Sadrži sve neophodne informacije za kompajliranje i linkovanje aplikacije, kao što su:

- Koji Qt moduli se koriste (na primer: widgets, gui, network)

- Koji source (.cpp) i header (.h) fajlovi su deo projekta

- Podešavanje kompajlera i konfuguracionih flegova

- Ostali resursi

Ukratko, .pro fajl govori qmake sistemu šta da bilduje i kako da to uradi.

Primer .pro fajla iz priloženog projekta:

```pro
QT += core gui widgets

TARGET = mini_photo_editor
TEMPLATE = app

SOURCES += main.cpp \
    mainwindow.cpp \
    imageArea.cpp \
    resizeDialog.cpp \
    cropDialog.cpp \
    rotateDialog.cpp \
    brightnessDialog.cpp \
    historyPanel.cpp \
    imageProcessor.cpp \
    metadataDialog.cpp

HEADERS += \
    mainwindow.h \
    imagearea.h \
    resizeDialog.h \
    cropDialog.h \
    rotateDialog.h \
    brightnessDialog.h \
    historyPanel.h \
    imageProcessor.h \
    metadataDialog.h
```

---

## ✅ Zaključak

Ovaj projekat (tutorijal) pokazuje kako da:
- Napravite jednostavan korisnički interfejs u Qt-u napisan u C++ kodu
- Manipulaciju slikama
- Korišćenje ugrađenih struktura podaaka (stek u ovom slučaju)

---

## 📍 Autor

Milica Stanković, [Elektronski fakultet]  
📧 [milicastankovic@elfak.rs]