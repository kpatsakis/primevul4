bool kvm_rdpmc(struct kvm_vcpu *vcpu)
{
	u32 ecx = kvm_register_read(vcpu, VCPU_REGS_RCX);
	u64 data;
	int err;

	err = kvm_pmu_rdpmc(vcpu, ecx, &data);
	if (err)
		return err;
	kvm_register_write(vcpu, VCPU_REGS_RAX, (u32)data);
	kvm_register_write(vcpu, VCPU_REGS_RDX, data >> 32);
	return err;
}
