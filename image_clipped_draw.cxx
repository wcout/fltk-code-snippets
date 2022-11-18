#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Image_Surface.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <cstdlib>
#include <cassert>
#include <config.h>

static const char TITLE[] =
#ifdef FLTK_USE_CAIRO
	"Cairo"
#else
	"X11"
#endif
;

static const int CS = 200;
char INFO[50];
static int HiresFlag = 0;
static float ScreenScale = 3.;
static Fl_RGB_Scaling ScalingAlgorithm = FL_RGB_SCALING_BILINEAR;

class TestWin : public Fl_Double_Window {
public:
	TestWin() : Fl_Double_Window(CS+120,CS+120, INFO), _image(create_image()) { show(); }
	void draw() {
		fl_rectf(0, 0, w(), h(), FL_WHITE);
		_image->draw(100, 100, 100, 100, 50, 50 );
	}
	Fl_RGB_Image *create_image() {
		Fl_Image_Surface *img_surf = new Fl_Image_Surface(CS, CS, HiresFlag);
		assert(img_surf);

		img_surf->set_current();

		fl_color(FL_RED);
		fl_rectf(0, 0, CS, CS);

		fl_color(FL_BLUE);
		fl_rectf(50, 50, 100, 100);

		Fl_RGB_Image *image = image = img_surf->image();
		assert(image);
		delete img_surf;

		Fl_Display_Device::display_device()->set_current();
		return image;
	}
private:
	Fl_RGB_Image *_image;
};

void parseArgs(int argc_, char *argv_[]) {
  for (int i = 1; i < argc_; i++ ) {
    if (argv_[i][0] == '-' ) {
        for (int j=1; j < strlen(argv_[i]); j++) {
        switch (argv_[i][j]) {
          case 'b': ScalingAlgorithm = FL_RGB_SCALING_BILINEAR; break;
          case 'n': ScalingAlgorithm = FL_RGB_SCALING_NEAREST; break;
          case 'h': HiresFlag = 1; break;
          case 'l': HiresFlag = 0; break;
        }
      }
    }
    else {
      double scale = atof(argv_[i]);
      if (scale >= .5 && scale <= 10.) ScreenScale = scale;
    }
  }
}

int main(int argc_, char *argv_[]) {
   parseArgs(argc_, argv_);
   bool bilinear = ScalingAlgorithm == FL_RGB_SCALING_BILINEAR;
   printf("ScreenScale: %f\n", ScreenScale);
   printf("Scaling    : %s\n", bilinear ? "bilinear" : "nearest");
   printf("HiresFlag  : %d\n", HiresFlag);
   snprintf(INFO, sizeof(INFO), "%s - %0.2f %c %d", TITLE, ScreenScale, bilinear ? 'B' : 'N', HiresFlag);
	Fl::screen_scale(0, ScreenScale);
	Fl_RGB_Image::scaling_algorithm(ScalingAlgorithm);
	TestWin win;
	return Fl::run();
}
