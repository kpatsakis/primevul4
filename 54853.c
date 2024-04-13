void snd_seq_autoload_init(void)
{
	atomic_dec(&snd_seq_in_init);
#ifdef CONFIG_SND_SEQUENCER_MODULE
	/* initial autoload only when snd-seq is a module */
	queue_autoload_drivers();
#endif
}
