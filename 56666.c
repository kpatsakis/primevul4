static int vmx_set_hv_timer(struct kvm_vcpu *vcpu, u64 guest_deadline_tsc)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u64 tscl = rdtsc();
	u64 guest_tscl = kvm_read_l1_tsc(vcpu, tscl);
	u64 delta_tsc = max(guest_deadline_tsc, guest_tscl) - guest_tscl;

	/* Convert to host delta tsc if tsc scaling is enabled */
	if (vcpu->arch.tsc_scaling_ratio != kvm_default_tsc_scaling_ratio &&
			u64_shl_div_u64(delta_tsc,
				kvm_tsc_scaling_ratio_frac_bits,
				vcpu->arch.tsc_scaling_ratio,
				&delta_tsc))
		return -ERANGE;

	/*
	 * If the delta tsc can't fit in the 32 bit after the multi shift,
	 * we can't use the preemption timer.
	 * It's possible that it fits on later vmentries, but checking
	 * on every vmentry is costly so we just use an hrtimer.
	 */
	if (delta_tsc >> (cpu_preemption_timer_multi + 32))
		return -ERANGE;

	vmx->hv_deadline_tsc = tscl + delta_tsc;
	vmcs_set_bits(PIN_BASED_VM_EXEC_CONTROL,
			PIN_BASED_VMX_PREEMPTION_TIMER);

	return delta_tsc == 0;
}
