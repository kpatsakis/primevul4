gather_conf_info(struct fst_card_info *card, struct fst_port_info *port,
		 struct fstioc_info *info)
{
	int i;

	memset(info, 0, sizeof (struct fstioc_info));

	i = port->index;
	info->kernelVersion = LINUX_VERSION_CODE;
	info->nports = card->nports;
	info->type = card->type;
	info->state = card->state;
	info->proto = FST_GEN_HDLC;
	info->index = i;
#if FST_DEBUG
	info->debug = fst_debug_mask;
#endif

	/* Only mark information as valid if card is running.
	 * Copy the data anyway in case it is useful for diagnostics
	 */
	info->valid = ((card->state == FST_RUNNING) ? FSTVAL_ALL : FSTVAL_CARD)
#if FST_DEBUG
	    | FSTVAL_DEBUG
#endif
	    ;

	info->lineInterface = FST_RDW(card, portConfig[i].lineInterface);
	info->internalClock = FST_RDB(card, portConfig[i].internalClock);
	info->lineSpeed = FST_RDL(card, portConfig[i].lineSpeed);
	info->invertClock = FST_RDB(card, portConfig[i].invertClock);
	info->v24IpSts = FST_RDL(card, v24IpSts[i]);
	info->v24OpSts = FST_RDL(card, v24OpSts[i]);
	info->clockStatus = FST_RDW(card, clockStatus[i]);
	info->cableStatus = FST_RDW(card, cableStatus);
	info->cardMode = FST_RDW(card, cardMode);
	info->smcFirmwareVersion = FST_RDL(card, smcFirmwareVersion);

	/*
	 * The T2U can report cable presence for both A or B
	 * in bits 0 and 1 of cableStatus.  See which port we are and 
	 * do the mapping.
	 */
	if (card->family == FST_FAMILY_TXU) {
		if (port->index == 0) {
			/*
			 * Port A
			 */
			info->cableStatus = info->cableStatus & 1;
		} else {
			/*
			 * Port B
			 */
			info->cableStatus = info->cableStatus >> 1;
			info->cableStatus = info->cableStatus & 1;
		}
	}
	/*
	 * Some additional bits if we are TE1
	 */
	if (card->type == FST_TYPE_TE1) {
		info->lineSpeed = FST_RDL(card, suConfig.dataRate);
		info->clockSource = FST_RDB(card, suConfig.clocking);
		info->framing = FST_RDB(card, suConfig.framing);
		info->structure = FST_RDB(card, suConfig.structure);
		info->interface = FST_RDB(card, suConfig.interface);
		info->coding = FST_RDB(card, suConfig.coding);
		info->lineBuildOut = FST_RDB(card, suConfig.lineBuildOut);
		info->equalizer = FST_RDB(card, suConfig.equalizer);
		info->loopMode = FST_RDB(card, suConfig.loopMode);
		info->range = FST_RDB(card, suConfig.range);
		info->txBufferMode = FST_RDB(card, suConfig.txBufferMode);
		info->rxBufferMode = FST_RDB(card, suConfig.rxBufferMode);
		info->startingSlot = FST_RDB(card, suConfig.startingSlot);
		info->losThreshold = FST_RDB(card, suConfig.losThreshold);
		if (FST_RDB(card, suConfig.enableIdleCode))
			info->idleCode = FST_RDB(card, suConfig.idleCode);
		else
			info->idleCode = 0;
		info->receiveBufferDelay =
		    FST_RDL(card, suStatus.receiveBufferDelay);
		info->framingErrorCount =
		    FST_RDL(card, suStatus.framingErrorCount);
		info->codeViolationCount =
		    FST_RDL(card, suStatus.codeViolationCount);
		info->crcErrorCount = FST_RDL(card, suStatus.crcErrorCount);
		info->lineAttenuation = FST_RDL(card, suStatus.lineAttenuation);
		info->lossOfSignal = FST_RDB(card, suStatus.lossOfSignal);
		info->receiveRemoteAlarm =
		    FST_RDB(card, suStatus.receiveRemoteAlarm);
		info->alarmIndicationSignal =
		    FST_RDB(card, suStatus.alarmIndicationSignal);
	}
}
