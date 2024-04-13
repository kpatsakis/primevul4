void kvm_arch_vcpu_uninit(struct kvm_vcpu *vcpu)
{
	int idx;

	kvm_pmu_destroy(vcpu);
	kfree(vcpu->arch.mce_banks);
	kvm_free_lapic(vcpu);
	idx = srcu_read_lock(&vcpu->kvm->srcu);
	kvm_mmu_destroy(vcpu);
	srcu_read_unlock(&vcpu->kvm->srcu, idx);
	free_page((unsigned long)vcpu->arch.pio_data);
	if (!lapic_in_kernel(vcpu))
		static_key_slow_dec(&kvm_no_apic_vcpu);
}
