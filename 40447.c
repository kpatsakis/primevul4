int crypto_hash_walk_done(struct crypto_hash_walk *walk, int err)
{
	unsigned int alignmask = walk->alignmask;
	unsigned int nbytes = walk->entrylen;

	walk->data -= walk->offset;

	if (nbytes && walk->offset & alignmask && !err) {
		walk->offset = ALIGN(walk->offset, alignmask + 1);
		walk->data += walk->offset;

		nbytes = min(nbytes,
			     ((unsigned int)(PAGE_SIZE)) - walk->offset);
		walk->entrylen -= nbytes;

		return nbytes;
	}

	kunmap_atomic(walk->data);
	crypto_yield(walk->flags);

	if (err)
		return err;

	if (nbytes) {
		walk->offset = 0;
		walk->pg++;
		return hash_walk_next(walk);
	}

	if (!walk->total)
		return 0;

	walk->sg = scatterwalk_sg_next(walk->sg);

	return hash_walk_new_entry(walk);
}
