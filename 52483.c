static void acm_tty_throttle(struct tty_struct *tty)
{
	struct acm *acm = tty->driver_data;

	spin_lock_irq(&acm->read_lock);
	acm->throttle_req = 1;
	spin_unlock_irq(&acm->read_lock);
}
