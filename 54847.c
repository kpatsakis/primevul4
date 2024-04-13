static void autoload_drivers(struct work_struct *work)
{
	/* avoid reentrance */
	if (atomic_inc_return(&snd_seq_in_init) == 1)
		bus_for_each_dev(&snd_seq_bus_type, NULL, NULL,
				 request_seq_drv);
	atomic_dec(&snd_seq_in_init);
}
