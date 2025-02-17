static void do_perf_sw_event(enum perf_type_id type, u32 event_id,
				    u64 nr,
				    struct perf_sample_data *data,
				    struct pt_regs *regs)
{
	struct swevent_htable *swhash = &__get_cpu_var(swevent_htable);
	struct perf_event *event;
	struct hlist_head *head;

	rcu_read_lock();
	head = find_swevent_head_rcu(swhash, type, event_id);
	if (!head)
		goto end;

	hlist_for_each_entry_rcu(event, head, hlist_entry) {
		if (perf_swevent_match(event, type, event_id, data, regs))
			perf_swevent_event(event, nr, data, regs);
	}
end:
	rcu_read_unlock();
}
