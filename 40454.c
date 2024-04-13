static int hash_walk_next(struct crypto_hash_walk *walk)
{
	unsigned int alignmask = walk->alignmask;
	unsigned int offset = walk->offset;
	unsigned int nbytes = min(walk->entrylen,
				  ((unsigned int)(PAGE_SIZE)) - offset);

	walk->data = kmap_atomic(walk->pg);
	walk->data += offset;

	if (offset & alignmask) {
		unsigned int unaligned = alignmask + 1 - (offset & alignmask);
		if (nbytes > unaligned)
			nbytes = unaligned;
	}

	walk->entrylen -= nbytes;
	return nbytes;
}
