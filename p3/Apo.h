#ifndef APO_H
#define APO_H

#include <cassert>
template <typename T> class Apo {
public:
    explicit Apo(size_t maxNodos); // constructor
    void insertar(const T& e);
    void suprimir();
    T cima() const;
    bool vacio() const;
    Apo(const Apo<T>& a); // ctor. de copia
    Apo<T>& operator =(const Apo<T>& a); // asignaci�n de apo

    ~Apo(); // destructor
private:
    typedef int nodo; // �ndice del vector
    // entre 0 y maxNodos-1
    T* nodos; // vector de nodos
    int maxNodos; // tama�o del vector
    nodo ultimo; // �ltimo nodo del �rbol
};

template <typename T>
inline Apo<T>::Apo(size_t maxNodos) :
    nodos(new T[maxNodos]),
    maxNodos(maxNodos),
    ultimo(-1) // apo vac�o
{}

template <typename T>
inline T Apo<T>::cima() const
{
    assert(ultimo > -1); // apo no vac�o
    return nodos[0];
}
template <typename T>
inline bool Apo<T>::vacio() const
{
    return (ultimo > -1);
}

template <typename T>
inline void Apo<T>::insertar(const T& e)
{
    assert(ultimo < maxNodos-1); // apo no lleno
    nodo p = ++ultimo;
    while (p > 0 && e < nodos[(p-1)/2]) { // flotar e
        nodos[p] = nodos[(p-1)/2];
        p = (p-1)/2;
    }
    nodos[p] = e;
}

template <typename T>
inline void Apo<T>::suprimir()
{
    assert(ultimo > -1); // apo no vac�o
    T e = nodos[ultimo];
    --ultimo;
    if (ultimo > -1) { // apo no vac�o.
       nodo p = 0;
       if (ultimo > 0) { // quedan dos o m�s elementos. Reordenar
          bool fin = false;
          while (p <= (ultimo-1)/2 && !fin) { // hundir e
            nodo pMin; // hijo menor del nodo p
            if (2*p+1 == ultimo) pMin = 2*p+1;
            else if (nodos[2*p+1] < nodos[2*p+2]) pMin = 2*p+1;
            else pMin = 2*p+2;
            if (nodos[pMin] < e) { // subir el hijo menor
               nodos[p] = nodos[pMin];
               p = pMin;
            }
            else // e <= hijos
                 fin = true;
          }
       }
       nodos[p] = e; // colocar e
    }
}

template <typename T>
inline Apo<T>::~Apo()
{
    delete[] nodos;
}

template <typename T>
Apo<T>::Apo(const Apo<T>& a) :
    nodos(new T[a.maxNodos]),
    maxNodos(a.maxNodos),
    ultimo(a.ultimo)
{
    // copiar el vector
    for (nodo n = 0; n <= ultimo; n++)
        nodos[n] = a.nodos[n];
}

template <typename T>
Apo<T>& Apo<T>::operator =(const Apo<T>& a)
{
    if (this != &a) { // evitar autoasignaci�n
       // Destruir el vector y crear uno nuevo si es necesario
       if (maxNodos != a.maxNodos) {
          delete[] nodos;
          maxNodos = a.maxNodos;
          nodos = new T[maxNodos];
       }
       ultimo = a.ultimo;
       // Copiar el vector
       for (nodo n = 0; n <= ultimo; n++)
         nodos[n] = a.nodos[n];
    }
    return *this;
}
#endif // APO_H
