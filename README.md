# Pràctica-AP3-Q3-2022
Aquest és un repositori per anar actualitzant la pràctica d'AP3

# Nomenclatura: 
- C : Nombre de cotxes
- M : Millores
- K : Classes (subconjunt de millores)
- n_e: Conjunt de n cotxes consecutius a la estació e
- c_e: Com a molt c_e dels n_e poden requerir l'instal·lació de la millora

# Penalització
Si no es compleixen les restriccions de n_e i c_e, hi ha unes penalitzacions. La penalització total s'obte sumant les penalitzacions de totes les estacions. Si s'excedeix c_e, la penalització és de k-c_e euros, on k no és res més que la quantitat de cotxes consecutius que han requerit la millora i han excedit el màxim permès en aquella estació. 
- Nota: les finestres incompletes de cotxes també es tenen en compte (inici i final de la seqüència). 

# Format d'entrada: 
- 3 naturals estrictament majors que zero: C, M, K. Nombre de cotxes, millores i classes. 
- M enters estrictament majors que zero amb la c_e de cada millora (es millores tenen per nom nombres {0...n} i totes es fan en estacions diferents). 
- M enters estrictament majors que zero indicant n_e (també per a cada millora)
- K línies i a cada línia: un natural que identifica la classe, un natural que indica el nombre de cotxes a produir d'aquesta classe, i M 0's i 1's que indiquen si la classe requereix una millora o no {0...n}
