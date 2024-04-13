static void snd_seq_dev_release(struct device *dev)
{
	struct snd_seq_device *sdev = to_seq_dev(dev);

	if (sdev->private_free)
		sdev->private_free(sdev);
	kfree(sdev);
}
