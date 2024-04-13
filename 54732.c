static int hid_resume_common(struct hid_device *hid, bool driver_suspended)
{
	int status = 0;

	hid_restart_io(hid);
	if (driver_suspended && hid->driver && hid->driver->resume)
		status = hid->driver->resume(hid);
	return status;
}
