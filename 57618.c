int skcipher_walk_async(struct skcipher_walk *walk,
			struct skcipher_request *req)
{
	walk->flags |= SKCIPHER_WALK_PHYS;

	INIT_LIST_HEAD(&walk->buffers);

	return skcipher_walk_skcipher(walk, req);
}
