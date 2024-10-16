#include "widget.h"
#include "./ui_widget.h"
#include<QFont>
#include<QGridLayout>
#include <QKeyEvent>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
    display=new QLineEdit("0");

    display->setReadOnly(true);
    display->setAlignment(Qt::AlignLeft);
    display->setMaxLength(15);

    QFont font=display->font();
    font.setPointSize(font.pointSize()+8);
    display->setFont(font);

    for(int i=0;i<10;++i){
        digitButtons[i]=createButton(QString::number(i),SLOT(digitClicked()));
    }

    Button* pointBtn=createButton(".",SLOT(pointClicked()));
    Button* backspaceBtn=createButton("BackSpace",SLOT(backSpaceClicked()));
    Button* clearBtn=createButton("clear",SLOT(clear()));
    Button* clearAllBtn=createButton("clear all",SLOT(clearAll()));
    Button* divBtn=createButton("\303\267",SLOT(multiplicativeOperatorClicked()));
    Button* timesBtn=createButton("\303\227",SLOT(multiplicativeOperatorClicked()));
    Button* minusBtn=createButton("-",SLOT(additiveOperatorClicked()));
    Button* plusBtn=createButton("+",SLOT(additiveOperatorClicked()));
    Button* sqrtBtn=createButton("Sqrt",SLOT(unaryOperatorClicked()));
    Button* powerBtn=createButton("x\302\262",SLOT(unaryOperatorClicked()));
    Button* reciprocalBtn=createButton("1/x",SLOT(unaryOperatorClicked()));
    Button* equalBtn=createButton("=",SLOT(equalClicked()));
    Button* changeBtn=createButton("\302\261",SLOT(changeClicked()));

    auto mainLayout=new QGridLayout(this);

    mainLayout->addWidget(display,0,0,1,7);
    mainLayout->addWidget(backspaceBtn,1,0,1,2);
    mainLayout->addWidget(clearBtn,1,2,1,2);
    mainLayout->addWidget(clearAllBtn,1,4,1,3);

    mainLayout->addItem(new QSpacerItem(20,40,QSizePolicy::Expanding),2,0);

    for(int i=1;i<10;++i){
        int row=((9-i)/3)+2;
        int col=((i-1)%3)+1;
        mainLayout->addWidget(digitButtons[i],row,col);
    }

    mainLayout->addWidget(digitButtons[0],5,1);
    mainLayout->addWidget(pointBtn,5,2);
    mainLayout->addWidget(changeBtn,5,3);

    mainLayout->addWidget(divBtn,2,4);
    mainLayout->addWidget(timesBtn,3,4);
    mainLayout->addWidget(minusBtn,4,4);
    mainLayout->addWidget(plusBtn,5,4);

    mainLayout->addWidget(sqrtBtn,2,5);
    mainLayout->addWidget(powerBtn,3,5);
    mainLayout->addWidget(reciprocalBtn,4,5);
    mainLayout->addWidget(equalBtn,5,5);

    mainLayout->addItem(new QSpacerItem(20,40,QSizePolicy::Expanding),3,6);
    setStyleSheet(
        "QLineEdit { background-color: #E0E044; color: black; border: 2px solid #E0E099 ; border-radius: 50%; }"
        "QToolButton { background-color: #4CAF50; color: white; border: 1px solid #4CAF50; border-radius: 50%;}"
        "QToolButton:hover { background-color: #45a049; }"
        "QToolButton:pressed { background-color: #3e8e41; }"
        );

}

Widget::~Widget()
{
    delete ui;
}

void Widget::keyPressEvent(QKeyEvent *event) {
    mapKeyToButton(event);
}

void Widget::digitClicked()
{
    Button* btn=qobject_cast<Button*>(sender());
    int digitValue=btn->text().toInt();
    if(display->text()=="0"&&digitValue==0){
        return;
    }
    if(waitingForOperand){
        display->clear();
        waitingForOperand=false;
    }
    display->setText(display->text()+QString::number(digitValue));
}

// void Widget::digitClicked(int digitValue) {
//     if (display->text() == "0" && digitValue == 0) {
//         return;
//     }
//     if (waitingForOperand) {
//         display->clear();
//         waitingForOperand = false;
//     }
//     display->setText(display->text() + QString::number(digitValue));
// }

void Widget::pointClicked()
{
    if(waitingForOperand){
        display->setText("0");
    }
    if(!display->text().contains('.')){
        display->setText(display->text()+".");
    }
    waitingForOperand=false;
}

void Widget::backSpaceClicked()
{
    if(waitingForOperand){
        return;
    }
    QString txt=display->text();
    txt.chop(1);
    if(txt.isEmpty()){
        txt="0";
        waitingForOperand=true;
    }
    display->setText(txt);
}

void Widget::clear()
{
    if(waitingForOperand){
        return;
    }
    display->setText("0");
    waitingForOperand=true;
}

void Widget::clearAll()
{
    sumSoFar=0;
    factorSoFar=0;
    pendingAdditiveOperator.clear();
    pendingMultipcativeOperator.clear();
    display->setText("0");
    waitingForOperand=true;
}

void Widget::multiplicativeOperatorClicked()
{
    Button* btn=qobject_cast<Button*>(sender());
    if(!btn){
        return;
    }
    QString oprator=btn->text();
    double operand=display->text().toDouble();
    if(!pendingMultipcativeOperator.isEmpty()){
        if(!calculate(operand,pendingMultipcativeOperator)){
            abortOperator();
            return;
        }
        display->setText(QString::number(factorSoFar));
    }
    else{
        factorSoFar=operand;
    }
    pendingMultipcativeOperator=oprator;
    waitingForOperand=true;
}

void Widget::additiveOperatorClicked()
{

    Button* btn=qobject_cast<Button*>(sender());
    QString oprator=btn->text();
    double operand=display->text().toDouble();
    if(!pendingMultipcativeOperator.isEmpty()){
        if(!calculate(operand,pendingMultipcativeOperator)){
            abortOperator();
            return;
        }
        display->setText(QString::number(factorSoFar));
        operand=factorSoFar;
        factorSoFar=0;
        pendingMultipcativeOperator.clear();
    }
    if(!pendingAdditiveOperator.isEmpty()){
        if(!calculate(operand,pendingAdditiveOperator)){
            abortOperator();
            return;
        }
        display->setText(QString::number(sumSoFar));
    }
    else{
        sumSoFar=operand;
    }
    pendingAdditiveOperator=oprator;
    waitingForOperand=true;
}

// void Widget::additiveOperatorClicked(const QString &oprator) {
//     double operand = display->text().toDouble();
//     if (!pendingMultipcativeOperator.isEmpty()) {
//         if (!calculate(operand, pendingMultipcativeOperator)) {
//             abortOperator();
//             return;
//         }
//         display->setText(QString::number(factorSoFar));
//         operand = factorSoFar;
//         factorSoFar = 0;
//         pendingMultipcativeOperator.clear();
//     }
//     if (!pendingAdditiveOperator.isEmpty()) {
//         if (!calculate(operand, pendingAdditiveOperator)) {
//             abortOperator();
//             return;
//         }
//         display->setText(QString::number(sumSoFar));
//     } else {
//         sumSoFar = operand;
//     }
//     pendingAdditiveOperator = oprator;
//     waitingForOperand = true;
// }

// void Widget::multiplicativeOperatorClicked(const QString &oprator) {
//     double operand = display->text().toDouble();
//     if (!pendingMultipcativeOperator.isEmpty()) {
//         if (!calculate(operand, pendingMultipcativeOperator)) {
//             abortOperator();
//             return;
//         }
//         display->setText(QString::number(factorSoFar));
//     } else {
//         factorSoFar = operand;
//     }
//     pendingMultipcativeOperator = oprator;
//     waitingForOperand = true;
// }
void Widget::unaryOperatorClicked()
{
    Button* btn=qobject_cast<Button*>(sender());
    QString oprator=btn->text();
    double operand=display->text().toDouble();
    double result=0;
    if(oprator==("Sqrt")){
        if(operand<0){
            abortOperator();
            return;
        }
        result=std::sqrt(operand);
    }else if(oprator=="x\302\262"){
        result=std::pow(operand,2);
    }else if(oprator=="1/x"){
        if(operand==0){
            abortOperator();
            return;
        }
        result=1.0/operand;
    }
    display->setText(QString::number(result));
    waitingForOperand=true;
}

void Widget::equalClicked()
{
    double operand=display->text().toDouble();
    if(!pendingMultipcativeOperator.isEmpty()){
        if(!calculate(operand,pendingMultipcativeOperator)){
            abortOperator();
            return;
        }
        operand=factorSoFar;
        factorSoFar=0;
        pendingMultipcativeOperator.clear();
    }
    if(!pendingAdditiveOperator.isEmpty()){
        if(!calculate(operand,pendingAdditiveOperator)){
            abortOperator();
            return;
        }
        pendingAdditiveOperator.clear();
    }else{
        sumSoFar=operand;
    }
    display->setText(QString::number(sumSoFar));
    sumSoFar=0;
    waitingForOperand=true;
}

void Widget::changeClicked()
{

    QString txt=display->text();
    double v=txt.toDouble();
    if(v>0){
        txt.prepend("-");
    }else if(v<0)
    {
        txt.remove(0,1);
    }
    display->setText(txt);
}

Button *Widget::createButton(const QString &text, const char *number)
{
    Button* btn=new Button(text);
    connect(btn,SIGNAL(clicked()),this,number);
    return btn;
}
// Button *Widget::createButton(const QString &text, const char *number) {
//     Button* btn = new Button(text);
//     // 不再需要连接按钮的点击信号
//     return btn;
// }
bool Widget::calculate(double rightOperand, const QString &pendingOperator)
{
    if(pendingOperator=="+"){
        sumSoFar+=rightOperand;
    }
    else if(pendingOperator=="-"){
        sumSoFar-=rightOperand;
    }
    else if(pendingOperator=="\303\227"){
        factorSoFar*=rightOperand;
    }
    else if(pendingOperator=="\303\267"){
        if(rightOperand==0.0){
            return false;
        }
        factorSoFar/=rightOperand;
    }
    // else if(pendingOperator=="*"){
    //     factorSoFar*=rightOperand;
    // }
    // else if(pendingOperator=="/"){
    //     if(rightOperand==0.0){
    //         return false;
    //     }
    //     factorSoFar/=rightOperand;
    // }
    return true;
}

void Widget::abortOperator()
{
    clearAll();
    display->setText("#####");
}

void Widget::mapKeyToButton(QKeyEvent *event)
{
    int key = event->key();
    switch (key) {
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
        digitClicked();
         // digitClicked(key - Qt::Key_0);
        break;
    case Qt::Key_Period:
        pointClicked();
        break;
    case Qt::Key_Backspace:
        backSpaceClicked();
        break;
    case Qt::Key_C:
        clear();
        break;
    case Qt::Key_A:
        clearAll();
        break;
    case Qt::Key_Plus:
        additiveOperatorClicked();
        // additiveOperatorClicked("+");
        break;
    case Qt::Key_Minus:
        additiveOperatorClicked();
        // additiveOperatorClicked("-");
        break;
    case Qt::Key_Asterisk:
        multiplicativeOperatorClicked();
        // multiplicativeOperatorClicked("*");
        break;
    case Qt::Key_Slash:
        multiplicativeOperatorClicked();
        // multiplicativeOperatorClicked("/");
        break;
    case Qt::Key_Equal:
    case Qt::Key_Enter:
    case Qt::Key_Return:
        equalClicked();
        break;
    default:
        break;
    }
}
