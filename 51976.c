static void TraceBezier(PrimitiveInfo *primitive_info,
  const size_t number_coordinates)
{
  double
    alpha,
    *coefficients,
    weight;

  PointInfo
    end,
    point,
    *points;

  register PrimitiveInfo
    *p;

  register ssize_t
    i,
    j;

  size_t
    control_points,
    quantum;

  /*
    Allocate coeficients.
  */
  quantum=number_coordinates;
  for (i=0; i < (ssize_t) number_coordinates; i++)
  {
    for (j=i+1; j < (ssize_t) number_coordinates; j++)
    {
      alpha=fabs(primitive_info[j].point.x-primitive_info[i].point.x);
      if (alpha > (double) quantum)
        quantum=(size_t) alpha;
      alpha=fabs(primitive_info[j].point.y-primitive_info[i].point.y);
      if (alpha > (double) quantum)
        quantum=(size_t) alpha;
    }
  }
  quantum=(size_t) MagickMin((double) quantum/number_coordinates,
    (double) BezierQuantum);
  control_points=quantum*number_coordinates;
  coefficients=(double *) AcquireQuantumMemory((size_t)
    number_coordinates,sizeof(*coefficients));
  points=(PointInfo *) AcquireQuantumMemory((size_t) control_points,
    sizeof(*points));
  if ((coefficients == (double *) NULL) ||
      (points == (PointInfo *) NULL))
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  /*
    Compute bezier points.
  */
  end=primitive_info[number_coordinates-1].point;
  for (i=0; i < (ssize_t) number_coordinates; i++)
    coefficients[i]=Permutate((ssize_t) number_coordinates-1,i);
  weight=0.0;
  for (i=0; i < (ssize_t) control_points; i++)
  {
    p=primitive_info;
    point.x=0.0;
    point.y=0.0;
    alpha=pow((double) (1.0-weight),(double) number_coordinates-1.0);
    for (j=0; j < (ssize_t) number_coordinates; j++)
    {
      point.x+=alpha*coefficients[j]*p->point.x;
      point.y+=alpha*coefficients[j]*p->point.y;
      alpha*=weight/(1.0-weight);
      p++;
    }
    points[i]=point;
    weight+=1.0/control_points;
  }
  /*
    Bezier curves are just short segmented polys.
  */
  p=primitive_info;
  for (i=0; i < (ssize_t) control_points; i++)
  {
    TracePoint(p,points[i]);
    p+=p->coordinates;
  }
  TracePoint(p,end);
  p+=p->coordinates;
  primitive_info->coordinates=(size_t) (p-primitive_info);
  for (i=0; i < (ssize_t) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
  points=(PointInfo *) RelinquishMagickMemory(points);
  coefficients=(double *) RelinquishMagickMemory(coefficients);
}
