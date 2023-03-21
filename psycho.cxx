#include <FL/Fl_Double_Window.H>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <cstdlib>

int FPS = 800;

class PsychoWin : public Fl_Double_Window
{
	typedef Fl_Double_Window Inherited;
public:
	PsychoWin(int mode_) : Inherited(800, 600), _mode(mode_),_frame(0)
	{
		resizable(this);
		show();
		Fl::add_timeout(0.1, cb_update, this);
	}
	void draw()
	{
		static const int LW = 5;

		fl_line_style(FL_SOLID,LW);

		if (_mode == 1)
		{
			fl_color(fl_rgb_color(random()%256, random()%256, random()%256));
			fl_circle(0, 0, (_frame*LW)%(w()+h()));

			fl_color(fl_rgb_color(random()%256, random()%256, random()%256));
			fl_circle(w(), h(), (_frame*LW)%(w()+h()));

			fl_color(fl_rgb_color(random()%256, random()%256, random()%256));
			fl_circle(w(), 0, (_frame*LW)%(w()+h()));

			fl_color(fl_rgb_color(random()%256, random()%256, random()%256));
			fl_circle(0, h(), (_frame*LW)%(w()+h()));

			fl_color(fl_rgb_color(random()%256, random()%256, random()%256));
			fl_circle(w()/2, h()/2, (_frame*LW)%(w()/2+h()/2));
		}
		else
		{
			int r=(_frame*LW)%(w()+h());
			fl_color(fl_rgb_color(random()%256, random()%256, random()%256));
			fl_rect(0-r/2, 0-r/2, r,r);

			fl_color(fl_rgb_color(random()%256, random()%256, random()%256));
			fl_rect(w()-r/2, h()-r/2, r,r);

			fl_color(fl_rgb_color(random()%256, random()%256, random()%256));
			fl_rect(w()-r/2, 0-r/2, r, r);

			fl_color(fl_rgb_color(random()%256, random()%256, random()%256));
			fl_rect(0-r/2, h()-r/2, r,r);

			fl_color(fl_rgb_color(random()%256, random()%256, random()%256));
			fl_rect(w()/2-r/2, h()/2-r/2, r,r);
		}
	}
	void onUpdate()
	{
		_frame++;
		redraw();
	}
	static void cb_update(void *d_)
	{
		Fl::repeat_timeout(1./FPS, cb_update, d_);
		((PsychoWin *)d_)->onUpdate();
	}
private:
	int _mode;
	unsigned _frame;
};


int main(int argc_, char *argv_[])
{
	PsychoWin win(argc_ > 1 ? atoi(argv_[1]) : 1);
	return Fl::run();
}
