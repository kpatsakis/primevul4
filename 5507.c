void bpf_verifier_vlog(struct bpf_verifier_log *log, const char *fmt,
		       va_list args)
{
	unsigned int n;

	n = vscnprintf(log->kbuf, BPF_VERIFIER_TMP_LOG_SIZE, fmt, args);

	WARN_ONCE(n >= BPF_VERIFIER_TMP_LOG_SIZE - 1,
		  "verifier log line truncated - local buffer too short\n");

	if (log->level == BPF_LOG_KERNEL) {
		bool newline = n > 0 && log->kbuf[n - 1] == '\n';

		pr_err("BPF: %s%s", log->kbuf, newline ? "" : "\n");
		return;
	}

	n = min(log->len_total - log->len_used - 1, n);
	log->kbuf[n] = '\0';
	if (!copy_to_user(log->ubuf + log->len_used, log->kbuf, n + 1))
		log->len_used += n;
	else
		log->ubuf = NULL;
}