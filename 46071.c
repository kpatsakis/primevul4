int kvm_fix_hypercall(struct kvm_vcpu *vcpu)
{
	char instruction[3];
	unsigned long rip = kvm_rip_read(vcpu);

	/*
	 * Blow out the MMU to ensure that no other VCPU has an active mapping
	 * to ensure that the updated hypercall appears atomically across all
	 * VCPUs.
	 */
	kvm_mmu_zap_all(vcpu->kvm);

	kvm_x86_ops->patch_hypercall(vcpu, instruction);

	return emulator_write_emulated(rip, instruction, 3, NULL, vcpu);
}
