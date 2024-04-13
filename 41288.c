static void tg3_ape_driver_state_change(struct tg3 *tp, int kind)
{
	u32 event;
	u32 apedata;

	if (!tg3_flag(tp, ENABLE_APE))
		return;

	switch (kind) {
	case RESET_KIND_INIT:
		tg3_ape_write32(tp, TG3_APE_HOST_SEG_SIG,
				APE_HOST_SEG_SIG_MAGIC);
		tg3_ape_write32(tp, TG3_APE_HOST_SEG_LEN,
				APE_HOST_SEG_LEN_MAGIC);
		apedata = tg3_ape_read32(tp, TG3_APE_HOST_INIT_COUNT);
		tg3_ape_write32(tp, TG3_APE_HOST_INIT_COUNT, ++apedata);
		tg3_ape_write32(tp, TG3_APE_HOST_DRIVER_ID,
			APE_HOST_DRIVER_ID_MAGIC(TG3_MAJ_NUM, TG3_MIN_NUM));
		tg3_ape_write32(tp, TG3_APE_HOST_BEHAVIOR,
				APE_HOST_BEHAV_NO_PHYLOCK);
		tg3_ape_write32(tp, TG3_APE_HOST_DRVR_STATE,
				    TG3_APE_HOST_DRVR_STATE_START);

		event = APE_EVENT_STATUS_STATE_START;
		break;
	case RESET_KIND_SHUTDOWN:
		/* With the interface we are currently using,
		 * APE does not track driver state.  Wiping
		 * out the HOST SEGMENT SIGNATURE forces
		 * the APE to assume OS absent status.
		 */
		tg3_ape_write32(tp, TG3_APE_HOST_SEG_SIG, 0x0);

		if (device_may_wakeup(&tp->pdev->dev) &&
		    tg3_flag(tp, WOL_ENABLE)) {
			tg3_ape_write32(tp, TG3_APE_HOST_WOL_SPEED,
					    TG3_APE_HOST_WOL_SPEED_AUTO);
			apedata = TG3_APE_HOST_DRVR_STATE_WOL;
		} else
			apedata = TG3_APE_HOST_DRVR_STATE_UNLOAD;

		tg3_ape_write32(tp, TG3_APE_HOST_DRVR_STATE, apedata);

		event = APE_EVENT_STATUS_STATE_UNLOAD;
		break;
	case RESET_KIND_SUSPEND:
		event = APE_EVENT_STATUS_STATE_SUSPEND;
		break;
	default:
		return;
	}

	event |= APE_EVENT_STATUS_DRIVER_EVNT | APE_EVENT_STATUS_STATE_CHNGE;

	tg3_ape_send_event(tp, event);
}
