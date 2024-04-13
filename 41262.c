int kvm_io_bus_write(struct kvm *kvm, enum kvm_bus bus_idx, gpa_t addr,
		     int len, const void *val)
{
	int i;
	struct kvm_io_bus *bus;

	bus = srcu_dereference(kvm->buses[bus_idx], &kvm->srcu);
	for (i = 0; i < bus->dev_count; i++)
		if (!kvm_iodevice_write(bus->devs[i], addr, len, val))
			return 0;
	return -EOPNOTSUPP;
}
