static int unit_set(struct idr *p, void *ptr, int n)
{
	int unit;

	unit = idr_alloc(p, ptr, n, n + 1, GFP_KERNEL);
	if (unit == -ENOSPC)
		unit = -EINVAL;
	return unit;
}
