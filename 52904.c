static struct object_entry *insert_object(unsigned char *sha1)
{
	unsigned int h = sha1[0] << 8 | sha1[1];
	struct object_entry *e = object_table[h];

	while (e) {
		if (!hashcmp(sha1, e->idx.sha1))
			return e;
		e = e->next;
	}

	e = new_object(sha1);
	e->next = object_table[h];
	e->idx.offset = 0;
	object_table[h] = e;
	return e;
}
