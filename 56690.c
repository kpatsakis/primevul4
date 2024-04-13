static int dnxhd_get_profile(int cid)
{
    switch(cid) {
    case 1270:
        return FF_PROFILE_DNXHR_444;
    case 1271:
        return FF_PROFILE_DNXHR_HQX;
    case 1272:
        return FF_PROFILE_DNXHR_HQ;
    case 1273:
        return FF_PROFILE_DNXHR_SQ;
    case 1274:
        return FF_PROFILE_DNXHR_LB;
    }
    return FF_PROFILE_DNXHD;
}
