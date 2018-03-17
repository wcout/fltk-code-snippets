/*
   extension to bresenham.c for filled circles/ellipses
   wcout 2018/03/25
*/
void plotFilledCircle(int xm, int ym, int r)
{
   int x = -r, y = 0, err = 2-2*r;                /* bottom left to top right */
   do {
      plotLine(xm-x, ym+y, xm+x, ym+y);                /*   I. Quadrant +x +y */
      plotLine(xm+x, ym-y, xm-x, ym-y);                /* III. Quadrant -x -y */
      r = err;
      if (r <= y) err += ++y*2+1;                             /* e_xy+e_y < 0 */
      if (r > x || err > y)                  /* e_xy+e_x > 0 or no 2nd y-step */
         err += ++x*2+1;                                     /* -> x-step now */
   } while (x < 0);
}

void plotFilledCircleAA(int xm, int ym, int r)
{                     /* draw a black anti-aliased circle on white background */
   int XM = xm;
   int YM = ym;
   int R = r;
   int x = -r, y = 0;           /* II. quadrant from bottom left to top right */
   int i, x2, e2, err = 2-2*r;                             /* error of 1.step */
   r = 1-err;
   do {
      i = 255*abs(err-2*(x+y)-2)/r;               /* get blend value of pixel */
      setPixelAA(xm-x, ym+y, i);                             /*   I. Quadrant */
      setPixelAA(xm-y, ym-x, i);                             /*  II. Quadrant */
      setPixelAA(xm+x, ym-y, i);                             /* III. Quadrant */
      setPixelAA(xm+y, ym+x, i);                             /*  IV. Quadrant */
      e2 = err; x2 = x;                                    /* remember values */
      if (err+y > 0) {                                              /* x step */
         i = 255*(err-2*x-1)/r;                              /* outward pixel */
         if (i < 256) {
            setPixelAA(xm-x, ym+y+1, i);
            setPixelAA(xm-y-1, ym-x, i);
            setPixelAA(xm+x, ym-y-1, i);
            setPixelAA(xm+y+1, ym+x, i);
         }
         err += ++x*2+1;
      }
      if (e2+x2 <= 0) {                                             /* y step */
         i = 255*(2*y+3-e2)/r;                                /* inward pixel */
         if (i < 256) {
            setPixelAA(xm-x2-1, ym+y, i);
            setPixelAA(xm-y, ym-x2-1, i);
            setPixelAA(xm+x2+1, ym-y, i);
            setPixelAA(xm+y, ym+x2+1, i);
         }
         err += ++y*2+1;
      }
   } while (x < 0);
   plotFilledCircle(XM, YM, R);
}

void plotFilledEllipse(int xm, int ym, int a, int b)
{
   int x = -a, y = 0;           /* II. quadrant from bottom left to top right */
   long e2 = (long)b*b, err = (long)x*(2*e2+x)+e2;         /* error of 1.step */

   do {
       plotLine(xm-x, ym+y, xm+x,ym+y);                      /*   I. Quadrant */
       plotLine(xm-x, ym-y, xm+x, ym-y);                     /*  IV. Quadrant */
       e2 = 2*err;
       if (e2 >= (x*2+1)*(long)b*b)                           /* e_xy+e_x > 0 */
          err += (++x*2+1)*(long)b*b;
       if (e2 <= (y*2+1)*(long)a*a)                           /* e_xy+e_y < 0 */
          err += (++y*2+1)*(long)a*a;
   } while (x <= 0);

   while (y++ < b) {                  /* too early stop of flat ellipses a=1, */
       setPixel(xm, ym+y);                        /* -> finish tip of ellipse */
       setPixel(xm, ym-y);
   }
}

void plotFilledEllipseRect(int x0, int y0, int x1, int y1)
{                              /* rectangular parameter enclosing the ellipse */
   long a = abs(x1-x0), b = abs(y1-y0), b1 = b&1;                 /* diameter */
   double dx = 4*(1.0-a)*b*b, dy = 4*(b1+1)*a*a;           /* error increment */
   double err = dx+dy+b1*a*a;                              /* error of 1.step */

   if (x0 > x1) { x0 = x1; x1 += a; }        /* if called with swapped points */
   if (y0 > y1) y0 = y1;                                  /* .. exchange them */
   y0 += (b+1)/2; y1 = y0-b1;                               /* starting pixel */
   a = 8*a*a; b1 = 8*b*b;

   do {
      plotLine(x1, y0, x0, y0);                              /*   I. Quadrant */
      plotLine(x1, y1, x0, y1);                              /*  IV. Quadrant */
      int e2 = 2*err;
      if (e2 <= dy) { y0++; y1--; err += dy += a; }                 /* y step */
      if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; }  /* x step */
   } while (x0 <= x1);

   while (y0-y1 <= b) {                /* too early stop of flat ellipses a=1 */
      setPixel(x0-1, y0);                         /* -> finish tip of ellipse */
      setPixel(x1+1, y0++);
      setPixel(x0-1, y1);
      setPixel(x1+1, y1--);
   }
}

void plotFilledEllipseRectAA(int x0, int y0, int x1, int y1)
{        /* draw a black anti-aliased rectangular ellipse on white background */
	int X0 = x0;
	int Y0 = y0;
	int X1 = x1;
	int Y1 = y1;
   long a = abs(x1-x0), b = abs(y1-y0), b1 = b&1;                 /* diameter */
   float dx = 4*(a-1.0)*b*b, dy = 4*(b1+1)*a*a;            /* error increment */
   float ed, i, err = b1*a*a-dx+dy;                        /* error of 1.step */
	bool f;

   if (a == 0 || b == 0) return plotLine(x0,y0, x1,y1);
   if (x0 > x1) { x0 = x1; x1 += a; }        /* if called with swapped points */
   if (y0 > y1) y0 = y1;                                  /* .. exchange them */
   y0 += (b+1)/2; y1 = y0-b1;                               /* starting pixel */
   a = 8*a*a; b1 = 8*b*b;

   for (;;) {                             /* approximate ed=sqrt(dx*dx+dy*dy) */
      i = std::min(dx,dy); ed = std::max(dx,dy);
      if (y0 == y1+1 && err > dy && a > b1) ed = 255*4./a;           /* x-tip */
      else ed = 255/(ed+2*ed*i*i/(4*ed*ed+i*i));             /* approximation */
      i = ed*fabs(err+dx-dy);           /* get intensity value by pixel error */
      setPixelAA(x0,y0, i); setPixelAA(x0,y1, i);
      setPixelAA(x1,y0, i); setPixelAA(x1,y1, i);

      if (f = 2*err+dy >= 0) {                  /* x step, remember condition */
         if (x0 >= x1) break;
         i = ed*(err+dx);
         if (i < 255) {
            setPixelAA(x0,y0+1, i); setPixelAA(x0,y1-1, i);
            setPixelAA(x1,y0+1, i); setPixelAA(x1,y1-1, i);
         }          /* do error increment later since values are still needed */
      }
      if (2*err <= dx) {                                            /* y step */
         i = ed*(dy-err);
         if (i < 255) {
            setPixelAA(x0+1,y0, i); setPixelAA(x1-1,y0, i);
            setPixelAA(x0+1,y1, i); setPixelAA(x1-1,y1, i);
         }
         y0++; y1--; err += dy += a;
      }
      if (f) { x0++; x1--; err -= dx -= b1; }            /* x error increment */
   }
   if (--x0 == x1++)                       /* too early stop of flat ellipses */
      while (y0-y1 < b) {
         i = 255*4*fabs(err+dx)/b1;               /* -> finish tip of ellipse */
         setPixelAA(x0,++y0, i); setPixelAA(x1,y0, i);
         setPixelAA(x0,--y1, i); setPixelAA(x1,y1, i);
         err += dy += a;
      }
   plotFilledEllipseRect(X0, Y0, X1, Y1);
}
