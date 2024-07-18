## Objectif 

Ce git concerne différentes techniques pour générér un nombre **entier** aléatoire dans une plage donnée, peut importe le générateur aléatoire utilisé en amont.

Ici, Mersenne Twister est utilisé à des fin d'exemples, mais ce git ne concerne pas particulièrement Mersenne Twister, mais plutôt l'usage effectué par un programmeur d'une valeur aléatoire généré par un générateur de nombre aléatoire (peut importe le générateur).

Dans ce bout de code, on regarde combien de fois il faut jouer d'affilé au loto (20'000'000 grilles) avant de gagner en misant toujours sur le même numéro, en utilisant la formule flottante : `valeur_generer = mini + (maxi-mini) * mt_genrand_real1();` afin de générer un nombre entier, où `mt_genrand_real1()` renvoie un flotant (32 bits) entre `[0..1]`, et avec mini = 0 et maxi = 20'000'000.


Cette formule est juste et fonctionne d'un point de vue théorique/mathématique, mais d'un point de vue pratique/informatique je la recommande uniquement pour générer des valeurs aléatoire réelles (des flottants), et non pour générer des valeurs entières.


Normalement, peut importe quelle valeur est choisie entre `[0, 20'000'000[`, le joueur devrait toujours avoir la possibilité de gagner sur le long terme (ie : chaque valeur entre `[0, 20'000'000[` doit sortir, tôt ou tard...), chaque valeur étant équiprobable (1 chance sur 20'000'000).


En misant toujours sur :
- le nombre 1, on fini par gagner au bout de 17929537 tentatives...
- le nombre 64, il faut 18267832 tentatives...


Cependant, à cause de la précision, certains nombre tel que 10000003, 10000009, 10000016, 10000022, 10000028, 10000034, 10000040, 10000047, 10000053, 10000059, 10000065... et plein d'autres ne pourront jamais sortir (dommage pour les joueurs)

## Résultat et Analyse :

Pour générer des valeurs entière, la formule `valeur_generer = mini + (maxi-mini) * mt_genrand_real1();` a plusieurs incovéniant :

- La précision des floattants empêche certaines valeur de se produire
- La répartition n'est pas tout à fait uniforme (je chipote)

On pourrait se dire : 

Très bien, dans ce cas il faut utiliser des `f64` (floats 64 bits) au lieu de `f32`, mais 
: 

- La valeur flottante généré par Mersenne Twister (`mt_genrand_real1()`) dans cette version reste en 32 bits (`f32`), la caster/convertir en float 64 bits (`f64`) ne la rendra pas plus précise

- Pour générer un `u32` dans une plage avec la formule flottante, il faudrait donc pouvoir généré un `f64` aléatoire afin d'avoir plus de précision (de manière à ce que les 52/53 bits de mantisses flottantes suffisent à couvrir tout les cas pour un `u32`), ce qui n'est pas possible avec la version 32 bits de Mersenne Twister. (De plus, la génération d'un flottant par Mersenne Twister est basé sur la génération d'un entier).

```c
/* generates a random number on [0,1]-real-interval */
f32 mt_genrand_real1(void)
{
    return (f32)(mt_genrand_int32()*(1.0/4294967295.0)); 
    /* divided by 2^32-1 */ 
}
```

Donc il faudrait une version 64 bits de Mersenne Twister pour pouvoir générer *proprement* un entier de 32 bits dans une plage.

De plus, pour générer un `u64` dans une plage donnée, avec cette formule flottante, et en possédant une version 64 bits de Mersenne Twister, il faudrait avoir accès à des flottants plus grand que 64 bits (float de 128 bits?, donc la version 128 bits de Mersenne Twister), ce qui n'est pas (encore) possible. 

De plus, la taille de l'ensemble des flottants entre `[0., 1.]` n'est pas forcément un multiple de la taille de la plage voulant ête générer, donc des problèmes d'arrondi favoriseront/défavoriseront certaines valeurs à d'autres.

Exemple : La distribution de **tout** les flottants `[0., 1.]` entre `[0, 1000[` (entier):
(voir le fichier `float_distribution_0_1000.md`)
```cpp
0 : 4294968 val ( 0.100000 %)
1 : 4294967 val ( 0.100000 %)
...
62 : 4294968 val ( 0.100000 %)
63 : 4294976 val ( 0.100000 %)
64 : 4294944 val ( 0.099999 %) // <- défavorisé
65 : 4294976 val ( 0.100000 %)
66 : 4294976 val ( 0.100000 %)
67 : 4294944 val ( 0.099999 %) // <- défavorisé
68 : 4294976 val ( 0.100000 %)
69 : 4294976 val ( 0.100000 %)
70 : 4294976 val ( 0.100000 %)
71 : 4294944 val ( 0.099999 %) // <- défavorisé
...
986 : 4294912 val ( 0.099999 %)
987 : 4294912 val ( 0.099999 %)
988 : 4295168 val ( 0.100005 %) // <- favorisé
989 : 4294912 val ( 0.099999 %)
990 : 4294912 val ( 0.099999 %)
991 : 4294912 val ( 0.099999 %)
992 : 4294912 val ( 0.099999 %)
993 : 4295168 val ( 0.100005 %) // <- favorisé
994 : 4294912 val ( 0.099999 %)
995 : 4294912 val ( 0.099999 %)
996 : 4294912 val ( 0.099999 %)
997 : 4295168 val ( 0.100005 %) // <- favorisé
998 : 4294912 val ( 0.099999 %)
999 : 4294911 val ( 0.099999 %)
```
(Ce résultat peut être obtenu avec la fonction `afficher_distribution_float(0, 1000);` dans le main)

L'erreur de précision constatée est certe très très très petite, infime, ne changera pas grand chose au résultat final, je chipote surtout ;)

Il n'est pas facile de gérer ces problèmes avec des floats à cause de leur encodage, contrairement aux entiers :

```c
// [mini, maxi[ 
// Toute les valeurs sont vraiment équiprobale :)
u32 mt_u32_uniform_exclude(u32 mini, u32 maxi) 
{ 
    check(mini < maxi);
    // range of 0 : ex [4, 5[, always the same number generated
    if(mini == maxi - 1) { return mini; }

    u32 range = maxi - mini;
    // calcul la plus grande valeur possible à stocker qui soit un multiple de la range
    u32 maxi_wrap = (u32_max / range) * range;

    u32 val;
    do
    {
        val = mt_genrand_int32();
        // rejette les quelques rares valeurs plus grande que maxi_wrap
    } while(val >= maxi_wrap); 

    return val % range + mini;
}
```

Ce projet utilise la version 32 bit de Mersenne Twister par Makoto Matsumoto :

http://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/MT2002/emt19937ar.html

Ce git a été mit en place pour David HILL et Benjamin ANTUNES à l'ISIMA (Institut d'informatique d'Auvergne), à des fins de curiosité :)

## Compilation commands :

- Place yourself at the parent folder of `src`
- Don't forget to create a `bin` directory in the same folder as `src`

- Debug Compilation :

```shell
g++ -O0 -g -DIS_IN_DEBUG=1 -DTRACK_MEMORY=1 ./src/betterCPP/*.c* ./src/betterCPP/core/*.c* ./src/betterCPP/low_level/*.c* ./src/betterCPP/simulation/*.c* ./src/betterCPP/mersenne_twister/*.c* ./src/betterCPP/math/*.c* ./src/betterCPP/text/*.c* ./src/*.c* -o ./bin/test.exe -Wall -Wextra -Wcomment -Wno-unknown-pragmas
```

- Optimized Compilation :

```shell
g++ -O3 ./src/betterCPP/*.c* ./src/betterCPP/core/*.c* ./src/betterCPP/low_level/*.c* ./src/betterCPP/math/*.c* ./src/betterCPP/text/*.c* ./src/betterCPP/simulation/*.c* ./src/betterCPP/mersenne_twister/*.c* ./src/*.c* -o ./bin/test.exe -Wall -Wextra -Wcomment -Wno-unknown-pragmas
```

Run :

```shell
./bin/test.exe
```

Output :

```rust
sizeof(u32   ) = 4 bytes
sizeof(fmax  ) = 4 bytes

La valeur 1 a a ete obtenu en 17929537 tentatives
La valeur 64 a a ete obtenu en 18267832 tentatives
cela fait deja 1000000000 tentatives sans arriver a trouver la valeur 10000003 (range 0..20000000)
cela fait deja 2000000000 tentatives sans arriver a trouver la valeur 10000003 (range 0..20000000)
cela fait deja 3000000000 tentatives sans arriver a trouver la valeur 10000003 (range 0..20000000)
cela fait deja 4000000000 tentatives sans arriver a trouver la valeur 10000003 (range 0..20000000)
cela fait deja 5000000000 tentatives sans arriver a trouver la valeur 10000003 (range 0..20000000)
cela fait deja 6000000000 tentatives sans arriver a trouver la valeur 10000003 (range 0..20000000)
...
```


