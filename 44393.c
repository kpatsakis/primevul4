void notrace __ppc64_runlatch_on(void)
{
	struct thread_info *ti = current_thread_info();
	unsigned long ctrl;

	ctrl = mfspr(SPRN_CTRLF);
	ctrl |= CTRL_RUNLATCH;
	mtspr(SPRN_CTRLT, ctrl);

	ti->local_flags |= _TLF_RUNLATCH;
}
