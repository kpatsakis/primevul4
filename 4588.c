GF_ISOFile *gf_isom_create_movie(const char *fileName, GF_ISOOpenMode OpenMode, const char *tmp_dir)
{
	GF_Err e;

	GF_ISOFile *mov = gf_isom_new_movie();
	if (!mov) return NULL;
	mov->openMode = OpenMode;
	//then set up our movie

	//in WRITE, the input dataMap is ALWAYS NULL
	mov->movieFileMap = NULL;

	//but we have the edit one
	if (OpenMode == GF_ISOM_OPEN_WRITE) {
		const char *ext;
		//THIS IS NOT A TEMP FILE, WRITE mode is used for "live capture"
		//this file will be the final file...
		mov->fileName = fileName ? gf_strdup(fileName) : NULL;
		e = gf_isom_datamap_new(fileName, NULL, GF_ISOM_DATA_MAP_WRITE, &mov->editFileMap);
		if (e) goto err_exit;

		/*brand is set to ISOM or QT by default - it may be touched until sample data is added to track*/
		ext = gf_file_ext_start(fileName);
		if (ext && (!strnicmp(ext, ".mov", 4) || !strnicmp(ext, ".qt", 3))) {
			gf_isom_set_brand_info((GF_ISOFile *) mov, GF_ISOM_BRAND_QT, 512);
		} else {
			gf_isom_set_brand_info((GF_ISOFile *) mov, GF_ISOM_BRAND_ISOM, 1);
		}
	} else {
		//we are in EDIT mode but we are creating the file -> temp file
		mov->finalName = fileName ? gf_strdup(fileName) : NULL;
		e = gf_isom_datamap_new("_gpac_isobmff_tmp_edit", tmp_dir, GF_ISOM_DATA_MAP_WRITE, &mov->editFileMap);
		if (e) {
			gf_isom_set_last_error(NULL, e);
			gf_isom_delete_movie(mov);
			return NULL;
		}
		//brand is set to ISOM by default
		gf_isom_set_brand_info( (GF_ISOFile *) mov, GF_ISOM_BRAND_ISOM, 1);
	}

	//create an MDAT
	mov->mdat = (GF_MediaDataBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_MDAT);
	if (!mov->mdat) {
		gf_isom_set_last_error(NULL, GF_OUT_OF_MEM);
		gf_isom_delete_movie(mov);
		return NULL;
	}
	gf_list_add(mov->TopBoxes, mov->mdat);

	//default behavior is capture mode, no interleaving (eg, no rewrite of mdat)
	mov->storageMode = GF_ISOM_STORE_FLAT;
	return mov;

err_exit:
	gf_isom_set_last_error(NULL, e);
	if (mov) gf_isom_delete_movie(mov);
	return NULL;
}