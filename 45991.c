void comedi_free_subdevice_minor(struct comedi_subdevice *s)
{
	unsigned long flags;
	struct comedi_device_file_info *info;

	if (s == NULL)
		return;
	if (s->minor < 0)
		return;

	BUG_ON(s->minor >= COMEDI_NUM_MINORS);
	BUG_ON(s->minor < COMEDI_FIRST_SUBDEVICE_MINOR);

	spin_lock_irqsave(&comedi_file_info_table_lock, flags);
	info = comedi_file_info_table[s->minor];
	comedi_file_info_table[s->minor] = NULL;
	spin_unlock_irqrestore(&comedi_file_info_table_lock, flags);

	if (s->class_dev) {
		device_destroy(comedi_class, MKDEV(COMEDI_MAJOR, s->minor));
		s->class_dev = NULL;
	}
	kfree(info);
}
