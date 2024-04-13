static void unit_put(struct idr *p, int n)
{
	idr_remove(p, n);
}
