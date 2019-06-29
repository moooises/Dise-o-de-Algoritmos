// ###### Config options ################


// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

using namespace Asedio;
void asignarValores(std::list<Defense*>& ,std::list<float>& );
std::list<Defense*>::iterator devolverIterador(int ,std::list<Defense*>& );
std::list<float> asignar_Orden(std::list<Defense*>& ,std::list<float>& ,std::list<int>& );
Defense* sacar_defensa(int ,std::list<Defense*>& );
//void mochila(std::list<int>& ,std::list<Defense*>& ,std::list<float>& ,unsigned int ,float*** );
//void recuperar_mochila(float ***,std::list<Defense*>& ,unsigned int ,std::list<int>& ,std::vector<int>& );







void asignarValores(std::list<Defense*>& defenses,std::list<float>& valores)
{
  std::list<Defense*>::iterator it = defenses.begin();
  float valor;
  ++it;

  while(it!=defenses.end())
  {
  valor=(*it)->health+(((*it)->damage*(*it)->dispersion*(*it)->attacksPerSecond)/((float)(*it)->cost))*(*it)->range;
  //valor=(*it)->health+(((*it)->range*(*it)->dispersion*(*it)->attacksPerSecond)/((float)(*it)->cost))*(*it)->damage;
//valor=(*it)->health+(((*it)->attacksPerSecond*(*it)->damage*(*it)->range)*(*it)->dispersion)/(*it)->cost;

    //valor=(*it)->health+((*it)->attacksPerSecond*(*it)->damage*(*it)->range)/(*it)->dispersion;
    valores.push_back(valor);
    ++it;
  }

}

std::list<Defense*>::iterator devolverIterador(int j,std::list<Defense*>& defenses)
{
  std::list<Defense*>::iterator checkDefense=defenses.begin();
  int i=0;

  while(i!=j && checkDefense!=defenses.end())
  {
    ++checkDefense;
    ++i;
  }

  return checkDefense;
}

std::list<float> asignar_Orden(std::list<Defense*>& defenses,std::list<float>& valores,std::list<int>& orden)
{

  bool visitados[valores.size()];
  float valor;
  int select;
  std::list<float> valores_ordenados;
  std::list<float>::iterator orden_v;
  std::list<Defense*>::iterator checkDefense;
  Defense* actual;

  //std::cout<<"Tamaño de valores: "<<valores.size()<<std::endl;

  for(int i=0;i<valores.size();++i)
  {
    valor=0;
    orden_v=valores.begin();
    checkDefense=defenses.begin() ;
    ++checkDefense;

    for(int j=0;j<valores.size();++j)
    {
    //  std::cout<<"j: "<<j<<std::endl;

      if(valor<=*orden_v && !visitados[j])
      {
        select=j;
        valor=*orden_v;
        actual=*checkDefense;
      }
      ++checkDefense;
      ++orden_v;
    }
    visitados[select]=true;
    //Si esto peta, ponerlo sin funcion


    orden.push_back(actual->id);
    valores_ordenados.push_back(valor);
    //std::cout<<"i:          "<<i<<std::endl;

  }

return valores_ordenados;
}


Defense* sacar_defensa(int id,std::list<Defense*>& defenses)
{
  List<Defense*>::iterator currentDefense = defenses.begin();
  while(currentDefense!=defenses.end() && (*currentDefense)->id!=id)
  {
    ++currentDefense;
  }
  return (*currentDefense);

}
std::list<int> sacar_costes(std::list<int>& orden,std::list<Defense*> defenses)
{
  std::list<int>::iterator it=orden.begin();//La peor valorada esta al final
  std::list<int> costes;
  while(it!=orden.end())
  {
    Defense* actual=sacar_defensa(*it,defenses);//Aqui
    costes.push_back(actual->cost);
    ++it;
  }
  return costes;

}


void DEF_LIB_EXPORTED selectDefenses(std::list<Defense*> defenses, unsigned int ases, std::list<int> &selectedIDs
            , float mapWidth, float mapHeight, std::list<Object*> obstacles) {

    //Introducimos la primera si o si

    std::list<Defense*>::iterator checkDefense=defenses.begin();
    std::cout<<(*checkDefense)->id<<std::endl;
    selectedIDs.push_back((*checkDefense)->id);
    ases-=(*checkDefense)->cost;

    std::list<float> valores;
    asignarValores(defenses,valores);
    std::list<int> orden;

    //asignarIds(defenses,valores,orden);
    valores=asignar_Orden(defenses,valores,orden);

/**
    float **tabla = new float*[valores.size()];
    for(int i=0;i<valores.size();++i)
    {
      tabla[i]= new float[ases];
      for(int j=0;j<ases;++j)
      {
        //probar sin esto
        tabla[i][j]=0;
      }
    }
    */
    std::vector<std::vector<float>> tabla;
    tabla.resize(valores.size());
    for(int i=0;i<valores.size();++i)
    {
      tabla[i].resize(ases+1);
    }

    //mochila(orden,defenses,valores,ases,&tabla);
    //----------------------------------

    valores.reverse();//Estan introducidos de mayor a menor y los quiero al contrario
    orden.reverse();

    std::list<int> costes=sacar_costes(orden,defenses);
    std::list<float>::iterator itv=valores.begin();
    std::list<int>::iterator it=costes.begin();

    for(int j=0;j<ases+1;++j)
    {

        if((float)j<(*it))
        {
          tabla[0][j]=0;
        }
        else
        {

         tabla[0][j]=*itv;
        }


    }


    for(int i=1;i<valores.size();++i)
    {

      ++it;
      ++itv;
      for(int j=0;j<ases+1;++j)
      {
      //  std::cout<<"i: "<<i<<" j: "<<j<<std::endl;

        if(j==0)tabla[i][j]=0;
        else
        {
          if(j<*it)tabla[i][j]=tabla[i-1][j];
          else tabla[i][j]=std::max(tabla[i-1][j],tabla[i-1][j-*it]+*itv);

        }

      }

    }

    //----------------------------------



    //recuperar_mochila(&tabla,defenses,ases,selectedIDs,orden);
    //--------------------------------------
    orden.reverse();
    costes.reverse();
    std::list<int>::iterator ito=orden.begin();//Ahora la mejor valorada esta al comienzo

    std::list<int>::iterator itc=costes.begin();


    for(int i=orden.size()-1;i>0;--i)
    {
      Defense* actual=sacar_defensa(*ito,defenses);
      if(tabla[i][ases]!=tabla[i-1][ases])
      {

        selectedIDs.push_back(*ito);//se pasa la mejor defensa
        ases=ases-*itc;
      }
      ++ito;
      ++itc;

    }
    if(tabla[0][ases]>0)selectedIDs.push_back(*ito);
    //-----------------------------------------

}
