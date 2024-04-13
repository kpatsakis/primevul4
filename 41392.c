static int tg3_load_firmware_cpu(struct tg3 *tp, u32 cpu_base,
				 u32 cpu_scratch_base, int cpu_scratch_size,
				 struct fw_info *info)
{
	int err, lock_err, i;
	void (*write_op)(struct tg3 *, u32, u32);

	if (cpu_base == TX_CPU_BASE && tg3_flag(tp, 5705_PLUS)) {
		netdev_err(tp->dev,
			   "%s: Trying to load TX cpu firmware which is 5705\n",
			   __func__);
		return -EINVAL;
	}

	if (tg3_flag(tp, 5705_PLUS))
		write_op = tg3_write_mem;
	else
		write_op = tg3_write_indirect_reg32;

	/* It is possible that bootcode is still loading at this point.
	 * Get the nvram lock first before halting the cpu.
	 */
	lock_err = tg3_nvram_lock(tp);
	err = tg3_halt_cpu(tp, cpu_base);
	if (!lock_err)
		tg3_nvram_unlock(tp);
	if (err)
		goto out;

	for (i = 0; i < cpu_scratch_size; i += sizeof(u32))
		write_op(tp, cpu_scratch_base + i, 0);
	tw32(cpu_base + CPU_STATE, 0xffffffff);
	tw32(cpu_base + CPU_MODE, tr32(cpu_base+CPU_MODE)|CPU_MODE_HALT);
	for (i = 0; i < (info->fw_len / sizeof(u32)); i++)
		write_op(tp, (cpu_scratch_base +
			      (info->fw_base & 0xffff) +
			      (i * sizeof(u32))),
			      be32_to_cpu(info->fw_data[i]));

	err = 0;

out:
	return err;
}
