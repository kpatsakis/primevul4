int __exit snd_card_info_done(void)
{
	snd_info_free_entry(snd_card_info_entry);
#ifdef MODULE
	snd_info_free_entry(snd_card_module_info_entry);
#endif
	return 0;
}
