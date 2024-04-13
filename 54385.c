kvm_assign_ioeventfd(struct kvm *kvm, struct kvm_ioeventfd *args)
{
	enum kvm_bus              bus_idx;
	int ret;

	bus_idx = ioeventfd_bus_from_flags(args->flags);
	/* must be natural-word sized, or 0 to ignore length */
	switch (args->len) {
	case 0:
	case 1:
	case 2:
	case 4:
	case 8:
		break;
	default:
		return -EINVAL;
	}

	/* check for range overflow */
	if (args->addr + args->len < args->addr)
		return -EINVAL;

	/* check for extra flags that we don't understand */
	if (args->flags & ~KVM_IOEVENTFD_VALID_FLAG_MASK)
		return -EINVAL;

	/* ioeventfd with no length can't be combined with DATAMATCH */
	if (!args->len && (args->flags & KVM_IOEVENTFD_FLAG_DATAMATCH))
		return -EINVAL;

	ret = kvm_assign_ioeventfd_idx(kvm, bus_idx, args);
	if (ret)
		goto fail;

	/* When length is ignored, MMIO is also put on a separate bus, for
	 * faster lookups.
	 */
	if (!args->len && bus_idx == KVM_MMIO_BUS) {
		ret = kvm_assign_ioeventfd_idx(kvm, KVM_FAST_MMIO_BUS, args);
		if (ret < 0)
			goto fast_fail;
	}

	return 0;

fast_fail:
	kvm_deassign_ioeventfd_idx(kvm, bus_idx, args);
fail:
	return ret;
}
