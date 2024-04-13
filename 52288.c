x86_Init(struct _7zip *zip)
{
	zip->bcj_state = 0;
	zip->bcj_prevPosT = (size_t)0 - 1;
	zip->bcj_prevMask = 0;
	zip->bcj_ip = 5;
}
