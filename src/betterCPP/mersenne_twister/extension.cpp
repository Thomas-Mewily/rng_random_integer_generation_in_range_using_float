
#include <math.h>
#include "__base__.hpp"

fmax mt_real_uniform(fmax mini, fmax maxi)
{
    return mt_real()*(maxi-mini)+mini;
}

fmax mt_real()
{
    return mt_genrand_real1();
}

u32 mt_u32(){ return mt_genrand_int32(); }

// [a, b[ 
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

// [a, b]
u32 mt_u32_uniform(u32 mini, u32 maxi) 
{ 
    check(maxi != u32_max);
    return mt_u32_uniform_exclude(mini, maxi+1);
}

fmax mt_neg_exp(fmax mean, fmax rng_val_strictly_positive)
{
    return -mean * log(rng_val_strictly_positive); // zero excluded
}

fmax mt_neg_exp(fmax mean)
{
    return mt_neg_exp(mean, 1-mt_genrand_real2()); // zero excluded
}

fmax mt_u32_to_fmax(u32 val)
{
    return (fmax)(val*(1.0/4294967295.0)); 
}