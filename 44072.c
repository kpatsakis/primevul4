static struct sk_filter *__sk_migrate_filter(struct sk_filter *fp,
					     struct sock *sk)
{
	struct sock_filter *old_prog;
	struct sk_filter *old_fp;
	int i, err, new_len, old_len = fp->len;

	/* We are free to overwrite insns et al right here as it
	 * won't be used at this point in time anymore internally
	 * after the migration to the internal BPF instruction
	 * representation.
	 */
	BUILD_BUG_ON(sizeof(struct sock_filter) !=
		     sizeof(struct sock_filter_int));

	/* For now, we need to unfiddle BPF_S_* identifiers in place.
	 * This can sooner or later on be subject to removal, e.g. when
	 * JITs have been converted.
	 */
	for (i = 0; i < fp->len; i++)
		sk_decode_filter(&fp->insns[i], &fp->insns[i]);

	/* Conversion cannot happen on overlapping memory areas,
	 * so we need to keep the user BPF around until the 2nd
	 * pass. At this time, the user BPF is stored in fp->insns.
	 */
	old_prog = kmemdup(fp->insns, old_len * sizeof(struct sock_filter),
			   GFP_KERNEL);
	if (!old_prog) {
		err = -ENOMEM;
		goto out_err;
	}

	/* 1st pass: calculate the new program length. */
	err = sk_convert_filter(old_prog, old_len, NULL, &new_len);
	if (err)
		goto out_err_free;

	/* Expand fp for appending the new filter representation. */
	old_fp = fp;
	fp = __sk_migrate_realloc(old_fp, sk, sk_filter_size(new_len));
	if (!fp) {
		/* The old_fp is still around in case we couldn't
		 * allocate new memory, so uncharge on that one.
		 */
		fp = old_fp;
		err = -ENOMEM;
		goto out_err_free;
	}

	fp->bpf_func = sk_run_filter_int_skb;
	fp->len = new_len;

	/* 2nd pass: remap sock_filter insns into sock_filter_int insns. */
	err = sk_convert_filter(old_prog, old_len, fp->insnsi, &new_len);
	if (err)
		/* 2nd sk_convert_filter() can fail only if it fails
		 * to allocate memory, remapping must succeed. Note,
		 * that at this time old_fp has already been released
		 * by __sk_migrate_realloc().
		 */
		goto out_err_free;

	kfree(old_prog);
	return fp;

out_err_free:
	kfree(old_prog);
out_err:
	/* Rollback filter setup. */
	if (sk != NULL)
		sk_filter_uncharge(sk, fp);
	else
		kfree(fp);
	return ERR_PTR(err);
}
