static struct object_entry *new_object(unsigned char *sha1)
{
	struct object_entry *e;

	if (blocks->next_free == blocks->end)
		alloc_objects(object_entry_alloc);

	e = blocks->next_free++;
	hashcpy(e->idx.sha1, sha1);
	return e;
}
