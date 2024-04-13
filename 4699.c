int fmtutil_default_box_handler(deark *c, struct de_boxesctx *bctx)
{
	struct de_boxdata *curbox = bctx->curbox;

	if(curbox->is_uuid) {
		if(!de_memcmp(curbox->uuid, "\xb1\x4b\xf8\xbd\x08\x3d\x4b\x43\xa5\xae\x8c\xd7\xd5\xa6\xce\x03", 16)) {
			de_dbg(c, "GeoTIFF data at %"I64_FMT", len=%"I64_FMT, curbox->payload_pos, curbox->payload_len);
			dbuf_create_file_from_slice(bctx->f, curbox->payload_pos, curbox->payload_len, "geo.tif", NULL, DE_CREATEFLAG_IS_AUX);
		}
		else if(!de_memcmp(curbox->uuid, "\xbe\x7a\xcf\xcb\x97\xa9\x42\xe8\x9c\x71\x99\x94\x91\xe3\xaf\xac", 16)) {
			de_dbg(c, "XMP data at %"I64_FMT", len=%"I64_FMT, curbox->payload_pos, curbox->payload_len);
			dbuf_create_file_from_slice(bctx->f, curbox->payload_pos, curbox->payload_len, "xmp", NULL, DE_CREATEFLAG_IS_AUX);
		}
		else if(!de_memcmp(curbox->uuid, "\x2c\x4c\x01\x00\x85\x04\x40\xb9\xa0\x3e\x56\x21\x48\xd6\xdf\xeb", 16)) {
			de_dbg(c, "Photoshop resources at %"I64_FMT", len=%"I64_FMT, curbox->payload_pos, curbox->payload_len);
			de_dbg_indent(c, 1);
			fmtutil_handle_photoshop_rsrc(c, bctx->f, curbox->payload_pos, curbox->payload_len, 0x0);
			de_dbg_indent(c, -1);
		}
		else if(!de_memcmp(curbox->uuid, "\x05\x37\xcd\xab\x9d\x0c\x44\x31\xa7\x2a\xfa\x56\x1f\x2a\x11\x3e", 16) ||
			!de_memcmp(curbox->uuid, "JpgTiffExif->JP2", 16))
		{
			de_dbg(c, "Exif data at %"I64_FMT", len=%"I64_FMT, curbox->payload_pos, curbox->payload_len);
			de_dbg_indent(c, 1);
			fmtutil_handle_exif(c, curbox->payload_pos, curbox->payload_len);
			de_dbg_indent(c, -1);
		}
	}
	return 1;
}