/*
	Demonstrate how to draw fast antialiased lines, circles and ellipses
	using the bresenham algorithm.

	The FLTK interface hides the implementation, which 'draws' points
	into a memory buffer using transparency. The result is finally
	drawn by FLTK as alpha image.

	wcout 2018/03/25
*/
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

static uchar *Screen = 0;
static int W = 0;
static int H = 0;


static void setPixelAA( int x, int y, unsigned char alpha )
{
	// draw a pixel in current color at x/y with alpha value
	if ( Screen )
	{
		if ( x <  0 || y < 0 || x >= W || y >= H )
			return;
		uchar *pixel = Screen + y * 4 * W + x * 4;
		Fl::get_color( fl_color(), pixel[0], pixel[1], pixel[2] );
		pixel[3] = 255 - alpha;
	}
}

static void setPixel( int x, int y )
{
	// draw a pixel without alpha
	setPixelAA( x, y, 0 );
}


void fl_begin_aa( int w_, int h_ )
{
	Screen = new uchar[w_ * h_ * 4];
	W = w_;
	H = h_;
	memset( Screen, 0 , W * H * 4 );
}

void fl_end_aa( int x_ = 0, int y_ = 0 )
{
	if ( !Screen )
		return;
	Fl_RGB_Image *rgb = new Fl_RGB_Image( Screen, W, H, 4 );
	rgb->draw( x_, y_ );
	delete[] Screen;
	Screen = 0;
}

void fl_line_aa( int x0_, int y0_, int x1_, int y1_, float width_ = 1. )
{
	width_ == 1. ? plotLineAA( x0_, y0_, x1_, y1_ ) :
	               plotLineWidth( x0_, y0_, x1_, y1_, width_ );
}

void fl_circle_aa( int x_, int y_, int r_ )
{
	plotCircleAA( x_, y_, r_ );
}

void fl_circle_aa( int x_, int y_, int w_, int h_ )
{
	w_ == h_ ? plotCircleAA( x_ + w_ / 2, y_ + h_ / 2, w_ / 2 ) :
	           plotEllipseRectAA( x_, y_, x_ +  w_ - 1, y_ + h_ - 1 );
}

void fl_pie_aa( int x_, int y_, int r_ )
{
	plotFilledCircleAA( x_, y_, r_ );
}

void fl_pie_aa( int x_, int y_, int w_, int h_ )
{
	w_ == h_ ? plotFilledCircleAA( x_ + w_ / 2, y_ + h_ / 2, w_ / 2 ) :
	           plotFilledEllipseRectAA( x_, y_, x_ + w_ - 1, y_ + h_ - 1 );
}


// demo program
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_RGB_Image.H>

static const Fl_Color BgColor = FL_WHITE;


class MyWindow : public Fl_Double_Window
{
typedef Fl_Double_Window Inherited;
public:
	MyWindow( int w_, int h_, const char *l_ = 0 ) :
		Fl_Double_Window( w_, h_, l_ )
	{
	}
	void draw()
	{
		// set desired background color
		fl_rectf( 0, 0, w(), h(), BgColor );

		// start aa drawing (setup buffer)
		fl_begin_aa( w(), h() );

		// 1px lines
		fl_color( FL_RED );
		fl_line_aa( 0, 0, w(), h());
		fl_color( FL_YELLOW );
		fl_line_aa( 0, 0, w(), h() / 2 );
		fl_color( FL_BLUE );
		fl_line_aa( 0, 0, w(), h() / 4 );
		fl_color( FL_CYAN );
		fl_line_aa( 0, 0, w(), h() / 8 );

		// line with width
		fl_color( FL_GREEN );
		fl_line_aa( 0, 0, w(), h() / 3, 8 );

		// circles (speed test)
		fl_color( FL_RED );
		for ( int r = 1; r < h() / 2; r += 3 )
		{
			fl_circle_aa( w() / 2, h() / 2, r );
		}

		// ellipse
		fl_color( FL_MAGENTA );
		fl_circle_aa( 0, 0, w(), h() );

		// filled circle
		fl_color( FL_GRAY );
		fl_pie_aa( w() / 3, h() / 3, h() / 4 );

		// filled ellipse
		fl_color( FL_BLUE );
		fl_pie_aa( w() / 5, h() - h() / 3,  w() / 4, h() / 5 );

		// end aa drawing (draw buffer)
		fl_end_aa();
	}
};

int main()
{
	MyWindow win( 600, 600, "antialiasing" );
	win.resizable( win );
	win.end();
	win.show();
   return Fl::run();
}
