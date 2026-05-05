#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QVector>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// =====================================================
// Struct untuk menyimpan data tiap sel di grid
// =====================================================
struct Cell {
    bool hasMine = false;       // apakah sel ini mengandung bom?
    bool isRevealed = false;    // apakah sudah dibuka?
    bool isFlagged = false;     // apakah sudah diberi bendera?
    int adjacentMines = 0;      // berapa bom di sekitarnya?
    QPushButton* button = nullptr; // pointer ke tombol di UI
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // Slot = fungsi yang dipanggil ketika ada signal (event)
    void onCellLeftClicked();   // dipanggil saat klik kiri sel
    void onCellRightClicked();  // dipanggil saat klik kanan sel
    void onResetClicked();      // dipanggil saat tombol reset ditekan

private:
    Ui::MainWindow *ui;

    // =====================================================
    // Konstanta ukuran game
    // =====================================================
    static const int ROWS = 9;
    static const int COLS = 9;
    static const int TOTAL_MINES = 10;

    // =====================================================
    // Data game
    // =====================================================
    QVector<QVector<Cell>> grid; // grid 2D berisi data tiap sel
    bool gameOver = false;
    bool firstClick = true;      // bom ditempatkan setelah klik pertama
    int revealedCount = 0;
    int flagCount = 0;

    // =====================================================
    // Widget UI
    // =====================================================
    QWidget* centralWidget;
    QGridLayout* gridLayout;
    QLabel* statusLabel;
    QLabel* mineCountLabel;
    QPushButton* resetButton;

    // =====================================================
    // Fungsi-fungsi helper
    // =====================================================
    void setupUI();              // membuat semua widget
    void initGrid();             // inisialisasi data grid
    void placeMines(int safeRow, int safeCol); // taruh bom (hindari sel pertama)
    void calculateNumbers();     // hitung angka tiap sel
    void revealCell(int row, int col);         // buka satu sel
    void floodFill(int row, int col);          // buka sel kosong berantai
    void revealAllMines();       // tampilkan semua bom saat kalah
    void checkWin();             // cek apakah sudah menang
    bool isValid(int row, int col); // cek apakah koordinat valid
    void updateMineCounter();    // update label jumlah bom
};

#endif // MAINWINDOW_H