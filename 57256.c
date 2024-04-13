static int snd_msnd_isa_remove(struct device *pdev, unsigned int dev)
{
	snd_msnd_unload(dev_get_drvdata(pdev));
	return 0;
}
