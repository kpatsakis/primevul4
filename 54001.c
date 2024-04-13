static void record_steal_time(struct kvm_vcpu *vcpu)
{
	accumulate_steal_time(vcpu);

	if (!(vcpu->arch.st.msr_val & KVM_MSR_ENABLED))
		return;

	if (unlikely(kvm_read_guest_cached(vcpu->kvm, &vcpu->arch.st.stime,
		&vcpu->arch.st.steal, sizeof(struct kvm_steal_time))))
		return;

	vcpu->arch.st.steal.steal += vcpu->arch.st.accum_steal;
	vcpu->arch.st.steal.version += 2;
	vcpu->arch.st.accum_steal = 0;

	kvm_write_guest_cached(vcpu->kvm, &vcpu->arch.st.stime,
		&vcpu->arch.st.steal, sizeof(struct kvm_steal_time));
}
