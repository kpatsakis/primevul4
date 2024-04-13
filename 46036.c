static int emulator_pio_out_emulated(int size, unsigned short port,
			      const void *val, unsigned int count,
			      struct kvm_vcpu *vcpu)
{
	trace_kvm_pio(1, port, size, 1);

	vcpu->arch.pio.port = port;
	vcpu->arch.pio.in = 0;
	vcpu->arch.pio.count = count;
	vcpu->arch.pio.size = size;

	memcpy(vcpu->arch.pio_data, val, size * count);

	if (!kernel_pio(vcpu, vcpu->arch.pio_data)) {
		vcpu->arch.pio.count = 0;
		return 1;
	}

	vcpu->run->exit_reason = KVM_EXIT_IO;
	vcpu->run->io.direction = KVM_EXIT_IO_OUT;
	vcpu->run->io.size = size;
	vcpu->run->io.data_offset = KVM_PIO_PAGE_OFFSET * PAGE_SIZE;
	vcpu->run->io.count = count;
	vcpu->run->io.port = port;

	return 0;
}
