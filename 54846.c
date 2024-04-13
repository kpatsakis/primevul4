static int __init alsa_seq_device_init(void)
{
	int err;

	err = bus_register(&snd_seq_bus_type);
	if (err < 0)
		return err;
	err = seq_dev_proc_init();
	if (err < 0)
		bus_unregister(&snd_seq_bus_type);
	return err;
}
