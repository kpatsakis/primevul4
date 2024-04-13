gss_to_krb5_name_1(struct svc_req *rqstp, krb5_context ctx, gss_name_t gss_name,
		   krb5_principal *princ, gss_buffer_t gss_str)
{
     OM_uint32 status, minor_stat;
     gss_OID gss_type;
     char *str;
     int success;

     status = gss_display_name(&minor_stat, gss_name, gss_str, &gss_type);
     if ((status != GSS_S_COMPLETE) || (gss_type != gss_nt_krb5_name)) {
	  krb5_klog_syslog(LOG_ERR, _("gss_to_krb5_name: failed display_name "
				      "status %d"), status);
	  log_badauth(status, minor_stat, rqstp->rq_xprt, NULL);
	  return 0;
     }
     str = malloc(gss_str->length +1);
     if (str == NULL)
	  return 0;
     *str = '\0';

     strncat(str, gss_str->value, gss_str->length);
     success = (krb5_parse_name(ctx, str, princ) == 0);
     free(str);
     return success;
}
