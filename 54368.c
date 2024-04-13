static enum kvm_bus ioeventfd_bus_from_flags(__u32 flags)
{
	if (flags & KVM_IOEVENTFD_FLAG_PIO)
		return KVM_PIO_BUS;
	if (flags & KVM_IOEVENTFD_FLAG_VIRTIO_CCW_NOTIFY)
		return KVM_VIRTIO_CCW_NOTIFY_BUS;
	return KVM_MMIO_BUS;
}
