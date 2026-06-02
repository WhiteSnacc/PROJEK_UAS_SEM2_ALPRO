#ifndef SEVENSEGMENT_H
#define SEVENSEGMENT_H

#include <QWidget>
#include <QTimer>
#include <QPainter>

// ─────────────────────────────────────────────────────────────────────
// SevenSegmentDisplay — widget angka gaya kalkulator/jam digital
// ─────────────────────────────────────────────────────────────────────
class SevenSegmentDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit SevenSegmentDisplay(int digits = 3, QWidget* parent = nullptr);
    void setValue(int value);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int currentValue;
    int numDigits;

    // Segmen yang nyala untuk tiap angka 0-9
    // bit 0=a(atas) 1=b(kanan atas) 2=c(kanan bawah)
    //     3=d(bawah) 4=e(kiri bawah) 5=f(kiri atas) 6=g(tengah)
    static const uint8_t SEGMENTS[10];

    void drawDigit(QPainter& p, int x, int y, int w, int h, uint8_t segs);
};

// ─────────────────────────────────────────────────────────────────────
// GameTimer — timer game dengan tampilan 7-segment
// ─────────────────────────────────────────────────────────────────────
class GameTimer : public QWidget
{
    Q_OBJECT
public:
    explicit GameTimer(QWidget* parent = nullptr);

    void startTimer();
    void stopTimer();
    void resetTimer();
    int  elapsed() const { return seconds; }

private slots:
    void onTick();

private:
    QTimer*              ticker;
    SevenSegmentDisplay* display;
    int                  seconds;
};

#endif // SEVENSEGMENT_H