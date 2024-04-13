static void dump_tags(void)
{
	static const char *msg = "fast-import";
	struct tag *t;
	struct strbuf ref_name = STRBUF_INIT;
	struct strbuf err = STRBUF_INIT;
	struct ref_transaction *transaction;

	transaction = ref_transaction_begin(&err);
	if (!transaction) {
		failure |= error("%s", err.buf);
		goto cleanup;
	}
	for (t = first_tag; t; t = t->next_tag) {
		strbuf_reset(&ref_name);
		strbuf_addf(&ref_name, "refs/tags/%s", t->name);

		if (ref_transaction_update(transaction, ref_name.buf,
					   t->sha1, NULL, 0, msg, &err)) {
			failure |= error("%s", err.buf);
			goto cleanup;
		}
	}
	if (ref_transaction_commit(transaction, &err))
		failure |= error("%s", err.buf);

 cleanup:
	ref_transaction_free(transaction);
	strbuf_release(&ref_name);
	strbuf_release(&err);
}
