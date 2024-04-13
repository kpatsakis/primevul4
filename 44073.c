static struct sk_filter *__sk_migrate_realloc(struct sk_filter *fp,
					      struct sock *sk,
					      unsigned int len)
{
	struct sk_filter *fp_new;

	if (sk == NULL)
		return krealloc(fp, len, GFP_KERNEL);

	fp_new = sock_kmalloc(sk, len, GFP_KERNEL);
	if (fp_new) {
		memcpy(fp_new, fp, sizeof(struct sk_filter));
		/* As we're kepping orig_prog in fp_new along,
		 * we need to make sure we're not evicting it
		 * from the old fp.
		 */
		fp->orig_prog = NULL;
		sk_filter_uncharge(sk, fp);
	}

	return fp_new;
}
