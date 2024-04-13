crm_xml_escape(const char *text)
{
    int index;
    int changes = 0;
    int length = 1 + strlen(text);
    char *copy = strdup(text);

    /*
     * When xmlCtxtReadDoc() parses &lt; and friends in a
     * value, it converts them to their human readable
     * form.
     *
     * If one uses xmlNodeDump() to convert it back to a
     * string, all is well, because special characters are
     * converted back to their escape sequences.
     *
     * However xmlNodeDump() is randomly dog slow, even with the same
     * input. So we need to replicate the escapeing in our custom
     * version so that the result can be re-parsed by xmlCtxtReadDoc()
     * when necessary.
     */

    for (index = 0; index < length; index++) {
        switch (copy[index]) {
            case 0:
                break;
            case '<':
                copy = crm_xml_escape_shuffle(copy, index, &length, "&lt;");
                changes++;
                break;
            case '>':
                copy = crm_xml_escape_shuffle(copy, index, &length, "&gt;");
                changes++;
                break;
            case '"':
                copy = crm_xml_escape_shuffle(copy, index, &length, "&quot;");
                changes++;
                break;
            case '\'':
                copy = crm_xml_escape_shuffle(copy, index, &length, "&apos;");
                changes++;
                break;
            case '&':
                copy = crm_xml_escape_shuffle(copy, index, &length, "&amp;");
                changes++;
                break;
            case '\t':
                /* Might as well just expand to a few spaces... */
                copy = crm_xml_escape_shuffle(copy, index, &length, "    ");
                changes++;
                break;
            case '\n':
                /* crm_trace("Convert: \\%.3o", copy[index]); */
                copy = crm_xml_escape_shuffle(copy, index, &length, "\\n");
                changes++;
                break;
            case '\r':
                copy = crm_xml_escape_shuffle(copy, index, &length, "\\r");
                changes++;
                break;
                /* For debugging...
            case '\\':
                crm_trace("Passthrough: \\%c", copy[index+1]);
                break;
                */
            default:
                /* Check for and replace non-printing characters with their octal equivalent */
                if(copy[index] < ' ' || copy[index] > '~') {
                    char *replace = crm_strdup_printf("\\%.3o", copy[index]);

                    /* crm_trace("Convert to octal: \\%.3o", copy[index]); */
                    copy = crm_xml_escape_shuffle(copy, index, &length, replace);
                    free(replace);
                    changes++;
                }
        }
    }

    if (changes) {
        crm_trace("Dumped '%s'", copy);
    }
    return copy;
}
