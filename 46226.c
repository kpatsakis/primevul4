static struct extent_map *next_extent_map(struct extent_map *em)
{
	struct rb_node *next;

	next = rb_next(&em->rb_node);
	if (!next)
		return NULL;
	return container_of(next, struct extent_map, rb_node);
}
