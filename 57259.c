static int snd_msnd_pinnacle_cfg_reset(int cfg)
{
	int i;

	/* Reset devices if told to */
	printk(KERN_INFO LOGNAME ": Resetting all devices\n");
	for (i = 0; i < 4; ++i)
		if (snd_msnd_write_cfg_logical(cfg, i, 0, 0, 0, 0))
			return -EIO;

	return 0;
}
