/*

	Test usage of SVG images to draw buttons with
	gradient/roundness/borderwidth.

	SVG images are created "on the fly".

	Needs FLTK 1.4 with SVG support enabled.

	wcout 2018/03/15

*/
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_SVG_Image.H>
#include <FL/fl_draw.H>
#include <FL/Fl.H>

#include <string>
#include <fstream>
#include <sstream>
#include <cmath>	// lround()

using namespace std;


// helpers
enum GRADIENT_TYPE
{
	NONE = 0,
	RADIAL = 1,
	LEFT_RIGHT = 2,
	TOP_BOTTOM = 3,
	DIAGONAL = 4
};

struct Style
{
	Fl_Color color;
	Fl_Color textColor;
	int borderWidth;
	Fl_Color borderColor;
	int roundness;
	int gradient;
	Style()
	{
		init();
	}
	void init()
	{
		color = FL_BACKGROUND_COLOR;
		textColor = FL_FOREGROUND_COLOR;
		borderWidth = 1;
		borderColor = FL_BLACK;
		roundness = 10;
		gradient = NONE;
	}
};

static std::string create_svg( int w_, int h_, const Style& style_, bool down_ = false, double opacity_ = 1.0 )
{
	std::ostringstream os;
	os << "<svg height=\"" << h_ << "\" width=\"" << w_ << "\" >";
	Fl_Color c = style_.color;
	Fl_Color from = down_ ? fl_darker( fl_darker( c ) ) : fl_lighter( fl_lighter( c ) );
	Fl_Color to = down_ ? fl_lighter( fl_lighter( c ) ) : fl_darker( fl_darker( c ) );
	uchar from_r, from_g, from_b;
	Fl::get_color( from, from_r, from_g, from_b );
	uchar to_r, to_g, to_b;
	Fl::get_color( to, to_r, to_g, to_b );
	uchar bd_r, bd_g, bd_b;
	c = down_ ? fl_darker( style_.borderColor ) : style_.borderColor;
	Fl::get_color( c, bd_r, bd_g, bd_b );
	int roundness = style_.roundness < 0 ? w_ / 2 : style_.roundness;

	int d = style_.borderWidth ? (style_.borderWidth+1)/2 : 0;
	std::ostringstream fill;
	if ( style_.gradient )
	{
		fill << "url(#grad1)";
		switch ( style_.gradient )
		{
			case RADIAL:
				os << "<defs><radialGradient id=\"grad1\" x1=\"50%\" y1=\"50%\" x2=\"100%\" y2=\"100%\">\n";
				break;
			case DIAGONAL:
				os << "<defs><linearGradient id=\"grad1\" x1=\"0%\" y1=\"0%\" x2=\"100%\" y2=\"100%\">\n";
				break;
			case TOP_BOTTOM:
				os << "<defs><linearGradient id=\"grad1\" x1=\"50%\" y1=\"0%\" x2=\"50%\" y2=\"100%\">\n";
				break;
			case LEFT_RIGHT:
			default:
				os << "<defs><linearGradient id=\"grad1\" x1=\"0%\" y1=\"50%\" x2=\"100%\" y2=\"50%\">\n";
				break;
		}
		os <<
			"<stop offset=\"0%\" style=\"stop-color:rgb("<<(int)from_r<<","<<(int)from_g<<","<<(int)from_b<<");stop-opacity:"<<opacity_<<"\" />\n"
			"<stop offset=\"100%\" style=\"stop-color:rgb("<<(int)to_r<<","<<(int)to_g<<","<<(int)to_b<<");stop-opacity:"<<opacity_<<"\" />\n";
		switch ( style_.gradient )
		{
			case RADIAL: os << "</radialGradient></defs>\n"; break;
			default: os << "</linearGradient></defs>\n"; break;
		}
	}
	else
	{
		uchar r, g, b;
		Fl::get_color( style_.color, r, g, b );
		fill << "rgb(" << (int)r<<","<<(int)g<<","<<(int)b<<")";
	}
	os <<
		"<rect x=\""<<d<<"\" y=\""<<d<<"\" rx=\""<<roundness<<"\" width=\""
		<<w_-d*2<<"\" height=\""<<h_-d*2<<"\" fill=\""<< fill.str() << "\" "
		"style=\"fill:"<<fill.str()<<";stroke:rgb("<<(int)bd_r<<","<<(int)bd_g<<","<<(int)bd_b<<");stroke-width:"<<style_.borderWidth<<";\" />\n"
		"</svg>\n";
	return  os.str();
}


// FLTK interface
class SVG_Button : public Fl_Button
{
typedef Fl_Button Inherited;
public:
	SVG_Button( int x_, int y_, int w_, int h_, const char *l_ = 0 ) :
		Inherited( x_, y_, w_, h_, l_ ),
		_up( 0 ),
		_uphi( 0 ),
		_down( 0 ),
		_image( 0 )
	{
		labelsize( h_ / 2 );
		callback( cb, this );
		init_images();
	}
	static void cb( Fl_Widget *wgt_, void *d_ )
	{
		printf( "%p clicked!\n", d_ );
	}
	bool inside() const
	{
		// checks if the last event occured on top of the image
		// i.e. an opaque part of the image data.
		int dx = Fl::event_x() - x();
		int dy = Fl::event_y() - y();
		return ( dx >= 0 && dy >= 0 && dx < w() && dy < h() &&
				   ( _image && _image->array[w() * dy * 4 + dx * 4 + 3] ) );
	}
	virtual int handle( int e_ )
	{
		if ( ( e_ == FL_PUSH || e_ == FL_RELEASE ) && !inside() )
			return 1;
		int ret = Inherited::handle( e_ );
		if ( e_ == FL_ENTER || e_ == FL_MOVE )
		{
			if ( inside() )
			{
				if ( _image == _up )
				{
					_image = _uphi; // show mouse over effect
					redraw();
				}
			}
			else
			{
				if ( _image == _uphi )
				{
					_image = _up;
					redraw();
				}
			}
		}
		else if ( e_ == FL_LEAVE || e_ == FL_PUSH )
		{
			if ( inside() || e_ == FL_LEAVE )
			{
				_image = _up;
				redraw();
			}
		}
		return ret;
	}
	void init_images()
	{
		delete _up;
		delete _uphi;
		delete _down;
		_up = _down = _uphi = 0;

		string up_data = create_svg( w(), h(), style() );
		string down_data = create_svg( w(), h(), style(), true );
#if 0
		// dump svg image to file
		ofstream ofs("xxxx.svg");
		ofs << up_data;
		ofs.close();
#endif
		_up = new Fl_SVG_Image( 0, up_data.c_str() );
		_up->proportional = false;
		_down = new Fl_SVG_Image( 0, down_data.c_str() );
		_down->proportional = false;
		_down->color_average( FL_BLACK, 0.8 );
		_uphi = (Fl_SVG_Image *)_up->copy();
		_uphi->color_average( FL_WHITE, 0.8 );
		_image = _up;
	}
	virtual void resize( int x_, int y_, int w_, int h_ )
	{
		if ( !_up || ( _up && (  _up->w() != w_ || _up->h() != h_ ) ) )
		{
			double f = (double)h_ / h();
			labelsize( lround( (double)labelsize() * f ) );
			init_images();
		}
		Inherited::resize( x_, y_, w_, h_ );
	}
	virtual void draw()
	{
//		fl_rectf( x(), y(), w(), h(), parent()->color() ); // HACK: get rid of minor artefacts on edges of SVG's
		if ( value() )
		{
			_down->resize( w(), h() );
			_down->draw( x(), y() );
		}
		else
		{
			_image->resize( w(), h() );
			_image->draw( x(), y() );
		}
		draw_label( x() + value(), y() + value(), w(), h() );
	}
	void style( const Style& s_ )
	{
		_style = s_;
		labelcolor( _style.textColor );
		init_images();
	}
	const Style& style() const
	{
		return _style;
	}
private:
	Fl_SVG_Image *_up;
	Fl_SVG_Image *_uphi;
	Fl_SVG_Image *_down;
	Fl_SVG_Image *_image;
	Style _style;
};

// test program
int main()
{
	Fl::scheme( "plastic" ); // used, because it creates a window background
	                         // to watch out for outline drawing artefacts

	Fl_Window win( 500, 300, "SVG Button" );
	win.color( FL_CYAN );

	Style s;
	s.gradient = RADIAL;
	s.color = FL_BLUE;
	s.textColor = FL_WHITE;
	SVG_Button b( 10, 10, 130, 50, "TEST" );
	b.style(s);

	SVG_Button b2( 300, 150, 130, 130, "@>" );
	s.color = FL_WHITE;
	s.textColor = FL_GRAY;
	s.borderWidth = 3;
	s.borderColor = FL_RED;
	b2.style(s);

	SVG_Button b3( 10, 150, 130, 130, "@<" );
	s.color = FL_GREEN;
	s.textColor = FL_GREEN;
	s.borderWidth = 0;
	b3.style(s);

	SVG_Button b4( 160, 90, 120, 120, "@search" );
	s.color = FL_YELLOW;
	s.textColor = FL_RED;
	s.borderColor = FL_GREEN;
	s.borderWidth = 7;
	s.roundness = -1;	// -1 means: round
	b4.style(s);

	SVG_Button b5( 300, 10, 130, 50, "TEST" );
	s.init();
	s.gradient = TOP_BOTTOM;
	s.borderWidth = 4;
	s.borderColor = 0x30303000;
	s.roundness = 0;
	b5.style(s);

	win.end();
	win.resizable( win );
	win.show();
	return Fl::run();
}
