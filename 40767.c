static void intel_snb_check_microcode(void)
{
	int pebs_broken = 0;
	int cpu;

	get_online_cpus();
	for_each_online_cpu(cpu) {
		if ((pebs_broken = intel_snb_pebs_broken(cpu)))
			break;
	}
	put_online_cpus();

	if (pebs_broken == x86_pmu.pebs_broken)
		return;

	/*
	 * Serialized by the microcode lock..
	 */
	if (x86_pmu.pebs_broken) {
		pr_info("PEBS enabled due to microcode update\n");
		x86_pmu.pebs_broken = 0;
	} else {
		pr_info("PEBS disabled due to CPU errata, please upgrade microcode\n");
		x86_pmu.pebs_broken = 1;
	}
}
