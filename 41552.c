static void tg3_ump_link_report(struct tg3 *tp)
{
	u32 data[4];

	if (!tg3_flag(tp, 5780_CLASS) || !tg3_flag(tp, ENABLE_ASF))
		return;

	tg3_phy_gather_ump_data(tp, data);

	tg3_wait_for_event_ack(tp);

	tg3_write_mem(tp, NIC_SRAM_FW_CMD_MBOX, FWCMD_NICDRV_LINK_UPDATE);
	tg3_write_mem(tp, NIC_SRAM_FW_CMD_LEN_MBOX, 14);
	tg3_write_mem(tp, NIC_SRAM_FW_CMD_DATA_MBOX + 0x0, data[0]);
	tg3_write_mem(tp, NIC_SRAM_FW_CMD_DATA_MBOX + 0x4, data[1]);
	tg3_write_mem(tp, NIC_SRAM_FW_CMD_DATA_MBOX + 0x8, data[2]);
	tg3_write_mem(tp, NIC_SRAM_FW_CMD_DATA_MBOX + 0xc, data[3]);

	tg3_generate_fw_event(tp);
}
