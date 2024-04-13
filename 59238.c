static void perf_event_free_bpf_handler(struct perf_event *event)
{
	struct bpf_prog *prog = event->prog;

	if (!prog)
		return;

	WRITE_ONCE(event->overflow_handler, event->orig_overflow_handler);
	event->prog = NULL;
	bpf_prog_put(prog);
}
