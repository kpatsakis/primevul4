int comedi_alloc_subdevice_minor(struct comedi_device *dev,
				 struct comedi_subdevice *s)
{
	unsigned long flags;
	struct comedi_device_file_info *info;
	struct device *csdev;
	unsigned i;
	int retval;

	info = kmalloc(sizeof(struct comedi_device_file_info), GFP_KERNEL);
	if (info == NULL)
		return -ENOMEM;
	info->device = dev;
	info->read_subdevice = s;
	info->write_subdevice = s;
	spin_lock_irqsave(&comedi_file_info_table_lock, flags);
	for (i = COMEDI_FIRST_SUBDEVICE_MINOR; i < COMEDI_NUM_MINORS; ++i) {
		if (comedi_file_info_table[i] == NULL) {
			comedi_file_info_table[i] = info;
			break;
		}
	}
	spin_unlock_irqrestore(&comedi_file_info_table_lock, flags);
	if (i == COMEDI_NUM_MINORS) {
		kfree(info);
		printk(KERN_ERR
		       "comedi: error: "
		       "ran out of minor numbers for board device files.\n");
		return -EBUSY;
	}
	s->minor = i;
	csdev = COMEDI_DEVICE_CREATE(comedi_class, dev->class_dev,
				     MKDEV(COMEDI_MAJOR, i), NULL, NULL,
				     "comedi%i_subd%i", dev->minor,
				     (int)(s - dev->subdevices));
	if (!IS_ERR(csdev))
		s->class_dev = csdev;
	dev_set_drvdata(csdev, info);
	retval = device_create_file(csdev, &dev_attr_max_read_buffer_kb);
	if (retval) {
		printk(KERN_ERR
		       "comedi: "
		       "failed to create sysfs attribute file \"%s\".\n",
		       dev_attr_max_read_buffer_kb.attr.name);
		comedi_free_subdevice_minor(s);
		return retval;
	}
	retval = device_create_file(csdev, &dev_attr_read_buffer_kb);
	if (retval) {
		printk(KERN_ERR
		       "comedi: "
		       "failed to create sysfs attribute file \"%s\".\n",
		       dev_attr_read_buffer_kb.attr.name);
		comedi_free_subdevice_minor(s);
		return retval;
	}
	retval = device_create_file(csdev, &dev_attr_max_write_buffer_kb);
	if (retval) {
		printk(KERN_ERR
		       "comedi: "
		       "failed to create sysfs attribute file \"%s\".\n",
		       dev_attr_max_write_buffer_kb.attr.name);
		comedi_free_subdevice_minor(s);
		return retval;
	}
	retval = device_create_file(csdev, &dev_attr_write_buffer_kb);
	if (retval) {
		printk(KERN_ERR
		       "comedi: "
		       "failed to create sysfs attribute file \"%s\".\n",
		       dev_attr_write_buffer_kb.attr.name);
		comedi_free_subdevice_minor(s);
		return retval;
	}
	return i;
}
