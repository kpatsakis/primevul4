static int tg3_load_5701_a0_firmware_fix(struct tg3 *tp)
{
	struct fw_info info;
	const __be32 *fw_data;
	int err, i;

	fw_data = (void *)tp->fw->data;

	/* Firmware blob starts with version numbers, followed by
	   start address and length. We are setting complete length.
	   length = end_address_of_bss - start_address_of_text.
	   Remainder is the blob to be loaded contiguously
	   from start address. */

	info.fw_base = be32_to_cpu(fw_data[1]);
	info.fw_len = tp->fw->size - 12;
	info.fw_data = &fw_data[3];

	err = tg3_load_firmware_cpu(tp, RX_CPU_BASE,
				    RX_CPU_SCRATCH_BASE, RX_CPU_SCRATCH_SIZE,
				    &info);
	if (err)
		return err;

	err = tg3_load_firmware_cpu(tp, TX_CPU_BASE,
				    TX_CPU_SCRATCH_BASE, TX_CPU_SCRATCH_SIZE,
				    &info);
	if (err)
		return err;

	/* Now startup only the RX cpu. */
	tw32(RX_CPU_BASE + CPU_STATE, 0xffffffff);
	tw32_f(RX_CPU_BASE + CPU_PC, info.fw_base);

	for (i = 0; i < 5; i++) {
		if (tr32(RX_CPU_BASE + CPU_PC) == info.fw_base)
			break;
		tw32(RX_CPU_BASE + CPU_STATE, 0xffffffff);
		tw32(RX_CPU_BASE + CPU_MODE,  CPU_MODE_HALT);
		tw32_f(RX_CPU_BASE + CPU_PC, info.fw_base);
		udelay(1000);
	}
	if (i >= 5) {
		netdev_err(tp->dev, "%s fails to set RX CPU PC, is %08x "
			   "should be %08x\n", __func__,
			   tr32(RX_CPU_BASE + CPU_PC), info.fw_base);
		return -ENODEV;
	}
	tw32(RX_CPU_BASE + CPU_STATE, 0xffffffff);
	tw32_f(RX_CPU_BASE + CPU_MODE,  0x00000000);

	return 0;
}
