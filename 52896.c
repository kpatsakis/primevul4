static struct object_entry *find_object(unsigned char *sha1)
{
	unsigned int h = sha1[0] << 8 | sha1[1];
	struct object_entry *e;
	for (e = object_table[h]; e; e = e->next)
		if (!hashcmp(sha1, e->idx.sha1))
			return e;
	return NULL;
}
