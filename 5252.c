static struct digest_algo *digest_algo_get_by_name(const char *name)
{
	struct digest_algo *d = NULL;
	struct digest_algo *tmp;
	int priority = -1;

	if (!name)
		return NULL;

	list_for_each_entry(tmp, &digests, list) {
		if (strcmp(tmp->base.name, name) != 0)
			continue;

		if (tmp->base.priority <= priority)
			continue;

		d = tmp;
		priority = tmp->base.priority;
	}

	return d;
}