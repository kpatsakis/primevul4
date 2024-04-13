void kvm_arch_vcpu_free(struct kvm_vcpu *vcpu)
{
	/* Make sure we're not using the vcpu anymore */
	hrtimer_cancel(&vcpu->arch.dec_timer);

	kvmppc_remove_vcpu_debugfs(vcpu);

	switch (vcpu->arch.irq_type) {
	case KVMPPC_IRQ_MPIC:
		kvmppc_mpic_disconnect_vcpu(vcpu->arch.mpic, vcpu);
		break;
	case KVMPPC_IRQ_XICS:
		if (xive_enabled())
			kvmppc_xive_cleanup_vcpu(vcpu);
		else
			kvmppc_xics_free_icp(vcpu);
		break;
	}

	kvmppc_core_vcpu_free(vcpu);
}
