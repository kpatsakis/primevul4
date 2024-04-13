static void update_pvclock_gtod(struct timekeeper *tk)
{
	struct pvclock_gtod_data *vdata = &pvclock_gtod_data;
	u64 boot_ns;

	boot_ns = ktime_to_ns(ktime_add(tk->tkr.base_mono, tk->offs_boot));

	write_seqcount_begin(&vdata->seq);

	/* copy pvclock gtod data */
	vdata->clock.vclock_mode	= tk->tkr.clock->archdata.vclock_mode;
	vdata->clock.cycle_last		= tk->tkr.cycle_last;
	vdata->clock.mask		= tk->tkr.mask;
	vdata->clock.mult		= tk->tkr.mult;
	vdata->clock.shift		= tk->tkr.shift;

	vdata->boot_ns			= boot_ns;
	vdata->nsec_base		= tk->tkr.xtime_nsec;

	write_seqcount_end(&vdata->seq);
}
