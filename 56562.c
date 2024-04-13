static void unhandled_cp_access(struct kvm_vcpu *vcpu,
				struct sys_reg_params *params)
{
	u8 hsr_ec = kvm_vcpu_trap_get_class(vcpu);
	int cp = -1;

	switch(hsr_ec) {
	case ESR_ELx_EC_CP15_32:
	case ESR_ELx_EC_CP15_64:
		cp = 15;
		break;
	case ESR_ELx_EC_CP14_MR:
	case ESR_ELx_EC_CP14_64:
		cp = 14;
		break;
	default:
		WARN_ON(1);
	}

	kvm_err("Unsupported guest CP%d access at: %08lx\n",
		cp, *vcpu_pc(vcpu));
	print_sys_reg_instr(params);
	kvm_inject_undefined(vcpu);
}
