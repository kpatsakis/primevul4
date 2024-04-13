int kvmppc_kvm_pv(struct kvm_vcpu *vcpu)
{
	int nr = kvmppc_get_gpr(vcpu, 11);
	int r;
	unsigned long __maybe_unused param1 = kvmppc_get_gpr(vcpu, 3);
	unsigned long __maybe_unused param2 = kvmppc_get_gpr(vcpu, 4);
	unsigned long __maybe_unused param3 = kvmppc_get_gpr(vcpu, 5);
	unsigned long __maybe_unused param4 = kvmppc_get_gpr(vcpu, 6);
	unsigned long r2 = 0;

	if (!(kvmppc_get_msr(vcpu) & MSR_SF)) {
		/* 32 bit mode */
		param1 &= 0xffffffff;
		param2 &= 0xffffffff;
		param3 &= 0xffffffff;
		param4 &= 0xffffffff;
	}

	switch (nr) {
	case KVM_HCALL_TOKEN(KVM_HC_PPC_MAP_MAGIC_PAGE):
	{
#if defined(CONFIG_PPC_BOOK3S_64) && defined(CONFIG_KVM_BOOK3S_PR_POSSIBLE)
		/* Book3S can be little endian, find it out here */
		int shared_big_endian = true;
		if (vcpu->arch.intr_msr & MSR_LE)
			shared_big_endian = false;
		if (shared_big_endian != vcpu->arch.shared_big_endian)
			kvmppc_swab_shared(vcpu);
		vcpu->arch.shared_big_endian = shared_big_endian;
#endif

		if (!(param2 & MAGIC_PAGE_FLAG_NOT_MAPPED_NX)) {
			/*
			 * Older versions of the Linux magic page code had
			 * a bug where they would map their trampoline code
			 * NX. If that's the case, remove !PR NX capability.
			 */
			vcpu->arch.disable_kernel_nx = true;
			kvm_make_request(KVM_REQ_TLB_FLUSH, vcpu);
		}

		vcpu->arch.magic_page_pa = param1 & ~0xfffULL;
		vcpu->arch.magic_page_ea = param2 & ~0xfffULL;

#ifdef CONFIG_PPC_64K_PAGES
		/*
		 * Make sure our 4k magic page is in the same window of a 64k
		 * page within the guest and within the host's page.
		 */
		if ((vcpu->arch.magic_page_pa & 0xf000) !=
		    ((ulong)vcpu->arch.shared & 0xf000)) {
			void *old_shared = vcpu->arch.shared;
			ulong shared = (ulong)vcpu->arch.shared;
			void *new_shared;

			shared &= PAGE_MASK;
			shared |= vcpu->arch.magic_page_pa & 0xf000;
			new_shared = (void*)shared;
			memcpy(new_shared, old_shared, 0x1000);
			vcpu->arch.shared = new_shared;
		}
#endif

		r2 = KVM_MAGIC_FEAT_SR | KVM_MAGIC_FEAT_MAS0_TO_SPRG7;

		r = EV_SUCCESS;
		break;
	}
	case KVM_HCALL_TOKEN(KVM_HC_FEATURES):
		r = EV_SUCCESS;
#if defined(CONFIG_PPC_BOOK3S) || defined(CONFIG_KVM_E500V2)
		r2 |= (1 << KVM_FEATURE_MAGIC_PAGE);
#endif

		/* Second return value is in r4 */
		break;
	case EV_HCALL_TOKEN(EV_IDLE):
		r = EV_SUCCESS;
		kvm_vcpu_block(vcpu);
		kvm_clear_request(KVM_REQ_UNHALT, vcpu);
		break;
	default:
		r = EV_UNIMPLEMENTED;
		break;
	}

	kvmppc_set_gpr(vcpu, 4, r2);

	return r;
}
