_kdc_log_timestamp(krb5_context context,
		   krb5_kdc_configuration *config,
		   const char *type,
		   KerberosTime authtime, KerberosTime *starttime,
		   KerberosTime endtime, KerberosTime *renew_till)
{
    char authtime_str[100], starttime_str[100],
	endtime_str[100], renewtime_str[100];

    krb5_format_time(context, authtime,
		     authtime_str, sizeof(authtime_str), TRUE);
    if (starttime)
	krb5_format_time(context, *starttime,
			 starttime_str, sizeof(starttime_str), TRUE);
    else
	strlcpy(starttime_str, "unset", sizeof(starttime_str));
    krb5_format_time(context, endtime,
		     endtime_str, sizeof(endtime_str), TRUE);
    if (renew_till)
	krb5_format_time(context, *renew_till,
			 renewtime_str, sizeof(renewtime_str), TRUE);
    else
	strlcpy(renewtime_str, "unset", sizeof(renewtime_str));

    kdc_log(context, config, 5,
	    "%s authtime: %s starttime: %s endtime: %s renew till: %s",
	    type, authtime_str, starttime_str, endtime_str, renewtime_str);
}
