//
//  Test program for Fl_Image::scale().
//
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <cstdio>
#include <cstdlib>

static Fl_Image *orig = 0;
static bool draw_grid = true;
static bool proportional = true;

class Canvas : public Fl_Box {
	typedef Fl_Box Inherited;
public:
	Canvas(int x_, int y_, int w_, int h_) :
		Inherited(x_, y_, w_, h_)
	{
		if (!draw_grid)
			box(FL_FLAT_BOX);
	}
	~Canvas()
	{
		Fl::remove_timeout(do_resize_cb, this);
	}
	virtual void draw()
	{
		if (draw_grid)
		{
			// draw a transparency grid as background
			static const Fl_Color C1 = fl_rgb_color(0xcc, 0xcc, 0xcc);
			static const Fl_Color C2 = fl_rgb_color(0x88, 0x88, 0x88);
			static const int SZ = 8;
			for (int y = 0; y < h(); y += SZ)
			{
				for (int x = 0; x < w(); x += SZ)
				{
					fl_color(x % (SZ * 2) ? y % (SZ * 2) ? C1 : C2 : y % (SZ * 2) ? C2 : C1);
					fl_rectf(x, y, 32, 32);
				}
			}
		}
		// draw the current image frame over the grid
		Inherited::draw();
	}
	void do_resize(int W_, int H_)
	{
		image()->scale(W_, H_, proportional, 1);
      printf("resized to %d x %d\n", image()->w(), image()->h());
		window()->cursor(FL_CURSOR_DEFAULT);
		parent()->redraw();
	} // do_resize
	static void do_resize_cb(void *d_)
	{
		Canvas *c = (Canvas *)d_;
		c->do_resize(c->w(), c->h());
	}
	virtual void resize(int x_, int y_, int w_, int h_)
	{
		Inherited::resize(x_, y_, w_, h_);
		// decouple resize event from actual resize operation
		// to avoid lockups..
		Fl::remove_timeout(do_resize_cb, this);
		Fl::add_timeout(0.1, do_resize_cb, this);
		window()->cursor(FL_CURSOR_WAIT);
	}
};

int main(int argc_, char *argv_[])
{
	fl_register_images();
	Fl_File_Icon::load_system_icons();
	// setup play parameters from args
	int bilinear = false;
	const char *fileName = 0;
	for (int i = 1; i < argc_; i++)
	{
		if (!strcmp(argv_[i], "-g")) // disable grid
			draw_grid = false;
		else if (!strcmp(argv_[i], "-b")) // enable bilinear scaling
			bilinear = true;
		else if (!strcmp(argv_[i], "-p")) // disable proportional scaling
			proportional = false;
		else if (argv_[i][0] != '-' && !fileName)
		{
			fileName = argv_[i];
		}
		else
		{
			fprintf(stderr, "Test program for Fl_Image scale with filter.\n");
			fprintf(stderr, "Usage: %s fileName [-b] [-g] [-p]\n", argv_[0]);
			exit(0);
		}
	}

	Fl_RGB_Image::scaling_algorithm(bilinear ? FL_RGB_SCALING_BILINEAR : FL_RGB_SCALING_NEAREST);
	Fl_RGB_Image::RGB_scaling(Fl_RGB_Image::scaling_algorithm());
	printf("Using '%s' scaling!\n", bilinear ? "bilinear" : "nearest" );

	while (1)
	{
		if (!fileName)
			fileName = fl_file_chooser(
				"Image file?", "*.{bm,bmp,gif,jpg,pbm,pgm,png,ppm,xbm,xpm"
				",svg"
				"}", NULL);
		if (!fileName)
			break;

		Fl_Double_Window win(640, 480);

		// prepare a container for the image
		Canvas canvas(0, 0, win.w(), win.h());
		win.resizable(win);

		win.end();
		win.show();

		// create/load the image
		orig = Fl_Shared_Image::get(/*name_=*/ fileName);

		// check if loading succeeded
		if (!orig /* || orig->fail()*/)
		{
			fprintf(stderr, "Failed to load image '%s': %d\n", fileName, orig ? orig->ld() : -1);
			exit(0);
		}

		canvas.image(orig);
		win.copy_label(fileName);

		// set initial size to fit into window
		double ratio = (double)orig->w() / orig->h();
		int W = win.w() - 40;
		int H = (double)W / ratio;
		printf("%s: original size: %d x %d\n", fileName, orig->w(), orig->h());
		win.size(W, H);

		Fl::run();
		fileName = 0;
	}
} // main
