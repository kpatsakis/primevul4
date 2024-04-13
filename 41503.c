static void tg3_set_bdinfo(struct tg3 *tp, u32 bdinfo_addr,
			   dma_addr_t mapping, u32 maxlen_flags,
			   u32 nic_addr)
{
	tg3_write_mem(tp,
		      (bdinfo_addr + TG3_BDINFO_HOST_ADDR + TG3_64BIT_REG_HIGH),
		      ((u64) mapping >> 32));
	tg3_write_mem(tp,
		      (bdinfo_addr + TG3_BDINFO_HOST_ADDR + TG3_64BIT_REG_LOW),
		      ((u64) mapping & 0xffffffff));
	tg3_write_mem(tp,
		      (bdinfo_addr + TG3_BDINFO_MAXLEN_FLAGS),
		       maxlen_flags);

	if (!tg3_flag(tp, 5705_PLUS))
		tg3_write_mem(tp,
			      (bdinfo_addr + TG3_BDINFO_NIC_ADDR),
			      nic_addr);
}
