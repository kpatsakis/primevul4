static void free_pg_vec(void **pg_vec, unsigned int order, unsigned int len)
{
	unsigned int i;

	for (i = 0; i < len; i++) {
		if (pg_vec[i] != NULL) {
			if (is_vmalloc_addr(pg_vec[i]))
				vfree(pg_vec[i]);
			else
				free_pages((unsigned long)pg_vec[i], order);
		}
	}
	kfree(pg_vec);
}
