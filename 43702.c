static int kvm_arch_setup_async_pf(struct kvm_vcpu *vcpu, gva_t gva, gfn_t gfn)
{
	struct kvm_arch_async_pf arch;

	arch.token = (vcpu->arch.apf.id++ << 12) | vcpu->vcpu_id;
	arch.gfn = gfn;
	arch.direct_map = vcpu->arch.mmu.direct_map;
	arch.cr3 = vcpu->arch.mmu.get_cr3(vcpu);

	return kvm_setup_async_pf(vcpu, gva, gfn, &arch);
}
