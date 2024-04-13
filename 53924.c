void kvm_arch_vcpu_postcreate(struct kvm_vcpu *vcpu)
{
	struct msr_data msr;
	struct kvm *kvm = vcpu->kvm;

	if (vcpu_load(vcpu))
		return;
	msr.data = 0x0;
	msr.index = MSR_IA32_TSC;
	msr.host_initiated = true;
	kvm_write_tsc(vcpu, &msr);
	vcpu_put(vcpu);

	if (!kvmclock_periodic_sync)
		return;

	schedule_delayed_work(&kvm->arch.kvmclock_sync_work,
					KVMCLOCK_SYNC_PERIOD);
}
