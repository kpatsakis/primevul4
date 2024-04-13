long keyctl_negate_key(key_serial_t id, unsigned timeout, key_serial_t ringid)
{
	return keyctl_reject_key(id, timeout, ENOKEY, ringid);
}
