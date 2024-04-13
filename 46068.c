int kvm_fast_pio_out(struct kvm_vcpu *vcpu, int size, unsigned short port)
{
	unsigned long val = kvm_register_read(vcpu, VCPU_REGS_RAX);
	int ret = emulator_pio_out_emulated(size, port, &val, 1, vcpu);
	/* do not return to emulator after return from userspace */
	vcpu->arch.pio.count = 0;
	return ret;
}
