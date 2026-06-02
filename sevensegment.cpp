#include "sevensegment.h"
#include <QHBoxLayout>
#include <QPainterPath>

// ─────────────────────────────────────────────────────────────────────
// Tabel segmen: angka 0-9
// bit: 0=a 1=b 2=c 3=d 4=e 5=f 6=g
//
//   aaa
//  f   b
//  f   b
//   ggg
//  e   c
//  e   c
//   ddd
// ─────────────────────────────────────────────────────────────────────
const uint8_t SevenSegmentDisplay::SEGMENTS[10] = {
    //  a     b     c     d     e     f     g
    0b0111111,  // 0: a b c d e f
    0b0000110,  // 1:     b c
    0b1011011,  // 2: a b   d e   g
    0b1001111,  // 3: a b c d     g
    0b1100110,  // 4:   b c     f g
    0b1101101,  // 5: a   c d   f g
    0b1111101,  // 6: a   c d e f g
    0b0000111,  // 7: a b c
    0b1111111,  // 8: a b c d e f g
    0b1101111,  // 9: a b c d   f g
};

// ─────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────
SevenSegmentDisplay::SevenSegmentDisplay(int digits, QWidget* parent)
    : QWidget(parent), currentValue(0), numDigits(digits)
{
    // Ukuran tiap digit: 20x34px, jarak 4px, padding 6px
    int w = numDigits * 24 + 12;
    setFixedSize(w, 46);
}

void SevenSegmentDisplay::setValue(int value)
{
    // Clamp ke range yang bisa ditampilkan
    int maxVal = 1;
    for (int i = 0; i < numDigits; i++) maxVal *= 10;
    currentValue = qBound(0, value, maxVal - 1);
    update(); // trigger repaint
}

// ─────────────────────────────────────────────────────────────────────
// paintEvent — digambar ulang tiap setValue() dipanggil
// ─────────────────────────────────────────────────────────────────────
void SevenSegmentDisplay::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Background hitam dengan border
    p.setBrush(QColor(10, 10, 10));
    p.setPen(QColor(40, 40, 40));
    p.drawRoundedRect(rect(), 4, 4);

    int digitW = 20;
    int digitH = 34;
    int gap    = 4;
    int padX   = 6;
    int padY   = (height() - digitH) / 2;

    // Pisahkan tiap digit dari nilai
    QVector<int> digits(numDigits, 0);
    int val = currentValue;
    for (int i = numDigits - 1; i >= 0; i--) {
        digits[i] = val % 10;
        val /= 10;
    }

    // Gambar tiap digit
    for (int i = 0; i < numDigits; i++) {
        int x = padX + i * (digitW + gap);
        drawDigit(p, x, padY, digitW, digitH, SEGMENTS[digits[i]]);
    }
}

// ─────────────────────────────────────────────────────────────────────
// drawDigit — gambar satu digit di posisi (x, y)
// ─────────────────────────────────────────────────────────────────────
void SevenSegmentDisplay::drawDigit(QPainter& p, int x, int y, int w, int h, uint8_t segs)
{
    QColor onColor  = QColor(220, 30, 30);   // merah terang = nyala
    QColor offColor = QColor(50, 10, 10);    // merah gelap  = mati

    int thick = 3;   // ketebalan segmen
    int inset = 2;   // jarak segmen dari tepi

    // ── Koordinat 7 segmen ──────────────────────────────────────
    // a: atas horizontal
    // b: kanan atas vertikal
    // c: kanan bawah vertikal
    // d: bawah horizontal
    // e: kiri bawah vertikal
    // f: kiri atas vertikal
    // g: tengah horizontal

    int mid = y + h / 2;

    struct Seg { int x1, y1, x2, y2; bool horiz; };
    Seg segDefs[7] = {
                      // a: atas
                      { x+inset, y,        x+w-inset, y,        true  },
                      // b: kanan atas
                      { x+w,     y+inset,  x+w,       mid-inset, false },
                      // c: kanan bawah
                      { x+w,     mid+inset,x+w,       y+h-inset, false },
                      // d: bawah
                      { x+inset, y+h,      x+w-inset, y+h,       true  },
                      // e: kiri bawah
                      { x,       mid+inset,x,         y+h-inset, false },
                      // f: kiri atas
                      { x,       y+inset,  x,         mid-inset, false },
                      // g: tengah
                      { x+inset, mid,      x+w-inset, mid,       true  },
                      };

    for (int i = 0; i < 7; i++) {
        bool on = (segs >> i) & 1;
        QColor color = on ? onColor : offColor;

        QPen pen(color, thick, Qt::SolidLine, Qt::RoundCap);
        p.setPen(pen);

        if (segDefs[i].horiz) {
            // Garis horizontal: gambar sedikit menjorok ke dalam
            p.drawLine(segDefs[i].x1 + thick/2, segDefs[i].y1,
                       segDefs[i].x2 - thick/2, segDefs[i].y2);
        } else {
            // Garis vertikal
            p.drawLine(segDefs[i].x1, segDefs[i].y1 + thick/2,
                       segDefs[i].x2, segDefs[i].y2 - thick/2);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────
// GameTimer
// ─────────────────────────────────────────────────────────────────────
GameTimer::GameTimer(QWidget* parent)
    : QWidget(parent), seconds(0)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    display = new SevenSegmentDisplay(3, this);
    layout->addWidget(display);

    ticker = new QTimer(this);
    ticker->setInterval(1000); // tick tiap 1 detik
    connect(ticker, &QTimer::timeout, this, &GameTimer::onTick);
}

void GameTimer::startTimer()
{
    ticker->start();
}

void GameTimer::stopTimer()
{
    ticker->stop();
}

void GameTimer::resetTimer()
{
    ticker->stop();
    seconds = 0;
    display->setValue(0);
}

void GameTimer::onTick()
{
    seconds++;
    if (seconds > 999) seconds = 999; // cap di 999
    display->setValue(seconds);
}