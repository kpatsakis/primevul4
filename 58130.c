static int gs_usb_set_phys_id(struct net_device *dev,
			      enum ethtool_phys_id_state state)
{
	int rc = 0;

	switch (state) {
	case ETHTOOL_ID_ACTIVE:
		rc = gs_usb_set_identify(dev, GS_CAN_IDENTIFY_ON);
		break;
	case ETHTOOL_ID_INACTIVE:
		rc = gs_usb_set_identify(dev, GS_CAN_IDENTIFY_OFF);
		break;
	default:
		break;
	}

	return rc;
}
