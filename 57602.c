static inline void skcipher_map_dst(struct skcipher_walk *walk)
{
	walk->dst.virt.addr = skcipher_map(&walk->out);
}
