static int compat_sioc_ifmap(struct net *net, unsigned int cmd,
			struct compat_ifreq __user *uifr32)
{
	struct ifreq ifr;
	struct compat_ifmap __user *uifmap32;
	mm_segment_t old_fs;
	int err;

	uifmap32 = &uifr32->ifr_ifru.ifru_map;
	err = copy_from_user(&ifr, uifr32, sizeof(ifr.ifr_name));
	err |= get_user(ifr.ifr_map.mem_start, &uifmap32->mem_start);
	err |= get_user(ifr.ifr_map.mem_end, &uifmap32->mem_end);
	err |= get_user(ifr.ifr_map.base_addr, &uifmap32->base_addr);
	err |= get_user(ifr.ifr_map.irq, &uifmap32->irq);
	err |= get_user(ifr.ifr_map.dma, &uifmap32->dma);
	err |= get_user(ifr.ifr_map.port, &uifmap32->port);
	if (err)
		return -EFAULT;

	old_fs = get_fs();
	set_fs(KERNEL_DS);
	err = dev_ioctl(net, cmd, (void  __user __force *)&ifr);
	set_fs(old_fs);

	if (cmd == SIOCGIFMAP && !err) {
		err = copy_to_user(uifr32, &ifr, sizeof(ifr.ifr_name));
		err |= put_user(ifr.ifr_map.mem_start, &uifmap32->mem_start);
		err |= put_user(ifr.ifr_map.mem_end, &uifmap32->mem_end);
		err |= put_user(ifr.ifr_map.base_addr, &uifmap32->base_addr);
		err |= put_user(ifr.ifr_map.irq, &uifmap32->irq);
		err |= put_user(ifr.ifr_map.dma, &uifmap32->dma);
		err |= put_user(ifr.ifr_map.port, &uifmap32->port);
		if (err)
			err = -EFAULT;
	}
	return err;
}
