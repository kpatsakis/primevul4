static void nested_vmx_disable_intercept_for_msr(unsigned long *msr_bitmap_l1,
					       unsigned long *msr_bitmap_nested,
					       u32 msr, int type)
{
	int f = sizeof(unsigned long);

	if (!cpu_has_vmx_msr_bitmap()) {
		WARN_ON(1);
		return;
	}

	/*
	 * See Intel PRM Vol. 3, 20.6.9 (MSR-Bitmap Address). Early manuals
	 * have the write-low and read-high bitmap offsets the wrong way round.
	 * We can control MSRs 0x00000000-0x00001fff and 0xc0000000-0xc0001fff.
	 */
	if (msr <= 0x1fff) {
		if (type & MSR_TYPE_R &&
		   !test_bit(msr, msr_bitmap_l1 + 0x000 / f))
			/* read-low */
			__clear_bit(msr, msr_bitmap_nested + 0x000 / f);

		if (type & MSR_TYPE_W &&
		   !test_bit(msr, msr_bitmap_l1 + 0x800 / f))
			/* write-low */
			__clear_bit(msr, msr_bitmap_nested + 0x800 / f);

	} else if ((msr >= 0xc0000000) && (msr <= 0xc0001fff)) {
		msr &= 0x1fff;
		if (type & MSR_TYPE_R &&
		   !test_bit(msr, msr_bitmap_l1 + 0x400 / f))
			/* read-high */
			__clear_bit(msr, msr_bitmap_nested + 0x400 / f);

		if (type & MSR_TYPE_W &&
		   !test_bit(msr, msr_bitmap_l1 + 0xc00 / f))
			/* write-high */
			__clear_bit(msr, msr_bitmap_nested + 0xc00 / f);

	}
}
