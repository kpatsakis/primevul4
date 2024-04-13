static int acm_tty_put_char(struct tty_struct *tty, unsigned char ch)
{
	struct acm *acm = tty->driver_data;
	struct acm_wb *cur;
	int wbn;
	unsigned long flags;

overflow:
	cur = acm->putbuffer;
	if (!cur) {
		spin_lock_irqsave(&acm->write_lock, flags);
		wbn = acm_wb_alloc(acm);
		if (wbn >= 0) {
			cur = &acm->wb[wbn];
			acm->putbuffer = cur;
		}
		spin_unlock_irqrestore(&acm->write_lock, flags);
		if (!cur)
			return 0;
	}

	if (cur->len == acm->writesize) {
		acm_tty_flush_chars(tty);
		goto overflow;
	}

	cur->buf[cur->len++] = ch;
	return 1;
}
