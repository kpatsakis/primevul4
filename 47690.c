write_xml_stream(xmlNode * xml_node, const char *filename, FILE * stream, gboolean compress)
{
    int res = 0;
    char *buffer = NULL;
    unsigned int out = 0;

    CRM_CHECK(stream != NULL, return -1);

    crm_trace("Writing XML out to %s", filename);
    if (xml_node == NULL) {
        crm_err("Cannot write NULL to %s", filename);
        fclose(stream);
        return -1;
    }


    crm_log_xml_trace(xml_node, "Writing out");

    buffer = dump_xml_formatted(xml_node);
    CRM_CHECK(buffer != NULL && strlen(buffer) > 0, crm_log_xml_warn(xml_node, "dump:failed");
              goto bail);

    if (compress) {
#if HAVE_BZLIB_H
        int rc = BZ_OK;
        unsigned int in = 0;
        BZFILE *bz_file = NULL;

        bz_file = BZ2_bzWriteOpen(&rc, stream, 5, 0, 30);
        if (rc != BZ_OK) {
            crm_err("bzWriteOpen failed: %d", rc);
        } else {
            BZ2_bzWrite(&rc, bz_file, buffer, strlen(buffer));
            if (rc != BZ_OK) {
                crm_err("bzWrite() failed: %d", rc);
            }
        }

        if (rc == BZ_OK) {
            BZ2_bzWriteClose(&rc, bz_file, 0, &in, &out);
            if (rc != BZ_OK) {
                crm_err("bzWriteClose() failed: %d", rc);
                out = -1;
            } else {
                crm_trace("%s: In: %d, out: %d", filename, in, out);
            }
        }
#else
        crm_err("Cannot write compressed files:" " bzlib was not available at compile time");
#endif
    }

    if (out <= 0) {
        res = fprintf(stream, "%s", buffer);
        if (res < 0) {
            crm_perror(LOG_ERR, "Cannot write output to %s", filename);
            goto bail;
        }
    }

  bail:

    if (fflush(stream) != 0) {
        crm_perror(LOG_ERR, "fflush for %s failed:", filename);
        res = -1;
    }

    if (fsync(fileno(stream)) < 0) {
        crm_perror(LOG_ERR, "fsync for %s failed:", filename);
        res = -1;
    }

    fclose(stream);

    crm_trace("Saved %d bytes to the Cib as XML", res);
    free(buffer);

    return res;
}
