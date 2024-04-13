static json_t * get_cert_from_file_path(const char * path) {
  gnutls_x509_crt_t cert = NULL;
  gnutls_datum_t cert_dat = {NULL, 0}, export_dat = {NULL, 0};
  FILE * fl;
  size_t len, issued_for_len = 128;
  char * cert_content, issued_for[128] = {};
  json_t * j_return = NULL;

  fl = fopen(path, "r");
  if (fl != NULL) {
    fseek(fl, 0, SEEK_END);
    len = ftell(fl);
    cert_content = o_malloc(len);
    if (cert_content != NULL) {
      if (fseek(fl, 0, SEEK_SET) == -1) {
        y_log_message(Y_LOG_LEVEL_ERROR, "get_cert_from_file_path - Error fseek");
        j_return = json_pack("{si}", "result", G_ERROR);
      } else if (fread(cert_content, 1, len, fl) != len) {
        y_log_message(Y_LOG_LEVEL_ERROR, "get_cert_from_file_path - Error fread");
        j_return = json_pack("{si}", "result", G_ERROR);
      } else {
        cert_dat.data = (unsigned char *)cert_content;
        cert_dat.size = len;
        if (!gnutls_x509_crt_init(&cert)) {
          if (gnutls_x509_crt_import(cert, &cert_dat, GNUTLS_X509_FMT_DER) >= 0 || gnutls_x509_crt_import(cert, &cert_dat, GNUTLS_X509_FMT_PEM) >= 0) {
            if (!gnutls_x509_crt_get_dn(cert, issued_for, &issued_for_len)) {
              if (gnutls_x509_crt_export2(cert, GNUTLS_X509_FMT_PEM, &export_dat) >= 0) {
                j_return = json_pack("{sis{ss%ss%}}", "result", G_OK, "certificate", "dn", issued_for, issued_for_len, "x509", export_dat.data, export_dat.size);
                gnutls_free(export_dat.data);
              } else {
                y_log_message(Y_LOG_LEVEL_ERROR, "get_cert_from_file_path - Error gnutls_x509_crt_export2");
                j_return = json_pack("{si}", "result", G_ERROR);
              }
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "get_cert_from_file_path - Error gnutls_x509_crt_get_dn");
              j_return = json_pack("{si}", "result", G_ERROR);
            }
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "get_cert_from_file_path - Error gnutls_x509_crt_import");
            j_return = json_pack("{si}", "result", G_ERROR);
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "get_cert_from_file_path - Error gnutls_x509_crt_init");
          j_return = json_pack("{si}", "result", G_ERROR);
        }
        gnutls_x509_crt_deinit(cert);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_cert_from_file_path - Error o_malloc cert_content");
      j_return = json_pack("{si}", "result", G_ERROR_MEMORY);
    }
    o_free(cert_content);
    fclose(fl);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_cert_from_file_path - Error fopen %s", path);
    j_return = json_pack("{si}", "result", G_ERROR);
  }
  return j_return;
}