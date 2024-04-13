int kvmppc_handle_vsx_store(struct kvm_run *run, struct kvm_vcpu *vcpu,
			int rs, unsigned int bytes, int is_default_endian)
{
	u64 val;
	enum emulation_result emulated = EMULATE_DONE;

	vcpu->arch.io_gpr = rs;

	/* Currently, mmio_vsx_copy_nums only allowed to be less than 4 */
	if ( (vcpu->arch.mmio_vsx_copy_nums > 4) ||
		(vcpu->arch.mmio_vsx_copy_nums < 0) ) {
		return EMULATE_FAIL;
	}

	while (vcpu->arch.mmio_vsx_copy_nums) {
		if (kvmppc_get_vsr_data(vcpu, rs, &val) == -1)
			return EMULATE_FAIL;

		emulated = kvmppc_handle_store(run, vcpu,
			 val, bytes, is_default_endian);

		if (emulated != EMULATE_DONE)
			break;

		vcpu->arch.paddr_accessed += run->mmio.len;

		vcpu->arch.mmio_vsx_copy_nums--;
		vcpu->arch.mmio_vsx_offset++;
	}

	return emulated;
}
