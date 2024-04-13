static __init void intel_arch_events_quirk(void)
{
	int bit;

	/* disable event that reported as not presend by cpuid */
	for_each_set_bit(bit, x86_pmu.events_mask, ARRAY_SIZE(intel_arch_events_map)) {
		intel_perfmon_event_map[intel_arch_events_map[bit].id] = 0;
		pr_warn("CPUID marked event: \'%s\' unavailable\n",
			intel_arch_events_map[bit].name);
	}
}
