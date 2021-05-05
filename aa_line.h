#ifndef FLTK_AA_LINE_H
#define FLTK_AA_LINE_H

#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <cassert>

// forward declaration of functions to be implemented by application
static void setPixelAA( int x, int y, unsigned char alpha );
static void setPixel( int x, int y );

#include "contrib/bresenham.c"
#include "contrib/bresenham_ext.c"


// FLTK interface
#include <FL/fl_draw.H>
#include <FL/Fl.H>

static uchar *OffScreenBuf = 0;
static int W = 0;
static int H = 0;


static void setPixelAA( int x, int y, unsigned char alpha )
{
	// draw a pixel in current color at x/y with alpha value
	if ( OffScreenBuf )
	{
		if ( x <  0 || y < 0 || x >= W || y >= H )
			return;
		uchar *pixel = OffScreenBuf + y * 4 * W + x * 4;
		Fl::get_color( fl_color(), pixel[0], pixel[1], pixel[2] );
		pixel[3] = 255 - alpha;
	}
}

static void setPixel( int x, int y )
{
	// draw a pixel without alpha
	setPixelAA( x, y, 0 );
}


static void fl_begin_aa( int w_, int h_ )
{
	OffScreenBuf = new uchar[w_ * h_ * 4];
	W = w_;
	H = h_;
	memset( OffScreenBuf, 0 , W * H * 4 );
}

static void fl_end_aa( int x_ = 0, int y_ = 0 )
{
	if ( !OffScreenBuf )
		return;
	Fl_RGB_Image *rgb = new Fl_RGB_Image( OffScreenBuf, W, H, 4 );
	if ( rgb )
		rgb->draw( x_, y_ );
	delete[] OffScreenBuf;
	OffScreenBuf = 0;
	delete rgb;
}

static void fl_line_aa( int x0_, int y0_, int x1_, int y1_, float width_ = 1. )
{
	width_ == 1. ? plotLineAA( x0_, y0_, x1_, y1_ ) :
	               plotLineWidth( x0_, y0_, x1_, y1_, width_ );
}

static void fl_circle_aa( int x_, int y_, int r_ )
{
	plotCircleAA( x_, y_, r_ );
}

static void fl_circle_aa( int x_, int y_, int w_, int h_ )
{
	w_ == h_ ? plotCircleAA( x_ + w_ / 2, y_ + h_ / 2, w_ / 2 ) :
	           plotEllipseRectAA( x_, y_, x_ +  w_ - 1, y_ + h_ - 1 );
}

static void fl_pie_aa( int x_, int y_, int r_ )
{
	plotFilledCircleAA( x_, y_, r_ );
}

static void fl_pie_aa( int x_, int y_, int w_, int h_ )
{
	w_ == h_ ? plotFilledCircleAA( x_ + w_ / 2, y_ + h_ / 2, w_ / 2 ) :
	           plotFilledEllipseRectAA( x_, y_, x_ + w_ - 1, y_ + h_ - 1 );
}

#endif
