#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextStream>
#include <fstream>
#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include <QStack>
#include <QDebug>

using namespace std;
//Clase en donde se guarda la información de los cuadruplos
class cuadruplo{

    public:
        QString n;
        QString oper;
        QString op1;
        QString op2;
        QString res;
        cuadruplo(QString,QString,QString,QString,QString);
        QString getN();
        QString getOper();
        QString getOp1();
        QString getOp2();
        QString getRes();
        void setRes(QString r);
        void setN(QString n);
        void setOp1(QString op1);
        void setOp2(QString op2);
        void setOper(QString oper);

};
QString cuadruplo::getN(){
    return n;
}
QString cuadruplo::getOper(){
    return oper;
}
QString cuadruplo::getOp1(){
    return op1;
}
QString cuadruplo::getOp2(){
    return op2;
}
QString cuadruplo::getRes(){
    return res;
}
void cuadruplo::setRes(QString _r){
    res=_r;
}
void cuadruplo::setN(QString _n){
    n=_n;
}
void cuadruplo::setOp1(QString _op1){
    op1=_op1;
}
void cuadruplo::setOp2(QString _op2){
    op2=_op2;
}
void cuadruplo::setOper(QString _oper){
    oper=_oper;
}
cuadruplo::cuadruplo(QString _n,QString _oper,QString _op1,QString _op2,QString _res){
    n=_n;
    oper=_oper;
    op1=_op1;
    op2=_op2;
    res=_res;
}


static QStack<int> pilaEjecucion;//Pila análisis sintactico
static QStack<QString> pilaOperandos;//Pila de operandos
static QStack<QString> pilaOperandosBusqueda;//Pila para facilitar la busqueda de tipos en un futuro
static QStack<int> pilaTipos;//Pila de tipos para los identificadores
static QStack<int> pilaTiposBusqueda;//Pila para los tipos
static QStack<int> pilaOperadores;//Pila de operadores que se ingresan
static QStack<int> pilaSaltos;//Pila de saltos
static int edo;//Estado del analizado léxico
static bool sinError; //Varibale para medir si hay error
static QList<QSharedPointer<cuadruplo>> cuadruplos;
static QList<QSharedPointer<cuadruplo>> cuadruplosR;
static int contadorRaW;//Variable que controla los elementos del read, write
static bool estaPresenteRead=false;
static bool estaPresenteWrite=false;
static int contCuadruplo; //Variable que cuenta el valor de los cuadruplos
static int contRes;//Variable que cuenta el valor de res

static QStack<int> pilaWhile;//Pila de control para la parte de adentro de los cuadruplos del while
static QStack<int> pilaFor;//Pila de control para la parte de adentro del for
static int matriz[23][32]={{4,125,500,500,107,108,500,0,0,0,2,1,109,13,10,12,16,17,18,19,21,20,128,129,126,127,123,124,500,111,130,101},
                    {2,100,100,100,100,100,100,100,100,100,2,1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,3,100,100,100},
                    {2,22,101,101,101,101,101,101,101,101,2,2,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,3,101,101,101},
                    {2,501,501,501,501,501,501,501,501,501,2,2,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501},
                    {4,5,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102},
                    {6,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502},
                    {6,103,7,7,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103},
                    {9,503,503,503,8,8,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503},
                    {9,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504},
                    {9,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104},
                    {11,11,11,11,11,11,11,11,11,11,11,11,11,11,505,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11},
                    {506,506,506,506,506,506,506,506,506,506,506,506,506,506,105,506,506,506,506,506,506,506,506,506,506,506,506,506,506,506,506,506},
                    {12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,106,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12},
                    {110,110,110,110,110,110,110,110,110,110,110,110,14,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110},
                    {14,14,14,14,14,14,14,14,14,14,14,14,15,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14},
                    {14,14,14,14,14,14,14,14,14,14,14,14,15,112,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14},
                    {507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,113,507,507,507,507,507,507,507,507,507,507,507,507,507,507},
                    {508,508,508,508,508,508,508,508,508,508,508,508,508,508,508,508,508,508,114,508,508,508,508,508,508,508,508,508,508,508,508,508},
                    {115,115,115,115,115,115,115,115,115,115,115,115,115,115,115,115,115,115,115,116,115,115,115,115,115,115,115,115,115,115,115,115},
                    {122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,117,122,122,122,122,122,122,122,122,122,122,122,122},
                    {118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,119,118,118,118,118,118,118,118,118,118,118,118,118},
                    {120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,121,120,120,120,120,120,120,120,120,120,120,120,120},
                    {509,509,509,509,509,509,509,509,509,509,509,22,509,509,509,509,509,509,509,509,509,509,509,509,509,509,509,509,509,509,509,131}};


static int matrizPredictiva[34][49]={{1,510,1,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510},
                                     {511,511,2,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511,511},
                                     {4,512,3,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512},
                                     {513,513,513,5,513,6,513,513,513,513,513,513,5,513,513,513,513,5,513,5,513,513,513,513,513,513,513,513,513,513,513,513,513,513,513,513,513,513,513,513,513,5,5,5,513,513,513,5,513},
                                     {514,514,514,7,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514},
                                     {515,515,515,515,515,515,9,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,515,8,515,515,515,515},
                                     {516,516,516,516,516,516,516,10,11,12,13,14,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516,516},
                                     {517,517,517,16,517,517,517,517,517,517,517,517,17,517,517,15,15,18,15,19,15,517,517,517,517,517,517,517,517,517,517,517,517,517,517,517,517,517,517,517,517,22,21,20,517,517,517,15,517},
                                     {518,518,518,518,518,518,518,518,518,518,518,518,23,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518},
                                     {519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,25,24,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519,519},
                                     {520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,26,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520},
                                     {521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,27,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521,521},
                                     {522,522,522,28,522,522,522,522,522,522,522,522,522,28,522,522,522,522,522,522,522,522,522,36,522,522,522,522,522,522,522,522,522,522,522,522,28,28,28,28,28,522,522,522,522,522,522,522,522},
                                     {523,523,523,523,523,523,523,523,523,523,523,523,523,523,29,523,523,523,523,523,523,30,523,523,523,523,523,523,523,523,523,523,523,523,523,523,523,523,523,523,523,523,523,523,29,29,29,523,523},
                                     {524,524,524,31,524,524,524,524,524,524,524,524,524,31,524,524,524,524,524,524,524,524,524,36,524,524,524,524,524,524,524,524,524,524,524,524,31,31,31,31,31,524,524,524,524,524,524,524,524},
                                     {525,525,525,525,525,525,525,525,525,525,525,525,525,525,32,525,525,525,525,525,525,32,33,525,525,525,525,525,525,525,525,525,525,525,525,525,525,525,525,525,525,525,525,525,32,32,32,525,525},
                                     {526,526,526,34,526,526,526,526,526,526,526,526,526,34,526,526,526,526,526,526,526,526,526,36,526,526,526,526,526,526,526,526,526,526,526,526,34,34,34,34,34,526,526,526,526,526,526,526,526},
                                     {527,527,527,35,527,527,527,527,527,527,527,527,527,35,527,527,527,527,527,527,527,527,527,36,527,527,527,527,527,527,527,527,527,527,527,527,35,35,35,35,35,527,527,527,527,527,527,527,527},
                                     {528,528,528,37,528,528,528,528,528,528,528,528,528,37,528,528,528,528,528,528,528,528,528,528,528,528,528,528,528,528,528,528,528,528,528,528,37,37,37,37,37,528,528,528,528,528,528,528,528},
                                     {529,529,529,529,529,529,529,529,529,529,529,529,529,529,38,529,529,529,529,529,529,38,38,529,529,39,39,39,39,39,39,529,529,529,529,529,529,529,529,529,529,529,529,529,38,38,38,529,529},
                                     {530,530,530,40,530,530,530,530,530,530,530,530,530,40,530,530,530,530,530,530,530,530,530,530,530,530,530,530,530,530,530,530,530,530,530,530,40,40,40,40,40,530,530,530,530,530,530,530,530},
                                     {531,531,531,531,531,531,531,531,531,531,531,531,531,531,41,531,531,531,531,531,531,41,41,531,531,41,41,41,41,41,41,42,43,531,531,531,531,531,531,531,531,531,531,531,41,41,41,531,531},
                                     {532,532,532,44,532,532,532,532,532,532,532,532,532,44,532,532,532,532,532,532,532,532,532,532,532,532,532,532,532,532,532,532,532,532,532,532,44,44,44,44,44,532,532,532,532,532,532,532,532},
                                     {533,533,533,533,533,533,533,533,533,533,533,533,533,533,45,533,533,533,533,533,533,45,45,533,533,45,45,45,45,45,45,45,45,46,47,48,533,533,533,533,533,533,533,533,45,45,45,533,533},
                                     {534,534,534,49,534,534,534,534,534,534,534,534,534,55,534,534,534,534,534,534,534,534,534,534,534,534,534,534,534,534,534,534,534,534,534,534,50,51,52,53,54,534,534,534,534,534,534,534,534},
                                     {535,535,535,535,535,535,535,535,535,535,535,535,535,535,535,535,535,535,535,535,535,535,535,535,535,56,57,58,59,60,61,535,535,535,535,535,535,535,535,535,535,535,535,535,535,535,535,535,535},
                                     {536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,62,536,536,536,536,536,536,536},
                                     {537,537,537,63,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537,537},
                                     {538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,64,538,538,538,538,538,538},
                                     {539,539,539,65,539,539,539,539,539,539,539,539,539,65,539,539,539,539,539,539,539,539,539,36,539,539,539,539,539,539,539,539,539,539,539,539,65,65,65,65,65,539,539,539,539,539,539,539,539},
                                     {540,540,540,540,540,540,540,540,540,540,540,540,540,540,66,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,67,540,540,540,540},
                                     {541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,541,68,541,541,541,541,541},
                                     {542,542,542,69,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542,542},
                                     {543,543,543,543,543,543,543,543,543,543,543,543,543,543,70,543,543,543,543,543,543,543,543,543,543,543,543,543,543,543,543,543,543,543,543,543,543,543,543,543,543,543,543,543,71,543,543,543,543}};



static int producciones[71][15]={{2,3},//A DECLARA-LIB
                                {718,132,8,4,133,101,134}, //end ESTATUTOS DECLARA begin id class
                                {-1},//ε
                                {3,124,131,135},//DECLARA-LIB ; id_lib import
                                {-1},//ε
                                {5,136},//B def
                                {6,700,101},//C acción 1 semantico id
                                {5,130},//B ,
                                {4,124,701,7,137},//DECLARA ; acción 2 semantico TIPO as
                                {138},//integer
                                {139},//float
                                {140},//char
                                {141},//string
                                {142},//boolean
                                {-1},//ε
                                {8,711,124,28},//ESTATUTOS ; EST_ASIG
                                {8,124,9},//ESTATUTOS ; EST_IF
                                {8,124,11},//ESTATUTOS ; EST_WHILE
                                {8,124,12},//ESTATUTOS ; EST_FOR
                                {8,124,32},//ESTATUTOS ; EST_READ
                                {8,124,29},//ESTATUTOS ; EST_WRITE
                                {8,124,27},//ESTATUTOS ; EST_ENTER
                                {709,10,8,707,127,13,126,143},//D ESTATUTOS ) EXPR ( if
                                {144},//endif
                                {144,8,708,145},//endif ESTATUTOS else
                                {720,714,146,8,707,127,13,715,126,719,147},//endwhile ESTATUTOS ) EXPR ( while
                                {722,714,148,8,713,127,13,712,123,28,126,721,149},//endfor ESTATUTOS ) EXPR : EST_ASIG ( for
                                {14,15},//E EXPR2
                                {-1},//ε
                                {13,703,114},//EXPR ||
                                {16,17},// F EXPR3
                                {-1},//ε
                                {15,703,113},//EXPR2 &&
                                {18},//G
                                {19},//EXPR4
                                {19,115},//EXPR4 !
                                {20,21},//H EXPR5
                                {-1},//ε
                                {21,26},//EXPR5 OPREL
                                {22,23},//I TERM
                                {-1},//ε
                                {21,703,107},//EXPR5 (insertar pila de operadores el operador recibido) +
                                {21,703,108},//EXPR5 (insertar pila de operadores el operador recibido) -
                                {24,25},//J FACT
                                {-1},//ε
                                {23,703,109},//TERM (insertar pila de operadores el operador recibido) *
                                {23,703,110},//TERM (insertar pila de operadores el operador recibido) /
                                {23,703,111},//TERM  (insertar pila de operadores el operador recibido) %
                                {702,101},//(Acción insertar en la pila de tipos el identificador recibido) id
                                {702,102},//(Acción insertar en la pila de tipos el identificador recibido) cteentera
                                {702,103},//(Acción insertar en la pila de tipos el identificador recibido) ctereal
                                {702,104},//(Acción insertar en la pila de tipos el identificador recibido) ctenotacioncientifica
                                {702,105},//(Acción insertar en la pila de tipos el identificador recibido) ctecaracter
                                {702,106},//(Acción insertar en la pila de tipos el identificador recibido) ctestring
                                {706,127,13,705,126},// ) EXPR (
                                {703,117},//==
                                {703,116},//!=
                                {703,120},//<
                                {703,121},//<=
                                {703,118},//>
                                {703,119},//>=
                                {716,150},//enter
                                {13,703,122,702,101},//EXPR (insertar pila de operadores el operador recibido) = (Acción insertar en la pila de tipos el identificador recibido) id
                                {717,30,126,151},//K ( write
                                {31,13},//L EXPR
                                {127},// )
                                {30,130},// K ,
                                {717,33,126,152},//M ( read
                                {34,702,101},//N id
                                {127},// )
                                {33,130}//M ,
                                };

static int matrizDeTipos[25][7]={{138,138,138,139,138,142,544},
                                 {138,139,139,139,544,142,544},
                                 {138,140,544,544,544,544,544},
                                 {138,141,544,544,544,544,544},
                                 {138,142,544,544,544,544,544},
                                 {139,138,139,139,544,142,544},
                                 {139,139,139,139,544,142,544},
                                 {139,140,544,544,544,544,544},
                                 {139,141,544,544,544,544,544},
                                 {139,142,544,544,544,544,544},
                                 {140,138,544,544,544,544,544},
                                 {140,139,544,544,544,544,544},
                                 {140,140,544,544,544,142,544},
                                 {140,141,544,544,544,544,544},
                                 {140,142,544,544,544,544,544},
                                 {141,138,544,544,544,544,544},
                                 {141,139,544,544,544,544,544},
                                 {141,140,544,544,544,142,544},
                                 {141,141,544,544,544,142,544},
                                 {141,142,544,544,544,544,544},
                                 {142,138,544,544,544,544,544},
                                 {142,139,544,544,544,544,544},
                                 {142,140,544,544,544,544,544},
                                 {142,141,544,544,544,544,544},
                                 {142,142,544,544,544,142,142}
                                };

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->Token->setReadOnly(true);
    ui->Error->setReadOnly(true);
    ui->Token_2->setReadOnly(true);
    ui->salida->setReadOnly(true);
    ui->tablaCuadruplos->setColumnCount(5);
    QStringList titulos;
    titulos<<"#"<<"oper"<<"op1"<<"op2"<<"res";
    ui->tablaCuadruplos->setHorizontalHeaderLabels(titulos);
    ui->pilaTipos->setReadOnly(true);
    ui->pilaOperandos->setReadOnly(true);
    ui->pilaOperadores->setReadOnly(true);
    ui->pilaSaltos->setReadOnly(true);
}
MainWindow::~MainWindow()
{
    delete ui;
}
QString pilaOp;
QString pilaT;
QString pilaOper;
QString pilaSal;
QString evaluaElemento(int elemento){
    if(elemento>=700){
        return "";
    }
    switch(elemento){
    case -1:
        return "ε";
    case -2:
        return "MFF";
    case 1:
        return "PROGRAM";
        break;
    case 2:
        return "A";
        break;
    case 3:
        return "DECLARA-LIB";
        break;
    case 4:
        return "DECLARA";
        break;
    case 5:
        return "B";
        break;
    case 6:
        return "C";
        break;
    case 7:
        return "TIPO";
        break;
    case 8:
        return "ESTATUTOS";
        break;
    case 9:
        return "EST_IF";
        break;
    case 10:
        return "D";
        break;
    case 11:
        return "EST_WHILE";
        break;
    case 12:
        return "EST_FOR";
        break;
    case 13:
        return "EXPR";
        break;
    case 14:
        return "E";
        break;
    case 15:
        return "EXPR2";
        break;
    case 16:
        return "F";
        break;
    case 17:
        return "EXPR3";
        break;
    case 18:
        return "G";
        break;
    case 19:
        return "EXPR4";
        break;
    case 20:
        return "H";
        break;
    case 21:
        return "EXPR5";
        break;
    case 22:
        return "I";
        break;
    case 23:
        return "TERM";
        break;
    case 24:
        return "J";
        break;
    case 25:
        return "FACT";
        break;
    case 26:
        return "OPREL";
        break;
    case 27:
        return "EST_ENTER";
        break;
    case 28:
        return "EST_ASIG";
        break;
    case 29:
        return "EST_WRITE";
        break;
    case 30:
        return "K";
        break;
    case 31:
        return "L";
        break;
    case 32:
        return "EST_READ";
        break;
    case 33:
        return "M";
        break;
    case 34:
        return "N";
        break;
    case 36:
        return "$";
        break;
    case 100:
        //cout<<"Palabra reservada"<<endl;

        break;
    case 101:
        //cout<<"Identificador"<<endl;
        return "Identificador";
        break;
    case 102:
        //cout<<"Entero"<<endl;
        return "Cteentera";
        break;
    case 103:
        //cout<<"Constante decimal"<<endl;
        return "Ctereal";
        break;
    case 104:
        //cout<<"Constante con notación cientifica"<<endl;
        return "Ctenotacion cientifica";
        break;
    case 105:
        //cout<<"Constante caracter"<<endl;
        return "CteCaracter";
        break;
    case 106:
        //cout<<"Constante String"<<endl;
        return "CteString";
        break;
    case 107:
        //cout<<"Suma"<<endl;
        return "+";
        break;
    case 108:
        //cout<<"Resta"<<endl;
        return "-";
        break;
    case 109:
        //cout<<"Multiplicación"<<endl;
        return "*";
        break;
    case 110:
        //cout<<"División"<<endl;
        return "/";
        break;
    case 111:
        //cout<<"Modulo"<<endl;
        return "%";
        break;
    case 112:
        //cout<<"Comentario"<<endl;

        break;
    case 113:
        //cout<<"Operador Y"<<endl;
        return "&&";
        break;
    case 114:
        //cout<<"Operador O"<<endl;
        return "||";
        break;
    case 115:
        //cout<<"Operador NOT"<<endl;
        return "!";
        break;
    case 116:
        //cout<<"Operador Diferente"<<endl;
        return "!=";
        break;
    case 117:
        //cout<<"Igual"<<endl;
        return "==";
        break;
    case 118:
        //cout<<"Mayor"<<endl;
        return ">";
        break;
    case 119:
        //cout<<"Mayor Igual"<<endl;
        return ">=";
        break;
    case 120:
        //cout<<"Menor"<<endl;
        return "<";
        break;
    case 121:
        //cout<<"Menor Igual"<<endl;
        return "<=";
        break;
    case 122:
        //cout<<"Asignación"<<endl;
        return "=";
        break;
    case 123:
        //cout<<"Signo dos puntos"<<endl;
        return ":";
        break;
    case 124:
        //cout<<"Signo de cierre"<<endl;
        return ";";
        break;
    case 125:
        //cout<<"Punto"<<endl;

        break;
    case 126:
        //cout<<"Parentesis abierto"<<endl;
        return "(";
        break;
    case 127:
        //cout<<"Par"<<endl;
        return ")";
        break;
    case 128:
        //cout<<"Corchete abierto"<<endl;

        break;
    case 129:
        //cout<<"Corchete cerrado"<<endl;

        break;
    case 130:
        //cout<<"Es una coma"<<endl;
        return ",";
        break;
    case 131:
        //cout<<#Es una libreria"<<endl;
        return "Identifiacdorlibreria";
        break;
    case 132:
        return "end";
        break;
    case 133:
        return "begin";
        break;
    case 134:
        return "class";
        break;
    case 135:
        return "import";
        break;
    case 136:
        return "def";
        break;
    case 137:
        return "as";
        break;
    case 138:
        return "integer";
        break;
    case 139:
        return "float";
        break;
    case 140:
        return "char";
        break;
    case 141:
        return "string";
        break;
    case 142:
        return "boolean";
        break;
    case 143:
        return "if";
        break;
    case 144:
        return "endif";
        break;
    case 145:
        return "else";
        break;
    case 146:
        return "endwhile";
        break;
    case 147:
        return "while";
        break;
    case 148:
        return "endfor";
        break;
    case 149:
        return "for";
        break;
    case 150:
        return "enter";
        break;
    case 151:
        return "write";
        break;
    case 152:
        return "read";
        break;
    default:
        return "desconocido";
    }
}
void imprimePilaOperandos(){
    for(int i=0;i<pilaOperandos.size();i++){
        pilaOp+=pilaOperandos.at(i)+" ";
    }
    pilaOp+="\n\n";
}
void imprimePilaTipos(){
    for(int i=0;i<pilaTipos.size();i++){
        pilaT+=evaluaElemento(pilaTipos.at(i))+" ";
    }
    pilaT+="\n\n";
}
void imprimePilaOperadores(){
    for(int i=0;i<pilaOperadores.size();i++){
        pilaOper+=evaluaElemento(pilaOperadores.at(i))+" ";
    }
    pilaOper+="\n\n";
}
void imprimePilaSaltos(){
    for(int i=0;i<pilaSaltos.size();i++)
        pilaSal+=QString::number(pilaSaltos.at(i))+" ";
    pilaSal+="\n\n";
}

void formaCuadruplo(QString n,QString oper,QString op1,QString op2,QString res){
    cuadruplo *obj=new cuadruplo(n,oper,op1,op2,res);
    cuadruplos.append(QSharedPointer<cuadruplo>(obj));
}

int Relaciona(char c){
   int valor;

   if(c>=48 && c<=57){
       return 0;
   }
   if(c>=65 && c<=90){
       return 10;
   }
   if(c>=97 && c<=122){
       return 11;
   }

   switch(c){

   case '.': valor=1;
       break;
   case 'E': valor=2;
       break;
   case 'e': valor=3;
       break;
   case '+': valor=4;
       break;
   case '-': valor=5;
       break;
   case 32:   valor=7;
       break;
   case '\n': valor=8;
               break;
   case '\t': valor=9;
       break;
   case '*': valor=12;
       break;
   case '/': valor=13;
       break;
       case 39: valor=14;
   break;
   case '"': valor=15;
       break;
   case '&': valor=16;
       break;
   case '|': valor=17;
       break;
   case '!': valor=18;
       break;
   case '=': valor=19;
       break;
   case '<':valor=20;
       break;
   case '>': valor=21;
       break;
   case '[': valor=22;
       break;
   case ']': valor=23;
       break;
   case '(': valor=24;
       break;
   case ')': valor=25;
       break;
   case ':': valor=26;
       break;
   case ';': valor=27;
       break;
   case '_': valor=28;
       break;
   case '%': valor=29;
       break;
   case ',': valor=30;
       break;

   default: valor=6;
   }
   return valor;
}
QString Tokens;
QString textoA;
QString texto;
void Token(int e){
    switch(e){
    case 100:
        //cout<<"Palabra reservada"<<endl;
        Tokens+=textoA+" Palabra reservada\n";
        break;
    case 101:
        //cout<<"Identificador"<<endl;
        Tokens+=textoA+" Identificador\n";
        break;
    case 102:
        //cout<<"Entero"<<endl;
        Tokens+=textoA+" Constante Entera\n";
        break;
    case 103:
        //cout<<"Constante decimal"<<endl;
        Tokens+=textoA+" Constante decimal\n";
        break;
    case 104:
        //cout<<"Constante con notación cientifica"<<endl;
        Tokens+=textoA+" Constante con notacion cientifica\n";
        break;
    case 105:
        //cout<<"Constante caracter"<<endl;
        Tokens+=textoA+" Constante caracter\n";
        break;
    case 106:
        //cout<<"Constante String"<<endl;
        Tokens+=textoA+" Constante String\n";
        break;
    case 107:
        //cout<<"Suma"<<endl;
        Tokens+=textoA+" Suma\n";
        break;
    case 108:
        //cout<<"Resta"<<endl;
        Tokens+=textoA+" Resta\n";
        break;
    case 109:
        //cout<<"Multiplicación"<<endl;
        Tokens+=textoA+" Multiplicacion\n";
        break;
    case 110:
        //cout<<"División"<<endl;
        Tokens+=textoA+" Division\n";
        break;
    case 111:
        //cout<<"Modulo"<<endl;
        Tokens+=textoA+" Modulo\n";
        break;
    case 112:
        //cout<<"Comentario"<<endl;
        Tokens+=textoA+" Comentario\n";
        break;
    case 113:
        //cout<<"Operador Y"<<endl;
        Tokens+=textoA+" Operador Y\n";
        break;
    case 114:
        //cout<<"Operador O"<<endl;
        Tokens+=textoA+" Operador O\n";
        break;
    case 115:
        //cout<<"Operador NOT"<<endl;
        Tokens+=textoA+" Operador NOT\n";
        break;
    case 116:
        //cout<<"Operador Diferente"<<endl;
        Tokens+=textoA+" Operador Diferente\n";
        break;
    case 117:
        //cout<<"Igual"<<endl;
        Tokens+=textoA+" Operador Igual\n";
        break;
    case 118:
        //cout<<"Mayor"<<endl;
        Tokens+=textoA+" Operador Mayor\n";
        break;
    case 119:
        //cout<<"Mayor Igual"<<endl;
        Tokens+=textoA+" Operador Mayor igual\n";
        break;
    case 120:
        //cout<<"Menor"<<endl;
        Tokens+=textoA+" Operador Menor\n";
        break;
    case 121:
        //cout<<"Menor Igual"<<endl;
        Tokens+=textoA+" Operador Menor Igual\n";
        break;
    case 122:
        //cout<<"Asignación"<<endl;
        Tokens+=textoA+" Operador de Asignacion\n";
        break;
    case 123:
        //cout<<"Signo dos puntos"<<endl;
        Tokens+=textoA+" Signo dos puntos\n";
        break;
    case 124:
        //cout<<"Signo de cierre"<<endl;
        Tokens+=textoA+" Signo de cierre ;\n";
        break;
    case 125:
        //cout<<"Punto"<<endl;
        Tokens+=textoA+" Punto\n";
        break;
    case 126:
        //cout<<"Parentesis abierto"<<endl;
        Tokens+=textoA+" Parentesis abierto\n";
        break;
    case 127:
        //cout<<"Parentesis cerrado"<<endl;
        Tokens+=textoA+" Paretesis cerrado\n";
        break;
    case 128:
        //cout<<"Corchete abierto"<<endl;
        Tokens+=textoA+" Corchete abierto\n";
        break;
    case 129:
        //cout<<"Corchete cerrado"<<endl;
        Tokens+=textoA+" Corchete cerrado\n";
        break;
    case 130:
        //cout<<"Es una coma"<<endl;
        Tokens+=textoA+" Es una coma\n";
        break;
    case 131:
        //cout<<#Es una libreria"<<endl;
        Tokens+=textoA+" Es una libreria\n";
        break;
    case 132:
        Tokens+=textoA+" Palabra reservada end\n";
        break;
    case 133:
        Tokens+=textoA+" Palabra reservada begin\n";
        break;
    case 134:
        Tokens+=textoA+" Palabra reservada class\n";
        break;
    case 135:
        Tokens+=textoA+" Palabra reservada import\n";
        break;
    case 136:
        Tokens+=textoA+" Palabra reservada def\n";
        break;
    case 137:
        Tokens+=textoA+" Palabra reservada as\n";
        break;
    case 138:
        Tokens+=textoA+" Palabra reservada integer\n";
        break;
    case 139:
        Tokens+=textoA+" Palabra reservada float\n";
        break;
    case 140:
        Tokens+=textoA+" Palabra reservada char\n";
        break;
    case 141:
        Tokens+=textoA+" Palabra reservada string\n";
        break;
    case 142:
        Tokens+=textoA+" Palabra reservada boolean\n";
        break;
    case 143:
        Tokens+=textoA+" Palabra reservada if\n";
        break;
    case 144:
        Tokens+=textoA+" Palabra reservada endif\n";
        break;
    case 145:
        Tokens+=textoA+" Palabra reservada else";
        break;
    case 146:
        Tokens+=textoA+" Palabra reservada endwhile\n";
        break;
    case 147:
        Tokens+=textoA+" Palabra reservada while\n";
        break;
    case 148:
        Tokens+=textoA+" Palabra reservada endfor\n";
        break;
    case 149:
        Tokens+=textoA+" Palabra reservada for\n";
        break;
    case 150:
        Tokens+=textoA+" Palabra reservada enter\n";
        break;
    case 151:
        Tokens+=textoA+" Palabra reservada write\n";
        break;
    case 152:
        Tokens+=textoA+" Palabra reservada read\n";
        break;
    case 153:
        Tokens+=textoA+" Palabra reservada principal\n";
        break;
    case 154:
        Tokens+=textoA+" Palabra reservada elseif\n";
        break;
    case 155:
        Tokens+=textoA+" Palabra reservada do\n";
        break;
    case 156:
        Tokens+=textoA+" Palabra reservada function\n";
        break;
    case 157:
        Tokens+=textoA+" Palabra reservada endfunction\n";
        break;
    case 158:
        Tokens+=textoA+" Palabra reservada null\n";
        break;
    case 159:
        Tokens+=textoA+" Palabra reservada include\n";
        break;
    }

}
QString errores;
void Errores(int e){
    switch(e){
    case 500:
        //cout<<"Error este simbolo no forma parte del lenguaje"<<endl;
        errores+=textoA+" Error este simbolo no forma parte del lenguaje\n";
        break;
    case 501:
        //cout<<"Error no puede declarar un identificador de esa manera"<<endl;
        errores+=textoA+" Error no puede declarar un identificador de esa manera\n";
        break;
    case 502:
        //cout<<"Error no puede escribir una constante usando letras"<<endl;
        errores+=textoA+" Error no puede escribir una constante usando letras\n";
        break;
    case 503:
        //cout<<"Error debe de escribir un numero o un signo positivo o negativo despues de la E o e"<<endl;
        errores+=textoA+" Error debe de escribir un numero o un signo positivo o negativo despues de la E o e\n";
        break;
    case 504:
        //cout<<"Error debe de escribir un numero para completar la expresión"<<endl;
        errores+=textoA+" Error debe de escribir un numero para completar la expresion\n";
        break;
    case 505:
        //cout<<"Error debe de escribir algo entre las '"<<endl;
        errores+=textoA+" Error debe escribir un caracter entre las '\n";
        break;
    case 506:
        //cout<<"Error indefinido"<<endl;
        errores+=textoA+" Error no cerro la '\n";
        break;
    case 507:
        //cout<<"No puede escribir algo entre la expresión"<<endl;
        errores+=textoA+" No puede escribir algo entre la expresion Y\n";
        break;
    case 508:
        //cout<<"No puede escribir algo entre la expresión"<<endl;
        errores+=textoA+" No puede escribir algo entre la expresion O\n";
        break;
    case 509:
        //cout<<"No puedes llamar a una libreria de esa manera
        errores+=textoA+" No puede declara una libreria de esa manera\n";
    case 510:
        errores+=textoA+" 510: Se esperaba la palabra class o import al inicio\n";
        break;
    case 511:
        errores+=textoA+" 511: Se esperaba la palabra class\n";
        break;
    case 512:
        errores+=textoA+" 512: Se esperaba la palabra import o class\n";
        break;
    case 513:
        errores+=textoA+" 513: Se esperaba un identificador, la palabra def o el inicio de un estatuto\n";
        break;
    case 514:
        errores+=textoA+" 514: Se esperaba un identificador\n";
        break;
    case 515:
        errores+=textoA+" 515: Se esperaba la palabra as o ','\n";
        break;
    case 516:
        errores+=textoA+" 516: Se esperaba un tipo de dato\n";
        break;
    case 517:
        errores+=textoA+" 517: Se esperaba un identificador o el  inicio o bien de un estatuto\n";
        break;
    case 518:
        errores+=textoA+" 518: Se esperaba la palabra if\n";
        break;
    case 519:
        errores+=textoA+" 519: Se esperaba la palabra else o endif\n";
        break;
    case 520:
        errores+=textoA+" 520: Se esperaba la palabra while\n";
        break;
    case 521:
        errores+=textoA+" 521: Se esperaba la palabra for\n";
        break;
    case 522:
        errores+=textoA+" 522: La expresión no puede iniciar de esa manera\n";
        break;
    case 523:
        errores+=textoA+" 523: Se esperaba un parentesis cerrado, el operador || o un signo de puntuación\n";
        break;
    case 524:
        errores+=textoA+" 524: La expresión no puede iniciar de esa manera\n";
        break;
    case 525:
        errores+=textoA+" 525: Se esperaba un parentesis cerrado, el operador || o un signo de puntuación\n";
        break;
    case 526:
        errores+=textoA+" 526: La expresión no puede iniciar de esa manera\n";
        break;
    case 527:
        errores+=textoA+" 527: La expresión no puede iniciar de esa manera\n";
        break;
    case 528:
        errores+=textoA+" 528: La expresión no puede iniciar de esa manera\n";
        break;
    case 529:
        errores+=textoA+" 529: Se esperaba un operador relacional o un signo de puntuación\n";
        break;
    case 530:
        errores+=textoA+" 530: La expresión no puede iniciar de esa forma\n";
        break;
    case 531:
        errores+=textoA+" 531: Se esperaba un parentesis cerrado, un operador o un signo de puntuación\n";
        break;
    case 532:
        errores+=textoA+" 532: La expresión no puede iniciar de esa forma\n";
        break;
    case 533:
        errores+=textoA+" 533: Se esperaba un parentesis cerrado, un operador o un signo de puntuación\n";
        break;
    case 534:
        errores+=textoA+" 534: Se esperaba un tipo de constante\n";
        break;
    case 535:
        errores+=textoA+" 535: Se esperaba un operador relacional\n";
        break;
    case 536:
        errores+=textoA+" 536: Se esperaba la palabra enter\n";
        break;
    case 537:
        errores+=textoA+" 537: Se esperaba un identificador\n";
        break;
    case 538:
        errores+=textoA+" 538: Se esperaba la palabra write\n";
        break;
    case 539:
        errores+=textoA+" 539: La expresión no puede iniciar de esa forma\n";
        break;
    case 540:
        errores+=textoA+" 540: Se esperaba un identificador o una coma\n";
        break;
    case 541:
        errores+=textoA+" 541: Se esperaba la palabra read\n";
        break;
    case 542:
        errores+=textoA+" 542: Se esperaba un identificador\n";
        break;
    case 543:
        errores+=textoA+" 543: Se esperaba un identificador o una coma\n";
        break;
    case 544:
        errores+="544: Error entre tipos\n";
        break;
    case 545:
        errores+=textoA+" 545: Variable no declarada\n";
        break;
    default:
        errores+="Revisa tu sintaxis quizas pusiste algo que no debe ir\n";
        break;
    }

}

int evaluaPR(){
    int conta=0;
    std::string cadenaStd = textoA.toStdString();
    for(int i=0;i<textoA.length();i++){
        char car=cadenaStd[i];
        if(car=='\n' || car=='\t' || car==32)
            conta++;
    }
    QString temp=textoA.mid(conta,textoA.length());

    if(textoA=="import" || temp=="import"){
        return 135;
    }
    if(textoA=="class" || temp=="class"){
        return 134;
    }
    if(textoA=="begin" || temp=="begin"){
        return 133;
    }
    if(textoA=="end" || temp=="end"){
        return 132;
    }
    if(textoA=="def" || temp=="def"){
        return 136;
    }
    if(textoA=="as" || temp=="as"){
        return 137;
    }
    if(textoA=="integer" || temp=="integer"){
        return 138;
    }
    if(textoA=="float" || temp=="float"){
        return 139;
    }
    if(textoA=="char" || temp=="char"){
        return 140;
    }
    if(textoA=="string" || temp=="string"){
        return 141;
    }
    if(textoA=="boolean" || temp=="boolean"){
        return 142;
    }
    if(textoA=="if" || temp=="if"){
        return 143;
    }
    if(textoA=="endif" || temp=="endif"){
        return 144;
    }
    if(textoA=="else" || temp=="else"){
        return 145;
    }
    if(textoA=="endwhile" || temp=="endwhile"){
        return 146;
    }
    if(textoA=="while" || temp=="while"){
        return 147;
    }
    if(textoA=="endfor" || temp=="endfor"){
        return 148;
    }
    if(textoA=="for" || temp=="for"){
        return 149;
    }
    if(textoA=="enter" || temp=="enter"){
        return 150;
    }
    if(textoA=="write" || temp=="write"){
        return 151;
    }
    if(textoA=="read" || temp=="read"){
        return 152;
    }
    if(textoA=="principal" || temp=="principal"){
        return 153;
    }
    if(textoA=="elseif" || temp=="elseif"){
        return 154;
    }
    if(textoA=="do" || temp=="do"){
        return 155;
    }
    if(textoA=="function" || temp=="function"){
        return 156;
    }
    if(textoA=="endfunction" || temp=="endfunction"){
        return 157;
    }
    if(textoA=="null" || temp=="null"){
        return 158;
    }
    if(textoA=="include" || temp=="include"){
        return 159;
    }
    return 101;
}

int Analiza(QString cadena){
    std::string cadenaStd = cadena.toStdString();
    edo=0;
    int col;
    char car=cadenaStd[0];

    textoA="";

    int numero=1;
    while(edo<=32){

        col=Relaciona(car);

        edo=matriz[edo][col];

        if(edo==107)
            textoA.append('+');
        if(edo==108)
            textoA.append('-');
        if(edo==109)
            textoA.append('*');
        if(edo==125)
            textoA.append('.');
        if(edo==128)
            textoA.append('[');
        if(edo==129)
            textoA.append(']');
        if(edo==126)
            textoA.append('(');
        if(edo==127)
            textoA.append(')');
        if(edo==124)
            textoA.append(';');
        if(edo==123)
            textoA.append(':');
        if(edo==111)
            textoA.append('%');
        if(edo==130)
            textoA.append(',');

        if(edo<100 || edo>=500){
            textoA.append(car);

        }
        if(edo==103 && (car=='e' || car=='E')){
            if(car=='e')
                textoA.append('e');
            if(car=='E')
                textoA.append('E');
            edo=7;
        }



        if(edo==119 && car=='='){
            textoA.append('=');
            edo=119;
        }

        if(edo==121 && car=='='){
            textoA.append('=');
            edo=121;
        }

        if(edo==116 && car=='='){
            textoA.append('=');
            edo=116;
        }

        if(edo==115 && car=='='){
            textoA.append('=');
            edo=116;
        }

        if(edo==117 && car=='='){
            textoA.append('=');
            edo=117;
        }



        if(edo==112 && car=='/'){
            textoA.append('/');
            edo=112;
        }

        if(edo==10 && car!=39){
            textoA.append(car);
            edo=11;
        }else if(edo==10 && car!=39){
            textoA.append(car);
            edo=105;
        }
        if(edo==105 && car==39){
            textoA.append(car);
            edo=105;
        }
        if(edo==100){
            edo=evaluaPR();
        }

        if(edo==22 && car=='l'){
            edo=22;
        }

        if(edo==22 && car=='y'){
            edo=22;
        }

        if(edo==22 && car=='e'){
            edo=131;
        }

        if(edo==101 && car=='.'){
            textoA.append('.');
            edo=22;
        }

        if(edo==106 && car=='"'){
            textoA.append(car);
            edo=106;
        }

        car=cadenaStd[numero];

        numero++;

    }
    if(textoA=="&&" || textoA.contains("&&")){
        edo=113;
    }else if (textoA.contains("&")) {
        edo=507;
    }
    if(textoA=="||" || textoA.contains("||")){
        edo=114;
    }else if (textoA.contains("|")) {
        edo=508;
    }

    //Elimina lo que hay enfrente de una cadena
    int conta=0;

    for(int i=0;i<textoA.length();i++){
        char car=cadenaStd[i];
        if(car==' ' || car=='\t' || car==32 )
            conta++;
    }
    int longitud=textoA.length();
    texto=texto.remove(0,longitud);
    if(conta!=0){
        textoA.remove(0,conta);
    }

    if(edo>=100 && edo<=199){
        Token(edo);
    }else{
        Errores(edo);
    }

return edo;
}

int RelacionaGramatica(int estado){
    switch(estado){
    case 100:
        //cout<<"Palabra reservada"<<endl;

        break;
    case 101:
        //cout<<"Identificador"<<endl;
        return 3;
        break;
    case 102:
        //cout<<"Entero"<<endl;
        return 36;
        break;
    case 103:
        //cout<<"Constante decimal"<<endl;
        return 37;
        break;
    case 104:
        //cout<<"Constante con notación cientifica"<<endl;
        return 38;
        break;
    case 105:
        //cout<<"Constante caracter"<<endl;
        return 39;
        break;
    case 106:
        //cout<<"Constante String"<<endl;
        return 40;
        break;
    case 107:
        //cout<<"Suma"<<endl;
        return 31;
        break;
    case 108:
        //cout<<"Resta"<<endl;
        return 32;
        break;
    case 109:
        //cout<<"Multiplicación"<<endl;
        return 33;
        break;
    case 110:
        //cout<<"División"<<endl;
        return 34;
        break;
    case 111:
        //cout<<"Modulo"<<endl;
        return 35;
        break;
    case 112:
        //cout<<"Comentario"<<endl;

        break;
    case 113:
        //cout<<"Operador Y"<<endl;
        return 22;
        break;
    case 114:
        //cout<<"Operador O"<<endl;
        return 21;
        break;
    case 115:
        //cout<<"Operador NOT"<<endl;
        return 23;
        break;
    case 116:
        //cout<<"Operador Diferente"<<endl;
        return 26;
        break;
    case 117:
        //cout<<"Igual"<<endl;
        return 25;
        break;
    case 118:
        //cout<<"Mayor"<<endl;
        return 29;
        break;
    case 119:
        //cout<<"Mayor Igual"<<endl;
        return 30;
        break;
    case 120:
        //cout<<"Menor"<<endl;
        return 27;
        break;
    case 121:
        //cout<<"Menor Igual"<<endl;
        return 28;
        break;
    case 122:
        //cout<<"Asignación"<<endl;
        return 24;
        break;
    case 123:
        //cout<<"Signo dos puntos"<<endl;
        return 45;
        break;
    case 124:
        //cout<<"Signo de cierre"<<endl;
        return 46;
        break;
    case 125:
        //cout<<"Punto"<<endl;

        break;
    case 126:
        //cout<<"Parentesis abierto"<<endl;
        return 13;
        break;
    case 127:
        //cout<<"Parentesis cerrado"<<endl;
        return 14;
        break;
    case 128:
        //cout<<"Corchete abierto"<<endl;

        break;
    case 129:
        //cout<<"Corchete cerrado"<<endl;

        break;
    case 130:
        //cout<<"Es una coma"<<endl;
        return 44;
        break;
    case 131:
        //cout<<#Es una libreria"<<endl;
        return 1;
        break;
    case 132:
        return 47;
        break;
    case 133:
        return 4;
        break;
    case 134:
        return 2;
        break;
    case 135:
        return 0;
        break;
    case 136:
        return 5;
        break;
    case 137:
        return 6;
        break;
    case 138:
        return 7;
        break;
    case 139:
        return 8;
        break;
    case 140:
        return 9;
        break;
    case 141:
        return 10;
        break;
    case 142:
        return 11;
        break;
    case 143:
        return 12;
        break;
    case 144:
        return 16;
        break;
    case 145:
        return 15;
        break;
    case 146:
        return 18;
        break;
    case 147:
        return 17;
        break;
    case 148:
        return 20;
        break;
    case 149:
        return 19;
        break;
    case 150:
        return 41;
        break;
    case 151:
        return 42;
        break;
    case 152:
        return 43;
        break;
    default:
        return 48;
    }
}

void LlenarPilaProduccion(int fila){
    for(int i=0;i<15;i++){
        if(producciones[fila][i]!=0){
        pilaEjecucion.push(producciones[fila][i]);
        }
    }
}


QString pasosPila;
void imprimePila(){
    for(int i=0;i<pilaEjecucion.size();i++){
        QString ele=evaluaElemento(pilaEjecucion.at(i));
        pasosPila+=ele+" ";
    }
    pasosPila+="\n\n";
}

int buscaTipo(){
    int pos=-1;
    if(!pilaOperandosBusqueda.empty()){
        for(int i=0;i<pilaOperandosBusqueda.size();i++){
            if(textoA.contains(pilaOperandosBusqueda.at(i))){
                pos=i;
                break;
            }
        }
    }
    return pos;
}
int relacionaMatrizTipos(int edo){
    if(edo>=107 && edo<=109)
        return 2;
    if(edo==110)
        return 3;
    if(edo==111)
        return 4;
    if(edo>=116 && edo<=121)
        return 5;
    if(edo>=113 && edo<=115)
        return 6;
}
void imprimeYLimpiaPilas(){
    pilaTipos.pop();
    imprimePilaTipos();
    pilaTipos.pop();
    imprimePilaTipos();
}

//LLenar la tabla con cuadruplos
void MainWindow::LlenarCuadruplo(){
    int con=0;
    while(con<cuadruplos.size()){
        ui->tablaCuadruplos->insertRow(ui->tablaCuadruplos->rowCount());
        int fila=ui->tablaCuadruplos->rowCount()-1;
        ui->tablaCuadruplos->setItem(fila,0,new QTableWidgetItem(cuadruplos.at(con)->getN()));
        ui->tablaCuadruplos->setItem(fila,1,new QTableWidgetItem(cuadruplos.at(con)->getOper()));
        ui->tablaCuadruplos->setItem(fila,2,new QTableWidgetItem(cuadruplos.at(con)->getOp1()));
        ui->tablaCuadruplos->setItem(fila,3,new QTableWidgetItem(cuadruplos.at(con)->getOp2()));
        ui->tablaCuadruplos->setItem(fila,4,new QTableWidgetItem(cuadruplos.at(con)->getRes()));
        con++;
    }
}

QString encuentraAsignacion(QString operando){
    int i=0;
    if(cuadruplos.size()>0){
        QString encontrado="";
        while(i<cuadruplos.size()){

            if(cuadruplos.at(i)->getOper().contains("=") && cuadruplos.at(i)->getOp1().contains(operando)){
                encontrado=cuadruplos.at(i)->getRes();

            }
            i++;
        }
        return encontrado;
    }
    return "";
}

bool verificaOperandos(QString op1,QString op2,QString oper){
        bool estaOp1,estaOp2;
        bool esFloatOp1,esFloatOp2;
        for(int i=0;i<op1.length();i++){
          std::string cadenaOp1 = op1.toStdString();
          if(cadenaOp1[i]=='.'){
              esFloatOp1=true;
              break;
          }
          if(!(cadenaOp1[i]>=48 && cadenaOp1[i]<=57)){
              estaOp1=true;
              break;
          }
        }
        for(int i=0;i<op2.length();i++){
          std::string cadenaOp2 = op2.toStdString();
          if(cadenaOp2[i]=='.'){
              esFloatOp2=true;
              break;
          }
          if(!(cadenaOp2[i]>=48 && cadenaOp2[i]<=57)){
              estaOp2=true;
              break;
          }
        }
        QString encuentraOp;
        QString operandoCiclo="";
        if(estaOp1){
            encuentraOp=encuentraAsignacion(op1);
            if(pilaWhile.size()>0){
                operandoCiclo=cuadruplos.at(pilaWhile.top()-1)->getOp1();
            }
            if(pilaFor.size()>0){
                operandoCiclo=cuadruplos.at(pilaFor.top()-1)->getOp1();
            }
            if(encuentraOp!=""){
                if(operandoCiclo!=""){
                    if(op1.contains(operandoCiclo)==false){
                        op1=encuentraOp;
                        estaOp1=false;
                    }
                }else{
                    op1=encuentraOp;
                    estaOp1=false;
                }

            }
        }
        if(estaOp2){
            encuentraOp=encuentraAsignacion(op2);
            if(pilaWhile.size()>0){
                operandoCiclo=cuadruplos.at(pilaWhile.top()-1)->getOp1();
            }
            if(pilaFor.size()>0){
                operandoCiclo=cuadruplos.at(pilaFor.top()-1)->getOp1();
            }
            if(encuentraOp!=""){
                if(operandoCiclo!=""){
                    if(op2.contains(operandoCiclo)==false){
                        op2=encuentraOp;
                        estaOp2=false;
                    }
                }else{
                    op2=encuentraOp;
                    estaOp2=false;
                }
            }
        }
        if((estaOp1 || estaOp2)){
            if(oper=="*" && (estaOp1 && estaOp2==false)){
                --contRes;
                pilaOperandos.pop();
                for(int i=1;i<=op2.toInt();i++){
                    pilaOperandos.push(op1);
                    imprimePilaOperandos();
                }
                for(int i=1;i<=(op2.toInt()-1);i++){
                    QString oper="+";
                    QString ope2=pilaOperandos.top();
                    pilaOperandos.pop();
                    imprimePilaOperandos();
                    QString ope1=pilaOperandos.top();
                    pilaOperandos.pop();
                    imprimePilaOperandos();
                    QString res="R"+QString::number(++contRes);
                    pilaOperandos.push(res);
                    imprimePilaOperandos();
                    formaCuadruplo(QString::number(++contCuadruplo),oper,ope1,ope2,res);
                }
                return false;
            }
            else
                return true;
        }else{
            if(esFloatOp1 || esFloatOp2){
                float ope1=op1.toFloat();
                float ope2=op2.toFloat();
                float res=0;
                if(oper=="+"){
                   res=ope1+ope2;
                   pilaOperandos.pop();
                   imprimePilaOperandos();
                   pilaOperandos.push(QString::number(res));
                   imprimePilaOperandos();
                   contRes--;
                }else if (oper=="-") {
                   res=ope1-ope2;
                   pilaOperandos.pop();
                   imprimePilaOperandos();
                   pilaOperandos.push(QString::number(res));
                   imprimePilaOperandos();
                   contRes--;
                }else if (oper=="*") {
                    res=ope1*ope2;
                    pilaOperandos.pop();
                    imprimePilaOperandos();
                    pilaOperandos.push(QString::number(res));
                    imprimePilaOperandos();
                    contRes--;
                }else if(oper=="/"){
                    try {
                        res=ope1/ope2;
                        pilaOperandos.pop();
                        imprimePilaOperandos();
                        pilaOperandos.push(QString::number(res));
                        imprimePilaOperandos();
                        contRes--;
                    } catch (int e) {
                        sinError=false;
                        errores+="Hay una división sobre 0\n";
                    }
                }else{
                   return true;

                }
                return false;
            }else{
                int ope1=op1.toInt();
                int ope2=op2.toInt();
                int res=0;
                if(oper=="+"){
                   res=ope1+ope2;
                   pilaOperandos.pop();
                   imprimePilaOperandos();
                   pilaOperandos.push(QString::number(res));
                   imprimePilaOperandos();
                   contRes--;
                }else if (oper=="-") {
                   res=ope1-ope2;
                   pilaOperandos.pop();
                   imprimePilaOperandos();
                   pilaOperandos.push(QString::number(res));
                   imprimePilaOperandos();
                   contRes--;
                }else if (oper=="*") {
                    res=ope1*ope2;
                    pilaOperandos.pop();
                    imprimePilaOperandos();
                    pilaOperandos.push(QString::number(res));
                    imprimePilaOperandos();
                    contRes--;
                }else if(oper=="/"){
                    try {
                        float ope1=op1.toFloat();
                        float ope2=op2.toFloat();
                        float res=ope1/ope2;
                        pilaOperandos.pop();
                        imprimePilaOperandos();
                        pilaOperandos.push(QString::number(res));
                        imprimePilaOperandos();
                        contRes--;
                    } catch (int e) {
                        sinError=false;
                        errores+="Hay una división sobre 0\n";
                    }
                }else if(oper=="%"){
                    ope1=op1.toInt();
                    ope2=op2.toInt();
                    res=ope1%ope2;
                    pilaOperandos.pop();
                    imprimePilaOperandos();
                    pilaOperandos.push(QString::number(res));
                    imprimePilaOperandos();
                    contRes--;
                }else{
                    return true;
                }
                return false;
            }

        }

}
void relacionaTiposOper(){
    int op1=pilaTipos.top();
    int op2=pilaTipos.at(pilaTipos.size()-2);
    if(pilaOperadores.top()>=107 && pilaOperadores.top()<=111){
        int fila=0;
        for(int i=0;i<25;i++){
            if(matrizDeTipos[i][0]==op1 && matrizDeTipos[i][1]==op2){
                fila=i;
                break;
            }
        }
        int columna=relacionaMatrizTipos(pilaOperadores.top());
        int supuesto=matrizDeTipos[fila][columna];
        if(supuesto<500){
            imprimeYLimpiaPilas();
            pilaTipos.push(supuesto);
            QString oper=evaluaElemento(pilaOperadores.top());
            pilaOperadores.pop();
            imprimePilaOperadores();
            QString op2=pilaOperandos.top();
            pilaOperandos.pop();
            imprimePilaOperandos();
            QString op1=pilaOperandos.top();
            pilaOperandos.pop();
            imprimePilaOperandos();
            QString res="R"+QString::number(++contRes);
            pilaOperandos.push(res);
            imprimePilaOperandos();
            if(verificaOperandos(op1,op2,oper)){
                if(op2!="0" && op1!="0"){
                    if(encuentraAsignacion(op1)!="0" && encuentraAsignacion(op2)!="0"){
                        formaCuadruplo(QString::number(++contCuadruplo),oper,op1,op2,res);
                    }else{
                        pilaOperandos.pop();
                        --contCuadruplo;
                        --contRes;
                        if(encuentraAsignacion(op1)!="0"){
                            pilaOperandos.push(op1);
                        }else{
                            pilaOperandos.push(op2);
                        }
                    }
                }else{
                    pilaOperandos.pop();
                    --contCuadruplo;
                    --contRes;
                    if(op1=="0"){
                        pilaOperandos.push(op2);
                    }else{
                        pilaOperandos.push(op1);
                    }
                }
            }
        }else{
            Errores(544);
            supuesto=op1;
            pilaTipos.pop();
            imprimePilaTipos();
            pilaTipos.pop();
            imprimePilaTipos();
            pilaTipos.push(supuesto);
            imprimePilaTipos();
            QString oper=evaluaElemento(pilaOperadores.top());
            pilaOperadores.pop();
            imprimePilaOperadores();
            QString op2=pilaOperandos.top();
            pilaOperandos.pop();
            imprimePilaOperandos();
            QString op1=pilaOperandos.top();
            pilaOperandos.pop();
            imprimePilaOperandos();
            QString res="R"+QString::number(++contRes);
            pilaOperandos.push(res);
            imprimePilaOperandos();
            if(verificaOperandos(op1,op2,oper)){
                if(op2!="0" && op1!="0"){
                    if(encuentraAsignacion(op1)!="0" && encuentraAsignacion(op2)!="0"){

                        formaCuadruplo(QString::number(++contCuadruplo),oper,op1,op2,res);
                    }else{
                        pilaOperandos.pop();
                        --contCuadruplo;
                        --contRes;
                        if(encuentraAsignacion(op1)!="0"){
                            pilaOperandos.push(op2);
                        }else{
                            pilaOperandos.push(op1);
                        }
                    }

                }else{
                    pilaOperandos.pop();
                    --contCuadruplo;
                    --contRes;
                    if(op1=="0"){
                        pilaOperandos.push(op2);
                    }else{
                        pilaOperandos.push(op1);
                    }
                }
            }

            sinError=false;
        }
    }else if(pilaOperadores.top()>=116 && pilaOperadores.top()<=121){
        int fila=0;
        for(int i=0;i<25;i++){
            if(matrizDeTipos[i][0]==op1 && matrizDeTipos[i][1]==op2){
                fila=i;
                break;
            }
        }
        int columna=relacionaMatrizTipos(pilaOperadores.top());
        int supuesto=matrizDeTipos[fila][columna];
        if(supuesto<500){
            imprimeYLimpiaPilas();
            pilaTipos.push(supuesto);
            QString oper=evaluaElemento(pilaOperadores.top());
            pilaOperadores.pop();
            imprimePilaOperadores();
            QString op2=pilaOperandos.top();
            pilaOperandos.pop();
            imprimePilaOperandos();
            QString op1=pilaOperandos.top();
            pilaOperandos.pop();
            imprimePilaOperandos();
            QString res="R"+QString::number(++contRes);
            pilaOperandos.push(res);
            imprimePilaOperandos();
            formaCuadruplo(QString::number(++contCuadruplo),oper,op1,op2,res);
        }else{
            Errores(544);
            supuesto=op1;
            pilaTipos.pop();
            imprimePilaTipos();
            pilaTipos.pop();
            imprimePilaTipos();
            pilaTipos.push(supuesto);
            imprimePilaTipos();
            QString oper=evaluaElemento(pilaOperadores.top());
            pilaOperadores.pop();
            imprimePilaOperadores();
            QString op2=pilaOperandos.top();
            pilaOperandos.pop();
            imprimePilaOperandos();
            QString op1=pilaOperandos.top();
            pilaOperandos.pop();
            imprimePilaOperandos();
            QString res="R"+QString::number(++contRes);
            pilaOperandos.push(res);
            imprimePilaOperandos();
            formaCuadruplo(QString::number(++contCuadruplo),oper,op1,op2,res);
            sinError=false;
        }
    }else if(pilaOperadores.top()>=113 && pilaOperadores.top()<=115){
        int fila=0;
        for(int i=0;i<25;i++){
            if(matrizDeTipos[i][0]==op1 && matrizDeTipos[i][1]==op2){
                fila=i;
                break;
            }
        }
        int columna=relacionaMatrizTipos(pilaOperadores.top());
        int supuesto=matrizDeTipos[fila][columna];
        if(supuesto<500){
            imprimeYLimpiaPilas();
            pilaTipos.push(supuesto);
            QString oper=evaluaElemento(pilaOperadores.top());
            pilaOperadores.pop();
            imprimePilaOperadores();
            QString op2=pilaOperandos.top();
            pilaOperandos.pop();
            imprimePilaOperandos();
            QString op1=pilaOperandos.top();
            pilaOperandos.pop();
            imprimePilaOperandos();
            QString res="R"+QString::number(++contRes);
            pilaOperandos.push(res);
            imprimePilaOperandos();
            formaCuadruplo(QString::number(++contCuadruplo),oper,op1,op2,res);

        }else{
            Errores(544);
            supuesto=op1;
            pilaTipos.pop();
            imprimePilaTipos();
            pilaTipos.pop();
            imprimePilaTipos();
            pilaTipos.push(supuesto);
            imprimePilaTipos();
            QString oper=evaluaElemento(pilaOperadores.top());
            pilaOperadores.pop();
            imprimePilaOperadores();
            QString op2=pilaOperandos.top();
            pilaOperandos.pop();
            imprimePilaOperandos();
            QString op1=pilaOperandos.top();
            pilaOperandos.pop();
            imprimePilaOperandos();
            QString res="R"+QString::number(++contRes);
            pilaOperandos.push(res);
            imprimePilaOperandos();
            formaCuadruplo(QString::number(++contCuadruplo),oper,op1,op2,res);

            sinError=false;
        }
    }
}

void accionesSemanticayCodigoIntermedio(int accion){
    bool existeOperando=false;
    QString oper,op1,op2,res;
    QStack<int> posicionesCuadruplos;
    QList<QSharedPointer<cuadruplo>> cuadruplosACambiar;
    int inicio,final;
    switch(accion){
        case 700:
            pilaOperandos.push(textoA);
            pilaOperandosBusqueda.push(textoA);
            for(int i=0;i<pilaOperandosBusqueda.size();i++){
                if(pilaOperandosBusqueda.size()>1){
                    if(pilaOperandosBusqueda.at(i).contains(pilaOperandos.top()))
                        existeOperando=true;
                        break;
                }
            }
            if(existeOperando){
                sinError=false;
                pilaOperandos.pop();
                pilaOperandosBusqueda.pop();
                errores+=textoA+" Variable ya definida\n";
                existeOperando=false;
            }
            imprimePilaOperandos();
            break;
        case 701:
            while(!pilaOperandos.isEmpty()){
                pilaTiposBusqueda.push(edo);
                pilaOperandos.pop();
            }
            imprimePilaOperandos();
            break;
        case 702:
            if(edo>=102 && edo<=106){
                if(edo==102){
                    pilaTipos.push(138);
                    imprimePilaTipos();
                }
                if(edo==103 || edo==104){
                    pilaTipos.push(139);
                    imprimePilaTipos();
                }
                if(edo==105){
                    pilaTipos.push(140);
                    imprimePilaTipos();
                }
                if(edo==106){
                    pilaTipos.push(141);
                    imprimePilaTipos();
                }
            }else{
                int i=buscaTipo();
                if(i==-1){
                    Errores(545);
                    pilaTipos.push(138);
                    imprimePilaTipos();
                    sinError=false;
                }else{
                    pilaTipos.push(pilaTiposBusqueda.at(i));
                    imprimePilaTipos();
            }   
            }
            pilaOperandos.push(textoA);
            if(estaPresenteRead || estaPresenteWrite){
                imprimePilaOperandos();
                contadorRaW++;
            }
            break;
        case 703:
            pilaOperadores.push(edo);
            imprimePilaOperadores();
            break;
        case 704:
            relacionaTiposOper();
            break;
        case 705:
            pilaOperadores.push(-2);
            imprimePilaOperadores();
            break;
        case 706:
            pilaOperadores.pop();
            imprimePilaOperadores();
            break;
        case 707://Accion salto en falso
            //Logica de la accion: pila_operandos.push()y pila_saltos(contcuadruplos).
            oper="SF";
            op1=pilaOperandos.top();
            pilaOperandos.pop();
            imprimePilaOperandos();
            formaCuadruplo(QString::number(++contCuadruplo),oper,op1,"","");
            pilaSaltos.append(contCuadruplo);
            imprimePilaSaltos();
            break;
        case 708:
            //Se hace un salto incondicional, tope_saltos(contcuadruplos+1) y pila:saltos(pos_actual)
            for(int i=0;i<cuadruplos.size();i++){
                if(cuadruplos.at(i)->getN()==QString::number(pilaSaltos.top())){
                    cuadruplos.at(i)->setRes(QString::number(contCuadruplo+2));
                    break;
                }
            }
            pilaSaltos.pop();
            imprimePilaSaltos();
            oper="SI";
            formaCuadruplo(QString::number(++contCuadruplo),oper,"","","");
            pilaSaltos.append(contCuadruplo);
            imprimePilaSaltos();
            break;
         case 709:
            //Rellenar tope_saltos(ContCuadruplo+1)
            for(int i=0;i<cuadruplos.size();i++){
                if(cuadruplos.at(i)->getN()==QString::number(pilaSaltos.top())){
                    cuadruplos.at(i)->setRes(QString::number(contCuadruplo+1));
                    break;
                }
            }
            pilaSaltos.pop();
            imprimePilaSaltos();
            break;
         case 722:
        pilaFor.push(contCuadruplo);
        for(int i=0;i<pilaFor.size();i++){
            cout<<pilaFor.at(i)<<endl;
        }
        inicio=pilaFor.at(pilaFor.size()-2);
        final=pilaFor.top();
        for(int i=inicio;i<=final;i++){
            cuadruplosR.append(cuadruplos.at(i-1));
            if(cuadruplos.at(i-1)->getOper().contains("=") && cuadruplos.at(i-1)->getRes().contains("R")==false){
                posicionesCuadruplos.push(i-1);
                cuadruplosACambiar.append(cuadruplos.at(i-1));
            }
        }


        if(posicionesCuadruplos.size()>0){
            for (int i=0;i<posicionesCuadruplos.size();i++) {
                cuadruplosR.removeAt(posicionesCuadruplos.at(i));
            }

            for (int i=0;i<cuadruplosACambiar.size();i++) {
                cuadruplosR.push_front(cuadruplosACambiar.at(i));
            }

            for(int i=0;i<cuadruplosR.size();i++){
                cuadruplosR.at(i)->setN(QString::number(inicio));
                ++inicio;
            }



            for(int i=0;i<cuadruplosR.size();i++){
                cuadruplos.append(cuadruplosR.at(i));
            }

            for(int i=cuadruplosR.size()-1;i>=0;i--){
                cuadruplos.removeAt(i);
            }



        }
            break;
         case 721:
            pilaFor.push(contCuadruplo+1);
            break;
         case 720://Optimizar asignaciones while
            pilaWhile.push(contCuadruplo);
            for(int i=0;i<pilaWhile.size();i++){
                cout<<pilaWhile.at(i)<<endl;
            }
            inicio=pilaWhile.at(pilaWhile.size()-2);
            final=pilaWhile.top();
            for(int i=inicio;i<=final;i++){
                cuadruplosR.append(cuadruplos.at(i-1));
                if(cuadruplos.at(i-1)->getOper().contains("=") && cuadruplos.at(i-1)->getRes().contains("R")==false){
                    posicionesCuadruplos.push(i-1);
                    cuadruplosACambiar.append(cuadruplos.at(i-1));
                }
            }


            if(posicionesCuadruplos.size()>0){
                for (int i=0;i<posicionesCuadruplos.size();i++) {
                    cuadruplosR.removeAt(posicionesCuadruplos.at(i));
                }

                for (int i=0;i<cuadruplosACambiar.size();i++) {
                    cuadruplosR.push_front(cuadruplosACambiar.at(i));
                }

                for(int i=0;i<cuadruplosR.size();i++){
                    cuadruplosR.at(i)->setN(QString::number(inicio));
                    ++inicio;
                }



                for(int i=0;i<cuadruplosR.size();i++){
                    cuadruplos.append(cuadruplosR.at(i));
                }

                for(int i=cuadruplosR.size()-1;i>=0;i--){
                    cuadruplos.removeAt(i);
                }



            }



            break;
         case 719:
            pilaWhile.push(contCuadruplo+1);
            break;
         case 718://Agrega un cuadruplo limpio lo cual indica fin de la ejecución
            formaCuadruplo(QString::number(++contCuadruplo),"","","","");
            break;
         case 717://Concatena lo necesario para estatutos write y read
            for(int i=0;i<contadorRaW;i++){
                if(estaPresenteRead){
                   oper="read";
                }
                if(estaPresenteWrite){
                   oper="write";
                }
                formaCuadruplo(QString::number(++contCuadruplo),oper,"","",pilaOperandos.at(i));

            }

            while(!pilaOperandos.isEmpty()){
                pilaOperandos.remove(0);
                imprimePilaOperandos();
            }
            contadorRaW=0;
            estaPresenteRead=false;
            estaPresenteWrite=false;
            break;
        case 716://Agrega el enter, forma cuadruplo y elimina el tope de la pila
            pilaOperadores.push(150);
            imprimePilaOperadores();
            formaCuadruplo(QString::number(++contCuadruplo),"enter","","","");
            pilaOperadores.pop();
            imprimePilaOperadores();
            break;
        case 715://Guardar siguiente cuadruplo para el ciclo while
            pilaSaltos.push(contCuadruplo+1);
            imprimePilaSaltos();
            break;
        case 714://Llenar top-1 con contador de cuadruplos,llenar top de la pila de saltos con contadorcuadruplos+1 y limpiar pilas
            oper="SI";
            formaCuadruplo(QString::number(++contCuadruplo),oper,"","",QString::number(pilaSaltos.at(pilaSaltos.size()-2)));
            for(int i=0;i<cuadruplos.size();i++){
                if(cuadruplos.at(i)->getN()==QString::number(pilaSaltos.top())){
                    cuadruplos.at(i)->setRes(QString::number(contCuadruplo+1));
                    break;
                }
            }
            pilaSaltos.pop();
            imprimePilaSaltos();
            pilaSaltos.pop();
            imprimePilaSaltos();
            break;
        case 713:
            if(cuadruplos.at(cuadruplos.size()-1)->getOper()=="<" || cuadruplos.at(cuadruplos.size()-1)->getOper()=="<="){
                oper="SF";
                op1=pilaOperandos.top();
                pilaOperandos.pop();
                imprimePilaOperandos();
            }else{
                oper="SV";
                op1=pilaOperandos.top();
                pilaOperandos.pop();
                imprimePilaOperandos();
            }
            formaCuadruplo(QString::number(++contCuadruplo),oper,op1,"","");
            pilaSaltos.append(contCuadruplo);
            imprimePilaSaltos();
            break;
        case 712://Guardar siguiente cuadruplo para el ciclo for
            pilaSaltos.push(contCuadruplo+2);
            imprimePilaSaltos();
        case 711://Formar cuadruplo de asignación
            int op1=pilaTipos.top();
            int op2=pilaTipos.at(pilaTipos.size()-2);
            if(pilaOperadores.top()==122){
                    if(op1==op2){
                        imprimeYLimpiaPilas();
                        QString oper=evaluaElemento(pilaOperadores.top());
                        pilaOperadores.pop();
                        imprimePilaOperadores();
                        QString op2="";
                        QString res=pilaOperandos.top();
                        pilaOperandos.pop();
                        imprimePilaOperandos();
                        QString op1=pilaOperandos.top();
                        pilaOperandos.pop();
                        imprimePilaOperandos();
                        formaCuadruplo(QString::number(++contCuadruplo),oper,op1,op2,res);
                    }else{
                        Errores(544);
                        imprimeYLimpiaPilas();
                        QString oper=evaluaElemento(pilaOperadores.top());
                        pilaOperadores.pop();
                        imprimePilaOperadores();
                        QString op2="";
                        QString res=pilaOperandos.top();
                        pilaOperandos.pop();
                        imprimePilaOperandos();
                        QString op1=pilaOperandos.top();
                        pilaOperandos.pop();
                        imprimePilaOperandos();
                        formaCuadruplo(QString::number(++contCuadruplo),oper,op1,op2,res);
                        sinError=false;
                    }
            }
            break;


    }
}

void ConstruyeGramatica(){
    int token=0,edoMP=0;
    int colMP=0,filaMP=0;
    while(!pilaEjecucion.empty()){
        pilaEjecucion.pop();
    }
    pilaEjecucion.push('$');
    pilaEjecucion.push(1);
    bool quieroToken=true,llena=true;
    imprimePila();
    sinError=true;
    while(!pilaEjecucion.empty()){

        if(quieroToken){
            token=Analiza(texto);
            quieroToken=false;
        }

        if(llena){
            colMP=RelacionaGramatica(token);
            filaMP=pilaEjecucion.top()-1;
            pilaEjecucion.pop();
            edoMP=matrizPredictiva[filaMP][colMP]-1;
            LlenarPilaProduccion(edoMP);
            llena=false;
            imprimePila();
        }
        if(pilaEjecucion.top()>=100){
            if(pilaEjecucion.top()==token){
                pilaEjecucion.pop();
                if(texto!=""){
                    quieroToken=true;
                }
                imprimePila();
                if(pilaEjecucion.top()>=700 && pilaEjecucion.top()<800){
                    accionesSemanticayCodigoIntermedio(pilaEjecucion.top());
                    pilaEjecucion.pop();
                    if(pilaEjecucion.top()>=700){
                        accionesSemanticayCodigoIntermedio(pilaEjecucion.top());
                        pilaEjecucion.pop();
                    }
                }
                if(token==151){
                    estaPresenteWrite=true;
                }
                if(token==152){
                    estaPresenteRead=true;
                }
                if((token>=101 && token<=106) || token==127){
                    if(!pilaOperadores.empty()){
                        imprimePilaOperandos();
                        accionesSemanticayCodigoIntermedio(704);
                    }
                }


            }else{
                QString tr=evaluaElemento(token);
                QString tp=evaluaElemento(pilaEjecucion.top());
                errores+="Hay errores en la sintaxis porque se esperaba un "+tp+" y se recibio un "+tr+"\n";
                token=pilaEjecucion.top();
                sinError=false;
            }
        }else{
            if(edoMP>500){
                Errores(edoMP+1);
                sinError=false;
                pilaEjecucion.pop();
                imprimePila();
            }else{
                if(pilaEjecucion.top()>0 && pilaEjecucion.top()<=34){
                    llena=true;
                }
            if(pilaEjecucion.top()==-1){
                pilaEjecucion.pop();
                imprimePila();
            }
            if(pilaEjecucion.top()=='$'){
                pilaEjecucion.pop();
                imprimePila();
                if(sinError){
                QMessageBox msgBox;
                msgBox.setText("Se llego al final de la entrada y no hay errores");
                msgBox.exec();
                break;
                }
            }
            }

       }

    }
}

static QString salida;
void generarSalida(){
    int i=0;
    int valor3=0;
    QList<QString> identidicadores;
    QList<QString> valorIdentificadores;
    while(i<cuadruplos.size()){

        if(cuadruplos.at(i)->getOp1()=="" && cuadruplos.at(i)->getOp2()=="" && cuadruplos.at(i)->getOper()=="" && cuadruplos.at(i)->getRes()==""){
           i=cuadruplos.size();
        }else{

            if(cuadruplos.at(i)->getOper().contains("=") && cuadruplos.at(i)->getOper().contains("==")==false && cuadruplos.at(i)->getOper().contains("<=")==false && cuadruplos.at(i)->getOper().contains(">=")==false){
                if(cuadruplos.at(i)->getRes().contains("R")){
                    QString op1=cuadruplos.at(i-1)->getOp1();
                    QString op2=cuadruplos.at(i-1)->getOp2();
                    bool declaradoOp1,declaradoOp2,evaluado=false;
                    int encontrado=0;
                    for (int j=0;j<identidicadores.size();j++) {
                        if(identidicadores.at(j).contains(op1)){
                            declaradoOp1=true;
                            encontrado=j;
                        }

                        if(identidicadores.at(j).contains(op2)){
                            declaradoOp2=true;
                            encontrado=j;
                        }
                    }

                    std::string cadenaOp1 = op1.toStdString();
                    bool esIdentificador1;
                    for(int j=0;j<op1.length();j++){
                        if(!(cadenaOp1[j]>=48 && cadenaOp1[j]<=57)){
                            esIdentificador1=true;
                        }
                    }

                    std::string cadenaOp2 = op2.toStdString();
                    bool esIdentificador2;
                    for(int j=0;j<op2.length();j++){
                        if(!(cadenaOp2[j]>=48 && cadenaOp2[j]<=57)){
                            esIdentificador2=true;
                        }
                    }

                    if(declaradoOp1 && evaluado==false){
                        if(esIdentificador1 && declaradoOp1){
                             int valor1=valorIdentificadores.at(encontrado).toInt();
                             int valor2=op2.toInt();
                             cout<<valor1<<endl;
                             cout<<valor2<<endl;
                             if(cuadruplos.at(i-1)->getOper().contains("+")){
                                 valor3=valor1+valor2;
                             }
                             if(cuadruplos.at(i-1)->getOper().contains("-")){
                                 valor3=valor1-valor2;
                             }
                             if(cuadruplos.at(i-1)->getOper().contains("*")){
                                 valor3=valor1*valor2;
                             }
                             if(cuadruplos.at(i-1)->getOper().contains("/")){
                                 valor3=valor1/valor2;
                             }
                             cout<<valor3<<endl;
                             evaluado=true;
                        }
                    }else{
                        int valor1=0;
                        int valor2=op2.toInt();

                        if(cuadruplos.at(i-1)->getOper().contains("+")){
                            valor3=valor1+valor2;
                        }
                        if(cuadruplos.at(i-1)->getOper().contains("-")){
                            valor3=valor1-valor2;
                        }
                        if(cuadruplos.at(i-1)->getOper().contains("*")){
                            valor3=valor1*valor2;
                        }
                        if(cuadruplos.at(i-1)->getOper().contains("/")){
                            valor3=valor1/valor2;
                        }
                        evaluado=true;
                    }


                    if(declaradoOp2 && evaluado==false){
                        if(esIdentificador2 && declaradoOp2){
                             int valor1=op1.toInt();
                             int valor2=valorIdentificadores.at(encontrado).toInt();

                             if(cuadruplos.at(i-1)->getOper().contains("+")){
                                 valor3=valor1+valor2;
                             }
                             if(cuadruplos.at(i-1)->getOper().contains("-")){
                                 valor3=valor1-valor2;
                             }
                             if(cuadruplos.at(i-1)->getOper().contains("*")){
                                 valor3=valor1*valor2;
                             }
                             if(cuadruplos.at(i-1)->getOper().contains("/")){
                                 valor3=valor1/valor2;
                             }
                        }
                    }else{
                        if(evaluado==false){
                            int valor1=op1.toInt();
                            int valor2=0;
                            cout<<"Entra"<<endl;
                            cout<<valor1<<endl;
                            cout<<valor2<<endl;
                            if(cuadruplos.at(i-1)->getOper().contains("+")){
                                valor3=valor1+valor2;
                            }
                            if(cuadruplos.at(i-1)->getOper().contains("-")){
                                valor3=valor1-valor2;
                            }
                            if(cuadruplos.at(i-1)->getOper().contains("*")){
                                valor3=valor1*valor2;
                            }
                            if(cuadruplos.at(i-1)->getOper().contains("/")){
                                valor3=valor1/valor2;
                            }
                        }

                    }



                    if(identidicadores.size()>0){
                        int pos=0;
                        bool existe;
                        for(int j=0;j<identidicadores.size();j++){
                            if(identidicadores.at(j).contains(cuadruplos.at(j)->getOp1())){
                                pos=j;
                                existe=true;
                            }
                        }
                        if(existe){
                            valorIdentificadores.replace(pos,QString::number(valor3));
                        }else{
                            identidicadores.append(cuadruplos.at(i)->getOp1());
                            valorIdentificadores.append(QString::number(valor3));
                        }
                    }else{
                        identidicadores.append(cuadruplos.at(i)->getOp1());
                        valorIdentificadores.append(QString::number(valor3));

                    }
                }else{
                    if(identidicadores.size()>0){
                      int pos=0;
                      bool existe;
                      for(int j=0;j<identidicadores.size();j++){
                          if(identidicadores.at(j).contains(cuadruplos.at(j)->getOp1())){
                              pos=j;
                              existe=true;
                          }
                      }
                      if(existe){
                          valorIdentificadores.replace(pos,cuadruplos.at(i)->getRes());
                      }else{
                          identidicadores.append(cuadruplos.at(i)->getOp1());
                          valorIdentificadores.append(cuadruplos.at(i)->getRes());
                      }
                    }else{
                        identidicadores.append(cuadruplos.at(i)->getOp1());
                        valorIdentificadores.append(cuadruplos.at(i)->getRes());
                    }
                }

            }

            if(cuadruplos.at(i)->getOper().contains("write")){

                if(cuadruplos.at(i)->getRes().contains(34)){
                   std::string cadena = cuadruplos.at(i)->getRes().toStdString();
                   QString imprime="";
                   for(int j=0;j<cuadruplos.at(i)->getRes().length();j++){
                       if(cadena[j]==34){
                           continue;
                       }else{
                           imprime+=cadena[j];
                       }
                   }
                   salida+=imprime;
                }else{
                    std::string cadena = cuadruplos.at(i)->getRes().toStdString();
                    bool esIdentificador;
                    for(int j=0;j<cuadruplos.at(i)->getRes().length();j++){
                        if(!(cadena[j]>=48 && cadena[j]<=57)){
                            esIdentificador=true;
                        }
                    }
                    if(esIdentificador){
                        if(cuadruplos.at(i)->getRes().contains("true") || cuadruplos.at(i)->getRes().contains("false")){
                            salida+=cuadruplos.at(i)->getRes();
                        }else{
                            bool dec;
                            int pos;
                            if(identidicadores.size()>0){
                                for(int j=0;j<identidicadores.size();j++){
                                    if(identidicadores.at(j).contains(cuadruplos.at(i)->getRes())){
                                       dec=true;
                                       pos=j;
                                    }
                                }
                                if(dec){
                                   salida+=valorIdentificadores.at(pos);
                                }else{
                                   salida+="0";
                                }
                            }else{
                                salida+="0";
                            }
                        }
                    }else{
                        salida+=cuadruplos.at(i)->getRes();
                    }
                }

            }
            if(cuadruplos.at(i)->getOper().contains("enter")){
                salida+="\n";

            }

            if(cuadruplos.at(i)->getOper().contains("SI")){
                i=cuadruplos.at(i)->getRes().toInt()-2;
            }

            if(cuadruplos.at(i)->getOper().contains("SF") || cuadruplos.at(i)->getOper().contains("SV")){
                    QString op1=cuadruplos.at(i-1)->getOp1();
                    int op2=cuadruplos.at(i-1)->getOp2().toInt();
                    QString oper=cuadruplos.at(i-1)->getOper();
                    bool exi;
                    int pos=0;

                    for (int j=0;j<identidicadores.size();j++) {
                                    if(identidicadores.at(j).contains(op1)){
                                        exi=true;
                                        pos=j;
                                    }
                                }

                                if(exi){
                                    int vOb=valorIdentificadores.at(pos).toInt();
                                    if(oper.contains(">")){
                                        if((vOb>op2)==false)
                                            i=cuadruplos.at(i)->getRes().toInt()-2;

                                    }
                                    if(oper.contains("<")){
                                        int vOb=valorIdentificadores.at(pos).toInt();
                                        cout<<vOb<<endl;
                                        cout<<op2<<endl;
                                        if((vOb<op2)==false)
                                            i=cuadruplos.at(i)->getRes().toInt()-2;

                                    }
                                    if(oper.contains(">=")){
                                        int vOb=valorIdentificadores.at(pos).toInt();
                                        if((vOb>=op2)==false)
                                            i=cuadruplos.at(i)->getRes().toInt()-2;

                                    }
                                    if(oper.contains("<=")){
                                        int vOb=valorIdentificadores.at(pos).toInt();
                                        if((vOb<=op2)==false)
                                            i=cuadruplos.at(i)->getRes().toInt()-2;

                                    }
                                    if(oper.contains("==")){
                                        int vOb=valorIdentificadores.at(pos).toInt();
                                        if((vOb==op2)==false)
                                            i=cuadruplos.at(i)->getRes().toInt()-2;
                                    }
                                    if(oper.contains("!=")){
                                        int vOb=valorIdentificadores.at(pos).toInt();
                                        if((vOb!=op2)==false)
                                            i=cuadruplos.at(i)->getRes().toInt()-2;

                                    }
                                }else{
                                   int vOb=0;
                                   if(oper.contains(">")){
                                       if((vOb>op2)==false)
                                           i=cuadruplos.at(i)->getRes().toInt()-2;

                                   }
                                   if(oper.contains("<")){
                                       int vOb=0;
                                       if((vOb<op2)==false)
                                           i=cuadruplos.at(i)->getRes().toInt()-2;

                                   }
                                   if(oper.contains(">=")){
                                       int vOb=0;
                                       if((vOb>=op2)==false)
                                           i=cuadruplos.at(i)->getRes().toInt()-2;

                                   }
                                   if(oper.contains("<=")){
                                       int vOb=0;
                                       if((vOb<=op2)==false)
                                           i=cuadruplos.at(i)->getRes().toInt()-2;

                                   }
                                   if(oper.contains("==")){
                                       if((vOb==op2)==false)
                                           i=cuadruplos.at(i)->getRes().toInt()-2;

                                   }
                                   if(oper.contains("!=")){
                                       int vOb=0;
                                       if((vOb!=op2)==false)
                                           i=cuadruplos.at(i)->getRes().toInt()-2;

                                   }

                                }
            }


        }
           i++;



    }
}

void optimizacionSaltos(){
    QStack<int> posiblesSaltos;
    for(int i=0;i<cuadruplos.size();i++){
        if(cuadruplos.at(i)->getOper().contains("SI")){
            int j=cuadruplos.at(i)->getRes().toInt()-1;
            if(cuadruplos.at(j)->getOper().contains("SI")){
                posiblesSaltos.push(i);
                for(int k=0;k<posiblesSaltos.size();k++){
                    cuadruplos.at(posiblesSaltos.at(k))->setRes(cuadruplos.at(j)->getRes());
                }
            }
        }
    }
}
void MainWindow::on_pushButton_clicked()
{
    ui->tablaCuadruplos->setRowCount(0);
    pilaWhile.clear();
    cuadruplos.clear();
    cuadruplosR.clear();
    contCuadruplo=0;
    contRes=0;
    Tokens="";
    errores="";
    pasosPila="";
    pilaOp="";
    pilaT="";
    pilaOper="";
    pilaSal="";
    salida="";
    ui->Token->setPlainText("");
    ui->Error->setPlainText("");
    ui->Token_2->setPlainText("");
    ui->pilaOperandos->setPlainText("");
    ui->pilaTipos->setPlainText("");
    ui->pilaOperadores->setPlainText("");
    ui->pilaSaltos->setPlainText("");
    ui->salida->setPlainText("");
    pilaSaltos.clear();
    pilaTipos.clear();
    pilaOperandosBusqueda.clear();
    pilaTiposBusqueda.clear();
    pilaOperadores.clear();
    texto=ui->textoAnalizar->toPlainText();
    ConstruyeGramatica();
    optimizacionSaltos();
    if(errores!="" && sinError==false){
        QMessageBox::about(this,"Mensaje","La sintaxis contiene algunos errores léxicos o sintácticos o semánticos");
    }else{
        generarSalida();
        ui->salida->setPlainText(salida);
    }
    ui->Token->appendPlainText(Tokens);
    ui->Error->appendPlainText(errores);
    ui->Token_2->appendPlainText(pasosPila);
    ui->pilaOperandos->appendPlainText(pilaOp);
    ui->pilaTipos->appendPlainText(pilaT);
    ui->pilaOperadores->appendPlainText(pilaOper);
    ui->pilaSaltos->appendPlainText(pilaSal);
    LlenarCuadruplo();
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->textoAnalizar->setPlainText("");
   QString ruta=QFileDialog::getOpenFileName(
                this,tr("Abrir archivo"),
                "C://",
                "Eyement (*.eye)");
    if(ruta!=""){
    QFile inputFile(ruta);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          ui->textoAnalizar->appendPlainText(line);
       }
       inputFile.close();
    }

}

}



void MainWindow::on_pushButton_3_clicked()
{
    ui->textoAnalizar->setPlainText("");
    ui->tablaCuadruplos->setRowCount(0);
    cuadruplos.clear();
    cuadruplosR.clear();
    pilaWhile.clear();
    contCuadruplo=0;
    contRes=0;
    Tokens="";
    errores="";
    pasosPila="";
    pilaOp="";
    pilaT="";
    pilaOper="";
    pilaSal="";
    ui->Token->setPlainText("");
    ui->Error->setPlainText("");
    ui->Token_2->setPlainText("");
    ui->pilaOperandos->setPlainText("");
    ui->pilaTipos->setPlainText("");
    ui->pilaOperadores->setPlainText("");
    ui->pilaSaltos->setPlainText("");
    pilaSaltos.clear();
    pilaTipos.clear();
    pilaOperandosBusqueda.clear();
    pilaTiposBusqueda.clear();
    pilaOperadores.clear();
}

void MainWindow::on_pushButton_4_clicked()
{

    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Guardar archivo"), "",
            tr("Eyement (*.eye)"));
    if (fileName.isEmpty())
            return;
        else {
            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly)) {
                QMessageBox::information(this, tr("Unable to open file"),
                    file.errorString());
                return;
            }
    }
    QFile file(fileName);
    if ( file.open(QIODevice::ReadWrite) ){
        QTextStream stream( &file );
        stream <<ui->textoAnalizar->toPlainText()<< endl;
    }

}
