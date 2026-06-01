#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include "highscore.h"

class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupDialog(HighScoreManager* hsm, QWidget *parent = nullptr);

    int     selectedRows()       const { return rowValue; }
    int     selectedCols()       const { return colValue; }
    int     selectedMines()      const { return mineValue; }
    QString selectedDifficulty() const { return difficulty; }

private slots:
    void onRowsChanged(int value);
    void onColsChanged(int value);
    void onMinesChanged(int value);
    void onStartClicked();
    void setEasy();
    void setMedium();
    void setHard();

private:
    QSlider*     rowsSlider;
    QSlider*     colsSlider;
    QSlider*     minesSlider;
    QLabel*      rowsValueLabel;
    QLabel*      colsValueLabel;
    QLabel*      minesValueLabel;
    QPushButton* startButton;
    QPushButton* easyButton;
    QPushButton* mediumButton;
    QPushButton* hardButton;
    QLabel*      hsLabel;

    HighScoreManager* hsManager;

    int     rowValue  = 9;
    int     colValue  = 9;
    int     mineValue = 10;
    QString difficulty = "custom";

    void updateMinesRange();
    void updateHighScoreLabel();

    QWidget* makeSliderRow(const QString& labelText,
                           QSlider*& slider,
                           QLabel*& valueLabel,
                           int min, int max, int defaultVal,
                           const QString& minHint, const QString& maxHint);
};

#endif // SETUPDIALOG_H