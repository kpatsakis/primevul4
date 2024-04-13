static inline gfp_t skcipher_walk_gfp(struct skcipher_walk *walk)
{
	return walk->flags & SKCIPHER_WALK_SLEEP ? GFP_KERNEL : GFP_ATOMIC;
}
