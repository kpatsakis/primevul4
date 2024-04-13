_kdc_encode_reply(krb5_context context,
		  krb5_kdc_configuration *config,
		  krb5_crypto armor_crypto, uint32_t nonce,
		  KDC_REP *rep, EncTicketPart *et, EncKDCRepPart *ek,
		  krb5_enctype etype,
		  int skvno, const EncryptionKey *skey,
		  int ckvno, const EncryptionKey *reply_key,
		  int rk_is_subkey,
		  const char **e_text,
		  krb5_data *reply)
{
    unsigned char *buf;
    size_t buf_size;
    size_t len = 0;
    krb5_error_code ret;
    krb5_crypto crypto;

    ASN1_MALLOC_ENCODE(EncTicketPart, buf, buf_size, et, &len, ret);
    if(ret) {
	const char *msg = krb5_get_error_message(context, ret);
	kdc_log(context, config, 0, "Failed to encode ticket: %s", msg);
	krb5_free_error_message(context, msg);
	return ret;
    }
    if(buf_size != len)
	krb5_abortx(context, "Internal error in ASN.1 encoder");

    ret = krb5_crypto_init(context, skey, etype, &crypto);
    if (ret) {
        const char *msg = krb5_get_error_message(context, ret);
	kdc_log(context, config, 0, "krb5_crypto_init failed: %s", msg);
	krb5_free_error_message(context, msg);
	free(buf);
	return ret;
    }

    ret = krb5_encrypt_EncryptedData(context,
				     crypto,
				     KRB5_KU_TICKET,
				     buf,
				     len,
				     skvno,
				     &rep->ticket.enc_part);
    free(buf);
    krb5_crypto_destroy(context, crypto);
    if(ret) {
	const char *msg = krb5_get_error_message(context, ret);
	kdc_log(context, config, 0, "Failed to encrypt data: %s", msg);
	krb5_free_error_message(context, msg);
	return ret;
    }

    if (armor_crypto) {
	krb5_data data;
	krb5_keyblock *strengthen_key = NULL;
	KrbFastFinished finished;

	kdc_log(context, config, 0, "FAST armor protection");

	memset(&finished, 0, sizeof(finished));
	krb5_data_zero(&data);

	finished.timestamp = kdc_time;
	finished.usec = 0;
	finished.crealm = et->crealm;
	finished.cname = et->cname;

	ASN1_MALLOC_ENCODE(Ticket, data.data, data.length,
			   &rep->ticket, &len, ret);
	if (ret)
	    return ret;
	if (data.length != len)
	    krb5_abortx(context, "internal asn.1 error");

	ret = krb5_create_checksum(context, armor_crypto,
				   KRB5_KU_FAST_FINISHED, 0,
				   data.data, data.length,
				   &finished.ticket_checksum);
	krb5_data_free(&data);
	if (ret)
	    return ret;

	ret = _kdc_fast_mk_response(context, armor_crypto,
				    rep->padata, strengthen_key, &finished,
				    nonce, &data);
	free_Checksum(&finished.ticket_checksum);
	if (ret)
	    return ret;

	if (rep->padata) {
	    free_METHOD_DATA(rep->padata);
	} else {
	    rep->padata = calloc(1, sizeof(*(rep->padata)));
	    if (rep->padata == NULL) {
		krb5_data_free(&data);
		return ENOMEM;
	    }
	}

	ret = krb5_padata_add(context, rep->padata,
			      KRB5_PADATA_FX_FAST,
			      data.data, data.length);
	if (ret)
	    return ret;

	/*
	 * Hide client name of privacy reasons
	 */
	if (1 /* r->fast_options.hide_client_names */) {
	    rep->crealm[0] = '\0';
	    free_PrincipalName(&rep->cname);
	    rep->cname.name_type = 0;
	}
    }

    if(rep->msg_type == krb_as_rep && !config->encode_as_rep_as_tgs_rep)
	ASN1_MALLOC_ENCODE(EncASRepPart, buf, buf_size, ek, &len, ret);
    else
	ASN1_MALLOC_ENCODE(EncTGSRepPart, buf, buf_size, ek, &len, ret);
    if(ret) {
	const char *msg = krb5_get_error_message(context, ret);
	kdc_log(context, config, 0, "Failed to encode KDC-REP: %s", msg);
	krb5_free_error_message(context, msg);
	return ret;
    }
    if(buf_size != len) {
	free(buf);
	kdc_log(context, config, 0, "Internal error in ASN.1 encoder");
	*e_text = "KDC internal error";
	return KRB5KRB_ERR_GENERIC;
    }
    ret = krb5_crypto_init(context, reply_key, 0, &crypto);
    if (ret) {
	const char *msg = krb5_get_error_message(context, ret);
	free(buf);
	kdc_log(context, config, 0, "krb5_crypto_init failed: %s", msg);
	krb5_free_error_message(context, msg);
	return ret;
    }
    if(rep->msg_type == krb_as_rep) {
	krb5_encrypt_EncryptedData(context,
				   crypto,
				   KRB5_KU_AS_REP_ENC_PART,
				   buf,
				   len,
				   ckvno,
				   &rep->enc_part);
	free(buf);
	ASN1_MALLOC_ENCODE(AS_REP, buf, buf_size, rep, &len, ret);
    } else {
	krb5_encrypt_EncryptedData(context,
				   crypto,
				   rk_is_subkey ? KRB5_KU_TGS_REP_ENC_PART_SUB_KEY : KRB5_KU_TGS_REP_ENC_PART_SESSION,
				   buf,
				   len,
				   ckvno,
				   &rep->enc_part);
	free(buf);
	ASN1_MALLOC_ENCODE(TGS_REP, buf, buf_size, rep, &len, ret);
    }
    krb5_crypto_destroy(context, crypto);
    if(ret) {
	const char *msg = krb5_get_error_message(context, ret);
	kdc_log(context, config, 0, "Failed to encode KDC-REP: %s", msg);
	krb5_free_error_message(context, msg);
	return ret;
    }
    if(buf_size != len) {
	free(buf);
	kdc_log(context, config, 0, "Internal error in ASN.1 encoder");
	*e_text = "KDC internal error";
	return KRB5KRB_ERR_GENERIC;
    }
    reply->data = buf;
    reply->length = buf_size;
    return 0;
}
