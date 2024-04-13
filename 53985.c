static int kvm_vm_ioctl_enable_cap(struct kvm *kvm,
				   struct kvm_enable_cap *cap)
{
	int r;

	if (cap->flags)
		return -EINVAL;

	switch (cap->cap) {
	case KVM_CAP_DISABLE_QUIRKS:
		kvm->arch.disabled_quirks = cap->args[0];
		r = 0;
		break;
	case KVM_CAP_SPLIT_IRQCHIP: {
		mutex_lock(&kvm->lock);
		r = -EINVAL;
		if (cap->args[0] > MAX_NR_RESERVED_IOAPIC_PINS)
			goto split_irqchip_unlock;
		r = -EEXIST;
		if (irqchip_in_kernel(kvm))
			goto split_irqchip_unlock;
		if (atomic_read(&kvm->online_vcpus))
			goto split_irqchip_unlock;
		r = kvm_setup_empty_irq_routing(kvm);
		if (r)
			goto split_irqchip_unlock;
		/* Pairs with irqchip_in_kernel. */
		smp_wmb();
		kvm->arch.irqchip_split = true;
		kvm->arch.nr_reserved_ioapic_pins = cap->args[0];
		r = 0;
split_irqchip_unlock:
		mutex_unlock(&kvm->lock);
		break;
	}
	default:
		r = -EINVAL;
		break;
	}
	return r;
}
