#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl.H>
#include <cstdio>

static void cb_swirl(void *d_)
{
	static int angle = 0;
	angle -= 14;
   if (angle < 0)
		angle += 360;
	Fl_Box *swirl = (Fl_Box *)d_;
//	printf("label: '%s'\n", swirl->label());
	char buf[30];
	snprintf(buf, sizeof(buf), "@00%03d%s", angle, &swirl->label()[6]);
	swirl->copy_label(buf);
//	swirl->labelcolor(angle);
	swirl->redraw();
	Fl::repeat_timeout(0.1, cb_swirl, swirl);
}

int main()
{
	Fl_Double_Window win(400, 400, "busy");
	Fl_Box swirl(50, 50, 300, 300, "@00360refresh");
	swirl.labelsize(200);
	swirl.labelcolor(FL_BLUE);
   swirl.deactivate();
   win.resizable(swirl);
	win.end();
	win.show();
	Fl::add_timeout(0.1, cb_swirl, &swirl);
	return Fl::run();
}