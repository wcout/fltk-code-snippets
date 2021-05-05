/*
	Demonstrate how to use SVG images created on the fly to
	draw (antialiased) circles/ellipses with or without color filling
	and with or without outline (of width and border color).

	Click in the window to toggle between SVG mode and FLTK standard drawing
	and test the difference in quality and speed. Resize the window to dynamically
	re-create the drawing.

	Speed of SVG rendering is of course way slower than FLTK drawing, but
	it seems fair enough for rendering things that don't change very often.

	Needs FLTK 1.4 with SVG support enabled.

	wcout 2018/03/15
*/
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include "svg_circle.h"
#include <cstdio>

static int USE_SVG = 1; // start in SVG mode

static void fltk_circle( int x_, int y_, int w_, int h_,
                         int width_ = 1,
                         Fl_Color color_ = FL_BLACK,
                         Fl_Color fill_color_ = Transparent )
{
	// try to draw (nearly) the same with FLTK's drawing routines
	int d = width_ > 0 ? ( width_ - 1 ) / 2 : 0;
	if ( fill_color_ != Transparent )
	{
		fl_color( fill_color_ );
		fl_pie( x_ + d, y_ + d, w_ - 2 * d, h_ - 2 *d, 0., 360. );
	}
	fl_line_style( FL_SOLID, width_ );
	fl_color( color_ );
	fl_arc( x_ + d, y_ + d, w_ - 2 * d, h_ - 2 *d, 0., 360. );
}

void circle( int x_, int y_, int w_, int h_,
             int width_ = 1,
             Fl_Color color_ = FL_BLACK,
             Fl_Color fill_color_ = Transparent )
{
	USE_SVG ? fl_svg_circle( x_, y_, w_, h_, width_, color_, fill_color_ ) :
             fltk_circle( x_, y_, w_, h_, width_, color_, fill_color_ );
}


// test program
class Drawing : public Fl_Box
{
public:
	Drawing( int x_, int y_, int w_, int h_ ) :
		Fl_Box( x_, y_, w_, h_)
	{
	}
	void draw()
	{
		Fl_Box::draw();

		// ellipse with line width 1 in black (default)
		circle( x(), y(), w(), h() );

		// filled ellipse with different colored outline of width 4
		circle( x() + 30, y() + 30, w() - 60, h() - 60, 4, FL_BLUE, FL_YELLOW );

		// filled ellipse with thick outline
		circle( x() + 60, y() + 60, w() - 120, h() - 120, 30, FL_RED, FL_GREEN );

		// draw many concentric circles (speed test)
		for ( int r = 2; r < h() / 4; r += 4 )
		{
			circle( x() + w() / 2 - r, y() +  h() / 2 - r, 2 * r, 2 * r, 1, FL_WHITE );
		}
	}
	int handle( int e_ )
	{
		// mouse click toggles between FLTK drawing and SVG drawing
		int ret = Fl_Box::handle( e_ );
		if ( e_ == FL_PUSH )
		{
			USE_SVG = !USE_SVG;
			window()->redraw();
		}
		return ret;
	}
};

int main( int argc, char **argv )
{
	Fl_Double_Window win( 480, 480, "svg circle" );
	Drawing d( 0, 0, win.w(), win.h() );
	win.end();
	win.resizable( &d );
	win.show( argc, argv );
	return Fl::run();
}
