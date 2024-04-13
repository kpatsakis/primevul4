oam_print (netdissect_options *ndo,
           const u_char *p, u_int length, u_int hec)
{
    uint32_t cell_header;
    uint16_t vpi, vci, cksum, cksum_shouldbe, idx;
    uint8_t  cell_type, func_type, payload, clp;

    union {
        const struct oam_fm_loopback_t *oam_fm_loopback;
        const struct oam_fm_ais_rdi_t *oam_fm_ais_rdi;
    } oam_ptr;


    ND_TCHECK(*(p+ATM_HDR_LEN_NOHEC+hec));
    cell_header = EXTRACT_32BITS(p+hec);
    cell_type = ((*(p+ATM_HDR_LEN_NOHEC+hec))>>4) & 0x0f;
    func_type = (*(p+ATM_HDR_LEN_NOHEC+hec)) & 0x0f;

    vpi = (cell_header>>20)&0xff;
    vci = (cell_header>>4)&0xffff;
    payload = (cell_header>>1)&0x7;
    clp = cell_header&0x1;

    ND_PRINT((ndo, "%s, vpi %u, vci %u, payload [ %s ], clp %u, length %u",
           tok2str(oam_f_values, "OAM F5", vci),
           vpi, vci,
           tok2str(atm_pty_values, "Unknown", payload),
           clp, length));

    if (!ndo->ndo_vflag) {
        return;
    }

    ND_PRINT((ndo, "\n\tcell-type %s (%u)",
           tok2str(oam_celltype_values, "unknown", cell_type),
           cell_type));

    if (oam_functype_values[cell_type] == NULL)
        ND_PRINT((ndo, ", func-type unknown (%u)", func_type));
    else
        ND_PRINT((ndo, ", func-type %s (%u)",
               tok2str(oam_functype_values[cell_type],"none",func_type),
               func_type));

    p += ATM_HDR_LEN_NOHEC + hec;

    switch (cell_type << 4 | func_type) {
    case (OAM_CELLTYPE_FM << 4 | OAM_FM_FUNCTYPE_LOOPBACK):
        oam_ptr.oam_fm_loopback = (const struct oam_fm_loopback_t *)(p + OAM_CELLTYPE_FUNCTYPE_LEN);
        ND_TCHECK(*oam_ptr.oam_fm_loopback);
        ND_PRINT((ndo, "\n\tLoopback-Indicator %s, Correlation-Tag 0x%08x",
               tok2str(oam_fm_loopback_indicator_values,
                       "Unknown",
                       oam_ptr.oam_fm_loopback->loopback_indicator & OAM_FM_LOOPBACK_INDICATOR_MASK),
               EXTRACT_32BITS(&oam_ptr.oam_fm_loopback->correlation_tag)));
        ND_PRINT((ndo, "\n\tLocation-ID "));
        for (idx = 0; idx < sizeof(oam_ptr.oam_fm_loopback->loopback_id); idx++) {
            if (idx % 2) {
                ND_PRINT((ndo, "%04x ", EXTRACT_16BITS(&oam_ptr.oam_fm_loopback->loopback_id[idx])));
            }
        }
        ND_PRINT((ndo, "\n\tSource-ID   "));
        for (idx = 0; idx < sizeof(oam_ptr.oam_fm_loopback->source_id); idx++) {
            if (idx % 2) {
                ND_PRINT((ndo, "%04x ", EXTRACT_16BITS(&oam_ptr.oam_fm_loopback->source_id[idx])));
            }
        }
        break;

    case (OAM_CELLTYPE_FM << 4 | OAM_FM_FUNCTYPE_AIS):
    case (OAM_CELLTYPE_FM << 4 | OAM_FM_FUNCTYPE_RDI):
        oam_ptr.oam_fm_ais_rdi = (const struct oam_fm_ais_rdi_t *)(p + OAM_CELLTYPE_FUNCTYPE_LEN);
        ND_TCHECK(*oam_ptr.oam_fm_ais_rdi);
        ND_PRINT((ndo, "\n\tFailure-type 0x%02x", oam_ptr.oam_fm_ais_rdi->failure_type));
        ND_PRINT((ndo, "\n\tLocation-ID "));
        for (idx = 0; idx < sizeof(oam_ptr.oam_fm_ais_rdi->failure_location); idx++) {
            if (idx % 2) {
                ND_PRINT((ndo, "%04x ", EXTRACT_16BITS(&oam_ptr.oam_fm_ais_rdi->failure_location[idx])));
            }
        }
        break;

    case (OAM_CELLTYPE_FM << 4 | OAM_FM_FUNCTYPE_CONTCHECK):
        /* FIXME */
        break;

    default:
        break;
    }

    /* crc10 checksum verification */
    ND_TCHECK2(*(p + OAM_CELLTYPE_FUNCTYPE_LEN + OAM_FUNCTION_SPECIFIC_LEN), 2);
    cksum = EXTRACT_16BITS(p + OAM_CELLTYPE_FUNCTYPE_LEN + OAM_FUNCTION_SPECIFIC_LEN)
        & OAM_CRC10_MASK;
    cksum_shouldbe = verify_crc10_cksum(0, p, OAM_PAYLOAD_LEN);

    ND_PRINT((ndo, "\n\tcksum 0x%03x (%scorrect)",
           cksum,
           cksum_shouldbe == 0 ? "" : "in"));

    return;

trunc:
    ND_PRINT((ndo, "[|oam]"));
    return;
}
