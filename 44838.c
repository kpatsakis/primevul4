fst_issue_cmd(struct fst_port_info *port, unsigned short cmd)
{
	struct fst_card_info *card;
	unsigned short mbval;
	unsigned long flags;
	int safety;

	card = port->card;
	spin_lock_irqsave(&card->card_lock, flags);
	mbval = FST_RDW(card, portMailbox[port->index][0]);

	safety = 0;
	/* Wait for any previous command to complete */
	while (mbval > NAK) {
		spin_unlock_irqrestore(&card->card_lock, flags);
		schedule_timeout_uninterruptible(1);
		spin_lock_irqsave(&card->card_lock, flags);

		if (++safety > 2000) {
			pr_err("Mailbox safety timeout\n");
			break;
		}

		mbval = FST_RDW(card, portMailbox[port->index][0]);
	}
	if (safety > 0) {
		dbg(DBG_CMD, "Mailbox clear after %d jiffies\n", safety);
	}
	if (mbval == NAK) {
		dbg(DBG_CMD, "issue_cmd: previous command was NAK'd\n");
	}

	FST_WRW(card, portMailbox[port->index][0], cmd);

	if (cmd == ABORTTX || cmd == STARTPORT) {
		port->txpos = 0;
		port->txipos = 0;
		port->start = 0;
	}

	spin_unlock_irqrestore(&card->card_lock, flags);
}
