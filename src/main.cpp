#include "__base__.hpp"

// Balaye l'ensemble des floattants pouvant être générés pour les assigner
// à une valeur dans une plage, puis affiche la distribution obtenu (qui doit être linéaire)
void afficher_distribution_float(u32 mini, u32 maxi)
{
    check(mini < maxi);
    u32 range = maxi - mini;

    vec<u32> qte_par_val = vec<u32>(maxi);
    repeat(i, range) { qte_par_val.push(0); }

    // Balaye l'ensemble de tout les floats générés par Mersenne Twister
    u32 i = 0;
    do
    {
        // u32 to [0, 1]. Même forume utilisé par mt_genrand_real1
        fmax f = mt_u32_to_fmax(i);
        u32 idx = f * range;
        if (idx < qte_par_val.length()) { qte_par_val[idx]++; }

        i++;
    }while(i != 0);

    printf("distribution avec des floattants (32 bits) entre %" u32_format " et %" u32_format " :\n", mini, maxi);
    repeat(i, qte_par_val.length())
    {
        printf("%" u32_format " : %" u32_format " val ( %" fmax_format " %%)\n", (u32)i + mini, qte_par_val[i], qte_par_val[i] * 100.0 / u32_max);
    }
    printf("\n");
}

typedef union 
{
    u32 as_uint;
    fmax as_float;
} Uint2Float;

void trouver_valeur_impossible_a_obtenir(u32 maxi)
{
    check(sizeof(u32) == sizeof(fmax)); // 4 octets chacun (32 bits)

    vec<bool> valeur_possible_a_obtenir = vec<bool>(maxi);
    repeat(i, maxi) { valeur_possible_a_obtenir.push(false); }

    u32 i = 0;
    do
    {
        Uint2Float uint2float;
        uint2float.as_uint = i;
        float f = uint2float.as_float;

        u32 idx = f * maxi; // Même formule si mini = 0
        if (idx < maxi) { valeur_possible_a_obtenir[idx] = true; }

        i++;
    }while(i != 0);

    printf("pour generer un nombre entre 0 et %" u32_format ", les nombres suivants sont impossibles a obtenir :\n", maxi);

    repeat(idx, maxi) 
    { 
        if (valeur_possible_a_obtenir[idx] == false)
        {
            printf("%" u32_format ", ", (u32)idx);
        }
    }
    printf("fin\n");
}

void combien_de_participation_avant_de_gagner_avec_le_num(u32 valeur_voulu, u32 maxi)
{
    u32 mini = 0;
    u32 valeur_generer;

    check(valeur_voulu < maxi);

    u64 tentatives = 0;

    u32 init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    mt_init_by_array(init, length);

    do
    {
        tentatives++;
        check_msg("nombre de tentative maximale atteinte", tentatives != u64_max);

        // revient à faire ceci car mini = 0
        // valeur_generer = maxi * mt_genrand_real1()
        //
        // typeof(mt_genrand_real1()) -> float 32 bits
        valeur_generer = mini + (maxi-mini) * mt_genrand_real1();

        // Il n'y aura pas de problème avec cette fonction :)
        // valeur_generer = mt_u32_uniform_exclude(0, maxi);

        if (tentatives % 1000000000 == 0) 
        { 
            printf("cela fait deja %" u64_format " tentatives sans arriver a trouver la valeur %" u32_format " (range 0..%" u32_format ")\n", tentatives, valeur_voulu, maxi);
        }

    }while(valeur_voulu != valeur_generer);


    printf("La valeur %" u32_format " a a ete obtenu en %" u64_format " tentatives\n", valeur_generer, tentatives);
}

int main(int argc, char const* argv[])
{
    unused(argc); unused(argv);

    only_in_debug(printf("%sCompiled%s at %s \n\n", COLOR_DEBUG_CATEGORY, COLOR_RESET, current_time));

    print_size(u32);
    print_size(fmax);
    check(sizeof(fmax) * 8 == 32); // 32 bit pour les float
    printf("\n");

    /*

    Pour afficher la distributions des nombre entre 0 et 1000 en utilisant les floats
    des petits écarts peuvent constater en raison de la précision 

    975 : 4294912 val ( 0.099999 %)
    976 : 4294912 val ( 0.099999 %)
    977 : 4294912 val ( 0.099999 %)
    978 : 4294912 val ( 0.099999 %)
    979 : 4295168 val ( 0.100005 %)
    980 : 4294912 val ( 0.099999 %)
    981 : 4294912 val ( 0.099999 %)
    982 : 4294912 val ( 0.099999 %)
    983 : 4295168 val ( 0.100005 %)
    984 : 4294912 val ( 0.099999 %)
    985 : 4294912 val ( 0.099999 %)
    986 : 4294912 val ( 0.099999 %)
    987 : 4294912 val ( 0.099999 %)
    988 : 4295168 val ( 0.100005 %)
    */
    // afficher_distribution_float(0, 1000);

    // La cassure se trouve entre 16 et 17 millions. 
    // La mantisse des float possède 23 bits de precision (24 bits avec le 1er un implicite), 
    // mais c'est compliquer à estimer/trouver ou elle se trouve précisément à cause de la précision variable

    // et 2 puissance 24 = 16777216
    u32 nb_ticket = 20'000'000;

    // désactiver ce commentaire pour afficher les valeurs impossibles:
    //trouver_valeur_impossible_a_obtenir(nb_ticket);

    combien_de_participation_avant_de_gagner_avec_le_num(1, nb_ticket);
    combien_de_participation_avant_de_gagner_avec_le_num(64, nb_ticket);

    // suspens... se termine jamais à cause de la précision
    combien_de_participation_avant_de_gagner_avec_le_num(10'000'003, nb_ticket);
    // même histoire pour 10000003, 10000009, 10000016, 10000022, 10000028, 10000034, 10000040, 10000047, 10000053, 10000059, 10000065... et plein d'autres

    return EXIT_SUCCESS;
}