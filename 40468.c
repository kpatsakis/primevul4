int blkcipher_walk_virt_block(struct blkcipher_desc *desc,
			      struct blkcipher_walk *walk,
			      unsigned int blocksize)
{
	walk->flags &= ~BLKCIPHER_WALK_PHYS;
	walk->blocksize = blocksize;
	return blkcipher_walk_first(desc, walk);
}
