static struct digest_algo *digest_algo_get_by_algo(enum hash_algo algo)
{
	struct digest_algo *d = NULL;
	struct digest_algo *tmp;
	int priority = -1;

	list_for_each_entry(tmp, &digests, list) {
		if (tmp->base.algo != algo)
			continue;

		if (tmp->base.priority <= priority)
			continue;

		d = tmp;
		priority = tmp->base.priority;
	}

	return d;
}