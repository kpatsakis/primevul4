static inline void blkcipher_map_src(struct blkcipher_walk *walk)
{
	walk->src.virt.addr = scatterwalk_map(&walk->in);
}
