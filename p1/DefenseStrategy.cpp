//1-Dentro del terreno -> La defensa debe estar dentro del terreno de juego, ojo con el radio
//2-Chequear distancia con obstaculos y con las otras defensas -> Hay una funcion ya definida

// ###### Config options ################

//#define PRINT_DEFENSE_STRATEGY 1    // generate map images

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

#ifdef PRINT_DEFENSE_STRATEGY
#include "ppm.h"
#endif

#ifdef CUSTOM_RAND_GENERATOR
RAND_TYPE SimpleRandomGenerator::a;
#endif

using namespace Asedio;

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


bool factible(int row,int col,int nCellsWidth,int nCellsHeight,float mapWidth,float mapHeight,
	List<Object*> obstacles,List<Defense*> defenses,int id)// identificador de la defensa para la lista de defensas
{
	//Primero buscamos la defensa con id
	List<Defense*>::iterator currentDefense = defenses.begin();
	bool found=false;
	while(currentDefense!=defenses.end() && !found)
	{
		if((*currentDefense)->id==id)
		{
			found=true;
		}
		else
		{
		++currentDefense;
		}
	}


	if(!found)return false;
	else
	{

		//Usar las funciones de distancia
		float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;

		//Comprobamos que no se sale del mapa
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
		while((checkDefense!=defenses.end()) && id!=(*checkDefense)->id )
		{
				float dist_def=_distance(celda,(*checkDefense)->position);
				if(dist_def<=(*checkDefense)->radio+(*currentDefense)->radio)
				{
					return false;
				}

				++checkDefense;
		}
		//Ni con ningun obstaculos
		//Todos los obstaculos de esta lista estan colocados
		List<Object*>::iterator checkObstacle=obstacles.begin();
		while(checkObstacle!=obstacles.end())
		{
				float dist= _distance(celda,(*checkObstacle)->position);

					if((*checkObstacle)->radio+(*currentDefense)->radio>=dist)
				{
					return false;
				}

			++checkObstacle;

		}


		return true;
	}
}


//Cuanto menor sea el valor devuelto mejor
float cellValue_centro(int row,int col,int nCellsWidth, int nCellsHeight,List<Object*> obstacles, List<Defense*> defenses,
	float cellWidth,float cellHeight){
		float maximus=std::numeric_limits<float>::max();
		float d=0;

		if(row==0 || col==0 || row==nCellsHeight-1 || col==nCellsWidth-1)return maximus;//No queremos la frontera de la matriz
		else
		{
			//Encontramos el centro de la matriz
			//puede haber 2, cuando resto==0
			int fila1=-1;
			int fila2=-1;
			int col1=-1;
			int col2=-1;

			if((nCellsWidth-1)%2!=0)
			{
				col1=(nCellsWidth-1)/2;
				col2=(nCellsWidth-1)/2+1;
			}
			else
			{
				col1=	(nCellsWidth-1)/2;
			}

			if((nCellsHeight-1)%2!=0)
			{
				fila1=(nCellsHeight-1)/2;
				fila2=(nCellsHeight-1)/2+1;
			}
			else
			{
				fila1=	(nCellsHeight-1)/2;
			}

			float dist1;
			float dist2;

			int iaux;
			int jaux;
			Vector3 cell=cellCenterToPosition(row,col,cellWidth,cellHeight);

			//suma de las distancia hasta los obstaculos
			List<Object*>::iterator checkObstacle=obstacles.begin();
			float d_aux;
			while(checkObstacle!=obstacles.end())
			{
				d_aux= _distance((*checkObstacle)->position,cell);
				if(d_aux<=39)d+= d_aux;//No nos interesan los que estan lejos,39 parece correcto
				++checkObstacle;
			}

				Vector3 center1=cellCenterToPosition(fila1,col1,cellWidth,cellHeight);
				//distancia hasta el centro 1
				dist1= _distance(cell,center1);

				//si hay dos centros;
			if(col2!=-1 && fila2!=-1)
			{
				Vector3 center2=cellCenterToPosition(fila2,col2,cellWidth,cellHeight);

				//distancia hasta el centro 2 si hubiera
				dist2= _distance(cell,center2);

				if(dist2>dist1)d+=dist1;
				else
				{
				 d+=dist2;
				 fila1=fila2;
				 col1=col2;
			 	}
			}
			//si solo hay un centro
			else
			{
				d+=dist1;
			}

			//calculamos las penalizaciones por estar cerca del border
			//tanto para las filas como para las columnas

			if(fila1!=row)
			{
				if(row<fila1)
				{
					d+=(float)(nCellsHeight-1)-(float)fila1;
				}
				else
				{
					d+=(float)row-(float)fila1;
				}
			}

			if(col1!=col)
			{
				if(col<col1)
				{
					d+=(float)(nCellsWidth-1)-(float)col1;
				}
				else
				{
					d+=(float)col-(float)fila1;
				}
			}


		}
		return d;
	}


float cellValue(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight
	, float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses,Vector3 centro_mapa) {

		float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
		float maximus=std::numeric_limits<float>::max();
		if(row==0 || row==nCellsHeight-1 || col==0 || col==nCellsWidth-1) return maximus;
		else
		{
					List<Defense*>::iterator centro_rec=defenses.begin();
					int rec_fila,rec_col;
					positionToCell((*centro_rec)->position,rec_fila,rec_col,cellWidth,cellHeight);
					Vector3 celda=cellCenterToPosition(row,col,cellWidth,cellHeight);
					float distance= _distance((*centro_rec)->position,celda);

					List<Object*>::iterator obs=obstacles.begin();
					float d_aux;


					while(obs!=obstacles.end())
					{
						d_aux=_distance((*obs)->position,celda);
						if(d_aux<=20)distance-=d_aux;
						++obs;
					}

					distance+=_distance(celda,centro_mapa);

					return distance;
		}

}


void DEF_LIB_EXPORTED placeDefenses(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , std::list<Object*> obstacles, std::list<Defense*> defenses) {


		//anchura y la altura de cada celda
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;

			float greatest=std::numeric_limits<float>::max();
			int iaux,jaux;

		//Colocamos el centro de recursos
		float** cellValues = new float* [nCellsHeight];
		for(int i = 0; i < nCellsHeight; ++i) {
			 cellValues[i] = new float[nCellsWidth];
			 for(int j = 0; j < nCellsWidth; ++j) {
					 cellValues[i][j] = cellValue_centro(i,j,nCellsWidth,nCellsHeight,obstacles,defenses,cellWidth,cellHeight);
					 if(cellValues[i][j]<greatest && factible(i,j,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses,0))
					 {

						 iaux=i;
						 jaux=j;
						 greatest=cellValues[i][j];
					 }
			 }
		}

		Vector3 centro_de_recoleccion= cellCenterToPosition(iaux,jaux,cellWidth,cellHeight);
		List<Defense*>::iterator currentDefense=defenses.begin();
		(*currentDefense)->position.x=centro_de_recoleccion.x;
		(*currentDefense)->position.y=centro_de_recoleccion.y;
		(*currentDefense)->position.z=0;
		int best_row;
		int best_col;

		Vector3 centro_mapa;
		centro_mapa.x=mapWidth/2;
		centro_mapa.y=mapHeight/2;
		centro_mapa.z=0;

		//Colocamos el resto de defensas
		for(int k=1;k<defenses.size();++k)
		{
			++currentDefense;
			greatest=std::numeric_limits<float>::max();

			for(int i=0;i<nCellsWidth;++i)
			{
				for(int j=0;j<nCellsHeight;++j)
				{
					cellValues[i][j]=cellValue(i,j,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses,centro_mapa);
					if(cellValues[i][j]<=greatest && factible(i,j,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses,k))
					{

							best_row=i;
							best_col=j;
							greatest=cellValues[i][j];

					}
				}
		}
		Vector3 best=cellCenterToPosition(best_row,best_col,cellWidth,cellHeight);
		(*currentDefense)->position.x=best.x;
		(*currentDefense)->position.y=best.y;
		(*currentDefense)->position.z=0;
}


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
}
