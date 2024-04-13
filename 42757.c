static void *irias_seq_start(struct seq_file *seq, loff_t *pos)
{
	spin_lock_irq(&irias_objects->hb_spinlock);

	return *pos ? irias_seq_idx(*pos - 1) : SEQ_START_TOKEN;
}
