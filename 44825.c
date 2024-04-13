fst_closeport(struct fst_port_info *port)
{
	if (port->card->state == FST_RUNNING) {
		if (port->run) {
			port->run = 0;
			fst_op_lower(port, OPSTS_RTS | OPSTS_DTR);

			fst_issue_cmd(port, STOPPORT);
		} else {
			dbg(DBG_OPEN, "close: port not running\n");
		}
	}
}
