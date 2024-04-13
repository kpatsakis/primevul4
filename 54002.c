static int set_tsc_khz(struct kvm_vcpu *vcpu, u32 user_tsc_khz, bool scale)
{
	u64 ratio;

	/* Guest TSC same frequency as host TSC? */
	if (!scale) {
		vcpu->arch.tsc_scaling_ratio = kvm_default_tsc_scaling_ratio;
		return 0;
	}

	/* TSC scaling supported? */
	if (!kvm_has_tsc_control) {
		if (user_tsc_khz > tsc_khz) {
			vcpu->arch.tsc_catchup = 1;
			vcpu->arch.tsc_always_catchup = 1;
			return 0;
		} else {
			WARN(1, "user requested TSC rate below hardware speed\n");
			return -1;
		}
	}

	/* TSC scaling required  - calculate ratio */
	ratio = mul_u64_u32_div(1ULL << kvm_tsc_scaling_ratio_frac_bits,
				user_tsc_khz, tsc_khz);

	if (ratio == 0 || ratio >= kvm_max_tsc_scaling_ratio) {
		WARN_ONCE(1, "Invalid TSC scaling ratio - virtual-tsc-khz=%u\n",
			  user_tsc_khz);
		return -1;
	}

	vcpu->arch.tsc_scaling_ratio = ratio;
	return 0;
}
