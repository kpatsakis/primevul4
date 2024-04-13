const char *ipx_frame_name(__be16 frame)
{
	char* rc = "None";

	switch (ntohs(frame)) {
	case ETH_P_IPX:		rc = "EtherII";	break;
	case ETH_P_802_2:	rc = "802.2";	break;
	case ETH_P_SNAP:	rc = "SNAP";	break;
	case ETH_P_802_3:	rc = "802.3";	break;
	}

	return rc;
}
