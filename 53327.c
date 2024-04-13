void put_tty_driver(struct tty_driver *d)
{
	tty_driver_kref_put(d);
}
