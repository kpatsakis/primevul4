static inline void skcipher_unmap_dst(struct skcipher_walk *walk)
{
	skcipher_unmap(&walk->out, walk->dst.virt.addr);
}
