static inline void blkcipher_unmap_dst(struct blkcipher_walk *walk)
{
	scatterwalk_unmap(walk->dst.virt.addr);
}
