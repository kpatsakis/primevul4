static void perf_event_switch(struct task_struct *task,
			      struct task_struct *next_prev, bool sched_in)
{
	struct perf_switch_event switch_event;

	/* N.B. caller checks nr_switch_events != 0 */

	switch_event = (struct perf_switch_event){
		.task		= task,
		.next_prev	= next_prev,
		.event_id	= {
			.header = {
				/* .type */
				.misc = sched_in ? 0 : PERF_RECORD_MISC_SWITCH_OUT,
				/* .size */
			},
			/* .next_prev_pid */
			/* .next_prev_tid */
		},
	};

	perf_iterate_sb(perf_event_switch_output,
		       &switch_event,
		       NULL);
}
