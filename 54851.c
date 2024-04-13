static int __init seq_dev_proc_init(void)
{
#ifdef CONFIG_SND_PROC_FS
	info_entry = snd_info_create_module_entry(THIS_MODULE, "drivers",
						  snd_seq_root);
	if (info_entry == NULL)
		return -ENOMEM;
	info_entry->content = SNDRV_INFO_CONTENT_TEXT;
	info_entry->c.text.read = snd_seq_device_info;
	if (snd_info_register(info_entry) < 0) {
		snd_info_free_entry(info_entry);
		return -ENOMEM;
	}
#endif
	return 0;
}
