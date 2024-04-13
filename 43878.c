static bool nested_vmx_exit_handled_io(struct kvm_vcpu *vcpu,
				       struct vmcs12 *vmcs12)
{
	unsigned long exit_qualification;
	gpa_t bitmap, last_bitmap;
	unsigned int port;
	int size;
	u8 b;

	if (nested_cpu_has(vmcs12, CPU_BASED_UNCOND_IO_EXITING))
		return 1;

	if (!nested_cpu_has(vmcs12, CPU_BASED_USE_IO_BITMAPS))
		return 0;

	exit_qualification = vmcs_readl(EXIT_QUALIFICATION);

	port = exit_qualification >> 16;
	size = (exit_qualification & 7) + 1;

	last_bitmap = (gpa_t)-1;
	b = -1;

	while (size > 0) {
		if (port < 0x8000)
			bitmap = vmcs12->io_bitmap_a;
		else if (port < 0x10000)
			bitmap = vmcs12->io_bitmap_b;
		else
			return 1;
		bitmap += (port & 0x7fff) / 8;

		if (last_bitmap != bitmap)
			if (kvm_read_guest(vcpu->kvm, bitmap, &b, 1))
				return 1;
		if (b & (1 << (port & 7)))
			return 1;

		port++;
		size--;
		last_bitmap = bitmap;
	}

	return 0;
}
