static int kvmppc_emulate_mmio_vsx_loadstore(struct kvm_vcpu *vcpu,
			struct kvm_run *run)
{
	enum emulation_result emulated = EMULATE_FAIL;
	int r;

	vcpu->arch.paddr_accessed += run->mmio.len;

	if (!vcpu->mmio_is_write) {
		emulated = kvmppc_handle_vsx_load(run, vcpu, vcpu->arch.io_gpr,
			 run->mmio.len, 1, vcpu->arch.mmio_sign_extend);
	} else {
		emulated = kvmppc_handle_vsx_store(run, vcpu,
			 vcpu->arch.io_gpr, run->mmio.len, 1);
	}

	switch (emulated) {
	case EMULATE_DO_MMIO:
		run->exit_reason = KVM_EXIT_MMIO;
		r = RESUME_HOST;
		break;
	case EMULATE_FAIL:
		pr_info("KVM: MMIO emulation failed (VSX repeat)\n");
		run->exit_reason = KVM_EXIT_INTERNAL_ERROR;
		run->internal.suberror = KVM_INTERNAL_ERROR_EMULATION;
		r = RESUME_HOST;
		break;
	default:
		r = RESUME_GUEST;
		break;
	}
	return r;
}
