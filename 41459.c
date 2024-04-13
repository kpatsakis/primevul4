static void tg3_process_error(struct tg3 *tp)
{
	u32 val;
	bool real_error = false;

	if (tg3_flag(tp, ERROR_PROCESSED))
		return;

	/* Check Flow Attention register */
	val = tr32(HOSTCC_FLOW_ATTN);
	if (val & ~HOSTCC_FLOW_ATTN_MBUF_LWM) {
		netdev_err(tp->dev, "FLOW Attention error.  Resetting chip.\n");
		real_error = true;
	}

	if (tr32(MSGINT_STATUS) & ~MSGINT_STATUS_MSI_REQ) {
		netdev_err(tp->dev, "MSI Status error.  Resetting chip.\n");
		real_error = true;
	}

	if (tr32(RDMAC_STATUS) || tr32(WDMAC_STATUS)) {
		netdev_err(tp->dev, "DMA Status error.  Resetting chip.\n");
		real_error = true;
	}

	if (!real_error)
		return;

	tg3_dump_state(tp);

	tg3_flag_set(tp, ERROR_PROCESSED);
	tg3_reset_task_schedule(tp);
}
