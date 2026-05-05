#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QMouseEvent>
#include <QFont>

// =====================================================================
// RightClickButton — subclass QPushButton untuk mendeteksi klik kanan
// Ini adalah teknik umum di Qt: override fungsi dari parent class
// =====================================================================
class RightClickButton : public QPushButton {
    Q_OBJECT
public:
    explicit RightClickButton(QWidget* parent = nullptr) : QPushButton(parent) {}

signals:
    void rightClicked(); // signal baru yang kita buat sendiri

protected:
    // Override mousePressEvent dari QPushButton
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::RightButton) {
            emit rightClicked(); // pancarkan signal klik kanan
        } else {
            QPushButton::mousePressEvent(event); // proses normal untuk klik kiri
        }
    }
};

// =====================================================================
// CONSTRUCTOR
// =====================================================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Minesweeper");
    setFixedSize(420, 520);

    setupUI();
    initGrid();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// =====================================================================
// setupUI — membuat semua widget
// =====================================================================
void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    // --- Header ---
    QHBoxLayout* headerLayout = new QHBoxLayout();

    mineCountLabel = new QLabel("💣 10", this);
    mineCountLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: red;");
    mineCountLabel->setMinimumWidth(70);

    resetButton = new QPushButton("🙂", this);
    resetButton->setFixedSize(44, 44);
    resetButton->setStyleSheet("font-size: 22px; background-color: #c0c0c0; border: 2px outset #fff;");
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::onResetClicked);

    statusLabel = new QLabel("", this);
    statusLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    statusLabel->setMinimumWidth(70);
    statusLabel->setStyleSheet("font-size: 14px; font-weight: bold;");

    headerLayout->addWidget(mineCountLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(resetButton);
    headerLayout->addStretch();
    headerLayout->addWidget(statusLabel);
    mainLayout->addLayout(headerLayout);

    // --- Grid ---
    QWidget* gridContainer = new QWidget(this);
    gridContainer->setStyleSheet("background-color: #999; padding: 3px;");
    gridLayout = new QGridLayout(gridContainer);
    gridLayout->setSpacing(2);
    gridLayout->setContentsMargins(3, 3, 3, 3);

    grid.resize(ROWS, QVector<Cell>(COLS));

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            // Pakai RightClickButton bukan QPushButton biasa
            RightClickButton* btn = new RightClickButton(this);
            btn->setFixedSize(40, 40);
            btn->setStyleSheet(
                "QPushButton {"
                "  background-color: #bdbdbd;"
                "  border: 2px outset #ffffff;"
                "  font-size: 15px;"
                "  font-weight: bold;"
                "}"
                "QPushButton:hover {"
                "  background-color: #cacaca;"
                "}"
                "QPushButton:pressed {"
                "  border: 2px inset #888;"
                "  background-color: #aaa;"
                "}"
                );

            grid[row][col].button = btn;
            btn->setProperty("row", row);
            btn->setProperty("col", col);

            // Klik kiri → signal clicked() → slot onCellLeftClicked
            connect(btn, &QPushButton::clicked, this, &MainWindow::onCellLeftClicked);

            // Klik kanan → signal rightClicked() → slot onCellRightClicked
            connect(btn, &RightClickButton::rightClicked, this, &MainWindow::onCellRightClicked);

            gridLayout->addWidget(btn, row, col);
        }
    }

    mainLayout->addWidget(gridContainer);

    // Petunjuk di bawah
    QLabel* hint = new QLabel("Klik kiri: buka sel  |  Klik kanan: tandai 🚩", this);
    hint->setAlignment(Qt::AlignCenter);
    hint->setStyleSheet("font-size: 11px; color: #555;");
    mainLayout->addWidget(hint);
}

// =====================================================================
// initGrid — reset semua data ke kondisi awal
// =====================================================================
void MainWindow::initGrid()
{
    gameOver = false;
    firstClick = true;
    revealedCount = 0;
    flagCount = 0;

    resetButton->setText("🙂");
    statusLabel->setText("");
    statusLabel->setStyleSheet("");
    updateMineCounter();

    QString defaultStyle =
        "QPushButton {"
        "  background-color: #bdbdbd;"
        "  border: 2px outset #ffffff;"
        "  font-size: 15px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #cacaca;"
        "}"
        "QPushButton:pressed {"
        "  border: 2px inset #888;"
        "  background-color: #aaa;"
        "}";

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            Cell& cell = grid[row][col];
            cell.hasMine = false;
            cell.isRevealed = false;
            cell.isFlagged = false;
            cell.adjacentMines = 0;

            QPushButton* btn = cell.button;
            btn->setText("");
            btn->setEnabled(true);
            btn->setStyleSheet(defaultStyle);
        }
    }
}

// =====================================================================
// placeMines
// =====================================================================
void MainWindow::placeMines(int safeRow, int safeCol)
{
    int placed = 0;
    while (placed < TOTAL_MINES) {
        int row = QRandomGenerator::global()->bounded(ROWS);
        int col = QRandomGenerator::global()->bounded(COLS);
        if (row == safeRow && col == safeCol) continue;
        if (grid[row][col].hasMine) continue;
        grid[row][col].hasMine = true;
        placed++;
    }
}

// =====================================================================
// calculateNumbers
// =====================================================================
void MainWindow::calculateNumbers()
{
    int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc[] = {-1,  0,  1,-1, 1,-1, 0, 1};

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (grid[row][col].hasMine) continue;
            int count = 0;
            for (int d = 0; d < 8; d++) {
                int nr = row + dr[d];
                int nc = col + dc[d];
                if (isValid(nr, nc) && grid[nr][nc].hasMine) count++;
            }
            grid[row][col].adjacentMines = count;
        }
    }
}

bool MainWindow::isValid(int row, int col)
{
    return row >= 0 && row < ROWS && col >= 0 && col < COLS;
}

// =====================================================================
// revealCell
// =====================================================================
void MainWindow::revealCell(int row, int col)
{
    Cell& cell = grid[row][col];
    if (cell.isRevealed || cell.isFlagged) return;

    cell.isRevealed = true;
    revealedCount++;
    QPushButton* btn = cell.button;

    if (cell.hasMine) {
        btn->setText("💣");
        btn->setEnabled(false);
        btn->setStyleSheet("QPushButton { background-color: #ff4444; border: 1px solid #888; font-size: 20px; }");
        return;
    }

    btn->setEnabled(false);

    // Warna standar Minesweeper untuk tiap angka
    QStringList colors = {"", "#0000ff", "#007b00", "#ff0000",
                          "#00007b", "#7b0000", "#007b7b", "#000000", "#7b7b7b"};

    if (cell.adjacentMines > 0) {
        btn->setText(QString::number(cell.adjacentMines));
        btn->setStyleSheet(QString(
                               "QPushButton {"
                               "  background-color: #d4d4d4;"
                               "  border: 1px solid #aaa;"
                               "  font-size: 15px;"
                               "  font-weight: bold;"
                               "  color: %1;"
                               "}"
                               ).arg(colors[cell.adjacentMines]));
    } else {
        btn->setText("");
        btn->setStyleSheet("QPushButton { background-color: #d4d4d4; border: 1px solid #aaa; }");
        floodFill(row, col);
    }
}

// =====================================================================
// floodFill
// =====================================================================
void MainWindow::floodFill(int row, int col)
{
    int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc[] = {-1,  0,  1,-1, 1,-1, 0, 1};

    for (int d = 0; d < 8; d++) {
        int nr = row + dr[d];
        int nc = col + dc[d];
        if (isValid(nr, nc) && !grid[nr][nc].isRevealed && !grid[nr][nc].hasMine) {
            revealCell(nr, nc);
        }
    }
}

// =====================================================================
// revealAllMines
// =====================================================================
void MainWindow::revealAllMines()
{
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            Cell& cell = grid[row][col];
            if (cell.hasMine && !cell.isRevealed) {
                cell.button->setText("💣");
                cell.button->setEnabled(false);
                cell.button->setStyleSheet(
                    "QPushButton { background-color: #ffcccc; border: 1px solid #888; font-size: 20px; }"
                    );
            }
        }
    }
}

// =====================================================================
// checkWin
// =====================================================================
void MainWindow::checkWin()
{
    int totalSafe = ROWS * COLS - TOTAL_MINES;
    if (revealedCount >= totalSafe) {
        gameOver = true;
        resetButton->setText("😎");
        statusLabel->setText("Menang! 🎉");
        statusLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: green;");
    }
}

void MainWindow::updateMineCounter()
{
    mineCountLabel->setText(QString("💣 %1").arg(TOTAL_MINES - flagCount));
}

// =====================================================================
// SLOT: onCellLeftClicked
// =====================================================================
void MainWindow::onCellLeftClicked()
{
    if (gameOver) return;

    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int row = btn->property("row").toInt();
    int col = btn->property("col").toInt();
    Cell& cell = grid[row][col];

    if (cell.isRevealed || cell.isFlagged) return;

    // Klik pertama: baru taruh bom supaya tidak langsung kalah
    if (firstClick) {
        firstClick = false;
        placeMines(row, col);
        calculateNumbers();
    }

    if (cell.hasMine) {
        gameOver = true;
        resetButton->setText("😵");
        statusLabel->setText("Kalah! 💥");
        statusLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: red;");
        revealCell(row, col);
        revealAllMines();
    } else {
        revealCell(row, col);
        checkWin();
    }
}

// =====================================================================
// SLOT: onCellRightClicked
// =====================================================================
void MainWindow::onCellRightClicked()
{
    if (gameOver) return;

    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int row = btn->property("row").toInt();
    int col = btn->property("col").toInt();
    Cell& cell = grid[row][col];

    if (cell.isRevealed) return;

    if (cell.isFlagged) {
        cell.isFlagged = false;
        flagCount--;
        btn->setText("");
        btn->setStyleSheet(
            "QPushButton { background-color: #bdbdbd; border: 2px outset #ffffff; font-size: 15px; font-weight: bold; }"
            "QPushButton:hover { background-color: #cacaca; }"
            );
    } else {
        cell.isFlagged = true;
        flagCount++;
        btn->setText("🚩");
        btn->setStyleSheet(
            "QPushButton { background-color: #bdbdbd; border: 2px outset #ffffff; font-size: 18px; }"
            );
    }

    updateMineCounter();
}

// =====================================================================
// SLOT: onResetClicked
// =====================================================================
void MainWindow::onResetClicked()
{
    initGrid();
}

// Diperlukan karena RightClickButton didefinisikan di .cpp (bukan .h)
#include "mainwindow.moc"