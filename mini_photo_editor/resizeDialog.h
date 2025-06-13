#pragma once
#include <QDialog>

class QLineEdit;
class QLabel;
class QCheckBox;

class ResizeDialog : public QDialog {
    Q_OBJECT
public:
    ResizeDialog(int currentWidth, int currentHeight, QWidget *parent = nullptr);

    int getNewWidth() const;
    int getNewHeight() const;

private slots:
    void onWidthChanged(const QString &text);
    void onHeightChanged(const QString &text);

private:
    QLineEdit *widthLineEdit;
    QLineEdit *heightLineEdit;
    QCheckBox *aspectRatioCheckBox;

    double aspectRatio; // width / height

    bool isAdjusting = false; // avoid recursive slot calls
};
