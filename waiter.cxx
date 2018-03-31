/*

  Test program of FLTK-Interface for a Hires-Timer under WIN32 platform.

  Updates a window with 256 FPS. After each 256 cycles a counter is
  incremented and displayed in the window. So it should change excactly
  every second.

  This program was tested on a real WIN7 machine and ran very precise with
  a frame rate of 256 Hz using about 25% CPU.

  wcout 2018/03/30

*/
#include <iostream>
#define LOG(x) { std::cout << x << std::endl; }
#include "Fl_Waiter.H"

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <cstdio>

static const unsigned int FPS = 256;

class MyWindow : public Fl_Double_Window
{
typedef Fl_Double_Window Inherited;
public:
	MyWindow( int w_, int h_, const char *l_ = 0 ) :
		Inherited( w_, h_, l_ ),
		_cnt( 0 ),
		_frames( 0 ),
		_paused( false )
	{
		color( fl_rgb_color( 0, 0, 0 ) ); // start with black bg color
		_box = new Fl_Box( 0, 0, w_, h_ );
		_box->labelcolor( FL_GREEN );
		_box->labeltype( FL_EMBOSSED_LABEL );
		end();
		setTitle();
	}
	void setTitle()
	{
		copy_label( _paused ? "paused - space continues" : "hit space to pause" );
	}
	virtual int handle( int e_ )
	{
		int ret = Inherited::handle( e_ );
		if ( e_ == FL_SHORTCUT && Fl::event_key() == ' ' )
		{
			_paused = !_paused;
			setTitle();
		}
		return ret;
	}
	virtual void update()
	{
		// check if in paused mode
		if ( _paused )
			return;

		// increment frame count
		_frames++;

		// just a demo update action: cycle gray-ramp of background color
		Fl_Color c = color();
		uchar r, g, b;
		Fl::get_color( c, r, g, b );
		r++;
		g++;
		b++;
		color( fl_rgb_color( r, g, b ) );

		if ( _frames == FPS ) // one cyle completed?
		{
			// display completed cycle count (=count seconds)
			_frames = 0;
			_cnt++;
			char buf[20];
			snprintf( buf, sizeof( buf ), "%d", _cnt );
			_box->labelsize( h() / 3 );
			_box->copy_label( buf );
		}
		// trigger redisplay of window contents
		redraw();
	}
private:
	Fl_Box *_box;
	int _cnt;
	int _frames;
	bool _paused;
};

int main()
{
	// create an instance of the waiter class
	static Fl_Waiter waiter;

	// tell waiter the frame rate to obey
	waiter.FPS( FPS );

	MyWindow win( 400, 400 );
	win.resizable( win );
	win.show();
	win.wait_for_expose();

	// enter the main loop (exits if window closed)
	while ( waiter.wait() )
	{
		win.update();
	}
	return 0;
}
