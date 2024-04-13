static bool wake_up_full_nohz_cpu(int cpu)
{
	if (tick_nohz_full_cpu(cpu)) {
		if (cpu != smp_processor_id() ||
		    tick_nohz_tick_stopped())
			smp_send_reschedule(cpu);
		return true;
	}

	return false;
}
