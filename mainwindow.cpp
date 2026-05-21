#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QDir>
// ─────────────────────────────────────────────────────────────────────
// RightClickButton — subclass QPushButton untuk deteksi klik kanan
// ─────────────────────────────────────────────────────────────────────
class RightClickButton : public QPushButton {
    Q_OBJECT
public:
    explicit RightClickButton(QWidget* parent = nullptr) : QPushButton(parent) {}
signals:
    void rightClicked();
protected:
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::RightButton)
            emit rightClicked();
        else
            QPushButton::mousePressEvent(event);
    }
};

// ─────────────────────────────────────────────────────────────────────
// CONSTRUCTOR
// ─────────────────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ── Logo aplikasi di pojok kiri atas jendela ──────────────────
    // QIcon menerima path resource Qt (prefix ":/")
    setWindowIcon(QIcon(":/images/app_icon.png"));
    setWindowTitle("Minesweeper");

    loadPixmaps();  // load semua gambar ke memory
    setupUI();
    initGrid();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ─────────────────────────────────────────────────────────────────────
// loadPixmaps — load semua PNG dari Qt Resource System
//
// Format path resource Qt: ":/prefix/namafile"
// prefix "images" didefinisikan di file .qrc
// ─────────────────────────────────────────────────────────────────────
void MainWindow::loadPixmaps()
{
    int cs = CELL_SIZE - 4; // ukuran gambar sedikit lebih kecil dari sel

    // QPixmap::scaled() mengubah ukuran gambar
    // Qt::KeepAspectRatio     → jaga proporsi gambar
    // Qt::SmoothTransformation → anti-aliasing supaya tidak pecah-pecah
    pxTileClose  = QPixmap(":/images/tile_close.png").scaled(cs, cs, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pxTileOpen   = QPixmap(":/images/tile_open.png").scaled(cs, cs, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pxMine       = QPixmap(":/images/mine.png").scaled(cs-4, cs-4, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pxFlag       = QPixmap(":/images/flag.png").scaled(cs-4, cs-4, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pxFaceNormal = QPixmap(":/images/face_normal.png").scaled(44, 44, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pxFaceLose   = QPixmap(":/images/face_lose.png").scaled(44, 44, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pxFaceWin    = QPixmap(":/images/face_win.png").scaled(44, 44, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    // untuk debug:
    qDebug() << "tile_close loaded:" << !pxTileClose.isNull();
    qDebug() << "tile_open loaded:" << !pxTileOpen.isNull();
    qDebug() << "mine loaded:" << !pxMine.isNull();
    qDebug() << "flag loaded:" << !pxFlag.isNull();
    qDebug() << "Resource files:" << QDir(":/images").entryList();
    qDebug() << "tile_open size:" << pxTileOpen.size();
    qDebug() << "tile_open null:" << pxTileOpen.isNull();
    // Simpan tile_open sebagai file untuk kita lihat hasil scaled-nya
    // pxTileOpen.save("C:/QtProgram/debug_tile_open.png");
    // pxMine.save("C:/QtProgram/debug_mine.png");
    // pxTileClose.save("C:/QtProgram/debug_tile_close.png");
}

// ─────────────────────────────────────────────────────────────────────
// Helper: terapkan gambar ke tombol
//
// Kita pakai QIcon karena QPushButton bisa menampilkan ikon.
// QIcon dibuat dari QPixmap.
// setIconSize() mengatur ukuran tampilan ikon di dalam tombol.
// ─────────────────────────────────────────────────────────────────────
void MainWindow::applyTileClose(QPushButton* btn)
{
    btn->setIcon(QIcon(pxTileClose));
    btn->setIconSize(pxTileClose.size());
    btn->setText("");
    btn->setStyleSheet(
        "QPushButton { background-color: #7a9c59; border: 2px outset #aacf77; }"
        "QPushButton:hover { background-color: #8ab068; }"
        "QPushButton:pressed { border-style: inset; background-color: #6b8f4e; }"
        );
}

void MainWindow::applyTileOpen(QPushButton* btn)
{
    btn->setIcon(QIcon(pxTileOpen));
    btn->setIconSize(pxTileOpen.size());
    btn->setText("");
    btn->setStyleSheet(
        "QPushButton { background-color: transparent; border: 1px solid #6b8f4e; }"
        );
}

void MainWindow::applyMine(QPushButton* btn, bool isHit)
{
    btn->setIcon(QIcon(pxMine));
    btn->setIconSize(pxMine.size());
    btn->setText("");
    QString bg = isHit ? "#cc3333" : "#8B6347";
    // QString bg = isHit ? "transparent" : "transparent";
    btn->setStyleSheet(QString(
                           "QPushButton { background-color: %1; border: 1px solid #6b8f4e; }"
                           ).arg(bg));
}

void MainWindow::applyFlag(QPushButton* btn)
{
    btn->setIcon(QIcon(pxFlag));
    btn->setIconSize(pxFlag.size());
    btn->setText("");
    btn->setStyleSheet(
        "QPushButton { background-color: #7a9c59; border: 2px outset #aacf77; }"
        );
}

// ─────────────────────────────────────────────────────────────────────
// setupUI
// ─────────────────────────────────────────────────────────────────────
void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Lebar window menyesuaikan: 9 sel × CELL_SIZE + padding
    int windowW = COLS * CELL_SIZE + 36;
    int windowH = ROWS * CELL_SIZE + 130;
    setFixedSize(windowW, windowH);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    // ── Background hijau rumput seperti gambar tile_close ──────────
    centralWidget->setStyleSheet("background-color: #5a7a42;");

    // ── Header ────────────────────────────────────────────────────
    QWidget* headerWidget = new QWidget(this);
    headerWidget->setStyleSheet(
        "background-color: #4a6a35;"
        "border-radius: 8px;"
        "border: 2px solid #3a5a28;"
        );
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(10, 6, 10, 6);

    mineCountLabel = new QLabel("💣 10", this);
    mineCountLabel->setStyleSheet(
        "font-size: 20px; font-weight: bold; color: #ffee88;"
        "background: transparent; border: none;"
        );
    mineCountLabel->setMinimumWidth(70);

    // Tombol reset pakai gambar muka
    resetButton = new QPushButton(this);
    resetButton->setFixedSize(52, 52);
    resetButton->setIcon(QIcon(pxFaceNormal));
    resetButton->setIconSize(QSize(44, 44));
    resetButton->setStyleSheet(
        "QPushButton { background-color: #6b8f4e; border: 2px outset #aacf77; border-radius: 26px; }"
        "QPushButton:hover { background-color: #7aaa5a; }"
        "QPushButton:pressed { border-style: inset; }"
        );
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::onResetClicked);

    statusLabel = new QLabel("", this);
    statusLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    statusLabel->setMinimumWidth(70);
    statusLabel->setStyleSheet(
        "font-size: 13px; font-weight: bold; background: transparent; border: none;"
        );

    headerLayout->addWidget(mineCountLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(resetButton);
    headerLayout->addStretch();
    headerLayout->addWidget(statusLabel);
    mainLayout->addWidget(headerWidget);

    // ── Grid ──────────────────────────────────────────────────────
    QWidget* gridContainer = new QWidget(this);
    gridContainer->setStyleSheet(
        "background-color: #4a6a35;"
        "border-radius: 6px;"
        "border: 2px solid #3a5a28;"
        );
    gridLayout = new QGridLayout(gridContainer);
    gridLayout->setSpacing(3);
    gridLayout->setContentsMargins(4, 4, 4, 4);

    grid.resize(ROWS, QVector<Cell>(COLS));

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            RightClickButton* btn = new RightClickButton(this);
            btn->setFixedSize(CELL_SIZE, CELL_SIZE);

            grid[row][col].button = btn;
            btn->setProperty("row", row);
            btn->setProperty("col", col);

            applyTileClose(btn); // tampilan awal: gambar tile tertutup

            connect(btn, &QPushButton::clicked,          this, &MainWindow::onCellLeftClicked);
            connect(btn, &RightClickButton::rightClicked, this, &MainWindow::onCellRightClicked);

            gridLayout->addWidget(btn, row, col);
        }
    }

    mainLayout->addWidget(gridContainer);

    QLabel* hint = new QLabel("Klik kiri: buka  |  Klik kanan: bendera 🚩", this);
    hint->setAlignment(Qt::AlignCenter);
    hint->setStyleSheet("font-size: 11px; color: #c8e8a0; background: transparent; border: none;");
    mainLayout->addWidget(hint);
}

// ─────────────────────────────────────────────────────────────────────
// initGrid
// ─────────────────────────────────────────────────────────────────────
void MainWindow::initGrid()
{
    gameOver = false; firstClick = true;
    revealedCount = 0; flagCount = 0;

    resetButton->setIcon(QIcon(pxFaceNormal));
    statusLabel->setText("");

    updateMineCounter();

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            Cell& cell = grid[r][c];
            cell.hasMine = cell.isRevealed = cell.isFlagged = false;
            cell.adjacentMines = 0;
            cell.button->setEnabled(true);
            applyTileClose(cell.button);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────
// placeMines & calculateNumbers
// ─────────────────────────────────────────────────────────────────────
void MainWindow::placeMines(int safeRow, int safeCol)
{
    int placed = 0;
    while (placed < TOTAL_MINES) {
        int r = QRandomGenerator::global()->bounded(ROWS);
        int c = QRandomGenerator::global()->bounded(COLS);
        if (r == safeRow && c == safeCol) continue;
        if (grid[r][c].hasMine) continue;
        grid[r][c].hasMine = true;
        placed++;
    }
}

void MainWindow::calculateNumbers()
{
    int dr[] = {-1,-1,-1, 0, 0, 1, 1, 1};
    int dc[] = {-1, 0, 1,-1, 1,-1, 0, 1};
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (grid[r][c].hasMine) continue;
            int count = 0;
            for (int d = 0; d < 8; d++) {
                int nr = r+dr[d], nc = c+dc[d];
                if (isValid(nr, nc) && grid[nr][nc].hasMine) count++;
            }
            grid[r][c].adjacentMines = count;
        }
    }
}

bool MainWindow::isValid(int r, int c)
{
    return r >= 0 && r < ROWS && c >= 0 && c < COLS;
}

// ─────────────────────────────────────────────────────────────────────
// revealCell — tampilkan isi sel
// ─────────────────────────────────────────────────────────────────────
void MainWindow::revealCell(int row, int col)
{
    Cell& cell = grid[row][col];
    if (cell.isRevealed || cell.isFlagged) return;

    cell.isRevealed = true;
    revealedCount++;
    QPushButton* btn = cell.button;
    btn->setEnabled(false);

    if (cell.hasMine) {
        applyMine(btn, true); // merah = bom yang diklik
        return;
    }

    // Tampilkan tile terbuka sebagai background
    applyTileOpen(btn);

    // Warna angka sesuai standar Minesweeper
    QStringList colors = {"","#1a1aff","#007700","#cc0000",
                          "#000077","#770000","#007777","#111111","#777777"};

    if (cell.adjacentMines > 0) {
        // Gabungkan tile_open + angka jadi satu gambar
        QPixmap combined(pxTileOpen.size());
        combined.fill(Qt::transparent);

        QPainter painter(&combined);
        painter.drawPixmap(0, 0, pxTileOpen); // gambar tile_open dulu

        // Lalu tulis angka di atasnya
        painter.setPen(QColor(colors[cell.adjacentMines]));

        QFont font = painter.font();
        font.setPointSize(14);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(combined.rect(), Qt::AlignCenter,
                         QString::number(cell.adjacentMines));
        painter.end();

        btn->setIcon(QIcon(combined));
        btn->setIconSize(combined.size());
        btn->setText("");
        btn->setStyleSheet(
            "QPushButton { background-color: transparent; border: 1px solid #6b8f4e; }"
            );
    } else {
        floodFill(row, col); // sel kosong: buka tetangganya
    }
}

void MainWindow::floodFill(int row, int col)
{
    int dr[] = {-1,-1,-1, 0, 0, 1, 1, 1};
    int dc[] = {-1, 0, 1,-1, 1,-1, 0, 1};
    for (int d = 0; d < 8; d++) {
        int nr = row+dr[d], nc = col+dc[d];
        if (isValid(nr,nc) && !grid[nr][nc].isRevealed && !grid[nr][nc].hasMine)
            revealCell(nr, nc);
    }
}

void MainWindow::revealAllMines()
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (grid[r][c].hasMine && !grid[r][c].isRevealed) {
                grid[r][c].button->setEnabled(false);
                applyMine(grid[r][c].button, false); // warna coklat = bom lain
            }
}

void MainWindow::checkWin()
{
    if (revealedCount >= ROWS * COLS - TOTAL_MINES) {
        gameOver = true;
        resetButton->setIcon(QIcon(pxFaceWin));
        statusLabel->setText("Menang! 🎉");
        statusLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #ffee88; background: transparent; border: none;");
    }
}

void MainWindow::updateMineCounter()
{
    mineCountLabel->setText(QString("💣 %1").arg(TOTAL_MINES - flagCount));
}

// ─────────────────────────────────────────────────────────────────────
// SLOT: onCellLeftClicked
// ─────────────────────────────────────────────────────────────────────
void MainWindow::onCellLeftClicked()
{
    if (gameOver) return;
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int row = btn->property("row").toInt();
    int col = btn->property("col").toInt();
    Cell& cell = grid[row][col];
    if (cell.isRevealed || cell.isFlagged) return;

    if (firstClick) {
        firstClick = false;
        placeMines(row, col);
        calculateNumbers();
    }

    if (cell.hasMine) {
        gameOver = true;
        resetButton->setIcon(QIcon(pxFaceLose)); // ganti muka jadi mati
        statusLabel->setText("Kalah! 💥");
        statusLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #ff8888; background: transparent; border: none;");
        revealCell(row, col);
        revealAllMines();
    } else {
        revealCell(row, col);
        checkWin();
    }
}

// ─────────────────────────────────────────────────────────────────────
// SLOT: onCellRightClicked
// ─────────────────────────────────────────────────────────────────────
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
        applyTileClose(btn); // kembalikan ke tile tertutup
    } else {
        cell.isFlagged = true;
        flagCount++;
        applyFlag(btn); // tampilkan gambar bendera
    }

    updateMineCounter();
}

// ─────────────────────────────────────────────────────────────────────
// SLOT: onResetClicked
// ─────────────────────────────────────────────────────────────────────
void MainWindow::onResetClicked()
{
    initGrid();
}

#include "mainwindow.moc"
