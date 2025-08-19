#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <FL/platform.H>
#include <cstdio>

class AppWindow : public Fl_Double_Window {
typedef Fl_Double_Window Inherited;
public:
AppWindow(int w_, int h_, const char *text_ = 0) :
	Inherited(w_, h_, "colortext"), _text( text_) {
	int W = 0;
	int H = 0;
	fl_open_display();
	fl_font(FL_HELVETICA_BOLD_ITALIC, h()/3);
	fl_measure(_text, W, H, 1);
	size(W+20, H+20);
	}
	void draw();
private:
	const char *_text;
};

void AppWindow::draw() {
	static int cnt = 0;
	static bool vertical = false;
	fl_rectf(0, 0, w(), h(), FL_BLACK);
	fl_font(FL_COURIER, 16);
	char buf[100];
	fl_color(FL_WHITE);
	sprintf(buf, "%d", cnt++);
	fl_draw(buf, 10, 30);
	fl_draw( "Hit 'space' to toggle mode", 0, h()-30, w(), 30, FL_ALIGN_CENTER);
	fl_font(FL_HELVETICA_BOLD_ITALIC, h()/3);
	int dy = h() / 100;
	int dx = w() / 100;
	fl_color(FL_GRAY);
	fl_draw(_text, 10, h()/2+4, w(), h()/6, FL_ALIGN_CENTER);
	if (Fl::event_key( 'v'))
		vertical = true;
	if (Fl::event_key( 'h'))
		vertical = false;
	if (Fl::event_key( ' '))
		vertical = !vertical;

	if (vertical) {
	for (int x = 0; x < w(); x += dx)
	{
		fl_push_clip(x, 0, x + dy, h());
		fl_color(Fl_Color(random() % 6  + 1));
		fl_draw(_text, 10, h()/2, w(), h()/6, FL_ALIGN_CENTER);
		fl_pop_clip();
		}
	}
	else {
	for (int y = 0; y < h(); y += dy)
	{
		fl_push_clip(0, y, w(), y + dy);
		fl_color( Fl_Color(random() % 3  + 1));
		fl_draw(_text, 10, h()/2, w(), h()/6, FL_ALIGN_CENTER);
		fl_pop_clip();
		}
	}
}

void cb_update(void *w_) {
	AppWindow *win = (AppWindow *)w_;
	Fl::repeat_timeout(0.05, cb_update, w_);
	win->redraw();
}

#include <string>
int main( int argc_, char *argv_[] ) {
	std::string text = "Hello FLTK!";
	if ( argc_ > 1 )
		text = argv_[1];
	if (text.size() > 30)
		text = text.substr(30);
	AppWindow win(400, 400, text.c_str());
	win.resizable(&win);
	win.show();
	Fl::add_timeout(0.1, cb_update, &win);
	return Fl::run();
}
