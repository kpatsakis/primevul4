static Node *check_file(struct linux_binprm *bprm)
{
	char *p = strrchr(bprm->interp, '.');
	struct list_head *l;

	list_for_each(l, &entries) {
		Node *e = list_entry(l, Node, list);
		char *s;
		int j;

		if (!test_bit(Enabled, &e->flags))
			continue;

		if (!test_bit(Magic, &e->flags)) {
			if (p && !strcmp(e->magic, p + 1))
				return e;
			continue;
		}

		s = bprm->buf + e->offset;
		if (e->mask) {
			for (j = 0; j < e->size; j++)
				if ((*s++ ^ e->magic[j]) & e->mask[j])
					break;
		} else {
			for (j = 0; j < e->size; j++)
				if ((*s++ ^ e->magic[j]))
					break;
		}
		if (j == e->size)
			return e;
	}
	return NULL;
}
