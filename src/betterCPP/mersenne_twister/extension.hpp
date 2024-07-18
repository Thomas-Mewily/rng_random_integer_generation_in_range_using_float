#ifndef mersenne_twister_extension
#define mersenne_twister_extension

#include "__base__.hpp"

void mt_default_init();
fmax mt_real_uniform(fmax a, fmax b);
// [0, 1] 
fmax mt_real();

u32 mt_u32();
// [a, b]
u32 mt_u32_uniform(u32 mini, u32 maxi);

// u32 to [0, 1]. Same forumla used in mt_genrand_real1
fmax mt_u32_to_fmax(u32 val);

// [a, b[ 
u32 mt_u32_uniform_exclude(u32 mini, u32 maxi);

fmax mt_neg_exp(fmax mean);
fmax mt_neg_exp(fmax mean, fmax rng_val_strictly_positive);


#endif