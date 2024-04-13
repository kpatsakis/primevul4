static long hiddev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct hiddev_list *list = file->private_data;
	struct hiddev *hiddev = list->hiddev;
	struct hid_device *hid;
	struct hiddev_collection_info cinfo;
	struct hiddev_report_info rinfo;
	struct hiddev_field_info finfo;
	struct hiddev_devinfo dinfo;
	struct hid_report *report;
	struct hid_field *field;
	void __user *user_arg = (void __user *)arg;
	int i, r = -EINVAL;

	/* Called without BKL by compat methods so no BKL taken */

	mutex_lock(&hiddev->existancelock);
	if (!hiddev->exist) {
		r = -ENODEV;
		goto ret_unlock;
	}

	hid = hiddev->hid;

	switch (cmd) {

	case HIDIOCGVERSION:
		r = put_user(HID_VERSION, (int __user *)arg) ?
			-EFAULT : 0;
		break;

	case HIDIOCAPPLICATION:
		if (arg >= hid->maxapplication)
			break;

		for (i = 0; i < hid->maxcollection; i++)
			if (hid->collection[i].type ==
			    HID_COLLECTION_APPLICATION && arg-- == 0)
				break;

		if (i < hid->maxcollection)
			r = hid->collection[i].usage;
		break;

	case HIDIOCGDEVINFO:
		{
			struct usb_device *dev = hid_to_usb_dev(hid);
			struct usbhid_device *usbhid = hid->driver_data;

			memset(&dinfo, 0, sizeof(dinfo));

			dinfo.bustype = BUS_USB;
			dinfo.busnum = dev->bus->busnum;
			dinfo.devnum = dev->devnum;
			dinfo.ifnum = usbhid->ifnum;
			dinfo.vendor = le16_to_cpu(dev->descriptor.idVendor);
			dinfo.product = le16_to_cpu(dev->descriptor.idProduct);
			dinfo.version = le16_to_cpu(dev->descriptor.bcdDevice);
			dinfo.num_applications = hid->maxapplication;

			r = copy_to_user(user_arg, &dinfo, sizeof(dinfo)) ?
				-EFAULT : 0;
			break;
		}

	case HIDIOCGFLAG:
		r = put_user(list->flags, (int __user *)arg) ?
			-EFAULT : 0;
		break;

	case HIDIOCSFLAG:
		{
			int newflags;

			if (get_user(newflags, (int __user *)arg)) {
				r = -EFAULT;
				break;
			}

			if ((newflags & ~HIDDEV_FLAGS) != 0 ||
			    ((newflags & HIDDEV_FLAG_REPORT) != 0 &&
			     (newflags & HIDDEV_FLAG_UREF) == 0))
				break;

			list->flags = newflags;

			r = 0;
			break;
		}

	case HIDIOCGSTRING:
		r = hiddev_ioctl_string(hiddev, cmd, user_arg);
		break;

	case HIDIOCINITREPORT:
		usbhid_init_reports(hid);
		r = 0;
		break;

	case HIDIOCGREPORT:
		if (copy_from_user(&rinfo, user_arg, sizeof(rinfo))) {
			r = -EFAULT;
			break;
		}

		if (rinfo.report_type == HID_REPORT_TYPE_OUTPUT)
			break;

		report = hiddev_lookup_report(hid, &rinfo);
		if (report == NULL)
			break;

		hid_hw_request(hid, report, HID_REQ_GET_REPORT);
		hid_hw_wait(hid);

		r = 0;
		break;

	case HIDIOCSREPORT:
		if (copy_from_user(&rinfo, user_arg, sizeof(rinfo))) {
			r = -EFAULT;
			break;
		}

		if (rinfo.report_type == HID_REPORT_TYPE_INPUT)
			break;

		report = hiddev_lookup_report(hid, &rinfo);
		if (report == NULL)
			break;

		hid_hw_request(hid, report, HID_REQ_SET_REPORT);
		hid_hw_wait(hid);

		r = 0;
		break;

	case HIDIOCGREPORTINFO:
		if (copy_from_user(&rinfo, user_arg, sizeof(rinfo))) {
			r = -EFAULT;
			break;
		}

		report = hiddev_lookup_report(hid, &rinfo);
		if (report == NULL)
			break;

		rinfo.num_fields = report->maxfield;

		r = copy_to_user(user_arg, &rinfo, sizeof(rinfo)) ?
			-EFAULT : 0;
		break;

	case HIDIOCGFIELDINFO:
		if (copy_from_user(&finfo, user_arg, sizeof(finfo))) {
			r = -EFAULT;
			break;
		}

		rinfo.report_type = finfo.report_type;
		rinfo.report_id = finfo.report_id;

		report = hiddev_lookup_report(hid, &rinfo);
		if (report == NULL)
			break;

		if (finfo.field_index >= report->maxfield)
			break;

		field = report->field[finfo.field_index];
		memset(&finfo, 0, sizeof(finfo));
		finfo.report_type = rinfo.report_type;
		finfo.report_id = rinfo.report_id;
		finfo.field_index = field->report_count - 1;
		finfo.maxusage = field->maxusage;
		finfo.flags = field->flags;
		finfo.physical = field->physical;
		finfo.logical = field->logical;
		finfo.application = field->application;
		finfo.logical_minimum = field->logical_minimum;
		finfo.logical_maximum = field->logical_maximum;
		finfo.physical_minimum = field->physical_minimum;
		finfo.physical_maximum = field->physical_maximum;
		finfo.unit_exponent = field->unit_exponent;
		finfo.unit = field->unit;

		r = copy_to_user(user_arg, &finfo, sizeof(finfo)) ?
			-EFAULT : 0;
		break;

	case HIDIOCGUCODE:
		/* fall through */
	case HIDIOCGUSAGE:
	case HIDIOCSUSAGE:
	case HIDIOCGUSAGES:
	case HIDIOCSUSAGES:
	case HIDIOCGCOLLECTIONINDEX:
		r = hiddev_ioctl_usage(hiddev, cmd, user_arg);
		break;

	case HIDIOCGCOLLECTIONINFO:
		if (copy_from_user(&cinfo, user_arg, sizeof(cinfo))) {
			r = -EFAULT;
			break;
		}

		if (cinfo.index >= hid->maxcollection)
			break;

		cinfo.type = hid->collection[cinfo.index].type;
		cinfo.usage = hid->collection[cinfo.index].usage;
		cinfo.level = hid->collection[cinfo.index].level;

		r = copy_to_user(user_arg, &cinfo, sizeof(cinfo)) ?
			-EFAULT : 0;
		break;

	default:
		if (_IOC_TYPE(cmd) != 'H' || _IOC_DIR(cmd) != _IOC_READ)
			break;

		if (_IOC_NR(cmd) == _IOC_NR(HIDIOCGNAME(0))) {
			int len = strlen(hid->name) + 1;
			if (len > _IOC_SIZE(cmd))
				 len = _IOC_SIZE(cmd);
			r = copy_to_user(user_arg, hid->name, len) ?
				-EFAULT : len;
			break;
		}

		if (_IOC_NR(cmd) == _IOC_NR(HIDIOCGPHYS(0))) {
			int len = strlen(hid->phys) + 1;
			if (len > _IOC_SIZE(cmd))
				len = _IOC_SIZE(cmd);
			r = copy_to_user(user_arg, hid->phys, len) ?
				-EFAULT : len;
			break;
		}
	}

ret_unlock:
	mutex_unlock(&hiddev->existancelock);
	return r;
}
