static int do_monotonic_boot(s64 *t, cycle_t *cycle_now)
{
	struct pvclock_gtod_data *gtod = &pvclock_gtod_data;
	unsigned long seq;
	int mode;
	u64 ns;

	do {
		seq = read_seqcount_begin(&gtod->seq);
		mode = gtod->clock.vclock_mode;
		ns = gtod->nsec_base;
		ns += vgettsc(cycle_now);
		ns >>= gtod->clock.shift;
		ns += gtod->boot_ns;
	} while (unlikely(read_seqcount_retry(&gtod->seq, seq)));
	*t = ns;

	return mode;
}
