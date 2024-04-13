static void* empty (int sz) {
	void *p = malloc (sz);
	if (p) memset (p, '\0', sz);
	return p;
 }
