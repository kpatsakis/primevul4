static int tg3_load_tso_firmware(struct tg3 *tp)
{
	struct fw_info info;
	const __be32 *fw_data;
	unsigned long cpu_base, cpu_scratch_base, cpu_scratch_size;
	int err, i;

	if (tg3_flag(tp, HW_TSO_1) ||
	    tg3_flag(tp, HW_TSO_2) ||
	    tg3_flag(tp, HW_TSO_3))
		return 0;

	fw_data = (void *)tp->fw->data;

	/* Firmware blob starts with version numbers, followed by
	   start address and length. We are setting complete length.
	   length = end_address_of_bss - start_address_of_text.
	   Remainder is the blob to be loaded contiguously
	   from start address. */

	info.fw_base = be32_to_cpu(fw_data[1]);
	cpu_scratch_size = tp->fw_len;
	info.fw_len = tp->fw->size - 12;
	info.fw_data = &fw_data[3];

	if (tg3_asic_rev(tp) == ASIC_REV_5705) {
		cpu_base = RX_CPU_BASE;
		cpu_scratch_base = NIC_SRAM_MBUF_POOL_BASE5705;
	} else {
		cpu_base = TX_CPU_BASE;
		cpu_scratch_base = TX_CPU_SCRATCH_BASE;
		cpu_scratch_size = TX_CPU_SCRATCH_SIZE;
	}

	err = tg3_load_firmware_cpu(tp, cpu_base,
				    cpu_scratch_base, cpu_scratch_size,
				    &info);
	if (err)
		return err;

	/* Now startup the cpu. */
	tw32(cpu_base + CPU_STATE, 0xffffffff);
	tw32_f(cpu_base + CPU_PC, info.fw_base);

	for (i = 0; i < 5; i++) {
		if (tr32(cpu_base + CPU_PC) == info.fw_base)
			break;
		tw32(cpu_base + CPU_STATE, 0xffffffff);
		tw32(cpu_base + CPU_MODE,  CPU_MODE_HALT);
		tw32_f(cpu_base + CPU_PC, info.fw_base);
		udelay(1000);
	}
	if (i >= 5) {
		netdev_err(tp->dev,
			   "%s fails to set CPU PC, is %08x should be %08x\n",
			   __func__, tr32(cpu_base + CPU_PC), info.fw_base);
		return -ENODEV;
	}
	tw32(cpu_base + CPU_STATE, 0xffffffff);
	tw32_f(cpu_base + CPU_MODE,  0x00000000);
	return 0;
}
