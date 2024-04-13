static int skcipher_done_slow(struct skcipher_walk *walk, unsigned int bsize)
{
	u8 *addr;

	addr = (u8 *)ALIGN((unsigned long)walk->buffer, walk->alignmask + 1);
	addr = skcipher_get_spot(addr, bsize);
	scatterwalk_copychunks(addr, &walk->out, bsize,
			       (walk->flags & SKCIPHER_WALK_PHYS) ? 2 : 1);
	return 0;
}
