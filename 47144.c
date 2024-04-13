int oz_usb_control_req(void *hpd, u8 req_id, struct usb_ctrlrequest *setup,
			const u8 *data, int data_len)
{
	unsigned wvalue = le16_to_cpu(setup->wValue);
	unsigned windex = le16_to_cpu(setup->wIndex);
	unsigned wlength = le16_to_cpu(setup->wLength);
	int rc = 0;

	if ((setup->bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD) {
		switch (setup->bRequest) {
		case USB_REQ_GET_DESCRIPTOR:
			rc = oz_usb_get_desc_req(hpd, req_id,
				setup->bRequestType, (u8)(wvalue>>8),
				(u8)wvalue, setup->wIndex, 0, wlength);
			break;
		case USB_REQ_SET_CONFIGURATION:
			rc = oz_usb_set_config_req(hpd, req_id, (u8)wvalue);
			break;
		case USB_REQ_SET_INTERFACE: {
				u8 if_num = (u8)windex;
				u8 alt = (u8)wvalue;

				rc = oz_usb_set_interface_req(hpd, req_id,
					if_num, alt);
			}
			break;
		case USB_REQ_SET_FEATURE:
			rc = oz_usb_set_clear_feature_req(hpd, req_id,
				OZ_SET_FEATURE_REQ,
				setup->bRequestType & 0xf, (u8)windex,
				setup->wValue);
			break;
		case USB_REQ_CLEAR_FEATURE:
			rc = oz_usb_set_clear_feature_req(hpd, req_id,
				OZ_CLEAR_FEATURE_REQ,
				setup->bRequestType & 0xf,
				(u8)windex, setup->wValue);
			break;
		}
	} else {
		rc = oz_usb_vendor_class_req(hpd, req_id, setup->bRequestType,
			setup->bRequest, setup->wValue, setup->wIndex,
			data, data_len);
	}
	return rc;
}
