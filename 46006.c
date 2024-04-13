static int do_insn_ioctl(struct comedi_device *dev,
			 struct comedi_insn __user *arg, void *file)
{
	struct comedi_insn insn;
	unsigned int *data = NULL;
	int ret = 0;

	data = kmalloc(sizeof(unsigned int) * MAX_SAMPLES, GFP_KERNEL);
	if (!data) {
		ret = -ENOMEM;
		goto error;
	}

	if (copy_from_user(&insn, arg, sizeof(struct comedi_insn))) {
		ret = -EFAULT;
		goto error;
	}

	/* This is where the behavior of insn and insnlist deviate. */
	if (insn.n > MAX_SAMPLES)
		insn.n = MAX_SAMPLES;
	if (insn.insn & INSN_MASK_WRITE) {
		if (copy_from_user(data,
				   insn.data,
				   insn.n * sizeof(unsigned int))) {
			ret = -EFAULT;
			goto error;
		}
	}
	ret = parse_insn(dev, &insn, data, file);
	if (ret < 0)
		goto error;
	if (insn.insn & INSN_MASK_READ) {
		if (copy_to_user(insn.data,
				 data,
				 insn.n * sizeof(unsigned int))) {
			ret = -EFAULT;
			goto error;
		}
	}
	ret = insn.n;

error:
	kfree(data);

	return ret;
}
