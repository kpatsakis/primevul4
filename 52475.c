static void acm_tty_close(struct tty_struct *tty, struct file *filp)
{
	struct acm *acm = tty->driver_data;
	dev_dbg(&acm->control->dev, "%s\n", __func__);
	tty_port_close(&acm->port, tty, filp);
}
