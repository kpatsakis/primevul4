remove_option(struct mtree_option **global, const char *value, size_t len)
{
	struct mtree_option *iter, *last;

	last = NULL;
	for (iter = *global; iter != NULL; last = iter, iter = iter->next) {
		if (strncmp(iter->value, value, len) == 0 &&
		    (iter->value[len] == '\0' ||
		     iter->value[len] == '='))
			break;
	}
	if (iter == NULL)
		return;
	if (last == NULL)
		*global = iter->next;
	else
		last->next = iter->next;

	free(iter->value);
	free(iter);
}
