void kvm_ioapic_destroy(struct kvm *kvm)
{
	struct kvm_ioapic *ioapic = kvm->arch.vioapic;

	cancel_delayed_work_sync(&ioapic->eoi_inject);
	kvm_io_bus_unregister_dev(kvm, KVM_MMIO_BUS, &ioapic->dev);
	kvm->arch.vioapic = NULL;
	kfree(ioapic);
}
