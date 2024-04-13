void tty_hangup(struct tty_struct *tty)
{
#ifdef TTY_DEBUG_HANGUP
	char	buf[64];
	printk(KERN_DEBUG "%s hangup...\n", tty_name(tty, buf));
#endif
	schedule_work(&tty->hangup_work);
}
