#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextStream>
#include <fstream>
#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include <QStack>

using namespace std;

static QStack<int> pilaEjecucion;
static int edo;

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
                                     {524,524,524,31,524,524,524,524,524,524,524,524,524,55,524,524,524,524,524,524,524,524,524,36,524,524,524,524,524,524,524,524,524,524,524,524,31,31,31,31,31,524,524,524,524,524,524,524,524},
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



static int producciones[71][8]={{2,3},
                                {132,8,4,133,101,134},
                                {-1},
                                {3,124,131,135},
                                {-1},
                                {5,136},
                                {6,101},
                                {5,130},
                                {4,124,7,137},
                                {138},
                                {139},
                                {140},
                                {141},
                                {142},
                                {-1},
                                {8,124,28},
                                {8,124,9},
                                {8,124,11},
                                {8,124,12},
                                {8,124,32},
                                {8,124,29},
                                {8,124,27},
                                {10,8,127,13,126,143},
                                {144},
                                {144,8,145},
                                {146,8,127,13,126,147},
                                {148,8,127,13,123,28,126,149},
                                {14,15},
                                {-1},
                                {13,114},
                                {16,17},
                                {-1},
                                {15,113},
                                {18},
                                {19},
                                {19,115},
                                {20,21},
                                {-1},
                                {21,26},
                                {22,23},
                                {-1},
                                {21,107},
                                {21,108},
                                {24,25},
                                {-1},
                                {23,109},
                                {23,110},
                                {23,111},
                                {101},
                                {102},
                                {103},
                                {104},
                                {105},
                                {106},
                                {127,13,126},
                                {117},
                                {116},
                                {120},
                                {121},
                                {118},
                                {119},
                                {150},
                                {13,122,101},
                                {30,126,151},
                                {31,13},
                                {127},
                                {30,130},
                                {33,126,152},
                                {34,101},
                                {127},
                                {33,130}};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->Token->setReadOnly(true);
    ui->Error->setReadOnly(true);
    ui->Token_2->setReadOnly(true);
}

MainWindow::~MainWindow()
{
    delete ui;
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
        errores+=textoA+" 517: Se esperaba un identificador o el  inicio o fien de un estatuto\n";
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


    int conta=0;
    std::string cadenaStd2 = textoA.toStdString();
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
    for(int i=0;i<8;i++){
        if(producciones[fila][i]!=0){
        pilaEjecucion.push(producciones[fila][i]);
        }
    }
}

QString evaluaElemento(int token){
    switch(token){
    case -1:
        return "ε";
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
        return "Operadorsuma";
        break;
    case 108:
        //cout<<"Resta"<<endl;
        return "Operadorresta";
        break;
    case 109:
        //cout<<"Multiplicación"<<endl;
        return "Operadormultipiliacación";
        break;
    case 110:
        //cout<<"División"<<endl;
        return "Operadordivisión";
        break;
    case 111:
        //cout<<"Modulo"<<endl;
        return "Operadormodulo";
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
        return "OperadorNOT";
        break;
    case 116:
        //cout<<"Operador Diferente"<<endl;
        return "Operadordiferente";
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
QString pasosPila;
void imprimePila(){
    for(int i=0;i<pilaEjecucion.size();i++){
        QString ele=evaluaElemento(pilaEjecucion.at(i));
        pasosPila+=ele+" ";
    }
    pasosPila+="\n";
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
    while(edoMP<500 && token<500 && !pilaEjecucion.empty()){

        if(quieroToken){
            token=Analiza(texto);
            quieroToken=false;
        }
        if(token>=500){
            break;
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
            }else{
                QString tr=evaluaElemento(token);
                QString tp=evaluaElemento(pilaEjecucion.top());
                QMessageBox msgBox;
                msgBox.setText("Hay errores en la sintaxis porque se esperaba un "+tp+" y se recibio un "+tr);
                msgBox.exec();
                break;
            }
        }else{
            if(edoMP>500){
                Errores(edoMP+1);
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
                QMessageBox msgBox;
                msgBox.setText("Se llego al final de la entrada y no hay errores");
                msgBox.exec();
                break;
            }
            }

        }
    }

}
void MainWindow::on_pushButton_clicked()
{
    Tokens="";
    errores="";
    pasosPila="";
    ui->Token->setPlainText("");
    ui->Error->setPlainText("");
    ui->Token_2->setPlainText("");
    texto=ui->textoAnalizar->toPlainText();
    ConstruyeGramatica();
    if(errores!=""){
        QMessageBox::about(this,"Mensaje","La sintaxis contiene algunos errores léxicos o sintácticos");
    }
    ui->Token->appendPlainText(Tokens);
    ui->Error->appendPlainText(errores);
    ui->Token_2->appendPlainText(pasosPila);

}

void MainWindow::on_pushButton_2_clicked()
{
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
    ui->Token->setPlainText("");
    ui->Error->setPlainText("");
    ui->Token_2->setPlainText("");
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
