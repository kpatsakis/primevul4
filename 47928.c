static sem_t *lxc_new_unnamed_sem(void)
{
	sem_t *s;
	int ret;

	s = malloc(sizeof(*s));
	if (!s)
		return NULL;
	ret = sem_init(s, 0, 1);
	if (ret) {
		free(s);
		return NULL;
	}
	return s;
}
