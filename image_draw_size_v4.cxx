#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Image_Surface.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <cstdlib>
#include <ctype.h>
#include <cassert>
#include <config.h>

static const char TITLE[] =
#ifdef FLTK_USE_CAIRO
	"Cairo"
#else
	"X11"
#endif
;

static const int CS = 20;
char INFO[50];
static int HiresFlag = 0;
static float ScreenScale = 4.;
static Fl_RGB_Scaling ScalingAlgorithm = FL_RGB_SCALING_BILINEAR;
static const char *FileName = 0;
static bool Desaturate = false;

class TestWin : public Fl_Double_Window {
public:
	TestWin() : Fl_Double_Window(CS*3,CS*3, INFO), _image(create_image()) { show(); }
	void draw() {
      printf("draw image %d x %d, d=%d, count=%d\n", _image->w(), _image->h(), _image->d(), _image->count());
		fl_rectf(0, 0, w(), h(), FL_YELLOW);
		fl_color(FL_BLUE); // for bitmaps!
		int ny = ((double)h() + 0.5) / _image->h();
		int nx = ((double)w() + 0.5) / _image->w();
		for ( int y = 0; y < ny; y++ )
			for ( int x = 0; x < nx; x++ )
				_image->draw(x*_image->w(), y*_image->h());
	}
	Fl_Image *create_image() {
		Fl_Image *image = 0;
      if ( FileName )
		{
         printf("Try open image '%s'\n", FileName);
			image = Fl_Shared_Image::get( FileName );
			if ( !image /*|| image->fail()*/ ) {
            printf("Failed to load '%s'\n", FileName);
				exit(EXIT_FAILURE);
         }
			else {
           printf("image size is %d x %d\n", image->w(), image->h());
           if (image->w() <= 0 || image->h() <= 0)
             exit(EXIT_FAILURE);
	   		else if ( image->w() > 400 || image->h() > 400 ) {
              fprintf(stderr,  "Image size too large, must be below 400x400.\n");
 //             exit(EXIT_FAILURE);
            }
           size(image->w() * 2, image->h() * 2);
         }
		}
      else {
   		Fl_Image_Surface *img_surf = new Fl_Image_Surface(CS, CS, HiresFlag);
	 		assert(img_surf);

			img_surf->set_current();

			fl_color(FL_RED);
			fl_rectf(0, 0, CS, CS);
			fl_line_style(FL_SOLID, 1);
 			fl_color(FL_CYAN);
			fl_line(0, 0, 0, CS);
			fl_line_style(0);

			fl_color(FL_BLUE);
			fl_rectf(1, 1, CS-2, CS-2);

			fl_color(FL_GREEN);
			fl_rectf(2, 2, CS-4, CS-4);

			image = img_surf->image();
			assert(image);
			delete img_surf;

			Fl_Display_Device::display_device()->set_current();
		}
      if (Desaturate) {
        printf("desaturate image\n");
        image->desaturate();
      }
		return image;
	}
private:
	Fl_Image *_image;
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
          case 'd': Desaturate = true; break;
        }
      }
    }
    else if (isdigit(argv_[i][0])) {
      double scale = atof(argv_[i]);
      if (scale >= .5 && scale <= 10.) ScreenScale = scale;
    }
    else {
      FileName = argv_[i];
    }
  }
}

int main(int argc_, char *argv_[]) {
   fl_register_images();
   parseArgs(argc_, argv_);
   bool bilinear = ScalingAlgorithm == FL_RGB_SCALING_BILINEAR;
   printf("ScreenScale: %f\n", ScreenScale);
   printf("Scaling    : %s\n", bilinear ? "bilinear" : "nearest");
   printf("HiresFlag  : %d\n", HiresFlag);
   printf("Desaturate : %d\n", Desaturate);
   snprintf(INFO, sizeof(INFO), "%s - %0.2f %c %d", TITLE, ScreenScale, bilinear ? 'B' : 'N', HiresFlag);
	Fl::screen_scale(0, ScreenScale);
	Fl_RGB_Image::scaling_algorithm(ScalingAlgorithm);
	TestWin win;
	return Fl::run();
}
