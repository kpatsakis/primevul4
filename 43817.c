static void reset_rsvds_bits_mask_ept(struct kvm_vcpu *vcpu,
		struct kvm_mmu *context, bool execonly)
{
	int maxphyaddr = cpuid_maxphyaddr(vcpu);
	int pte;

	context->rsvd_bits_mask[0][3] =
		rsvd_bits(maxphyaddr, 51) | rsvd_bits(3, 7);
	context->rsvd_bits_mask[0][2] =
		rsvd_bits(maxphyaddr, 51) | rsvd_bits(3, 6);
	context->rsvd_bits_mask[0][1] =
		rsvd_bits(maxphyaddr, 51) | rsvd_bits(3, 6);
	context->rsvd_bits_mask[0][0] = rsvd_bits(maxphyaddr, 51);

	/* large page */
	context->rsvd_bits_mask[1][3] = context->rsvd_bits_mask[0][3];
	context->rsvd_bits_mask[1][2] =
		rsvd_bits(maxphyaddr, 51) | rsvd_bits(12, 29);
	context->rsvd_bits_mask[1][1] =
		rsvd_bits(maxphyaddr, 51) | rsvd_bits(12, 20);
	context->rsvd_bits_mask[1][0] = context->rsvd_bits_mask[0][0];

	for (pte = 0; pte < 64; pte++) {
		int rwx_bits = pte & 7;
		int mt = pte >> 3;
		if (mt == 0x2 || mt == 0x3 || mt == 0x7 ||
				rwx_bits == 0x2 || rwx_bits == 0x6 ||
				(rwx_bits == 0x4 && !execonly))
			context->bad_mt_xwr |= (1ull << pte);
	}
}
