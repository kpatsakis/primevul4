static void snd_seq_device_info(struct snd_info_entry *entry,
				struct snd_info_buffer *buffer)
{
	bus_for_each_dev(&snd_seq_bus_type, NULL, buffer, print_dev_info);
}
