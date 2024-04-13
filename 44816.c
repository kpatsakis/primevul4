check_started_ok(struct fst_card_info *card)
{
	int i;

	/* Check structure version and end marker */
	if (FST_RDW(card, smcVersion) != SMC_VERSION) {
		pr_err("Bad shared memory version %d expected %d\n",
		       FST_RDW(card, smcVersion), SMC_VERSION);
		card->state = FST_BADVERSION;
		return;
	}
	if (FST_RDL(card, endOfSmcSignature) != END_SIG) {
		pr_err("Missing shared memory signature\n");
		card->state = FST_BADVERSION;
		return;
	}
	/* Firmware status flag, 0x00 = initialising, 0x01 = OK, 0xFF = fail */
	if ((i = FST_RDB(card, taskStatus)) == 0x01) {
		card->state = FST_RUNNING;
	} else if (i == 0xFF) {
		pr_err("Firmware initialisation failed. Card halted\n");
		card->state = FST_HALTED;
		return;
	} else if (i != 0x00) {
		pr_err("Unknown firmware status 0x%x\n", i);
		card->state = FST_HALTED;
		return;
	}

	/* Finally check the number of ports reported by firmware against the
	 * number we assumed at card detection. Should never happen with
	 * existing firmware etc so we just report it for the moment.
	 */
	if (FST_RDL(card, numberOfPorts) != card->nports) {
		pr_warn("Port count mismatch on card %d.  Firmware thinks %d we say %d\n",
			card->card_no,
			FST_RDL(card, numberOfPorts), card->nports);
	}
}
