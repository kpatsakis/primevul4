static int kvm_handle_cp_64(struct kvm_vcpu *vcpu,
			    const struct sys_reg_desc *global,
			    size_t nr_global,
			    const struct sys_reg_desc *target_specific,
			    size_t nr_specific)
{
	struct sys_reg_params params;
	u32 hsr = kvm_vcpu_get_hsr(vcpu);
	int Rt = (hsr >> 5) & 0xf;
	int Rt2 = (hsr >> 10) & 0xf;

	params.is_aarch32 = true;
	params.is_32bit = false;
	params.CRm = (hsr >> 1) & 0xf;
	params.is_write = ((hsr & 1) == 0);

	params.Op0 = 0;
	params.Op1 = (hsr >> 16) & 0xf;
	params.Op2 = 0;
	params.CRn = 0;

	/*
	 * Make a 64-bit value out of Rt and Rt2. As we use the same trap
	 * backends between AArch32 and AArch64, we get away with it.
	 */
	if (params.is_write) {
		params.regval = vcpu_get_reg(vcpu, Rt) & 0xffffffff;
		params.regval |= vcpu_get_reg(vcpu, Rt2) << 32;
	}

	if (!emulate_cp(vcpu, &params, target_specific, nr_specific))
		goto out;
	if (!emulate_cp(vcpu, &params, global, nr_global))
		goto out;

	unhandled_cp_access(vcpu, &params);

out:
	/* Split up the value between registers for the read side */
	if (!params.is_write) {
		vcpu_set_reg(vcpu, Rt, lower_32_bits(params.regval));
		vcpu_set_reg(vcpu, Rt2, upper_32_bits(params.regval));
	}

	return 1;
}
