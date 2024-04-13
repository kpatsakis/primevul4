validate_with_relaxng(xmlDocPtr doc, gboolean to_logs, const char *relaxng_file,
                      relaxng_ctx_cache_t ** cached_ctx)
{
    int rc = 0;
    gboolean valid = TRUE;
    relaxng_ctx_cache_t *ctx = NULL;

    CRM_CHECK(doc != NULL, return FALSE);
    CRM_CHECK(relaxng_file != NULL, return FALSE);

    if (cached_ctx && *cached_ctx) {
        ctx = *cached_ctx;

    } else {
        crm_info("Creating RNG parser context");
        ctx = calloc(1, sizeof(relaxng_ctx_cache_t));

        xmlLoadExtDtdDefaultValue = 1;
        ctx->parser = xmlRelaxNGNewParserCtxt(relaxng_file);
        CRM_CHECK(ctx->parser != NULL, goto cleanup);

        if (to_logs) {
            xmlRelaxNGSetParserErrors(ctx->parser,
                                      (xmlRelaxNGValidityErrorFunc) xml_log,
                                      (xmlRelaxNGValidityWarningFunc) xml_log,
                                      GUINT_TO_POINTER(LOG_ERR));
        } else {
            xmlRelaxNGSetParserErrors(ctx->parser,
                                      (xmlRelaxNGValidityErrorFunc) fprintf,
                                      (xmlRelaxNGValidityWarningFunc) fprintf, stderr);
        }

        ctx->rng = xmlRelaxNGParse(ctx->parser);
        CRM_CHECK(ctx->rng != NULL, crm_err("Could not find/parse %s", relaxng_file);
                  goto cleanup);

        ctx->valid = xmlRelaxNGNewValidCtxt(ctx->rng);
        CRM_CHECK(ctx->valid != NULL, goto cleanup);

        if (to_logs) {
            xmlRelaxNGSetValidErrors(ctx->valid,
                                     (xmlRelaxNGValidityErrorFunc) xml_log,
                                     (xmlRelaxNGValidityWarningFunc) xml_log,
                                     GUINT_TO_POINTER(LOG_ERR));
        } else {
            xmlRelaxNGSetValidErrors(ctx->valid,
                                     (xmlRelaxNGValidityErrorFunc) fprintf,
                                     (xmlRelaxNGValidityWarningFunc) fprintf, stderr);
        }
    }

    /* xmlRelaxNGSetValidStructuredErrors( */
    /*  valid, relaxng_invalid_stderr, valid); */

    xmlLineNumbersDefault(1);
    rc = xmlRelaxNGValidateDoc(ctx->valid, doc);
    if (rc > 0) {
        valid = FALSE;

    } else if (rc < 0) {
        crm_err("Internal libxml error during validation\n");
    }

  cleanup:

    if (cached_ctx) {
        *cached_ctx = ctx;

    } else {
        if (ctx->parser != NULL) {
            xmlRelaxNGFreeParserCtxt(ctx->parser);
        }
        if (ctx->valid != NULL) {
            xmlRelaxNGFreeValidCtxt(ctx->valid);
        }
        if (ctx->rng != NULL) {
            xmlRelaxNGFree(ctx->rng);
        }
        free(ctx);
    }

    return valid;
}
