static void tg3_write_sig_post_reset(struct tg3 *tp, int kind)
{
	if (tg3_flag(tp, ASF_NEW_HANDSHAKE)) {
		switch (kind) {
		case RESET_KIND_INIT:
			tg3_write_mem(tp, NIC_SRAM_FW_DRV_STATE_MBOX,
				      DRV_STATE_START_DONE);
			break;

		case RESET_KIND_SHUTDOWN:
			tg3_write_mem(tp, NIC_SRAM_FW_DRV_STATE_MBOX,
				      DRV_STATE_UNLOAD_DONE);
			break;

		default:
			break;
		}
	}

	if (kind == RESET_KIND_SHUTDOWN)
		tg3_ape_driver_state_change(tp, kind);
}
