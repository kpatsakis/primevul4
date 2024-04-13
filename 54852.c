void snd_seq_autoload_exit(void)
{
	atomic_inc(&snd_seq_in_init);
}
