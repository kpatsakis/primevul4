static inline void blkcipher_map_dst(struct blkcipher_walk *walk)
{
	walk->dst.virt.addr = scatterwalk_map(&walk->out);
}
