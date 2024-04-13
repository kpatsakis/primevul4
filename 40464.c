static inline void blkcipher_unmap_src(struct blkcipher_walk *walk)
{
	scatterwalk_unmap(walk->src.virt.addr);
}
