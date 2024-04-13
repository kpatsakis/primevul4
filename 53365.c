static struct device *tty_get_device(struct tty_struct *tty)
{
	dev_t devt = tty_devnum(tty);
	return class_find_device(tty_class, NULL, &devt, dev_match_devt);
}
