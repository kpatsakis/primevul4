static int update_branch(struct branch *b)
{
	static const char *msg = "fast-import";
	struct ref_transaction *transaction;
	unsigned char old_sha1[20];
	struct strbuf err = STRBUF_INIT;

	if (is_null_sha1(b->sha1)) {
		if (b->delete)
			delete_ref(b->name, NULL, 0);
		return 0;
	}
	if (read_ref(b->name, old_sha1))
		hashclr(old_sha1);
	if (!force_update && !is_null_sha1(old_sha1)) {
		struct commit *old_cmit, *new_cmit;

		old_cmit = lookup_commit_reference_gently(old_sha1, 0);
		new_cmit = lookup_commit_reference_gently(b->sha1, 0);
		if (!old_cmit || !new_cmit)
			return error("Branch %s is missing commits.", b->name);

		if (!in_merge_bases(old_cmit, new_cmit)) {
			warning("Not updating %s"
				" (new tip %s does not contain %s)",
				b->name, sha1_to_hex(b->sha1), sha1_to_hex(old_sha1));
			return -1;
		}
	}
	transaction = ref_transaction_begin(&err);
	if (!transaction ||
	    ref_transaction_update(transaction, b->name, b->sha1, old_sha1,
				   0, msg, &err) ||
	    ref_transaction_commit(transaction, &err)) {
		ref_transaction_free(transaction);
		error("%s", err.buf);
		strbuf_release(&err);
		return -1;
	}
	ref_transaction_free(transaction);
	strbuf_release(&err);
	return 0;
}
