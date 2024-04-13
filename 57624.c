int skcipher_walk_virt(struct skcipher_walk *walk,
		       struct skcipher_request *req, bool atomic)
{
	int err;

	walk->flags &= ~SKCIPHER_WALK_PHYS;

	err = skcipher_walk_skcipher(walk, req);

	walk->flags &= atomic ? ~SKCIPHER_WALK_SLEEP : ~0;

	return err;
}
