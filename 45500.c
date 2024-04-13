static __be16 ipx_map_frame_type(unsigned char type)
{
	__be16 rc = 0;

	switch (type) {
	case IPX_FRAME_ETHERII:	rc = htons(ETH_P_IPX);		break;
	case IPX_FRAME_8022:	rc = htons(ETH_P_802_2);	break;
	case IPX_FRAME_SNAP:	rc = htons(ETH_P_SNAP);		break;
	case IPX_FRAME_8023:	rc = htons(ETH_P_802_3);	break;
	}

	return rc;
}
