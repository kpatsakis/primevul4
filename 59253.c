static int perf_event_set_bpf_prog(struct perf_event *event, u32 prog_fd)
{
	bool is_kprobe, is_tracepoint;
	struct bpf_prog *prog;

	if (event->attr.type == PERF_TYPE_HARDWARE ||
	    event->attr.type == PERF_TYPE_SOFTWARE)
		return perf_event_set_bpf_handler(event, prog_fd);

	if (event->attr.type != PERF_TYPE_TRACEPOINT)
		return -EINVAL;

	if (event->tp_event->prog)
		return -EEXIST;

	is_kprobe = event->tp_event->flags & TRACE_EVENT_FL_UKPROBE;
	is_tracepoint = event->tp_event->flags & TRACE_EVENT_FL_TRACEPOINT;
	if (!is_kprobe && !is_tracepoint)
		/* bpf programs can only be attached to u/kprobe or tracepoint */
		return -EINVAL;

	prog = bpf_prog_get(prog_fd);
	if (IS_ERR(prog))
		return PTR_ERR(prog);

	if ((is_kprobe && prog->type != BPF_PROG_TYPE_KPROBE) ||
	    (is_tracepoint && prog->type != BPF_PROG_TYPE_TRACEPOINT)) {
		/* valid fd, but invalid bpf program type */
		bpf_prog_put(prog);
		return -EINVAL;
	}

	if (is_tracepoint) {
		int off = trace_event_get_offsets(event->tp_event);

		if (prog->aux->max_ctx_offset > off) {
			bpf_prog_put(prog);
			return -EACCES;
		}
	}
	event->tp_event->prog = prog;

	return 0;
}
