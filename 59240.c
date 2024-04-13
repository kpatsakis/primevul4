static void perf_event_free_bpf_prog(struct perf_event *event)
{
	struct bpf_prog *prog;

	perf_event_free_bpf_handler(event);

	if (!event->tp_event)
		return;

	prog = event->tp_event->prog;
	if (prog) {
		event->tp_event->prog = NULL;
		bpf_prog_put(prog);
	}
}
