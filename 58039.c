static int cp2112_raw_event(struct hid_device *hdev, struct hid_report *report,
			    u8 *data, int size)
{
	struct cp2112_device *dev = hid_get_drvdata(hdev);
	struct cp2112_xfer_status_report *xfer = (void *)data;

	switch (data[0]) {
	case CP2112_TRANSFER_STATUS_RESPONSE:
		hid_dbg(hdev, "xfer status: %02x %02x %04x %04x\n",
			xfer->status0, xfer->status1,
			be16_to_cpu(xfer->retries), be16_to_cpu(xfer->length));

		switch (xfer->status0) {
		case STATUS0_IDLE:
			dev->xfer_status = -EAGAIN;
			break;
		case STATUS0_BUSY:
			dev->xfer_status = -EBUSY;
			break;
		case STATUS0_COMPLETE:
			dev->xfer_status = be16_to_cpu(xfer->length);
			break;
		case STATUS0_ERROR:
			switch (xfer->status1) {
			case STATUS1_TIMEOUT_NACK:
			case STATUS1_TIMEOUT_BUS:
				dev->xfer_status = -ETIMEDOUT;
				break;
			default:
				dev->xfer_status = -EIO;
				break;
			}
			break;
		default:
			dev->xfer_status = -EINVAL;
			break;
		}

		atomic_set(&dev->xfer_avail, 1);
		break;
	case CP2112_DATA_READ_RESPONSE:
		hid_dbg(hdev, "read response: %02x %02x\n", data[1], data[2]);

		dev->read_length = data[2];
		if (dev->read_length > sizeof(dev->read_data))
			dev->read_length = sizeof(dev->read_data);

		memcpy(dev->read_data, &data[3], dev->read_length);
		atomic_set(&dev->read_avail, 1);
		break;
	default:
		hid_err(hdev, "unknown report\n");

		return 0;
	}

	wake_up_interruptible(&dev->wait);
	return 1;
}
