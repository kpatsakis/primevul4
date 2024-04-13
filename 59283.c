void perf_trace_run_bpf_submit(void *raw_data, int size, int rctx,
			       struct trace_event_call *call, u64 count,
			       struct pt_regs *regs, struct hlist_head *head,
			       struct task_struct *task)
{
	struct bpf_prog *prog = call->prog;

	if (prog) {
		*(struct pt_regs **)raw_data = regs;
		if (!trace_call_bpf(prog, raw_data) || hlist_empty(head)) {
			perf_swevent_put_recursion_context(rctx);
			return;
		}
	}
	perf_tp_event(call->event.type, count, raw_data, size, regs, head,
		      rctx, task);
}
