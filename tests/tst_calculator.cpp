#include <QtTest>
#include <QSignalSpy>
#include "calculator.h"

class CalculatorTest : public QObject {
    Q_OBJECT

private slots:
    void init()    { m_calc = new Calculator; }
    void cleanup() { delete m_calc; m_calc = nullptr; }

    void testAdd() {
        QSignalSpy spy(m_calc, &Calculator::resultReady);
        QSignalSpy err(m_calc, &Calculator::errorOccurred);
        m_calc->add(5, 3);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().at(0).toDouble(), 8.0);
        QCOMPARE(err.count(), 0);
        QCOMPARE(m_calc->result(), 8.0);
        QVERIFY(!m_calc->hasError());
    }

    void testSubtract() {
        QSignalSpy spy(m_calc, &Calculator::resultReady);
        m_calc->subtract(5, 8);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().at(0).toDouble(), -3.0);
    }

    void testMultiply() {
        QSignalSpy spy(m_calc, &Calculator::resultReady);
        m_calc->multiply(4, 2.5);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().at(0).toDouble(), 10.0);
    }

    void testDivide() {
        QSignalSpy spy(m_calc, &Calculator::resultReady);
        m_calc->divide(10, 4);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().at(0).toDouble(), 2.5);
    }

    void testDivideByZero() {
        QSignalSpy res(m_calc, &Calculator::resultReady);
        QSignalSpy err(m_calc, &Calculator::errorOccurred);
        m_calc->divide(10, 0);
        QCOMPARE(res.count(), 0);          // resultReady не излучается
        QCOMPARE(err.count(), 1);          // errorOccurred излучается ровно раз
        QVERIFY(!err.takeFirst().at(0).toString().isEmpty());
        QVERIFY(m_calc->hasError());
        QVERIFY(!m_calc->errorMessage().isEmpty());
    }

    void testReset() {
        m_calc->add(2, 2);
        QSignalSpy spy(m_calc, &Calculator::resultReady);
        m_calc->reset();
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().at(0).toDouble(), 0.0);
        QCOMPARE(m_calc->result(), 0.0);
        QVERIFY(!m_calc->hasError());
    }

    void testErrorClearedAfterSuccess() {
        m_calc->divide(1, 0);
        QVERIFY(m_calc->hasError());
        m_calc->add(1, 1);
        QVERIFY(!m_calc->hasError());
        QVERIFY(m_calc->errorMessage().isEmpty());
    }

    void testFail() {
        QSignalSpy err(m_calc, &Calculator::errorOccurred);
        m_calc->fail("bad input");
        QCOMPARE(err.count(), 1);
        QCOMPARE(err.takeFirst().at(0).toString(), QString("bad input"));
        QVERIFY(m_calc->hasError());
    }

    void testSignalToMultipleSlots() {
        int a = 0, b = 0;
        connect(m_calc, &Calculator::resultReady, this, [&a](double) { ++a; });
        connect(m_calc, &Calculator::resultReady, this, [&b](double) { ++b; });
        m_calc->add(1, 2);
        QCOMPARE(a, 1);
        QCOMPARE(b, 1);
    }

private:
    Calculator *m_calc = nullptr;
};

QTEST_GUILESS_MAIN(CalculatorTest)
#include "tst_calculator.moc"