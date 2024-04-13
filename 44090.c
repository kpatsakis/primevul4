int sk_unattached_filter_create(struct sk_filter **pfp,
				struct sock_fprog *fprog)
{
	unsigned int fsize = sk_filter_proglen(fprog);
	struct sk_filter *fp;

	/* Make sure new filter is there and in the right amounts. */
	if (fprog->filter == NULL)
		return -EINVAL;

	fp = kmalloc(sk_filter_size(fprog->len), GFP_KERNEL);
	if (!fp)
		return -ENOMEM;

	memcpy(fp->insns, fprog->filter, fsize);

	atomic_set(&fp->refcnt, 1);
	fp->len = fprog->len;
	/* Since unattached filters are not copied back to user
	 * space through sk_get_filter(), we do not need to hold
	 * a copy here, and can spare us the work.
	 */
	fp->orig_prog = NULL;

	/* __sk_prepare_filter() already takes care of uncharging
	 * memory in case something goes wrong.
	 */
	fp = __sk_prepare_filter(fp, NULL);
	if (IS_ERR(fp))
		return PTR_ERR(fp);

	*pfp = fp;
	return 0;
}
