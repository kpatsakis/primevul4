crm_xml_cleanup(void)
{
    int lpc = 0;
    relaxng_ctx_cache_t *ctx = NULL;

    crm_info("Cleaning up memory from libxml2");
    for (; lpc < xml_schema_max; lpc++) {

        switch (known_schemas[lpc].type) {
            case 0:
                /* None */
                break;
            case 1:
                /* DTD - Not cached */
                break;
            case 2:
                /* RNG - Cached */
                ctx = (relaxng_ctx_cache_t *) known_schemas[lpc].cache;
                if (ctx == NULL) {
                    break;
                }
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
                known_schemas[lpc].cache = NULL;
                break;
            default:
                break;
        }
        free(known_schemas[lpc].name);
        free(known_schemas[lpc].location);
        free(known_schemas[lpc].transform);
    }
    free(known_schemas);
    xsltCleanupGlobals();
    xmlCleanupParser();
}
