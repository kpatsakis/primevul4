static int print_dev_info(struct device *dev, void *data)
{
	struct snd_seq_device *sdev = to_seq_dev(dev);
	struct snd_info_buffer *buffer = data;

	snd_iprintf(buffer, "snd-%s,%s,%d\n", sdev->id,
		    dev->driver ? "loaded" : "empty",
		    dev->driver ? 1 : 0);
	return 0;
}
