int snd_interval_ranges(struct snd_interval *i, unsigned int count,
			const struct snd_interval *ranges, unsigned int mask)
{
	unsigned int k;
	struct snd_interval range_union;
	struct snd_interval range;

	if (!count) {
		snd_interval_none(i);
		return -EINVAL;
	}
	snd_interval_any(&range_union);
	range_union.min = UINT_MAX;
	range_union.max = 0;
	for (k = 0; k < count; k++) {
		if (mask && !(mask & (1 << k)))
			continue;
		snd_interval_copy(&range, &ranges[k]);
		if (snd_interval_refine(&range, i) < 0)
			continue;
		if (snd_interval_empty(&range))
			continue;

		if (range.min < range_union.min) {
			range_union.min = range.min;
			range_union.openmin = 1;
		}
		if (range.min == range_union.min && !range.openmin)
			range_union.openmin = 0;
		if (range.max > range_union.max) {
			range_union.max = range.max;
			range_union.openmax = 1;
		}
		if (range.max == range_union.max && !range.openmax)
			range_union.openmax = 0;
	}
	return snd_interval_refine(i, &range_union);
}
