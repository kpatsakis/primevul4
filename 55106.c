int kvm_arch_vcpu_init(struct kvm_vcpu *vcpu)
{
	int ret;

	hrtimer_init(&vcpu->arch.dec_timer, CLOCK_REALTIME, HRTIMER_MODE_ABS);
	vcpu->arch.dec_timer.function = kvmppc_decrementer_wakeup;
	vcpu->arch.dec_expires = ~(u64)0;

#ifdef CONFIG_KVM_EXIT_TIMING
	mutex_init(&vcpu->arch.exit_timing_lock);
#endif
	ret = kvmppc_subarch_vcpu_init(vcpu);
	return ret;
}
