static int validate_packed_leaf_certificate(gnutls_x509_crt_t cert, unsigned char * aaguid) {
  int ret = G_OK, i, c_valid = 0, o_valid = 0, ou_valid = 0, cn_valid = 0;
  unsigned int critial = 1, ca = 1;
  char cert_dn[128] = {0}, ** dn_exploded = NULL;
  unsigned char aaguid_oid[32];
  size_t cert_dn_len = 128, aaguid_oid_len = 32;

  do {
    if (gnutls_x509_crt_get_version(cert) != 3) {
      ret = G_ERROR_PARAM;
      y_log_message(Y_LOG_LEVEL_DEBUG, "validate_packed_leaf_certificate - Invalid certificate version");
      break;
    }

    if ((ret = gnutls_x509_crt_get_dn(cert, cert_dn, &cert_dn_len)) < 0) {
      ret = G_ERROR_PARAM;
      y_log_message(Y_LOG_LEVEL_DEBUG, "validate_packed_leaf_certificate - Error gnutls_x509_crt_get_dn");
      break;
    }

    if ((dn_exploded = ldap_explode_dn(cert_dn, 0)) == NULL) {
      ret = G_ERROR;
      y_log_message(Y_LOG_LEVEL_DEBUG, "validate_packed_leaf_certificate - Error ldap_explode_dn");
      break;
    }

    for (i=0; dn_exploded[i] != NULL; i++) {
      if (0 == o_strncasecmp(G_PACKED_CERT_C_KEY, dn_exploded[i], o_strlen(G_PACKED_CERT_C_KEY)) && string_array_has_value(iso_3166_list, dn_exploded[i]+o_strlen(G_PACKED_CERT_C_KEY))) {
        c_valid = 1;
      } else if (0 == o_strncasecmp(G_PACKED_CERT_O_KEY, dn_exploded[i], o_strlen(G_PACKED_CERT_O_KEY)) && o_strlen(dn_exploded[i]) > 2) {
        o_valid = 1;
      } else if (0 == o_strncasecmp(G_PACKED_CERT_CN_KEY, dn_exploded[i], o_strlen(G_PACKED_CERT_CN_KEY)) && o_strlen(dn_exploded[i]) > 3) {
        cn_valid = 1;
      } else if (0 == o_strncasecmp(G_PACKED_CERT_OU_KEY, dn_exploded[i], o_strlen(G_PACKED_CERT_OU_KEY)) && 0 == o_strcmp(G_PACKED_CERT_OU_VALUE, dn_exploded[i]+o_strlen(G_PACKED_CERT_OU_KEY))) {
        ou_valid = 1;
      }
    }
    ber_memvfree((void **)dn_exploded);

    if (!c_valid || !o_valid || !cn_valid || !ou_valid) {
      ret = G_ERROR_PARAM;
      y_log_message(Y_LOG_LEVEL_DEBUG, "validate_packed_leaf_certificate - Invalid dn - C:%s - O:%s - OU:%s - CN:%s", c_valid?"valid":"invalid", o_valid?"valid":"invalid", ou_valid?"valid":"invalid", cn_valid?"valid":"invalid");
      break;
    }

    if (gnutls_x509_crt_get_basic_constraints(cert, &critial, &ca, NULL) < 0) {
      ret = G_ERROR;
      y_log_message(Y_LOG_LEVEL_DEBUG, "validate_packed_leaf_certificate - Error gnutls_x509_crt_get_basic_constraints");
      break;
    }

    if (ca) {
      ret = G_ERROR_PARAM;
      y_log_message(Y_LOG_LEVEL_DEBUG, "validate_packed_leaf_certificate - Error basic constraints for CA is set to true");
      break;
    }

    if (gnutls_x509_crt_get_extension_by_oid(cert, G_PACKED_OID_AAGUID, 0, aaguid_oid, &aaguid_oid_len, NULL) >= 0) {
      if (aaguid_oid_len != AAGUID_LEN+2) {
        ret = G_ERROR_PARAM;
        y_log_message(Y_LOG_LEVEL_DEBUG, "validate_packed_leaf_certificate - Invalid aaguid_oid_len size %zu", aaguid_oid_len);
        break;
      }

      if (memcmp(aaguid_oid+2, aaguid, AAGUID_LEN)) {
        ret = G_ERROR_PARAM;
        y_log_message(Y_LOG_LEVEL_DEBUG, "validate_packed_leaf_certificate - Invalid aaguid_oid match");
        break;
      }
    }

  } while (0);

  return ret;
}