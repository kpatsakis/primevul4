static void kvm_vcpu_ioctl_x86_get_xsave(struct kvm_vcpu *vcpu,
					 struct kvm_xsave *guest_xsave)
{
	if (cpu_has_xsave) {
		memset(guest_xsave, 0, sizeof(struct kvm_xsave));
		fill_xsave((u8 *) guest_xsave->region, vcpu);
	} else {
		memcpy(guest_xsave->region,
			&vcpu->arch.guest_fpu.state.fxsave,
			sizeof(struct fxregs_state));
		*(u64 *)&guest_xsave->region[XSAVE_HDR_OFFSET / sizeof(u32)] =
			XFEATURE_MASK_FPSSE;
	}
}
