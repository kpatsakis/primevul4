set_conf_from_info(struct fst_card_info *card, struct fst_port_info *port,
		   struct fstioc_info *info)
{
	int err;
	unsigned char my_framing;

	/* Set things according to the user set valid flags 
	 * Several of the old options have been invalidated/replaced by the 
	 * generic hdlc package.
	 */
	err = 0;
	if (info->valid & FSTVAL_PROTO) {
		if (info->proto == FST_RAW)
			port->mode = FST_RAW;
		else
			port->mode = FST_GEN_HDLC;
	}

	if (info->valid & FSTVAL_CABLE)
		err = -EINVAL;

	if (info->valid & FSTVAL_SPEED)
		err = -EINVAL;

	if (info->valid & FSTVAL_PHASE)
		FST_WRB(card, portConfig[port->index].invertClock,
			info->invertClock);
	if (info->valid & FSTVAL_MODE)
		FST_WRW(card, cardMode, info->cardMode);
	if (info->valid & FSTVAL_TE1) {
		FST_WRL(card, suConfig.dataRate, info->lineSpeed);
		FST_WRB(card, suConfig.clocking, info->clockSource);
		my_framing = FRAMING_E1;
		if (info->framing == E1)
			my_framing = FRAMING_E1;
		if (info->framing == T1)
			my_framing = FRAMING_T1;
		if (info->framing == J1)
			my_framing = FRAMING_J1;
		FST_WRB(card, suConfig.framing, my_framing);
		FST_WRB(card, suConfig.structure, info->structure);
		FST_WRB(card, suConfig.interface, info->interface);
		FST_WRB(card, suConfig.coding, info->coding);
		FST_WRB(card, suConfig.lineBuildOut, info->lineBuildOut);
		FST_WRB(card, suConfig.equalizer, info->equalizer);
		FST_WRB(card, suConfig.transparentMode, info->transparentMode);
		FST_WRB(card, suConfig.loopMode, info->loopMode);
		FST_WRB(card, suConfig.range, info->range);
		FST_WRB(card, suConfig.txBufferMode, info->txBufferMode);
		FST_WRB(card, suConfig.rxBufferMode, info->rxBufferMode);
		FST_WRB(card, suConfig.startingSlot, info->startingSlot);
		FST_WRB(card, suConfig.losThreshold, info->losThreshold);
		if (info->idleCode)
			FST_WRB(card, suConfig.enableIdleCode, 1);
		else
			FST_WRB(card, suConfig.enableIdleCode, 0);
		FST_WRB(card, suConfig.idleCode, info->idleCode);
#if FST_DEBUG
		if (info->valid & FSTVAL_TE1) {
			printk("Setting TE1 data\n");
			printk("Line Speed = %d\n", info->lineSpeed);
			printk("Start slot = %d\n", info->startingSlot);
			printk("Clock source = %d\n", info->clockSource);
			printk("Framing = %d\n", my_framing);
			printk("Structure = %d\n", info->structure);
			printk("interface = %d\n", info->interface);
			printk("Coding = %d\n", info->coding);
			printk("Line build out = %d\n", info->lineBuildOut);
			printk("Equaliser = %d\n", info->equalizer);
			printk("Transparent mode = %d\n",
			       info->transparentMode);
			printk("Loop mode = %d\n", info->loopMode);
			printk("Range = %d\n", info->range);
			printk("Tx Buffer mode = %d\n", info->txBufferMode);
			printk("Rx Buffer mode = %d\n", info->rxBufferMode);
			printk("LOS Threshold = %d\n", info->losThreshold);
			printk("Idle Code = %d\n", info->idleCode);
		}
#endif
	}
#if FST_DEBUG
	if (info->valid & FSTVAL_DEBUG) {
		fst_debug_mask = info->debug;
	}
#endif

	return err;
}
