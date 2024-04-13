void bpf_jit_free(struct sk_filter *fp)
{
	if (fp->bpf_func != sk_run_filter) {
		struct work_struct *work = (struct work_struct *)fp->bpf_func;

		INIT_WORK(work, jit_free_defer);
		schedule_work(work);
	}
}
