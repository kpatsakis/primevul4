static void audit_log_proctitle(struct task_struct *tsk,
			 struct audit_context *context)
{
	int res;
	char *buf;
	char *msg = "(null)";
	int len = strlen(msg);
	struct audit_buffer *ab;

	ab = audit_log_start(context, GFP_KERNEL, AUDIT_PROCTITLE);
	if (!ab)
		return;	/* audit_panic or being filtered */

	audit_log_format(ab, "proctitle=");

	/* Not  cached */
	if (!context->proctitle.value) {
		buf = kmalloc(MAX_PROCTITLE_AUDIT_LEN, GFP_KERNEL);
		if (!buf)
			goto out;
		/* Historically called this from procfs naming */
		res = get_cmdline(tsk, buf, MAX_PROCTITLE_AUDIT_LEN);
		if (res == 0) {
			kfree(buf);
			goto out;
		}
		res = audit_proctitle_rtrim(buf, res);
		if (res == 0) {
			kfree(buf);
			goto out;
		}
		context->proctitle.value = buf;
		context->proctitle.len = res;
	}
	msg = context->proctitle.value;
	len = context->proctitle.len;
out:
	audit_log_n_untrustedstring(ab, msg, len);
	audit_log_end(ab);
}
