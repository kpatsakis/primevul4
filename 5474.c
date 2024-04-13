static void bpf_vlog_reset(struct bpf_verifier_log *log, u32 new_pos)
{
	char zero = 0;

	if (!bpf_verifier_log_needed(log))
		return;

	log->len_used = new_pos;
	if (put_user(zero, log->ubuf + new_pos))
		log->ubuf = NULL;
}