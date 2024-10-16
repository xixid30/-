#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "button.h"
#include <QLineEdit>
#include <QKeyEvent>
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


protected:
    void keyPressEvent(QKeyEvent *event) override;

public slots:
    void digitClicked();
    // void digitClicked(int digitValue);
    void pointClicked();
    void backSpaceClicked();
    void clear();
    void clearAll();
    void multiplicativeOperatorClicked();
    void additiveOperatorClicked();
    // void additiveOperatorClicked(const QString &oprator);
    // void multiplicativeOperatorClicked(const QString &oprator);
    void unaryOperatorClicked();
    void equalClicked();
    void changeClicked();
private:
    Button* createButton(const QString& text,const char*number);
    bool calculate(double rightOperand,const QString& pendingOperator);
    void abortOperator();
    void mapKeyToButton(QKeyEvent *event);
private:
    Ui::Widget *ui;
    double sumSoFar;
    double factorSoFar;
    bool waitingForOperand;
    QString pendingAdditiveOperator;
    QString pendingMultipcativeOperator;


    QLineEdit* display;
    Button* digitButtons[10];
};
#endif // WIDGET_H
