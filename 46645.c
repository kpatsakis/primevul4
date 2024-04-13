static inline bool md_ioctl_valid(unsigned int cmd)
{
	switch (cmd) {
	case ADD_NEW_DISK:
	case BLKROSET:
	case GET_ARRAY_INFO:
	case GET_BITMAP_FILE:
	case GET_DISK_INFO:
	case HOT_ADD_DISK:
	case HOT_REMOVE_DISK:
	case RAID_AUTORUN:
	case RAID_VERSION:
	case RESTART_ARRAY_RW:
	case RUN_ARRAY:
	case SET_ARRAY_INFO:
	case SET_BITMAP_FILE:
	case SET_DISK_FAULTY:
	case STOP_ARRAY:
	case STOP_ARRAY_RO:
	case CLUSTERED_DISK_NACK:
		return true;
	default:
		return false;
	}
}
