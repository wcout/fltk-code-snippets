#ifndef FLTK_SVG_CIRCLE_H
#define FLTK_SVG_CIRCLE_H

#include <FL/Fl.H>
#include <FL/Fl_SVG_Image.H>
#include <FL/fl_draw.H>
#include <cstdio>

static const Fl_Color Transparent = 0xffffffff; // give FLTK a definition for "transparent color"

// helpers
static Fl_SVG_Image *create_svg_circle( int x_, int y_, int w_, int h_,
                                        int stroke_w_,
                                        const char *stroke_color_,
                                        const char *fill_color_ = "none" )
{
	char buf[500];
	static const char *svg_circle_template =
		"<svg width=\"%d\" height=\"%d\">"
		"<ellipse cx=\"%d\" cy=\"%d\" rx=\"%d\" ry=\"%d\" stroke-width=\"%d\" stroke=\"%s\" fill=\"%s\"/>"
		"</svg>";
	snprintf( buf, sizeof(buf), svg_circle_template,
		       w_, h_, w_/ 2, h_/ 2,
	          w_ / 2 - stroke_w_, h_ / 2 - stroke_w_,
	          stroke_w_, stroke_color_, fill_color_ );
	return new Fl_SVG_Image( NULL, buf );
}

static Fl_SVG_Image *create_svg_circle( int x_, int y_, int w_, int h_,
	                                     int stroke_w_,
                                        Fl_Color stroke_color_,
                                        Fl_Color fill_color_ = Transparent )
{
	uchar sr, sg, sb;
	Fl::get_color( stroke_color_, sr, sg, sb );
	char c[50];
	char fc[50];
	snprintf( c, sizeof(c),"rgb(%d,%d,%d)", (int)sr, (int)sg, (int)sb );
	if ( fill_color_ != Transparent )
	{
		uchar fr, fg, fb;
		Fl::get_color( fill_color_, fr, fg, fb );
		snprintf( fc, sizeof(fc), "rgb(%d,%d,%d)", (int)fr, (int)fg, (int)fb );
	}
	return create_svg_circle( x_, y_, w_, h_, stroke_w_, c, fill_color_ == Transparent ? "none" : fc );
}

// FLTK interface hiding the implementation
static void fl_svg_circle( int x_, int y_, int w_, int h_,
                           int width_ = 1,
                           Fl_Color color_ = FL_BLACK,
                           Fl_Color fill_color_ = Transparent )
{
	if ( w_ < 0 || h_ < 0 ) // Crashes Fl_SVG_Image::rasterize() otherwise
		return;
	Fl_SVG_Image *c = create_svg_circle( x_, y_, w_, h_, width_, color_, fill_color_ );
	c->resize( w_, h_ ); // rasterize SVG
	c->draw( x_, y_ );
	delete c;
	return;
}

#endif
