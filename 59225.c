perf_event_alloc(struct perf_event_attr *attr, int cpu,
		 struct task_struct *task,
		 struct perf_event *group_leader,
		 struct perf_event *parent_event,
		 perf_overflow_handler_t overflow_handler,
		 void *context, int cgroup_fd)
{
	struct pmu *pmu;
	struct perf_event *event;
	struct hw_perf_event *hwc;
	long err = -EINVAL;

	if ((unsigned)cpu >= nr_cpu_ids) {
		if (!task || cpu != -1)
			return ERR_PTR(-EINVAL);
	}

	event = kzalloc(sizeof(*event), GFP_KERNEL);
	if (!event)
		return ERR_PTR(-ENOMEM);

	/*
	 * Single events are their own group leaders, with an
	 * empty sibling list:
	 */
	if (!group_leader)
		group_leader = event;

	mutex_init(&event->child_mutex);
	INIT_LIST_HEAD(&event->child_list);

	INIT_LIST_HEAD(&event->group_entry);
	INIT_LIST_HEAD(&event->event_entry);
	INIT_LIST_HEAD(&event->sibling_list);
	INIT_LIST_HEAD(&event->rb_entry);
	INIT_LIST_HEAD(&event->active_entry);
	INIT_LIST_HEAD(&event->addr_filters.list);
	INIT_HLIST_NODE(&event->hlist_entry);


	init_waitqueue_head(&event->waitq);
	init_irq_work(&event->pending, perf_pending_event);

	mutex_init(&event->mmap_mutex);
	raw_spin_lock_init(&event->addr_filters.lock);

	atomic_long_set(&event->refcount, 1);
	event->cpu		= cpu;
	event->attr		= *attr;
	event->group_leader	= group_leader;
	event->pmu		= NULL;
	event->oncpu		= -1;

	event->parent		= parent_event;

	event->ns		= get_pid_ns(task_active_pid_ns(current));
	event->id		= atomic64_inc_return(&perf_event_id);

	event->state		= PERF_EVENT_STATE_INACTIVE;

	if (task) {
		event->attach_state = PERF_ATTACH_TASK;
		/*
		 * XXX pmu::event_init needs to know what task to account to
		 * and we cannot use the ctx information because we need the
		 * pmu before we get a ctx.
		 */
		event->hw.target = task;
	}

	event->clock = &local_clock;
	if (parent_event)
		event->clock = parent_event->clock;

	if (!overflow_handler && parent_event) {
		overflow_handler = parent_event->overflow_handler;
		context = parent_event->overflow_handler_context;
#if defined(CONFIG_BPF_SYSCALL) && defined(CONFIG_EVENT_TRACING)
		if (overflow_handler == bpf_overflow_handler) {
			struct bpf_prog *prog = bpf_prog_inc(parent_event->prog);

			if (IS_ERR(prog)) {
				err = PTR_ERR(prog);
				goto err_ns;
			}
			event->prog = prog;
			event->orig_overflow_handler =
				parent_event->orig_overflow_handler;
		}
#endif
	}

	if (overflow_handler) {
		event->overflow_handler	= overflow_handler;
		event->overflow_handler_context = context;
	} else if (is_write_backward(event)){
		event->overflow_handler = perf_event_output_backward;
		event->overflow_handler_context = NULL;
	} else {
		event->overflow_handler = perf_event_output_forward;
		event->overflow_handler_context = NULL;
	}

	perf_event__state_init(event);

	pmu = NULL;

	hwc = &event->hw;
	hwc->sample_period = attr->sample_period;
	if (attr->freq && attr->sample_freq)
		hwc->sample_period = 1;
	hwc->last_period = hwc->sample_period;

	local64_set(&hwc->period_left, hwc->sample_period);

	/*
	 * we currently do not support PERF_FORMAT_GROUP on inherited events
	 */
	if (attr->inherit && (attr->read_format & PERF_FORMAT_GROUP))
		goto err_ns;

	if (!has_branch_stack(event))
		event->attr.branch_sample_type = 0;

	if (cgroup_fd != -1) {
		err = perf_cgroup_connect(cgroup_fd, event, attr, group_leader);
		if (err)
			goto err_ns;
	}

	pmu = perf_init_event(event);
	if (!pmu)
		goto err_ns;
	else if (IS_ERR(pmu)) {
		err = PTR_ERR(pmu);
		goto err_ns;
	}

	err = exclusive_event_init(event);
	if (err)
		goto err_pmu;

	if (has_addr_filter(event)) {
		event->addr_filters_offs = kcalloc(pmu->nr_addr_filters,
						   sizeof(unsigned long),
						   GFP_KERNEL);
		if (!event->addr_filters_offs)
			goto err_per_task;

		/* force hw sync on the address filters */
		event->addr_filters_gen = 1;
	}

	if (!event->parent) {
		if (event->attr.sample_type & PERF_SAMPLE_CALLCHAIN) {
			err = get_callchain_buffers(attr->sample_max_stack);
			if (err)
				goto err_addr_filters;
		}
	}

	/* symmetric to unaccount_event() in _free_event() */
	account_event(event);

	return event;

err_addr_filters:
	kfree(event->addr_filters_offs);

err_per_task:
	exclusive_event_destroy(event);

err_pmu:
	if (event->destroy)
		event->destroy(event);
	module_put(pmu->module);
err_ns:
	if (is_cgroup_event(event))
		perf_detach_cgroup(event);
	if (event->ns)
		put_pid_ns(event->ns);
	kfree(event);

	return ERR_PTR(err);
}
