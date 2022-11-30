#include <FL/Fl_Double_Window.H>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <cstdio>
#include <cstdlib>

#include <chrono>

#include <config.h>
static const char TITLE[] =
#ifdef FLTK_USE_CAIRO
	"Cairo"
#else
	"X11"
#endif
;

static int Mode = 1;

class App : public Fl_Double_Window
{
public:
	App(int w, int h, const char *l = 0) :
		Fl_Double_Window(w, h, l)
	{
		color(FL_BLACK);
		resizable(this);
	}
	void draw_circles()
	{
		static const int sep = 2;
		int R = w() - w() / 3;
		fl_color(FL_RED);
		for (int r = sep; r < R; r += sep, _N++)
			fl_circle(0, 0, r);

		fl_color(FL_YELLOW);
		for (int r = sep; r < R; r += sep, _N++)
			fl_circle(w(), 0, r);

		fl_color(FL_BLUE);
		for (int r = sep; r < R; r += sep, _N++)
			fl_circle(0, h(), r);

		fl_color(FL_GREEN);
		for (int r = sep; r < R; r += sep, _N++)
			fl_circle(w(), h(), r);
	}
	void draw_hv_lines()
	{
		static const int sep = 6;
		fl_color(FL_RED);
		for (int x = 0; x < w() + 1; x += sep, _N++)
			fl_line(x, 0, x, h());

		fl_color(FL_YELLOW);
		for (int x = 0; x < h() + 1; x += sep, _N++)
			fl_line(0, x, w(), x);

		fl_color(FL_BLUE);
		for (int x = sep / 2; x < w() + 1; x += sep, _N++)
			fl_line(x, 0, x, h());

		fl_color(FL_GREEN);
		for (int x = sep / 2; x < h() + 1; x += sep, _N++)
			fl_line(0, x, w(), x);

	}
	void draw_lines()
	{
		static const int sep = 10;
		fl_color(FL_RED);
		for (int x = 0; x < w() + 1; x += sep, _N++)
			fl_line(x, h(), w(), h() - x);

		fl_color(FL_YELLOW);
		for (int x = 0; x < h() + 1; x += sep, _N++)
			fl_line(0, x, w() - x, 0);

		fl_color(FL_BLUE);
		for (int x = 0; x < w() + 1; x += sep, _N++)
			fl_line(x, 0, w(), x);

		fl_color(FL_GREEN);
		for (int x = 0; x < h() + 1; x += sep, _N++)
			fl_line(x, w(), 0, x);

	}
	void draw()
	{
		static int count = 0;

		count++;
		if (count >1)
			printf("recursion %d\n", count);
		Fl_Double_Window::draw();

		_N = 0;
		_W = w();
		_H = h();
		Fl::remove_timeout(cb_measure, this);
		Fl::add_timeout(0.01, cb_measure, this);

		_start = std::chrono::system_clock::now();

		switch (Mode)
		{
			case 1:	draw_circles(); break;
			case 2:	draw_lines(); break;
			default:	draw_hv_lines();
		}

		fl_color(FL_WHITE); // white center pixel for measurement
		fl_point(w()/2, h()/2);

		count--;
	}
	bool onMeasure()
	{
		// wait till white center pixel is there...
		bool success = (w() == _W && h() == _H);
		if (success)
		{
			const uchar *screen = fl_read_image( 0, 0, 0, w(), h());
			success = screen != 0;
			if (success)
			{
				const int d = 3;
				int X = w() / 2;
				int Y = h() / 2;
				long index = Y * w() * d + X * d;
				unsigned char r = *(screen + index++);
				unsigned char g = *(screen + index++);
				unsigned char b = *(screen + index++);
				delete[] screen;
				success = (r == 255 && g == 255 && b == 255);
			}
		}
		if (!success) printf(".");
		else printf("\n");
		fflush(stdout);
		return success;
	}
	static void cb_measure(void *d_)
	{
		App *app = (App *)d_;
		if (!app->onMeasure())
			Fl::repeat_timeout(0.01, cb_measure, app);
		else
		{
			// drawing finished
			app->_end = std::chrono::system_clock::now();
			std::chrono::duration<double> diff = app->_end - app->_start;
			printf("Time to draw %d objects in %d x %d: %.9f\n",
				app->_N, app->w(), app->h(), diff.count());
		}
	}
private:
	int _W;
	int _H;
	int _N;
	std::chrono::time_point<std::chrono::system_clock> _start;
	std::chrono::time_point<std::chrono::system_clock> _end;
};

int main(int argc, char *argv[])
{
	if (argc > 1) Mode = atoi(argv[1]);
	printf("Mode: %d\n", Mode);
	App app(500, 500, TITLE);
	app.show();
	return Fl::run();
}
