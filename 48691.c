void kadm_1(rqstp, transp)
   struct svc_req *rqstp;
   register SVCXPRT *transp;
{
     union {
	  cprinc_arg create_principal_2_arg;
	  dprinc_arg delete_principal_2_arg;
	  mprinc_arg modify_principal_2_arg;
	  rprinc_arg rename_principal_2_arg;
	  gprinc_arg get_principal_2_arg;
	  chpass_arg chpass_principal_2_arg;
	  chrand_arg chrand_principal_2_arg;
	  cpol_arg create_policy_2_arg;
	  dpol_arg delete_policy_2_arg;
	  mpol_arg modify_policy_2_arg;
	  gpol_arg get_policy_2_arg;
	  setkey_arg setkey_principal_2_arg;
	  setv4key_arg setv4key_principal_2_arg;
	  cprinc3_arg create_principal3_2_arg;
	  chpass3_arg chpass_principal3_2_arg;
	  chrand3_arg chrand_principal3_2_arg;
	  setkey3_arg setkey_principal3_2_arg;
     } argument;
     char *result;
     bool_t (*xdr_argument)(), (*xdr_result)();
     char *(*local)();

     if (rqstp->rq_cred.oa_flavor != AUTH_GSSAPI &&
	 !check_rpcsec_auth(rqstp)) {
	  krb5_klog_syslog(LOG_ERR, "Authentication attempt failed: %s, "
			   "RPC authentication flavor %d",
			   client_addr(rqstp->rq_xprt),
			   rqstp->rq_cred.oa_flavor);
	  svcerr_weakauth(transp);
	  return;
     }

     switch (rqstp->rq_proc) {
     case NULLPROC:
	  (void) svc_sendreply(transp, xdr_void, (char *)NULL);
	  return;

     case CREATE_PRINCIPAL:
	  xdr_argument = xdr_cprinc_arg;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) create_principal_2_svc;
	  break;

     case DELETE_PRINCIPAL:
	  xdr_argument = xdr_dprinc_arg;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) delete_principal_2_svc;
	  break;

     case MODIFY_PRINCIPAL:
	  xdr_argument = xdr_mprinc_arg;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) modify_principal_2_svc;
	  break;

     case RENAME_PRINCIPAL:
	  xdr_argument = xdr_rprinc_arg;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) rename_principal_2_svc;
	  break;

     case GET_PRINCIPAL:
	  xdr_argument = xdr_gprinc_arg;
	  xdr_result = xdr_gprinc_ret;
	  local = (char *(*)()) get_principal_2_svc;
	  break;

     case GET_PRINCS:
	  xdr_argument = xdr_gprincs_arg;
	  xdr_result = xdr_gprincs_ret;
	  local = (char *(*)()) get_princs_2_svc;
	  break;

     case CHPASS_PRINCIPAL:
	  xdr_argument = xdr_chpass_arg;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) chpass_principal_2_svc;
	  break;

     case SETV4KEY_PRINCIPAL:
	  xdr_argument = xdr_setv4key_arg;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) setv4key_principal_2_svc;
	  break;

     case SETKEY_PRINCIPAL:
	  xdr_argument = xdr_setkey_arg;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) setkey_principal_2_svc;
	  break;

     case CHRAND_PRINCIPAL:
	  xdr_argument = xdr_chrand_arg;
	  xdr_result = xdr_chrand_ret;
	  local = (char *(*)()) chrand_principal_2_svc;
	  break;

     case CREATE_POLICY:
	  xdr_argument = xdr_cpol_arg;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) create_policy_2_svc;
	  break;

     case DELETE_POLICY:
	  xdr_argument = xdr_dpol_arg;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) delete_policy_2_svc;
	  break;

     case MODIFY_POLICY:
	  xdr_argument = xdr_mpol_arg;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) modify_policy_2_svc;
	  break;

     case GET_POLICY:
	  xdr_argument = xdr_gpol_arg;
	  xdr_result = xdr_gpol_ret;
	  local = (char *(*)()) get_policy_2_svc;
	  break;

     case GET_POLS:
	  xdr_argument = xdr_gpols_arg;
	  xdr_result = xdr_gpols_ret;
	  local = (char *(*)()) get_pols_2_svc;
	  break;

     case GET_PRIVS:
	  xdr_argument = xdr_u_int32;
	  xdr_result = xdr_getprivs_ret;
	  local = (char *(*)()) get_privs_2_svc;
	  break;

     case INIT:
	  xdr_argument = xdr_u_int32;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) init_2_svc;
	  break;

     case CREATE_PRINCIPAL3:
	  xdr_argument = xdr_cprinc3_arg;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) create_principal3_2_svc;
	  break;

     case CHPASS_PRINCIPAL3:
	  xdr_argument = xdr_chpass3_arg;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) chpass_principal3_2_svc;
	  break;

     case CHRAND_PRINCIPAL3:
	  xdr_argument = xdr_chrand3_arg;
	  xdr_result = xdr_chrand_ret;
	  local = (char *(*)()) chrand_principal3_2_svc;
	  break;

     case SETKEY_PRINCIPAL3:
	  xdr_argument = xdr_setkey3_arg;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) setkey_principal3_2_svc;
	  break;

     case PURGEKEYS:
	  xdr_argument = xdr_purgekeys_arg;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) purgekeys_2_svc;
	  break;

     case GET_STRINGS:
	  xdr_argument = xdr_gstrings_arg;
	  xdr_result = xdr_gstrings_ret;
	  local = (char *(*)()) get_strings_2_svc;
	  break;

     case SET_STRING:
	  xdr_argument = xdr_sstring_arg;
	  xdr_result = xdr_generic_ret;
	  local = (char *(*)()) set_string_2_svc;
	  break;

     default:
	  krb5_klog_syslog(LOG_ERR, "Invalid KADM5 procedure number: %s, %d",
			   client_addr(rqstp->rq_xprt), rqstp->rq_proc);
	  svcerr_noproc(transp);
	  return;
     }
     memset(&argument, 0, sizeof(argument));
     if (!svc_getargs(transp, xdr_argument, &argument)) {
	  svcerr_decode(transp);
	  return;
     }
     result = (*local)(&argument, rqstp);
     if (result != NULL && !svc_sendreply(transp, xdr_result, result)) {
	  krb5_klog_syslog(LOG_ERR, "WARNING! Unable to send function results, "
		 "continuing.");
	  svcerr_systemerr(transp);
     }
     if (!svc_freeargs(transp, xdr_argument, &argument)) {
	  krb5_klog_syslog(LOG_ERR, "WARNING! Unable to free arguments, "
		 "continuing.");
     }
     return;
}
