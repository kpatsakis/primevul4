static void update_pvclock_gtod(struct timekeeper *tk)
{
	struct pvclock_gtod_data *vdata = &pvclock_gtod_data;
	u64 boot_ns;

	boot_ns = ktime_to_ns(ktime_add(tk->tkr_mono.base, tk->offs_boot));

	write_seqcount_begin(&vdata->seq);

	/* copy pvclock gtod data */
	vdata->clock.vclock_mode	= tk->tkr_mono.clock->archdata.vclock_mode;
	vdata->clock.cycle_last		= tk->tkr_mono.cycle_last;
	vdata->clock.mask		= tk->tkr_mono.mask;
	vdata->clock.mult		= tk->tkr_mono.mult;
	vdata->clock.shift		= tk->tkr_mono.shift;

	vdata->boot_ns			= boot_ns;
	vdata->nsec_base		= tk->tkr_mono.xtime_nsec;

	write_seqcount_end(&vdata->seq);
}
