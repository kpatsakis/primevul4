static void acm_tty_hangup(struct tty_struct *tty)
{
	struct acm *acm = tty->driver_data;
	dev_dbg(&acm->control->dev, "%s\n", __func__);
	tty_port_hangup(&acm->port);
}
