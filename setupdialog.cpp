#include "setupdialog.h"
#include <QFrame>

SetupDialog::SetupDialog(HighScoreManager* hsm, QWidget *parent)
    : QDialog(parent), hsManager(hsm)
{
    setWindowTitle("Minesweeper — Setting");
    setFixedSize(420, 460);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    setStyleSheet(
        "QDialog { background-color: #4a6a35; }"
        "QLabel { color: #e8f5d0; background: transparent; }"
        "QSlider::groove:horizontal {"
        "  height: 8px; background: #3a5a28; border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "  width: 22px; height: 22px;"
        "  background: #aacf77; border: 2px solid #7a9c59;"
        "  border-radius: 11px; margin: -7px 0;"
        "}"
        "QSlider::sub-page:horizontal {"
        "  background: #7a9c59; border-radius: 4px;"
        "}"
        );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(12);

    // ── Title ─────────────────────────────────────────────────────
    QLabel* title = new QLabel("Minesweeper - by AlproGacor77🤑", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 19px; font-weight: bold; color: #ffee88;");
    mainLayout->addWidget(title);

    QLabel* subtitle = new QLabel("Set ur game before playin'", this);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("font-size: 12px; color: #c8e8a0;");
    mainLayout->addWidget(subtitle);

    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #3a5a28;");
    mainLayout->addWidget(line);

    // ── Difficulty Buttons ────────────────────────────────────────
    QHBoxLayout* difficultyLayout = new QHBoxLayout();

    QString diffStyle =
        "QPushButton {"
        "  background-color: #5d7f42; color: #ffee88;"
        "  font-weight: bold; border: 2px outset #aacf77;"
        "  border-radius: 6px; padding: 6px;"
        "}"
        "QPushButton:hover { background-color: #739c53; }"
        "QPushButton:pressed { border-style: inset; }";

    easyButton   = new QPushButton("Easy",   this);
    mediumButton = new QPushButton("Medium", this);
    hardButton   = new QPushButton("Hard",   this);

    easyButton->setStyleSheet(diffStyle);
    mediumButton->setStyleSheet(diffStyle);
    hardButton->setStyleSheet(diffStyle);

    connect(easyButton,   &QPushButton::clicked, this, &SetupDialog::setEasy);
    connect(mediumButton, &QPushButton::clicked, this, &SetupDialog::setMedium);
    connect(hardButton,   &QPushButton::clicked, this, &SetupDialog::setHard);

    difficultyLayout->addWidget(easyButton);
    difficultyLayout->addWidget(mediumButton);
    difficultyLayout->addWidget(hardButton);
    mainLayout->addLayout(difficultyLayout);

    // ── Sliders ───────────────────────────────────────────────────
    mainLayout->addWidget(makeSliderRow(
        "Rows", rowsSlider, rowsValueLabel,
        5, 14, 9, "5", "14"
        ));
    mainLayout->addWidget(makeSliderRow(
        "Columns", colsSlider, colsValueLabel,
        5, 26, 9, "5", "26"
        ));
    mainLayout->addWidget(makeSliderRow(
        "Total Mine", minesSlider, minesValueLabel,
        1, 30, 10, "Min.: 1", "Max.: 21% from total area"
        ));

    connect(rowsSlider,  &QSlider::valueChanged, this, &SetupDialog::onRowsChanged);
    connect(colsSlider,  &QSlider::valueChanged, this, &SetupDialog::onColsChanged);
    connect(minesSlider, &QSlider::valueChanged, this, &SetupDialog::onMinesChanged);

    // ── Best Time Display ─────────────────────────────────────────
    QFrame* hsFrame = new QFrame(this);
    hsFrame->setStyleSheet(
        "background-color: #3a5a28; border-radius: 6px; border: 1px solid #2a4a1e;"
        );
    QHBoxLayout* hsLayout = new QHBoxLayout(hsFrame);
    hsLayout->setContentsMargins(12, 6, 12, 6);

    QLabel* hsTitleLabel = new QLabel("🏆 Best Time:", this);
    hsTitleLabel->setStyleSheet(
        "font-size: 13px; font-weight: bold; color: #ffee88; background: transparent; border: none;"
        );

    hsLabel = new QLabel("No record yet", this);
    hsLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    hsLabel->setStyleSheet(
        "font-size: 13px; color: #aacf77; background: transparent; border: none;"
        );

    hsLayout->addWidget(hsTitleLabel);
    hsLayout->addStretch();
    hsLayout->addWidget(hsLabel);
    mainLayout->addWidget(hsFrame);

    // ── Reset Score Button ────────────────────────────────────────
    QPushButton* resetScoreBtn = new QPushButton("Reset This High Score", this);
    resetScoreBtn->setFixedHeight(32);
    resetScoreBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #3a5a28; color: #ff8888;"
        "  font-size: 12px; font-weight: bold;"
        "  border: 1px solid #ff6666; border-radius: 6px;"
        "}"
        "QPushButton:hover { background-color: #4a2020; }"
        "QPushButton:pressed { border-style: inset; }"
        );
    connect(resetScoreBtn, &QPushButton::clicked, this, [this]() {
        QString key = HighScoreManager::makeKey(difficulty, rowValue, colValue, mineValue);
        hsManager->resetKey(key);
        updateHighScoreLabel();
    });
    mainLayout->addWidget(resetScoreBtn);

    // ── Start Button ──────────────────────────────────────────────
    startButton = new QPushButton("  Start!", this);
    startButton->setFixedHeight(44);
    startButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #7a9c59; color: #ffee88;"
        "  font-size: 16px; font-weight: bold;"
        "  border: 2px outset #aacf77; border-radius: 8px;"
        "}"
        "QPushButton:hover { background-color: #8ab068; }"
        "QPushButton:pressed { border-style: inset; background-color: #6b8f4e; }"
        );
    connect(startButton, &QPushButton::clicked, this, &SetupDialog::onStartClicked);
    mainLayout->addWidget(startButton);

    updateMinesRange();
    updateHighScoreLabel();
}

QWidget* SetupDialog::makeSliderRow(const QString& labelText,
                                    QSlider*& slider, QLabel*& valueLabel,
                                    int min, int max, int defaultVal,
                                    const QString& minHint, const QString& maxHint)
{
    QWidget* row = new QWidget(this);
    row->setStyleSheet("background: transparent;");
    QVBoxLayout* vlay = new QVBoxLayout(row);
    vlay->setContentsMargins(0, 0, 0, 0);
    vlay->setSpacing(4);

    QHBoxLayout* topRow = new QHBoxLayout();
    QLabel* nameLabel = new QLabel(labelText + ":", this);
    nameLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #e8f5d0;");
    valueLabel = new QLabel(QString::number(defaultVal), this);
    valueLabel->setAlignment(Qt::AlignRight);
    valueLabel->setStyleSheet("font-size: 11px; font-weight: bold; color: #ffee88;");
    valueLabel->setMinimumWidth(30);
    topRow->addWidget(nameLabel);
    topRow->addStretch();
    topRow->addWidget(valueLabel);
    vlay->addLayout(topRow);

    slider = new QSlider(Qt::Horizontal, this);
    slider->setMinimum(min);
    slider->setMaximum(max);
    slider->setValue(defaultVal);
    slider->setTickPosition(QSlider::NoTicks);
    vlay->addWidget(slider);

    QHBoxLayout* hintRow = new QHBoxLayout();
    QLabel* hMin = new QLabel(minHint, this);
    QLabel* hMax = new QLabel(maxHint, this);
    hMin->setStyleSheet("font-size: 10px; color: #a0c878;");
    hMax->setStyleSheet("font-size: 10px; color: #a0c878;");
    hMax->setAlignment(Qt::AlignRight);
    hintRow->addWidget(hMin);
    hintRow->addStretch();
    hintRow->addWidget(hMax);
    vlay->addLayout(hintRow);

    return row;
}

void SetupDialog::onRowsChanged(int value)
{
    rowValue = value;
    rowsValueLabel->setText(QString::number(value));
    difficulty = "custom";
    updateMinesRange();
    updateHighScoreLabel();
}

void SetupDialog::onColsChanged(int value)
{
    colValue = value;
    colsValueLabel->setText(QString::number(value));
    difficulty = "custom";
    updateMinesRange();
    updateHighScoreLabel();
}

void SetupDialog::onMinesChanged(int value)
{
    mineValue = value;
    minesValueLabel->setText(QString::number(value));
    difficulty = "custom";
    updateHighScoreLabel();
}

void SetupDialog::updateMinesRange()
{
    int maxMines = qMax(1, (int)(rowValue * colValue * 0.21));
    minesSlider->setMaximum(maxMines);
    if (mineValue > maxMines) {
        mineValue = maxMines;
        minesSlider->setValue(mineValue);
        minesValueLabel->setText(QString::number(mineValue));
    }
}

void SetupDialog::updateHighScoreLabel()
{
    QString key = HighScoreManager::makeKey(difficulty, rowValue, colValue, mineValue);
    int best = hsManager->getBestTime(key);
    if (best < 0) {
        hsLabel->setText("No record yet");
        hsLabel->setStyleSheet(
            "font-size: 13px; color: #a0c878; background: transparent; border: none;"
            );
    } else {
        hsLabel->setText(HighScoreManager::formatTime(best));
        hsLabel->setStyleSheet(
            "font-size: 15px; font-weight: bold; color: #ffee88; background: transparent; border: none;"
            );
    }
}

void SetupDialog::onStartClicked()
{
    rowValue  = rowsSlider->value();
    colValue  = colsSlider->value();
    mineValue = minesSlider->value();
    accept();
}

void SetupDialog::setEasy()
{
    difficulty = "easy";
    rowsSlider->setValue(9);
    colsSlider->setValue(9);
    updateMinesRange();
    minesSlider->setValue(10);
    rowValue = 9; colValue = 9; mineValue = 10;
    rowsValueLabel->setText("9");
    colsValueLabel->setText("9");
    minesValueLabel->setText("10");
    updateHighScoreLabel();
}

void SetupDialog::setMedium()
{
    difficulty = "medium";
    rowsSlider->setValue(14);
    colsSlider->setValue(18);
    updateMinesRange();
    minesSlider->setValue(40);
    rowValue = 14; colValue = 18; mineValue = 40;
    rowsValueLabel->setText("14");
    colsValueLabel->setText("18");
    minesValueLabel->setText("40");
    updateHighScoreLabel();
}

void SetupDialog::setHard()
{
    difficulty = "hard";
    rowsSlider->setValue(14);
    colsSlider->setValue(26);
    updateMinesRange();
    minesSlider->setValue(75);
    rowValue = 14; colValue = 26; mineValue = 75;
    rowsValueLabel->setText("14");
    colsValueLabel->setText("26");
    minesValueLabel->setText("75");
    updateHighScoreLabel();
}
