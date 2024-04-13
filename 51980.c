static size_t TracePath(PrimitiveInfo *primitive_info,const char *path)
{
  char
    token[MagickPathExtent];

  const char
    *p;

  int
    attribute,
    last_attribute;

  double
    x,
    y;

  PointInfo
    end = {0.0, 0.0},
    points[4] = { {0.0,0.0}, {0.0,0.0}, {0.0,0.0}, {0.0,0.0} },
    point = {0.0, 0.0},
    start = {0.0, 0.0};

  PrimitiveType
    primitive_type;

  register PrimitiveInfo
    *q;

  register ssize_t
    i;

  size_t
    number_coordinates,
    z_count;

  attribute=0;
  number_coordinates=0;
  z_count=0;
  primitive_type=primitive_info->primitive;
  q=primitive_info;
  for (p=path; *p != '\0'; )
  {
    while (isspace((int) ((unsigned char) *p)) != 0)
      p++;
    if (*p == '\0')
      break;
    last_attribute=attribute;
    attribute=(int) (*p++);
    switch (attribute)
    {
      case 'a':
      case 'A':
      {
        MagickBooleanType
          large_arc,
          sweep;

        double
          angle;

        PointInfo
          arc;

        /*
          Compute arc points.
        */
        do
        {
          GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            GetNextToken(p,&p,MagickPathExtent,token);
          arc.x=StringToDouble(token,(char **) NULL);
          GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            GetNextToken(p,&p,MagickPathExtent,token);
          arc.y=StringToDouble(token,(char **) NULL);
          GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            GetNextToken(p,&p,MagickPathExtent,token);
          angle=StringToDouble(token,(char **) NULL);
          GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            GetNextToken(p,&p,MagickPathExtent,token);
          large_arc=StringToLong(token) != 0 ? MagickTrue : MagickFalse;
          GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            GetNextToken(p,&p,MagickPathExtent,token);
          sweep=StringToLong(token) != 0 ? MagickTrue : MagickFalse;
          GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            GetNextToken(p,&p,MagickPathExtent,token);
          x=StringToDouble(token,(char **) NULL);
          GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            GetNextToken(p,&p,MagickPathExtent,token);
          y=StringToDouble(token,(char **) NULL);
          end.x=(double) (attribute == (int) 'A' ? x : point.x+x);
          end.y=(double) (attribute == (int) 'A' ? y : point.y+y);
          TraceArcPath(q,point,end,arc,angle,large_arc,sweep);
          q+=q->coordinates;
          point=end;
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 'c':
      case 'C':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=point;
          for (i=1; i < 4; i++)
          {
            GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              GetNextToken(p,&p,MagickPathExtent,token);
            x=StringToDouble(token,(char **) NULL);
            GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              GetNextToken(p,&p,MagickPathExtent,token);
            y=StringToDouble(token,(char **) NULL);
            end.x=(double) (attribute == (int) 'C' ? x : point.x+x);
            end.y=(double) (attribute == (int) 'C' ? y : point.y+y);
            points[i]=end;
          }
          for (i=0; i < 4; i++)
            (q+i)->point=points[i];
          TraceBezier(q,4);
          q+=q->coordinates;
          point=end;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 'H':
      case 'h':
      {
        do
        {
          GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            GetNextToken(p,&p,MagickPathExtent,token);
          x=StringToDouble(token,(char **) NULL);
          point.x=(double) (attribute == (int) 'H' ? x: point.x+x);
          TracePoint(q,point);
          q+=q->coordinates;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 'l':
      case 'L':
      {
        do
        {
          GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            GetNextToken(p,&p,MagickPathExtent,token);
          x=StringToDouble(token,(char **) NULL);
          GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            GetNextToken(p,&p,MagickPathExtent,token);
          y=StringToDouble(token,(char **) NULL);
          point.x=(double) (attribute == (int) 'L' ? x : point.x+x);
          point.y=(double) (attribute == (int) 'L' ? y : point.y+y);
          TracePoint(q,point);
          q+=q->coordinates;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 'M':
      case 'm':
      {
        if (q != primitive_info)
          {
            primitive_info->coordinates=(size_t) (q-primitive_info);
            number_coordinates+=primitive_info->coordinates;
            primitive_info=q;
          }
        i=0;
        do
        {
          GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            GetNextToken(p,&p,MagickPathExtent,token);
          x=StringToDouble(token,(char **) NULL);
          GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            GetNextToken(p,&p,MagickPathExtent,token);
          y=StringToDouble(token,(char **) NULL);
          point.x=(double) (attribute == (int) 'M' ? x : point.x+x);
          point.y=(double) (attribute == (int) 'M' ? y : point.y+y);
          if (i == 0)
            start=point;
          i++;
          TracePoint(q,point);
          q+=q->coordinates;
          if ((i != 0) && (attribute == (int) 'M'))
            {
              TracePoint(q,point);
              q+=q->coordinates;
            }
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 'q':
      case 'Q':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=point;
          for (i=1; i < 3; i++)
          {
            GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              GetNextToken(p,&p,MagickPathExtent,token);
            x=StringToDouble(token,(char **) NULL);
            GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              GetNextToken(p,&p,MagickPathExtent,token);
            y=StringToDouble(token,(char **) NULL);
            if (*p == ',')
              p++;
            end.x=(double) (attribute == (int) 'Q' ? x : point.x+x);
            end.y=(double) (attribute == (int) 'Q' ? y : point.y+y);
            points[i]=end;
          }
          for (i=0; i < 3; i++)
            (q+i)->point=points[i];
          TraceBezier(q,3);
          q+=q->coordinates;
          point=end;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 's':
      case 'S':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=points[3];
          points[1].x=2.0*points[3].x-points[2].x;
          points[1].y=2.0*points[3].y-points[2].y;
          for (i=2; i < 4; i++)
          {
            GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              GetNextToken(p,&p,MagickPathExtent,token);
            x=StringToDouble(token,(char **) NULL);
            GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              GetNextToken(p,&p,MagickPathExtent,token);
            y=StringToDouble(token,(char **) NULL);
            if (*p == ',')
              p++;
            end.x=(double) (attribute == (int) 'S' ? x : point.x+x);
            end.y=(double) (attribute == (int) 'S' ? y : point.y+y);
            points[i]=end;
          }
          if (strchr("CcSs",last_attribute) == (char *) NULL)
            {
              points[0]=point;
              points[1]=point;
            }
          for (i=0; i < 4; i++)
            (q+i)->point=points[i];
          TraceBezier(q,4);
          q+=q->coordinates;
          point=end;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 't':
      case 'T':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=points[2];
          points[1].x=2.0*points[2].x-points[1].x;
          points[1].y=2.0*points[2].y-points[1].y;
          for (i=2; i < 3; i++)
          {
            GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              GetNextToken(p,&p,MagickPathExtent,token);
            x=StringToDouble(token,(char **) NULL);
            GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              GetNextToken(p,&p,MagickPathExtent,token);
            y=StringToDouble(token,(char **) NULL);
            end.x=(double) (attribute == (int) 'T' ? x : point.x+x);
            end.y=(double) (attribute == (int) 'T' ? y : point.y+y);
            points[i]=end;
          }
          if (strchr("QqTt",last_attribute) == (char *) NULL)
            {
              points[0]=point;
              points[1]=point;
            }
          for (i=0; i < 3; i++)
            (q+i)->point=points[i];
          TraceBezier(q,3);
          q+=q->coordinates;
          point=end;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 'v':
      case 'V':
      {
        do
        {
          GetNextToken(p,&p,MagickPathExtent,token);
          if (*token == ',')
            GetNextToken(p,&p,MagickPathExtent,token);
          y=StringToDouble(token,(char **) NULL);
          point.y=(double) (attribute == (int) 'V' ? y : point.y+y);
          TracePoint(q,point);
          q+=q->coordinates;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      case 'z':
      case 'Z':
      {
        point=start;
        TracePoint(q,point);
        q+=q->coordinates;
        primitive_info->coordinates=(size_t) (q-primitive_info);
        number_coordinates+=primitive_info->coordinates;
        primitive_info=q;
        z_count++;
        break;
      }
      default:
      {
        if (isalpha((int) ((unsigned char) attribute)) != 0)
          (void) FormatLocaleFile(stderr,"attribute not recognized: %c\n",
            attribute);
        break;
      }
    }
  }
  primitive_info->coordinates=(size_t) (q-primitive_info);
  number_coordinates+=primitive_info->coordinates;
  for (i=0; i < (ssize_t) number_coordinates; i++)
  {
    q--;
    q->primitive=primitive_type;
    if (z_count > 1)
      q->method=FillToBorderMethod;
  }
  q=primitive_info;
  return(number_coordinates);
}
