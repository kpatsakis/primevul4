void kvm_mmu_flush_tlb(struct kvm_vcpu *vcpu)
{
 	++vcpu->stat.tlb_flush;
 	kvm_make_request(KVM_REQ_TLB_FLUSH, vcpu);
 }
