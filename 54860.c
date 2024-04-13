void snd_seq_driver_unregister(struct snd_seq_driver *drv)
{
	driver_unregister(&drv->driver);
}
