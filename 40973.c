perf_event_alloc(struct perf_event_attr *attr, int cpu,
		 struct task_struct *task,
		 struct perf_event *group_leader,
		 struct perf_event *parent_event,
		 perf_overflow_handler_t overflow_handler,
		 void *context)
{
	struct pmu *pmu;
	struct perf_event *event;
	struct hw_perf_event *hwc;
	long err;

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

	init_waitqueue_head(&event->waitq);
	init_irq_work(&event->pending, perf_pending_event);

	mutex_init(&event->mmap_mutex);

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

		if (attr->type == PERF_TYPE_TRACEPOINT)
			event->hw.tp_target = task;
#ifdef CONFIG_HAVE_HW_BREAKPOINT
		/*
		 * hw_breakpoint is a bit difficult here..
		 */
		else if (attr->type == PERF_TYPE_BREAKPOINT)
			event->hw.bp_target = task;
#endif
	}

	if (!overflow_handler && parent_event) {
		overflow_handler = parent_event->overflow_handler;
		context = parent_event->overflow_handler_context;
	}

	event->overflow_handler	= overflow_handler;
	event->overflow_handler_context = context;

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
		goto done;

	pmu = perf_init_event(event);

done:
	err = 0;
	if (!pmu)
		err = -EINVAL;
	else if (IS_ERR(pmu))
		err = PTR_ERR(pmu);

	if (err) {
		if (event->ns)
			put_pid_ns(event->ns);
		kfree(event);
		return ERR_PTR(err);
	}

	if (!event->parent) {
		if (event->attach_state & PERF_ATTACH_TASK)
			static_key_slow_inc(&perf_sched_events.key);
		if (event->attr.mmap || event->attr.mmap_data)
			atomic_inc(&nr_mmap_events);
		if (event->attr.comm)
			atomic_inc(&nr_comm_events);
		if (event->attr.task)
			atomic_inc(&nr_task_events);
		if (event->attr.sample_type & PERF_SAMPLE_CALLCHAIN) {
			err = get_callchain_buffers();
			if (err) {
				free_event(event);
				return ERR_PTR(err);
			}
		}
		if (has_branch_stack(event)) {
			static_key_slow_inc(&perf_sched_events.key);
			if (!(event->attach_state & PERF_ATTACH_TASK))
				atomic_inc(&per_cpu(perf_branch_stack_events,
						    event->cpu));
		}
	}

	return event;
}
