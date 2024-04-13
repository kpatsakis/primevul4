void generic_start_io_acct(struct request_queue *q, int rw,
			   unsigned long sectors, struct hd_struct *part)
{
	int cpu = part_stat_lock();

	part_round_stats(q, cpu, part);
	part_stat_inc(cpu, part, ios[rw]);
	part_stat_add(cpu, part, sectors[rw], sectors);
	part_inc_in_flight(q, part, rw);

	part_stat_unlock();
}
