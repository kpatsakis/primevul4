static void CinemaProfileCompliance(const opj_image_t *jp2_image,
  opj_cparameters_t *parameters)
{
  /*
    Digital Cinema 4K profile compliant codestream.
  */
  parameters->tile_size_on=OPJ_FALSE;
  parameters->cp_tdx=1;
  parameters->cp_tdy=1;
  parameters->tp_flag='C';
  parameters->tp_on=1;
  parameters->cp_tx0=0;
  parameters->cp_ty0=0;
  parameters->image_offset_x0=0;
  parameters->image_offset_y0=0;
  parameters->cblockw_init=32;
  parameters->cblockh_init=32;
  parameters->csty|=0x01;
  parameters->prog_order=OPJ_CPRL;
  parameters->roi_compno=(-1);
  parameters->subsampling_dx=1;
  parameters->subsampling_dy=1;
  parameters->irreversible=1;
  if ((jp2_image->comps[0].w == 2048) || (jp2_image->comps[0].h == 1080))
    {
      /*
        Digital Cinema 2K.
      */
      parameters->cp_cinema=OPJ_CINEMA2K_24;
      parameters->cp_rsiz=OPJ_CINEMA2K;
      parameters->max_comp_size=1041666;
      if (parameters->numresolution > 6)
        parameters->numresolution=6;

    }
  if ((jp2_image->comps[0].w == 4096) || (jp2_image->comps[0].h == 2160))
    {
      /*
        Digital Cinema 4K.
      */
      parameters->cp_cinema=OPJ_CINEMA4K_24;
      parameters->cp_rsiz=OPJ_CINEMA4K;
      parameters->max_comp_size=1041666;
      if (parameters->numresolution < 1)
        parameters->numresolution=1;
      if (parameters->numresolution > 7)
        parameters->numresolution=7;
      parameters->numpocs=2;
      parameters->POC[0].tile=1;
      parameters->POC[0].resno0=0;
      parameters->POC[0].compno0=0;
      parameters->POC[0].layno1=1;
      parameters->POC[0].resno1=parameters->numresolution-1;
      parameters->POC[0].compno1=3;
      parameters->POC[0].prg1=OPJ_CPRL;
      parameters->POC[1].tile=1;
      parameters->POC[1].resno0=parameters->numresolution-1;
      parameters->POC[1].compno0=0;
      parameters->POC[1].layno1=1;
      parameters->POC[1].resno1=parameters->numresolution;
      parameters->POC[1].compno1=3;
      parameters->POC[1].prg1=OPJ_CPRL;
    }
  parameters->tcp_numlayers=1;
  parameters->tcp_rates[0]=((float) (jp2_image->numcomps*jp2_image->comps[0].w*
    jp2_image->comps[0].h*jp2_image->comps[0].prec))/(parameters->max_comp_size*
    8*jp2_image->comps[0].dx*jp2_image->comps[0].dy);
  parameters->cp_disto_alloc=1;
}
