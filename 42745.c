static void iriap_getvaluebyclass_confirm(struct iriap_cb *self,
					  struct sk_buff *skb)
{
	struct ias_value *value;
	int charset;
	__u32 value_len;
	__u32 tmp_cpu32;
	__u16 obj_id;
	__u16 len;
	__u8  type;
	__u8 *fp;
	int n;

	IRDA_ASSERT(self != NULL, return;);
	IRDA_ASSERT(self->magic == IAS_MAGIC, return;);
	IRDA_ASSERT(skb != NULL, return;);

	/* Initialize variables */
	fp = skb->data;
	n = 2;

	/* Get length, MSB first */
	len = get_unaligned_be16(fp + n);
	n += 2;

	IRDA_DEBUG(4, "%s(), len=%d\n", __func__, len);

	/* Get object ID, MSB first */
	obj_id = get_unaligned_be16(fp + n);
	n += 2;

	type = fp[n++];
	IRDA_DEBUG(4, "%s(), Value type = %d\n", __func__, type);

	switch (type) {
	case IAS_INTEGER:
		memcpy(&tmp_cpu32, fp+n, 4); n += 4;
		be32_to_cpus(&tmp_cpu32);
		value = irias_new_integer_value(tmp_cpu32);

		/*  Legal values restricted to 0x01-0x6f, page 15 irttp */
		IRDA_DEBUG(4, "%s(), lsap=%d\n", __func__, value->t.integer);
		break;
	case IAS_STRING:
		charset = fp[n++];

		switch (charset) {
		case CS_ASCII:
			break;
/*		case CS_ISO_8859_1: */
/*		case CS_ISO_8859_2: */
/*		case CS_ISO_8859_3: */
/*		case CS_ISO_8859_4: */
/*		case CS_ISO_8859_5: */
/*		case CS_ISO_8859_6: */
/*		case CS_ISO_8859_7: */
/*		case CS_ISO_8859_8: */
/*		case CS_ISO_8859_9: */
/*		case CS_UNICODE: */
		default:
			IRDA_DEBUG(0, "%s(), charset %s, not supported\n",
				   __func__, ias_charset_types[charset]);

			/* Aborting, close connection! */
			iriap_disconnect_request(self);
			return;
			/* break; */
		}
		value_len = fp[n++];
		IRDA_DEBUG(4, "%s(), strlen=%d\n", __func__, value_len);

		/* Make sure the string is null-terminated */
		if (n + value_len < skb->len)
			fp[n + value_len] = 0x00;
		IRDA_DEBUG(4, "Got string %s\n", fp+n);

		/* Will truncate to IAS_MAX_STRING bytes */
		value = irias_new_string_value(fp+n);
		break;
	case IAS_OCT_SEQ:
		value_len = get_unaligned_be16(fp + n);
		n += 2;

		/* Will truncate to IAS_MAX_OCTET_STRING bytes */
		value = irias_new_octseq_value(fp+n, value_len);
		break;
	default:
		value = irias_new_missing_value();
		break;
	}

	/* Finished, close connection! */
	iriap_disconnect_request(self);

	/* Warning, the client might close us, so remember no to use self
	 * anymore after calling confirm
	 */
	if (self->confirm)
		self->confirm(IAS_SUCCESS, obj_id, value, self->priv);
	else {
		IRDA_DEBUG(0, "%s(), missing handler!\n", __func__);
		irias_delete_value(value);
	}
}
