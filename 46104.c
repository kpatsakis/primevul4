static int vcpu_mmio_write(struct kvm_vcpu *vcpu, gpa_t addr, int len,
			   const void *v)
{
	if (vcpu->arch.apic &&
	    !kvm_iodevice_write(&vcpu->arch.apic->dev, addr, len, v))
		return 0;

	return kvm_io_bus_write(vcpu->kvm, KVM_MMIO_BUS, addr, len, v);
}
