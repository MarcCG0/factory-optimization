### Pràctica-AP3-Q3-2022
Aquest és un repositori per anar actualitzant la pràctica d'AP3

## Nomenclatura: 
- `C` : Nombre de cotxes
- `M` : Millores
- `K` : Classes (subconjunt de millores)
- `n_e`: Conjunt de n cotxes consecutius a la estació `e`
- `c_e`: Com a molt `c_e` dels `n_e` poden requerir l'instal·lació de la millora

## Penalització
Si no es compleixen les restriccions de `n_e` i `c_e`, hi ha unes penalitzacions. La penalització total s'obte sumant les penalitzacions de totes les estacions. Si s'excedeix `c_e`, la penalització és de `k-c_e` euros, on `k` no és res més que la quantitat de cotxes consecutius que han requerit la millora i han excedit el màxim permès en aquella estació. 
- Nota: les finestres incompletes de cotxes també es tenen en compte (inici i final de la seqüència). 

## Format d'entrada: 
- 3 naturals estrictament majors que zero: `C, M, K`. Nombre de cotxes, millores i classes. 
- `M` enters estrictament majors que zero amb la `c_e` de cada millora (es millores tenen per nom nombres {0...n} i totes es fan en estacions diferents). 
- `M` enters estrictament majors que zero indicant `n_e` (també per a cada millora)
- `K` línies i a cada línia: un natural que identifica la classe, un natural que indica el nombre de cotxes a produir d'aquesta classe, i `M` 0's i 1's que indiquen si la classe requereix una millora o no {0...n}

## Entrada: 
El programa rebrà dos arguments per línia de comandes: 
- el nom de fitxer d'entrada
- el nom de fitxer de sortida 

### Entrega
- Tres arxius: exh.cc, greedy.cc i mh.cc (NO COMPRIMITS)!!!
- `exh.cc` : cerca exhaustiva i sempre sobreescriure la última resposta òptima trobada
- `greedy.cc` : objectiu: no trobar solució súper òptima, però que sigui ràpid (mig segon en les instàncies més grans)
- `mh.cc` : (no Basic Local Search !!!), sobreescriure la última resposta òptima trobada

*Entrega al racó de la FIB | Deadline 9 de Gener*

# Criteris avaluació (2pts)
- 0.5 pts Llegibilitat del codi i correctesa. 
- 1.5 pts La qualitat i rapidesa del programa (trobar solucions òptimes ràpid)

# Checker
- Servirà per comprovar que el format de sortida sigui correcte i que la solució tingui el cost que es diu que té
- Alguns dels costos òptims estan a resultsEasy.txt per comprovar si ho estem fent bé


### Funcions C++
## Escriptura en un fitxer
- Aquest és un exemple d'escriptura en un fitxer (ens interesa doncs se sobreescriu borrant anteriors coses tal i com se'ns demana)
```c++
// basic file operations
#include <iostream>
#include <fstream>
using namespace std;

int main () {
  ofstream myfile;
  myfile.open ("example.txt");
  myfile << "Writing this to a file.\n";
  myfile.close();
  return 0;
}
````

### Autors
- Ignacio Gris Martín
- Marc Camps Garreta 

*UPC GCED Curs 2022-23*
