/*
	Demonstrate how to draw fast antialiased lines, circles and ellipses
	using the bresenham algorithm.

	The FLTK interface hides the implementation, which 'draws' points
	into a memory buffer using transparency. The result is finally
	drawn by FLTK as alpha image.

	wcout 2018/03/15
*/

// FLTK interface
#include "aa_line.h"

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
