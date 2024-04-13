static void nss_print_error_message(struct Curl_easy *data, PRUint32 err)
{
  failf(data, "%s", PR_ErrorToString(err, PR_LANGUAGE_I_DEFAULT));
}
