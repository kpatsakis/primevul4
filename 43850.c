static u64 ept_rsvd_mask(u64 spte, int level)
{
	int i;
	u64 mask = 0;

	for (i = 51; i > boot_cpu_data.x86_phys_bits; i--)
		mask |= (1ULL << i);

	if (level > 2)
		/* bits 7:3 reserved */
		mask |= 0xf8;
	else if (level == 2) {
		if (spte & (1ULL << 7))
			/* 2MB ref, bits 20:12 reserved */
			mask |= 0x1ff000;
		else
			/* bits 6:3 reserved */
			mask |= 0x78;
	}

	return mask;
}
