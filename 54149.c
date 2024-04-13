prepare_task_switch(struct rq *rq, struct task_struct *prev,
		    struct task_struct *next)
{
	trace_sched_switch(prev, next);
	sched_info_switch(rq, prev, next);
	perf_event_task_sched_out(prev, next);
	fire_sched_out_preempt_notifiers(prev, next);
	prepare_lock_switch(rq, next);
	prepare_arch_switch(next);
}
