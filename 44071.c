static u64 __get_raw_cpu_id(u64 ctx, u64 A, u64 X, u64 r4, u64 r5)
{
	return raw_smp_processor_id();
}
