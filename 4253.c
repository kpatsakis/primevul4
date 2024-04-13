bool scale_mv(MotionVector* out_mv, MotionVector mv, int colDist, int currDist)
{
  int td = Clip3(-128,127, colDist);
  int tb = Clip3(-128,127, currDist);

  if (td==0) {
    *out_mv = mv;
    return false;
  }
  else {
    int tx = (16384 + (abs_value(td)>>1)) / td;
    int distScaleFactor = Clip3(-4096,4095, (tb*tx+32)>>6);
    out_mv->x = Clip3(-32768,32767,
                      Sign(distScaleFactor*mv.x)*((abs_value(distScaleFactor*mv.x)+127)>>8));
    out_mv->y = Clip3(-32768,32767,
                      Sign(distScaleFactor*mv.y)*((abs_value(distScaleFactor*mv.y)+127)>>8));
    return true;
  }
}