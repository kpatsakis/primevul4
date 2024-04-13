static void ttwu_queue_remote(struct task_struct *p, int cpu)
{
	if (llist_add(&p->wake_entry, &cpu_rq(cpu)->wake_list))
		smp_send_reschedule(cpu);
}
