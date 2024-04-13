int snd_interval_list(struct snd_interval *i, unsigned int count,
		      const unsigned int *list, unsigned int mask)
{
        unsigned int k;
	struct snd_interval list_range;

	if (!count) {
		i->empty = 1;
		return -EINVAL;
	}
	snd_interval_any(&list_range);
	list_range.min = UINT_MAX;
	list_range.max = 0;
        for (k = 0; k < count; k++) {
		if (mask && !(mask & (1 << k)))
			continue;
		if (!snd_interval_test(i, list[k]))
			continue;
		list_range.min = min(list_range.min, list[k]);
		list_range.max = max(list_range.max, list[k]);
        }
	return snd_interval_refine(i, &list_range);
}
