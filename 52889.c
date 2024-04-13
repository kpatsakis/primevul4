static void dump_marks_helper(FILE *f,
	uintmax_t base,
	struct mark_set *m)
{
	uintmax_t k;
	if (m->shift) {
		for (k = 0; k < 1024; k++) {
			if (m->data.sets[k])
				dump_marks_helper(f, base + (k << m->shift),
					m->data.sets[k]);
		}
	} else {
		for (k = 0; k < 1024; k++) {
			if (m->data.marked[k])
				fprintf(f, ":%" PRIuMAX " %s\n", base + k,
					sha1_to_hex(m->data.marked[k]->idx.sha1));
		}
	}
}
