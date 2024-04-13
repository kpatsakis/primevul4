int kvm_handle_sys_reg(struct kvm_vcpu *vcpu, struct kvm_run *run)
{
	struct sys_reg_params params;
	unsigned long esr = kvm_vcpu_get_hsr(vcpu);
	int Rt = (esr >> 5) & 0x1f;
	int ret;

	trace_kvm_handle_sys_reg(esr);

	params.is_aarch32 = false;
	params.is_32bit = false;
	params.Op0 = (esr >> 20) & 3;
	params.Op1 = (esr >> 14) & 0x7;
	params.CRn = (esr >> 10) & 0xf;
	params.CRm = (esr >> 1) & 0xf;
	params.Op2 = (esr >> 17) & 0x7;
	params.regval = vcpu_get_reg(vcpu, Rt);
	params.is_write = !(esr & 1);

	ret = emulate_sys_reg(vcpu, &params);

	if (!params.is_write)
		vcpu_set_reg(vcpu, Rt, params.regval);
	return ret;
}
