static void auth_gssapi_display_status_1(
     char *m,
     OM_uint32 code,
     int type,
     int rec)
{
     OM_uint32 gssstat, minor_stat;
     gss_buffer_desc msg;
     OM_uint32 msg_ctx;

     msg_ctx = 0;
     while (1) {
	  gssstat = gss_display_status(&minor_stat, code,
				       type, GSS_C_NULL_OID,
				       &msg_ctx, &msg);
	  if (gssstat != GSS_S_COMPLETE) {
 	       if (!rec) {
		    auth_gssapi_display_status_1(m,gssstat,GSS_C_GSS_CODE,1);
		    auth_gssapi_display_status_1(m, minor_stat,
						 GSS_C_MECH_CODE, 1);
	       } else {
		   fputs ("GSS-API authentication error ", stderr);
		   fwrite (msg.value, msg.length, 1, stderr);
		   fputs (": recursive failure!\n", stderr);
	       }
	       return;
	  }

	  fprintf (stderr, "GSS-API authentication error %s: ", m);
	  fwrite (msg.value, msg.length, 1, stderr);
	  putc ('\n', stderr);
	  if (misc_debug_gssapi)
	      gssrpcint_printf("GSS-API authentication error %s: %*s\n",
			       m, (int)msg.length, (char *) msg.value);
	  (void) gss_release_buffer(&minor_stat, &msg);

	  if (!msg_ctx)
	       break;
     }
}
