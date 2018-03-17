/*
	Demonstrate how to use SVG images created on the fly to
	draw (antialiased) circles/ellipses with or without color filling
	and with or without outline (of width and border color).

	Click in the window to toggle between SVG mode and FLTK standard drawing
	and test the difference in quality and speed. Resize the window to dynamically
	re-create the drawing.

	Speed of SVG rendering is of course way slower than FLTK drawing, but
	it seems fair enough for rendering things that don't change very often.

	wcout 2018/03/15
*/
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_SVG_Image.H>
#include <FL/fl_draw.H>
#include <cstdio>

static int USE_SVG = 1; // start in SVG mode


static const Fl_Color Transparent = 0xffffffff; // give FLTK a definition for "transparent color"

// helpers
static Fl_SVG_Image *create_svg_circle( int x_, int y_, int w_, int h_,
                                        int stroke_w_,
                                        const char *stroke_color_,
                                        const char *fill_color_ = "none" )
{
	char buf[500];
	static const char *svg_circle_template =
		"<svg width=\"%d\" height=\"%d\">"
		"<ellipse cx=\"%d\" cy=\"%d\" rx=\"%d\" ry=\"%d\" stroke-width=\"%d\" stroke=\"%s\" fill=\"%s\"/>"
		"</svg>";
	snprintf( buf, sizeof(buf), svg_circle_template,
		       w_, h_, w_/ 2, h_/ 2,
	          w_ / 2 - stroke_w_, h_ / 2 - stroke_w_,
	          stroke_w_, stroke_color_, fill_color_ );
	return new Fl_SVG_Image( NULL, buf );
}

static Fl_SVG_Image *create_svg_circle( int x_, int y_, int w_, int h_,
	                                     int stroke_w_,
                                        Fl_Color stroke_color_,
                                        Fl_Color fill_color_ = Transparent )
{
	uchar sr, sg, sb;
	Fl::get_color( stroke_color_, sr, sg, sb );
	char c[50];
	char fc[50];
	snprintf( c, sizeof(c),"rgb(%d,%d,%d)", (int)sr, (int)sg, (int)sb );
	if ( fill_color_ != Transparent )
	{
		uchar fr, fg, fb;
		Fl::get_color( fill_color_, fr, fg, fb );
		snprintf( fc, sizeof(fc), "rgb(%d,%d,%d)", (int)fr, (int)fg, (int)fb );
	}
	return create_svg_circle( x_, y_, w_, h_, stroke_w_, c, fill_color_ == Transparent ? "none" : fc );
}



// FLTK interface hiding the implementation
void fl_svg_circle( int x_, int y_, int w_, int h_,
                    int width_ = 1,
                    Fl_Color color_ = FL_BLACK,
                    Fl_Color fill_color_ = Transparent )
{
	if ( w_ < 0 || h_ < 0 ) // Crashes Fl_SVG_Image::rasterize() otherwise
		return;
	if ( USE_SVG )
	{
		Fl_SVG_Image *c = create_svg_circle( x_, y_, w_, h_, width_, color_, fill_color_ );
		c->resize( w_, h_ ); // rasterize SVG
		c->draw( x_, y_ );
		delete c;
		return;
	}
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
		fl_svg_circle( x(), y(), w(), h() );

		// filled ellipse with different colored outline of width 4
		fl_svg_circle( x() + 30, y() + 30, w() - 60, h() - 60, 4, FL_BLUE, FL_YELLOW );

		// filled ellipse with thick outline
		fl_svg_circle( x() + 60, y() + 60, w() - 120, h() - 120, 30, FL_RED, FL_GREEN );

		// draw many concentric circles (speed test)
		for ( int r = 2; r < h() / 4; r += 4 )
		{
			fl_svg_circle( x() + w() / 2 - r, y() +  h() / 2 - r, 2 * r, 2 * r, 1, FL_WHITE );
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
