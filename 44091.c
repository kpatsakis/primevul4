void sk_unattached_filter_destroy(struct sk_filter *fp)
{
	sk_filter_release(fp);
}
