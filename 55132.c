int kvmppc_handle_vsx_load(struct kvm_run *run, struct kvm_vcpu *vcpu,
			unsigned int rt, unsigned int bytes,
			int is_default_endian, int mmio_sign_extend)
{
	enum emulation_result emulated = EMULATE_DONE;

	/* Currently, mmio_vsx_copy_nums only allowed to be less than 4 */
	if ( (vcpu->arch.mmio_vsx_copy_nums > 4) ||
		(vcpu->arch.mmio_vsx_copy_nums < 0) ) {
		return EMULATE_FAIL;
	}

	while (vcpu->arch.mmio_vsx_copy_nums) {
		emulated = __kvmppc_handle_load(run, vcpu, rt, bytes,
			is_default_endian, mmio_sign_extend);

		if (emulated != EMULATE_DONE)
			break;

		vcpu->arch.paddr_accessed += run->mmio.len;

		vcpu->arch.mmio_vsx_copy_nums--;
		vcpu->arch.mmio_vsx_offset++;
	}
	return emulated;
}
