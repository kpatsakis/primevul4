struct perf_guest_switch_msr *perf_guest_get_msrs(int *nr)
{
	if (x86_pmu.guest_get_msrs)
		return x86_pmu.guest_get_msrs(nr);
	*nr = 0;
	return NULL;
}
