int blkcipher_walk_virt(struct blkcipher_desc *desc,
			struct blkcipher_walk *walk)
{
	walk->flags &= ~BLKCIPHER_WALK_PHYS;
	walk->blocksize = crypto_blkcipher_blocksize(desc->tfm);
	return blkcipher_walk_first(desc, walk);
}
