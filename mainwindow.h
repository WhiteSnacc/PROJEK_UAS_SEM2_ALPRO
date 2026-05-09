#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QVector>
#include <QLabel>
#include <QPixmap>
#include <QIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Cell {
    bool hasMine       = false;
    bool isRevealed    = false;
    bool isFlagged     = false;
    int  adjacentMines = 0;
    QPushButton* button = nullptr;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onCellLeftClicked();
    void onCellRightClicked();
    void onResetClicked();

private:
    Ui::MainWindow *ui;

    static const int ROWS        = 9;
    static const int COLS        = 9;
    static const int TOTAL_MINES = 10;
    static const int CELL_SIZE   = 52;

    QVector<QVector<Cell>> grid;
    bool gameOver      = false;
    bool firstClick    = true;
    int  revealedCount = 0;
    int  flagCount     = 0;

    QWidget*     centralWidget;
    QGridLayout* gridLayout;
    QLabel*      statusLabel;
    QLabel*      mineCountLabel;
    QPushButton* resetButton;

    // Pixmap disimpan sebagai member — di-load sekali saat startup
    QPixmap pxTileClose;
    QPixmap pxTileOpen;
    QPixmap pxMine;
    QPixmap pxFlag;
    QPixmap pxFaceNormal;
    QPixmap pxFaceLose;
    QPixmap pxFaceWin;

    void loadPixmaps();
    void setupUI();
    void initGrid();
    void placeMines(int safeRow, int safeCol);
    void calculateNumbers();
    void revealCell(int row, int col);
    void floodFill(int row, int col);
    void revealAllMines();
    void checkWin();
    bool isValid(int row, int col);
    void updateMineCounter();

    void applyTileClose(QPushButton* btn);
    void applyTileOpen(QPushButton* btn);
    void applyMine(QPushButton* btn, bool isHit);
    void applyFlag(QPushButton* btn);
};

#endif // MAINWINDOW_H
