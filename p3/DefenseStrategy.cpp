#define BUILDING_DEF_STRATEGY_LIB 1
#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"
#include "cronometro.h"
#include "Apo.h"
#ifdef PRINT_DEFENSE_STRATEGY
#include "ppm.h"
#endif
#ifdef CUSTOM_RAND_GENERATOR
RAND_TYPE SimpleRandomGenerator::a;
#endif
using namespace Asedio;

struct Casilla
{
		float valor;
		int fila;
		int col;
		Casilla(float v,int f,int c):valor(v),fila(f),col(c){}
		Casilla(){}
};
bool operator <(const Casilla& a,const Casilla& b){return a.valor<b.valor;}
bool operator >(const Casilla& a,const Casilla& b){return b<a;}

float defaultCellValue(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight
    , float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses) {
float cellWidth = mapWidth / nCellsWidth;
float cellHeight = mapHeight / nCellsHeight;
Vector3 cellPosition((col * cellWidth) + cellWidth * 0.5f, (row * cellHeight) + cellHeight * 0.5f, 0);

float val = 0;
for (List<Object*>::iterator it=obstacles.begin(); it != obstacles.end(); ++it) {
val += _distance(cellPosition, (*it)->position);
}
return val;
}

// Devuelve la posición en el mapa del centro de la celda (i,j)
// i - fila
// j - columna
// cellWidth - ancho de las celdas
// cellHeight - alto de las celdas
Vector3 cellCenterToPosition(int i, int j, float cellWidth, float cellHeight)
{
return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0);
}

// Devuelve la celda a la que corresponde una posición en el mapa
// pos - posición que se quiere convertir
// i_out - fila a la que corresponde la posición pos (resultado)
// j_out - columna a la que corresponde la posición pos (resultado)
// cellWidth - ancho de las celdas
// cellHeight - alto de las celdas
void positionToCell(const Vector3 pos, int &i_out, int &j_out, float cellWidth, float cellHeight)
{
i_out = (int)(pos.y * 1.0f/cellHeight); j_out = (int)(pos.x * 1.0f/cellWidth);
}


bool factible(int row,int col,float mapWidth,float mapHeight,
	List<Object*> obstacles,List<Defense*> defenses,std::list<Defense*>::iterator currentDefense,float cellWidth,float cellHeight)// identificador de la defensa para la lista de defensas
{
Vector3 celda=cellCenterToPosition(row,col,cellWidth,cellHeight);

float positivo_x=celda.x+(*currentDefense)->radio;
float negativo_x=celda.x-(*currentDefense)->radio;
float positivo_y=celda.y+(*currentDefense)->radio;
float negativo_y=celda.y-(*currentDefense)->radio;

if(positivo_x>=mapWidth || negativo_x<=0) return false;
if(positivo_y>=mapHeight || negativo_y<=0) return false;

int row2=0;
int col2=0;;

List<Defense*>::iterator checkDefense = defenses.begin();
while(checkDefense!=defenses.end() && (*checkDefense)->id<(*currentDefense)->id)//Mejorar esta parte para que solo compruebe las que estan colocadas
{
float dist_def=_distance(celda,(*checkDefense)->position);
if(dist_def<=(*checkDefense)->radio+(*currentDefense)->radio )
{
return false;
}
++checkDefense;
}
int c=0;
std::list<Object*>::iterator checkObstacle=obstacles.begin();
while(checkObstacle!=obstacles.end())
{
float dist= _distance(celda,(*checkObstacle)->position);
if((*checkObstacle)->radio+(*currentDefense)->radio>=dist)
{
return false;
}
++checkObstacle;
++c;
}
return true;
}



Casilla sin_preordenacion(std::vector<bool>& selectedDefenses,Casilla* valores,int size,float actual_value)
{
Casilla selected;
int aux;
for(int i=0;i<size;++i)
{
if(actual_value<valores[i].valor && !selectedDefenses[i])
{
aux=i;
actual_value=valores[i].valor;
selected=valores[i];
}}
selectedDefenses[aux]=true;
return selected;
}

void  ordenar_insertar(Casilla* vect,int in,int ji){
int i,j;
Casilla v;
for(int i=in+1;i<=ji;i++){
v=vect[i];
j=i-1;
while (j >=in && vect[j].valor > v.valor)
{
vect[j + 1] = vect[j];
j--;
}
vect[j + 1] = v;
}}

void fusion(Casilla* v,int low,int mid,int high,int size)
{
int h,i,j,k;
Casilla w[size];
h=low;
i=low;
j=mid+1;
while((h<=mid)&&(j<=high))
{
if(v[h].valor<=v[j].valor)
{
w[i]=v[h];
++h;
}
else
{
w[i]=v[j];
++j;
}
++i;
}
if(h>mid)
{
for(k=j;k<=high;++k)
{
w[i]=v[k];
++i;
}
}
else
{
for(k=h;k<=mid;k++)
{
w[i]=v[k];
++i;
}
}
for(k=low;k<=high;++k)v[k]=w[k];
}

void sort_fusion(Casilla* defensas,int low,int high,int size)
{
int mid;
if(low<high)
{
mid=low+(high-low)/2;
sort_fusion(defensas,low,mid,size);
sort_fusion(defensas,mid+1,high,size);
fusion(defensas,low,mid,high,size);
}
}

int pivote(Casilla* posiciones,int low,int high)
{
float p=posiciones[high].valor;
int i=(low-1);
Casilla aux;
for(int j=low;j<=high-1;++j)
{
if(posiciones[j].valor<=p)
{
++i;
aux=posiciones[i];
posiciones[i]=posiciones[j];
posiciones[j]=aux;
}}
aux=posiciones[i+1];
posiciones[i+1]=posiciones[high];
posiciones[high]=aux;
return i+1;
}

void quicksort(Casilla* posiciones,int low,int high)
{
int p;
if(low<high)
{
p=pivote(posiciones,low,high);
quicksort(posiciones,low,p-1);
quicksort(posiciones,p+1,high);
}
}

/**
//No usamos la estructura heap, accemos uso del metodo heapify que simula monticulos a traves de un vector
void heapify(Casilla* posiciones,int n,int i)
{
int s=i;//El mas pequeño como raiz
int l=2*i+1;//izq
int r=2*i+2;//derch
Casilla aux;
if(l<n && posiciones[l].valor<posiciones[s].valor)s=l;
if(r<n && posiciones[r].valor<posiciones[s].valor)s=r;
if(s!=i)
{
aux=posiciones[i];
posiciones[i]=posiciones[s];
posiciones[s]=aux;
heapify(posiciones,n,s);
}
}

void heapsort(Casilla* posiciones,int n)
{
Casilla aux;
for(int i=(n/2)-1;i>=0;--i)heapify(posiciones,n,i);
for(int i=n-1;i>=0;--i)
{
aux=posiciones[0];
posiciones[0]=posiciones[i];
posiciones[i]=aux;
heapify(posiciones,i,0);
}}
*/

void heap_sort(Casilla* c,Apo<Casilla>& heap,int size)
{
for(int i=0;i<size;++i)
{
	c[i]=heap.cima();
	heap.suprimir();
}}

void imprimir_valores(Casilla* posiciones,int size)
{
for(int i=0;i<size;++i)
{
std::cout<<" "<<i<<" "<<posiciones[i].valor<<std::endl;
}}

void sin_ordenar(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , std::list<Object*> obstacles, std::list<Defense*> defenses,float cellWidth,float cellHeight)
{
	//anchura y la altura de cada celda
Casilla ca;
int iaux,jaux;
int size=nCellsWidth*nCellsHeight;
Casilla posiciones[size];
int pos=0;
std::vector<bool> selectedDefenses(size,false);
float** cellValues = new float* [nCellsHeight];
for(int i = 0; i < nCellsHeight; ++i) {
cellValues[i] = new float[nCellsWidth];
for(int j = 0; j < nCellsWidth; ++j) {
posiciones[pos]=Casilla(defaultCellValue(i,j,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses),i,j);
++pos;
}}
std::list<Defense*>::iterator currentDefense = defenses.begin();
bool fact=false;
int j;
for(int k=0;k<defenses.size();++k)
{
while(!fact)
{
ca=sin_preordenacion(selectedDefenses,posiciones,size,0);
if(factible(ca.fila,ca.col,mapWidth,mapHeight,obstacles,defenses,currentDefense,cellWidth,cellHeight))
{
Vector3 v3=cellCenterToPosition(ca.fila,ca.col,cellWidth,cellHeight);
(*currentDefense)->position.x=v3.x;
(*currentDefense)->position.y=v3.y;
(*currentDefense)->position.z=0;
fact=true;
}}
fact=false;
++currentDefense;
}}

void ordenar_fusion(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , std::list<Object*> obstacles, std::list<Defense*> defenses,float cellWidth,float cellHeight)
{
int size=nCellsWidth*nCellsHeight;
Casilla posiciones[size];
int pos=0;
float **cellValues=new float* [nCellsHeight];
for(int i = 0; i < nCellsHeight; ++i) {
cellValues[i] = new float[nCellsWidth];
for(int j = 0; j < nCellsWidth; ++j) {
posiciones[pos]=(Casilla(defaultCellValue(i,j,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses),i,j));
++pos;
}}
sort_fusion(posiciones,0,size-1,size);//Este va con j=size-1
std::list<Defense*>::iterator currentDefense = defenses.begin();
bool fact=false;
int j=size-1;
for(int k=0;k<defenses.size();++k)
{
//j=size-1;

while(j>=0 && !fact)
{
if(factible(posiciones[j].fila,posiciones[j].col,mapWidth,mapHeight,obstacles,defenses,currentDefense,cellWidth,cellHeight))
{
Vector3 v3=cellCenterToPosition(posiciones[j].fila,posiciones[j].col,cellWidth,cellHeight);
(*currentDefense)->position.x=v3.x;
(*currentDefense)->position.y=v3.y;
(*currentDefense)->position.z=0;
fact=true;
}
--j;
}
fact=false;
++currentDefense;
}}

void ordenar_rapido(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , std::list<Object*> obstacles, std::list<Defense*> defenses,float cellWidth,float cellHeight)
{
int size=nCellsWidth*nCellsHeight;
Casilla posiciones[size];
int pos=0;
float** cellValues = new float* [nCellsHeight];
for(int i = 0; i < nCellsHeight; ++i) {
cellValues[i] = new float[nCellsWidth];
for(int j = 0; j < nCellsWidth; ++j) {
posiciones[pos]=(Casilla(defaultCellValue(i,j,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses),i,j));
++pos;
}}
quicksort(posiciones,0,size-1);
std::list<Defense*>::iterator currentDefense = defenses.begin();
bool fact=false;
int j=size-1;
for(int k=0;k<defenses.size();++k)
{
//j=size-1;//comprobar este valor segun el de heap,si da 283 dejalo asi, si da 256 cambia a size
while(j>=0 && !fact)
{
if(factible(posiciones[j].fila,posiciones[j].col,mapWidth,mapHeight,obstacles,defenses,currentDefense,cellWidth,cellHeight))
{
Vector3 v3=cellCenterToPosition(posiciones[j].fila,posiciones[j].col,cellWidth,cellHeight);
(*currentDefense)->position.x=v3.x;
(*currentDefense)->position.y=v3.y;
(*currentDefense)->position.z=0;
fact=true;
}
--j;
}
fact=false;
++currentDefense;
}}

void ordenar_monticulo(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , std::list<Object*> obstacles, std::list<Defense*> defenses,float cellWidth,float cellHeight)
{
int size=nCellsWidth*nCellsHeight;
//Casilla posiciones[size];
Apo<Casilla> heap(size);
int pos=0;
float** cellValues = new float* [nCellsHeight];
for(int i = 0; i < nCellsHeight; ++i) {
cellValues[i] = new float[nCellsWidth];
for(int j = 0; j < nCellsWidth; ++j) {
heap.insertar((Casilla(defaultCellValue(i,j,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses),i,j)));
}}
//heap_sort(posiciones,heap,size);
std::list<Defense*>::iterator currentDefense = defenses.begin();
bool fact;
int j=0;
for(int k=0;k<defenses.size();++k)
{
//j=0;//comprobar este valor segun el de heap,si da 283 dejalo asi, si da 256 cambia a size
while(j<size && !fact)
{
Casilla p=heap.cima();
if(factible(p.fila,p.col,mapWidth,mapHeight,obstacles,defenses,currentDefense,cellWidth,cellHeight))
{
Vector3 v3=cellCenterToPosition(p.fila,p.col,cellWidth,cellHeight);
(*currentDefense)->position.x=v3.x;
(*currentDefense)->position.y=v3.y;
(*currentDefense)->position.z=0;
fact=true;
}
++j;
heap.suprimir();
}
fact=false;
++currentDefense;
}}

void DEF_LIB_EXPORTED placeDefenses3(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , std::list<Object*> obstacles, std::list<Defense*> defenses) {
float cellWidth = mapWidth / nCellsWidth;
float cellHeight = mapHeight / nCellsHeight;
ordenar_monticulo(freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses,cellWidth,cellHeight);

/**
cronometro c1,c2,c3,c4;
const double e_abs=0.01,e_rel=0.001;
long int r=0;
c1.activar();
do {
sin_ordenar(freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses,cellWidth,cellHeight);
++r;
}while(c1.tiempo()<e_abs/e_rel+e_abs);
c1.parar();

r=0;
c2.activar();
do {
ordenar_fusion(freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses,cellWidth,cellHeight);
++r;
}while(c2.tiempo()<e_abs/e_rel+e_abs);
c2.parar();

r=0;
c3.activar();
do {
ordenar_rapido(freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses,cellWidth,cellHeight);
++r;
}while(c3.tiempo()<e_abs/e_rel+e_abs);
c3.parar();

r=0;
c4.activar();
do {
ordenar_monticulo(freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses,cellWidth,cellHeight);
++r;
}while(c4.tiempo()<e_abs/e_rel+e_abs);
c4.parar();

std::cout << (nCellsWidth * nCellsHeight) << '\t' << c1.tiempo() / r << '\t' << c2.tiempo() / r << '\t' << c3.tiempo() / r << '\t' << c4.tiempo() / r << std::endl;
*/
/**
#ifdef PRINT_DEFENSE_STRATEGY

    float** cellValues = new float* [nCellsHeight];
    for(int i = 0; i < nCellsHeight; ++i) {
       cellValues[i] = new float[nCellsWidth];
       for(int j = 0; j < nCellsWidth; ++j) {
           cellValues[i][j] = ((int)(cellValue(i, j))) % 256;
       }
    }
    dPrintMap("strategy.ppm", nCellsHeight, nCellsWidth, cellHeight, cellWidth, freeCells
                         , cellValues, std::list<Defense*>(), true);

    for(int i = 0; i < nCellsHeight ; ++i)
        delete [] cellValues[i];
	delete [] cellValues;
	cellValues = NULL;

#endif
*/
}
