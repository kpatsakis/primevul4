static inline void skcipher_map_src(struct skcipher_walk *walk)
{
	walk->src.virt.addr = skcipher_map(&walk->in);
}
