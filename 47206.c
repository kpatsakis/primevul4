CLASS DCRaw()
{
order=0; /* Suppress valgrind error. */
shot_select=0, multi_out=0, aber[0] = aber[1] = aber[2] = aber[3] = 1;
gamm[0] = 0.45, gamm[1] = 4.5, gamm[2] = gamm[3] = gamm[4] = gamm[5] = 0;
bright=1, user_mul[0] = user_mul[1] = user_mul[2] = user_mul[3] = 0;
threshold=0, half_size=0, four_color_rgb=0, document_mode=0, highlight=0;
verbose=0, use_auto_wb=0, use_camera_wb=0, use_camera_matrix=-1;
output_color=1, output_bps=8, output_tiff=0, med_passes=0, no_auto_bright=0;
greybox[0] = greybox[1] = 0, greybox[2] = greybox[3] = UINT_MAX;
tone_curve_size = 0, tone_curve_offset = 0; /* Nikon Tone Curves UF*/
tone_mode_offset = 0, tone_mode_size = 0; /* Nikon ToneComp UF*/
messageBuffer = NULL;
lastStatus = DCRAW_SUCCESS;
ifname = NULL;
ifname_display = NULL;
ifpReadCount = 0;
ifpSize = 0;
ifpStepProgress = 0;
eofCount = 0;
}
