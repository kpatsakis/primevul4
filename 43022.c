static void drop_user_return_notifiers(void)
{
	unsigned int cpu = smp_processor_id();
	struct kvm_shared_msrs *smsr = per_cpu_ptr(shared_msrs, cpu);

	if (smsr->registered)
		kvm_on_user_return(&smsr->urn);
}
