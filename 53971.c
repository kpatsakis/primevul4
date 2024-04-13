static int kvm_vcpu_ioctl_interrupt(struct kvm_vcpu *vcpu,
				    struct kvm_interrupt *irq)
{
	if (irq->irq >= KVM_NR_INTERRUPTS)
		return -EINVAL;

	if (!irqchip_in_kernel(vcpu->kvm)) {
		kvm_queue_interrupt(vcpu, irq->irq, false);
		kvm_make_request(KVM_REQ_EVENT, vcpu);
		return 0;
	}

	/*
	 * With in-kernel LAPIC, we only use this to inject EXTINT, so
	 * fail for in-kernel 8259.
	 */
	if (pic_in_kernel(vcpu->kvm))
		return -ENXIO;

	if (vcpu->arch.pending_external_vector != -1)
		return -EEXIST;

	vcpu->arch.pending_external_vector = irq->irq;
	kvm_make_request(KVM_REQ_EVENT, vcpu);
	return 0;
}
