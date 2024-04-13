static int kvm_set_tsc_khz(struct kvm_vcpu *vcpu, u32 this_tsc_khz)
{
	u32 thresh_lo, thresh_hi;
	int use_scaling = 0;

	/* tsc_khz can be zero if TSC calibration fails */
	if (this_tsc_khz == 0) {
		/* set tsc_scaling_ratio to a safe value */
		vcpu->arch.tsc_scaling_ratio = kvm_default_tsc_scaling_ratio;
		return -1;
	}

	/* Compute a scale to convert nanoseconds in TSC cycles */
	kvm_get_time_scale(this_tsc_khz, NSEC_PER_SEC / 1000,
			   &vcpu->arch.virtual_tsc_shift,
			   &vcpu->arch.virtual_tsc_mult);
	vcpu->arch.virtual_tsc_khz = this_tsc_khz;

	/*
	 * Compute the variation in TSC rate which is acceptable
	 * within the range of tolerance and decide if the
	 * rate being applied is within that bounds of the hardware
	 * rate.  If so, no scaling or compensation need be done.
	 */
	thresh_lo = adjust_tsc_khz(tsc_khz, -tsc_tolerance_ppm);
	thresh_hi = adjust_tsc_khz(tsc_khz, tsc_tolerance_ppm);
	if (this_tsc_khz < thresh_lo || this_tsc_khz > thresh_hi) {
		pr_debug("kvm: requested TSC rate %u falls outside tolerance [%u,%u]\n", this_tsc_khz, thresh_lo, thresh_hi);
		use_scaling = 1;
	}
	return set_tsc_khz(vcpu, this_tsc_khz, use_scaling);
}
