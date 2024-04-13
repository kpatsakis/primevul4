int kvm_arch_vcpu_postcreate(struct kvm_vcpu *vcpu)
{
	int r;
	struct msr_data msr;
	struct kvm *kvm = vcpu->kvm;

	r = vcpu_load(vcpu);
	if (r)
		return r;
	msr.data = 0x0;
	msr.index = MSR_IA32_TSC;
	msr.host_initiated = true;
	kvm_write_tsc(vcpu, &msr);
	vcpu_put(vcpu);

	schedule_delayed_work(&kvm->arch.kvmclock_sync_work,
					KVMCLOCK_SYNC_PERIOD);

	return r;
}
