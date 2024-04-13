arcSegmentSide(const pointObj *p1, const pointObj *p2, const pointObj *q)
{
  double side = ( (q->x - p1->x) * (p2->y - p1->y) - (p2->x - p1->x) * (q->y - p1->y) );
  if ( FP_EQ(side,0.0) ) {
    return FP_COLINEAR;
  } else {
    if ( side < 0.0 )
      return FP_LEFT;
    else
      return FP_RIGHT;
  }
}
