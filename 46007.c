static int do_insnlist_ioctl(struct comedi_device *dev,
			     struct comedi_insnlist __user *arg, void *file)
{
	struct comedi_insnlist insnlist;
	struct comedi_insn *insns = NULL;
	unsigned int *data = NULL;
	int i = 0;
	int ret = 0;

	if (copy_from_user(&insnlist, arg, sizeof(struct comedi_insnlist)))
		return -EFAULT;

	data = kmalloc(sizeof(unsigned int) * MAX_SAMPLES, GFP_KERNEL);
	if (!data) {
		DPRINTK("kmalloc failed\n");
		ret = -ENOMEM;
		goto error;
	}

	insns =
	    kmalloc(sizeof(struct comedi_insn) * insnlist.n_insns, GFP_KERNEL);
	if (!insns) {
		DPRINTK("kmalloc failed\n");
		ret = -ENOMEM;
		goto error;
	}

	if (copy_from_user(insns, insnlist.insns,
			   sizeof(struct comedi_insn) * insnlist.n_insns)) {
		DPRINTK("copy_from_user failed\n");
		ret = -EFAULT;
		goto error;
	}

	for (i = 0; i < insnlist.n_insns; i++) {
		if (insns[i].n > MAX_SAMPLES) {
			DPRINTK("number of samples too large\n");
			ret = -EINVAL;
			goto error;
		}
		if (insns[i].insn & INSN_MASK_WRITE) {
			if (copy_from_user(data, insns[i].data,
					   insns[i].n * sizeof(unsigned int))) {
				DPRINTK("copy_from_user failed\n");
				ret = -EFAULT;
				goto error;
			}
		}
		ret = parse_insn(dev, insns + i, data, file);
		if (ret < 0)
			goto error;
		if (insns[i].insn & INSN_MASK_READ) {
			if (copy_to_user(insns[i].data, data,
					 insns[i].n * sizeof(unsigned int))) {
				DPRINTK("copy_to_user failed\n");
				ret = -EFAULT;
				goto error;
			}
		}
		if (need_resched())
			schedule();
	}

error:
	kfree(insns);
	kfree(data);

	if (ret < 0)
		return ret;
	return i;
}
