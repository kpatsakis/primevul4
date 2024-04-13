static void free_event(struct perf_event *event)
{
	irq_work_sync(&event->pending);

	if (!event->parent) {
		if (event->attach_state & PERF_ATTACH_TASK)
			static_key_slow_dec_deferred(&perf_sched_events);
		if (event->attr.mmap || event->attr.mmap_data)
			atomic_dec(&nr_mmap_events);
		if (event->attr.comm)
			atomic_dec(&nr_comm_events);
		if (event->attr.task)
			atomic_dec(&nr_task_events);
		if (event->attr.sample_type & PERF_SAMPLE_CALLCHAIN)
			put_callchain_buffers();
		if (is_cgroup_event(event)) {
			atomic_dec(&per_cpu(perf_cgroup_events, event->cpu));
			static_key_slow_dec_deferred(&perf_sched_events);
		}

		if (has_branch_stack(event)) {
			static_key_slow_dec_deferred(&perf_sched_events);
			/* is system-wide event */
			if (!(event->attach_state & PERF_ATTACH_TASK))
				atomic_dec(&per_cpu(perf_branch_stack_events,
						    event->cpu));
		}
	}

	if (event->rb) {
		ring_buffer_put(event->rb);
		event->rb = NULL;
	}

	if (is_cgroup_event(event))
		perf_detach_cgroup(event);

	if (event->destroy)
		event->destroy(event);

	if (event->ctx)
		put_ctx(event->ctx);

	call_rcu(&event->rcu_head, free_event_rcu);
}
