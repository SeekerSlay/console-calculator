#include <QCoreApplication>
#include <QTextStream>
#include <QStringList>
#include <QFile>
#include "calculator.h"

static void printHelp(QTextStream &out) {
    out << "Доступные команды:\n"
           "  add <a> <b>  - сложение\n"
           "  sub <a> <b>  - вычитание\n"
           "  mul <a> <b>  - умножение\n"
           "  div <a> <b>  - деление\n"
           "  reset        - сброс\n"
           "  help         - справка\n"
           "  quit         - выход\n";
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QTextStream in(stdin);
    QTextStream out(stdout);
    Calculator calc;

    QObject::connect(&calc, &Calculator::resultReady, [&out](double r) {
        out << "Результат: " << r << "\n";
        out.flush();
    });
    QObject::connect(&calc, &Calculator::errorOccurred, [&out](const QString &msg) {
        out << "Ошибка: " << msg << "\n";
        out.flush();
    });

    // Второй обработчик resultReady: история в файл (один сигнал -> много слотов)
    QFile historyFile("history.txt");
    historyFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream historyStream(&historyFile);
    QObject::connect(&calc, &Calculator::resultReady, [&historyStream](double r) {
        historyStream << "Result: " << r << "\n";
        historyStream.flush();
    });

    out << "=== Консольный калькулятор на Qt ===\n";
    printHelp(out);
    out << "\n> ";
    out.flush();

    QString line;
    while (in.readLineInto(&line)) {
        line = line.trimmed();
        if (!line.isEmpty()) {
            const QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            const QString command = parts.value(0).toLower();

            if (command == "quit" || command == "exit") {
                out << "До свидания!\n";
                break;
            } else if (command == "help") {
                printHelp(out);
            } else if (command == "reset") {
                calc.reset();
            } else if (parts.size() != 3) {
                calc.fail("Неверный формат. Используйте: <команда> <a> <b>");
            } else {
                bool ok1 = false, ok2 = false;
                const double a = parts[1].toDouble(&ok1);
                const double b = parts[2].toDouble(&ok2);
                if (!ok1 || !ok2) {
                    calc.fail("Не удалось преобразовать операнды в числа");
                } else if (command == "add") {
                    calc.add(a, b);
                } else if (command == "sub") {
                    calc.subtract(a, b);
                } else if (command == "mul") {
                    calc.multiply(a, b);
                } else if (command == "div") {
                    calc.divide(a, b);
                } else {
                    calc.fail("Неизвестная команда: " + command);
                }
            }
        }
        out << "> ";
        out.flush();
    }
    return 0;
}