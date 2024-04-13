int kvm_set_shared_msr(unsigned slot, u64 value, u64 mask)
{
	unsigned int cpu = smp_processor_id();
	struct kvm_shared_msrs *smsr = per_cpu_ptr(shared_msrs, cpu);
	int err;

	if (((value ^ smsr->values[slot].curr) & mask) == 0)
		return 0;
	smsr->values[slot].curr = value;
	err = wrmsrl_safe(shared_msrs_global.msrs[slot], value);
	if (err)
		return 1;

	if (!smsr->registered) {
		smsr->urn.on_user_return = kvm_on_user_return;
		user_return_notifier_register(&smsr->urn);
		smsr->registered = true;
	}
	return 0;
}
