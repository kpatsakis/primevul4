void comedi_free_board_minor(unsigned minor)
{
	unsigned long flags;
	struct comedi_device_file_info *info;

	BUG_ON(minor >= COMEDI_NUM_BOARD_MINORS);
	spin_lock_irqsave(&comedi_file_info_table_lock, flags);
	info = comedi_file_info_table[minor];
	comedi_file_info_table[minor] = NULL;
	spin_unlock_irqrestore(&comedi_file_info_table_lock, flags);

	if (info) {
		struct comedi_device *dev = info->device;
		if (dev) {
			if (dev->class_dev) {
				device_destroy(comedi_class,
					       MKDEV(COMEDI_MAJOR, dev->minor));
			}
			comedi_device_cleanup(dev);
			kfree(dev);
		}
		kfree(info);
	}
}
