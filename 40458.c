static inline unsigned int blkcipher_done_fast(struct blkcipher_walk *walk,
					       unsigned int n)
{
	if (walk->flags & BLKCIPHER_WALK_COPY) {
		blkcipher_map_dst(walk);
		memcpy(walk->dst.virt.addr, walk->page, n);
		blkcipher_unmap_dst(walk);
	} else if (!(walk->flags & BLKCIPHER_WALK_PHYS)) {
		if (walk->flags & BLKCIPHER_WALK_DIFF)
			blkcipher_unmap_dst(walk);
		blkcipher_unmap_src(walk);
	}

	scatterwalk_advance(&walk->in, n);
	scatterwalk_advance(&walk->out, n);

	return n;
}
