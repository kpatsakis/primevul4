static int sk_store_orig_filter(struct sk_filter *fp,
				const struct sock_fprog *fprog)
{
	unsigned int fsize = sk_filter_proglen(fprog);
	struct sock_fprog_kern *fkprog;

	fp->orig_prog = kmalloc(sizeof(*fkprog), GFP_KERNEL);
	if (!fp->orig_prog)
		return -ENOMEM;

	fkprog = fp->orig_prog;
	fkprog->len = fprog->len;
	fkprog->filter = kmemdup(fp->insns, fsize, GFP_KERNEL);
	if (!fkprog->filter) {
		kfree(fp->orig_prog);
		return -ENOMEM;
	}

	return 0;
}
