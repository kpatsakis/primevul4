void skcipher_walk_atomise(struct skcipher_walk *walk)
{
	walk->flags &= ~SKCIPHER_WALK_SLEEP;
}
