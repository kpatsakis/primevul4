int kvm_arch_vcpu_ioctl_run(struct kvm_vcpu *vcpu, struct kvm_run *run)
{
	int r;
	sigset_t sigsaved;

	if (vcpu->mmio_needed) {
		vcpu->mmio_needed = 0;
		if (!vcpu->mmio_is_write)
			kvmppc_complete_mmio_load(vcpu, run);
#ifdef CONFIG_VSX
		if (vcpu->arch.mmio_vsx_copy_nums > 0) {
			vcpu->arch.mmio_vsx_copy_nums--;
			vcpu->arch.mmio_vsx_offset++;
		}

		if (vcpu->arch.mmio_vsx_copy_nums > 0) {
			r = kvmppc_emulate_mmio_vsx_loadstore(vcpu, run);
			if (r == RESUME_HOST) {
				vcpu->mmio_needed = 1;
				return r;
			}
		}
#endif
	} else if (vcpu->arch.osi_needed) {
		u64 *gprs = run->osi.gprs;
		int i;

		for (i = 0; i < 32; i++)
			kvmppc_set_gpr(vcpu, i, gprs[i]);
		vcpu->arch.osi_needed = 0;
	} else if (vcpu->arch.hcall_needed) {
		int i;

		kvmppc_set_gpr(vcpu, 3, run->papr_hcall.ret);
		for (i = 0; i < 9; ++i)
			kvmppc_set_gpr(vcpu, 4 + i, run->papr_hcall.args[i]);
		vcpu->arch.hcall_needed = 0;
#ifdef CONFIG_BOOKE
	} else if (vcpu->arch.epr_needed) {
		kvmppc_set_epr(vcpu, run->epr.epr);
		vcpu->arch.epr_needed = 0;
#endif
	}

	if (vcpu->sigset_active)
		sigprocmask(SIG_SETMASK, &vcpu->sigset, &sigsaved);

	if (run->immediate_exit)
		r = -EINTR;
	else
		r = kvmppc_vcpu_run(run, vcpu);

	if (vcpu->sigset_active)
		sigprocmask(SIG_SETMASK, &sigsaved, NULL);

	return r;
}
