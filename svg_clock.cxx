//
// This is a modified version of the Erco's FLTK Cheat Page example
// svg_clock (http://seriss.com/people/erco/fltk/#FLTK-Simplex-Clock)
//
// It tries to minimize the cpu load of the clock drawing, by
// separating drawing of hands and clock face.
// It also adds a command line switch to display the clock without
// the surrounding box (using a shape() mask for the window),
// and making it draggable/resizable with mouse/mouse wheel.
//
#include <stdio.h>
#include <string.h>		/* sprintf().. */
#include <math.h>		/* fmod().. */
#include <time.h>		/* time(), localtime().. */
#include <sys/time.h>		/* gettimeofday() */
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_SVG_Image.H>
#include <FL/Fl_Image_Surface.H>

//
// Simplex clock simulator
// 1.0  04/10/2017 erco@seriss.com - javascript version
// 1.1  06/03/2018 erco@seriss.com - FLTK svg version
// 2.0  29/06/2018 wcout@gmx.net   - optimized drawing (separate hands/clock)
//                                 - optionally use shape mask
//                                 - resize/move with mouse
//
//      NOTE: If you notice drawing artefacts/wobbling of the clock hands
//            you can fix these with a change in FLTK's nanoSVG code:
//            In 'nanosvgrast.h' change NSVG__FIXSHIFT define from 10 to 14.
//            See STR-3476 for more details.

// Clock face and hands
//    Note that %.2f's are embedded in here where the clock hand's angles are,
//    which sprintf() will expand for us in ShowTime().
//
const char *G_clock_svg =
	"<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n"
	"<!-- Created with Inkscape (http://www.inkscape.org/) -->\n"
	"\n"
	"<svg\n"
	"   id='svg2837'\n"
	"   version='1.1'\n"
	"   inkscape:version='0.47 r22583'\n"
	"   width='512'\n"
	"   height='512'>\n"
	"  <!-- Draw outer black circle around clock face -->\n"
	"  <g transform='translate(256,256)'>\n"
	"     <circle cx='0' cy='0' r='240' stroke='black' fill='white' stroke-width='15' />\n"
	"  </g>\n"
	"  <g\n"
	"     inkscape:groupmode='layer'\n"
	"     id='layer2'\n"
	"     inkscape:label='Ink'\n"
	"     style='display:inline'\n"
	"     transform='translate(0,256)'>\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:5.19556952;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none'\n"
	"       d='m 240.58665,-193.94702 0,44.2123'\n"
	"       id='path2859' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:4.9362731;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none'\n"
	"       d='m 254.97219,-186.91263 c 4.9162,-6.2551 14.7486,-6.2551 18.37725,2.76668 "
	"3.15945,7.85528 -4.33099,14.7957 -4.33099,14.7957 l -13.81216,16.96094 20.13305,0'\n"
	"       id='path2900'\n"
	"       sodipodi:nodetypes='csccc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:5.19556952;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 339.23164,-170.35791 0,44.2123'\n"
	"       id='path2859-6' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:4.9362731;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 395.81274,-100.79143 c 4.91621,-6.25509 14.7486,-6.25509 18.37724,2.766682 "
	"3.15946,7.855284 -4.33098,14.79571 -4.33098,14.79571 l -13.81216,16.960936 20.13306,0'\n"
	"       id='path2900-2'\n"
	"       sodipodi:nodetypes='csccc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:5.19556952;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none'\n"
	"       d='m 416.94713,-15.517258 c 2.73771,-5.033848 13.38787,-8.003895 17.04441,-0.794823 "
	"3.65655,7.2090742 -3.5325,12.7170972 -9.5378,15.80804153'\n"
	"       id='path3718'\n"
	"       sodipodi:nodetypes='czc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:5.22313833;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 415.47568,14.100069 c 2.64575,5.971828 14.79,9.738055 19.40297,1.422706 "
	"4.61298,-8.31535 -3.74527,-15.82508637 -10.27399,-16.662872'\n"
	"       id='path3718-2'\n"
	"       sodipodi:nodetypes='czc' />\n"
	"    <path\n"
	"       style='fill:#000000;fill-opacity:1;stroke-width:2.5999999;stroke-miterlimit:4;stroke-dasharray:none'\n"
	"       id='path3742'\n"
	"       d='m 427.27152,2.0866716 c -1.13998,-0.2890936 -2.79716,-0.5429171 -4.39754,-0.3363732 "
	"-0.0218,-0.75555567 -0.0549,-3.3708658 0.021,-4.5717018 0.72379,-0.2428929 2.79288,-1.0569188 "
	"3.42693,-1.5314742 l 0.94963,6.4395492 z'\n"
	"       sodipodi:nodetypes='ccccc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:5.19556952;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 411.11845,108.4301 c 0.26495,-9.18455 0.0883,-46.187722 0.0883,-46.187722'\n"
	"       id='path3771'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:5.19556952;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 418.53676,96.59615 -24.46271,0 16.9176,-33.088631'\n"
	"       id='path3773'\n"
	"       sodipodi:nodetypes='ccc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:5.19556952;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 331.93516,147.43852 c 7.05878,-4.9784 12.23748,-4.24795 17.11041,1.99829 "
	"4.98303,6.55239 1.66341,17.07286 -5.99488,20.10785 -6.49446,1.87341 "
	"-11.86489,-2.87255 -11.86489,-2.87255 m 18.98382,-37.09336 -16.61084,0 -1.62362,17.85977'\n"
	"       id='path3890'\n"
	"       sodipodi:nodetypes='ccccccc' />\n"
	"    <path\n"
	"       style='fill:#000000;fill-opacity:1;stroke-width:2.5999999;stroke-linecap:butt;"
	"stroke-miterlimit:4;stroke-dasharray:none;display:inline'\n"
	"       id='path3916'\n"
	"       d='m 331.58525,169.52726 c -0.0302,0.13409 -2.06609,-0.7936 -2.03581,-0.92763 "
	"l 0,0 c 0,-1.91309 0.0701,-4.08128 0.0701,-5.94569 0.039,0.0565 3.2184,2.06518 "
	"3.25534,2.08011 -0.1181,1.51874 -0.71325,3.40347 -1.28965,4.79321 z'\n"
	"       sodipodi:nodetypes='cccccc' />\n"
	"    <path\n"
	"       style='fill:#000000;fill-opacity:1;stroke-width:2.5999999;stroke-linecap:butt;"
	"stroke-miterlimit:4;stroke-dasharray:none;display:inline'\n"
	"       id='path3916-1'\n"
	"       d='m 350.14159,132.14394 c -0.12971,-1.86442 -0.52964,-3.03348 -0.0236,-5.14951 "
	"0.039,0.0565 1.92916,4.65887 2.22496,5.17122 -0.67813,0 -1.41329,-0.0218 "
	"-2.20145,-0.0218 z'\n"
	"       sodipodi:nodetypes='cccc' />\n"
	"    <g\n"
	"       id='g4100'\n"
	"       transform='matrix(1.9982961,0,0,1.9982961,-150.096,-388.69778)'>\n"
	"      <g\n"
	"         transform='matrix(-1,0,0,-1,233.1875,386.75)'\n"
	"         id='g4046'>\n"
	"        <path\n"
	"           style='fill:none;stroke:#000000;stroke-width:2.5999999;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;"
	"stroke-dasharray:none;display:inline'\n"
	"           d='m 118.64488,181.63469 c -4.78002,3.30505 -8.05805,8.38749 -7.96875,14.46875 "
	"0.65434,8.59145 10.55031,6.6775 10.53125,0.625 0.98325,-6.75451 -9.5625,-10.03125 "
	"-10.5625,-0.625'\n"
	"           id='path3975-0'\n"
	"           sodipodi:nodetypes='cccc' />\n"
	"        <path\n"
	"           style='fill:#000000;fill-opacity:1;stroke-width:2.5999999;stroke-linecap:butt;"
	"stroke-miterlimit:4;stroke-dasharray:none;display:inline'\n"
	"           id='path3916-5-3'\n"
	"           d='m 117.8555,183.41675 0,0 c 0,-0.95736 -0.20709,-1.72988 -0.20709,-2.66288 "
	"0.33232,-0.21981 1.18849,-0.90387 1.70719,-1.24812 0,0.76784 0.0343,2.53516 "
	"0.0343,3.17209 -0.66058,0.51301 -1.19033,0.72917 -1.53441,0.73891 l -3e-5,0 z'\n"
	"           sodipodi:nodetypes='ccccccc' />\n"
	"      </g>\n"
	"    </g>\n"
	"    <g\n"
	"       style='display:inline'\n"
	"       id='g4046-2'\n"
	"       transform='matrix(1.9982961,0,0,1.9982961,24.088577,-210.44851)'>\n"
	"      <path\n"
	"         sodipodi:nodetypes='cccc'\n"
	"         id='path3975-0-5'\n"
	"         d='m 118.64488,181.63469 c -4.78002,3.30505 -8.05805,8.38749 -7.96875,14.46875 "
	"0.65434,8.59145 10.55031,6.6775 10.53125,0.625 0.98325,-6.75451 -9.5625,-10.03125 "
	"-10.5625,-0.625'\n"
	"         style='fill:none;stroke:#000000;stroke-width:2.5999999;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;"
	"display:inline' />\n"
	"      <path\n"
	"         sodipodi:nodetypes='ccccccc'\n"
	"         d='m 117.8555,183.41675 0,0 c 0,-0.95736 -0.20709,-1.72988 -0.20709,-2.66288 "
	"0.33232,-0.21981 1.18849,-0.90387 1.70719,-1.24812 0,0.76784 0.0343,2.53516 "
	"0.0343,3.17209 -0.66058,0.51301 -1.19033,0.72917 -1.53441,0.73891 l "
	"-3e-5,0 z'\n"
	"         id='path3916-5-3-8'\n"
	"         style='fill:#000000;fill-opacity:1;stroke-width:2.5999999;stroke-linecap:butt;"
	"stroke-miterlimit:4;stroke-dasharray:none;display:inline' />\n"
	"    </g>\n"
	"    <path\n"
	"       style='fill:#000000;fill-opacity:1;stroke-width:2.5999999;stroke-linecap:butt;"
	"stroke-miterlimit:4;stroke-dasharray:none;display:inline'\n"
	"       id='path3914'\n"
	"       d='m 330.80007,145.75777 c 1.11413,1.15229 2.35967,1.82792 3.76442,2.51092 0.016,0.018 "
	"-0.96138,1.07512 -1.189,1.25913 -1.09413,-0.63703 -2.92794,-1.71487 -4.04767,-2.35541 "
	"-0.0407,-0.0416 1.37168,-1.49723 1.47225,-1.41464 z'\n"
	"       sodipodi:nodetypes='ccccc' />\n"
	"    <g\n"
	"       id='g4164'\n"
	"       transform='matrix(1.9982961,0,0,1.9982961,-5.7191058,-254.81194)'>\n"
	"      <path\n"
	"         id='path4140'\n"
	"         d='m 81.803416,192.58136 11.623067,0 -10.032077,20.41771'\n"
	"         style='fill:none;stroke:#000000;stroke-width:2.5999999;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none' />\n"
	"      <path\n"
	"         sodipodi:nodetypes='ccccc'\n"
	"         d='m 82.399395,212.0759 c 0.55754,0.57664 1.852716,0.29364 2.555686,0.63543 0.008,0.009 "
	"-0.320934,0.71381 -0.399688,0.85667 -0.656907,0 -1.816292,-0.0342 -2.826341,-0.0342 "
	"0.03429,-0.0755 0.627826,-1.38206 0.670343,-1.45792 z'\n"
	"         id='path3914-7'\n"
	"         style='fill:#000000;fill-opacity:1;stroke-width:2.5999999;stroke-linecap:butt;"
	"stroke-miterlimit:4;stroke-dasharray:none;display:inline' />\n"
	"    </g>\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:4.95695305;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none'\n"
	"       d='m 106.90602,66.041519 c -4.36101,-0.03098 -8.911377,4.342737 -8.722027,9.650171 "
	"0.189358,5.307375 4.283877,9.518045 8.951557,9.526418 4.66767,0.0079 9.27071,-3.708498 "
	"9.41061,-9.526418 0.1399,-5.817979 -5.25044,-9.619018 -9.64014,-9.650171 z'\n"
	"       id='path4174'\n"
	"       sodipodi:nodetypes='czzzz' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:5.39827108;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;"
	"stroke-dasharray:none;display:inline'\n"
	"       d='m 106.57748,85.47287 c -4.64245,-0.03437 -9.486462,4.838215 -9.284888,10.75115 "
	"0.201574,5.91291 4.560338,10.60396 9.529238,10.61327 4.9689,0.008 9.86898,-4.13155 "
	"10.01792,-10.61327 0.14893,-6.481751 -5.58928,-10.71644 -10.26227,-10.75115 z'\n"
	"       id='path4174-3'\n"
	"       sodipodi:nodetypes='czzzz' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:5.19556952;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;"
	"display:inline'\n"
	"       d='m 115.44636,-105.95088 c -5.7434,-0.0639 -12.1094,10.000816 -11.86002,20.954218 "
	"0.24939,10.953278 5.64182,19.643197 11.78907,19.660474 6.14726,0.01659 11.33854,-7.134373 "
	"11.52277,-19.141305 0.18427,-12.007052 -5.67065,-21.409087 -11.45182,-21.473387 z'\n"
	"       id='path4174-7'\n"
	"       sodipodi:nodetypes='czzzz' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:5.19556952;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 88.467718,-63.502562 0,-45.461248'\n"
	"       id='path4220' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:5.19556952;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 157.65872,-126.82357 0,-45.46124'\n"
	"       id='path4220-0' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:5.19556952;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 182.38764,-126.69868 0,-45.46124'\n"
	"       id='path4220-0-4' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:7.99318409;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 146.29342,-190.39437 c 2.87254,4.62106 6.36957,11.86489 6.36957,11.86489'\n"
	"       id='path4266' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 126.68513,-177.78012 c 5.49532,7.36871 5.62022,7.49361 5.62022,7.49361'\n"
	"       id='path4268' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 109.0127,-163.04269 c 0.24979,0.24978 6.36958,6.86914 6.36958,6.86914'\n"
	"       id='path4268-8'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 92.526758,-146.86898 c 0.249786,0.24978 7.618502,6.86914 7.618502,6.86914'\n"
	"       id='path4268-8-1'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 78.413791,-129.13411 c 0.249787,0.24979 8.492759,6.36958 8.492759,6.36958'\n"
	"       id='path4268-8-1-0'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:7.99318409;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 65.799548,-109.40092 c 0.249786,0.24978 11.989776,6.61935 11.989776,6.61935'\n"
	"       id='path4266-4'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 56.236432,-89.029857 c 8.143352,3.707284 8.308119,3.754463 8.308119,3.754463'\n"
	"       id='path4268-83' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 48.379349,-67.481929 c 0.329539,0.09432 8.641493,2.841157 8.641493,2.841157'\n"
	"       id='path4268-8-8'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 42.206706,-45.26829 c 0.329541,0.09432 9.693507,2.225323 9.693507,2.225323'\n"
	"       id='path4268-8-1-6'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 38.777487,-22.865192 c 0.329538,0.09432 10.191653,1.35918 10.191653,1.35918'\n"
	"       id='path4268-8-1-0-9'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:7.86712933;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 37.563769,0.53907123 c 0.329539,0.0943198 13.256436,-0.1474742 13.256436,-0.1474742'\n"
	"       id='path4266-4-3'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 38.201235,23.22179 c 8.900732,-0.9138 9.066767,-0.956284 9.066767,-0.956284'\n"
	"       id='path4268-83-0' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 42.304406,45.787489 c 0.33205,-0.08513 8.893166,-1.912909 8.893166,-1.912909'\n"
	"       id='path4268-8-8-0'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='M 48.197604,68.076903 C 48.529653,67.991782 57.687638,65.1011 57.687638,65.1011'\n"
	"       id='path4268-8-1-6-7'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 56.55421,89.14406 c 0.332046,-0.08512 9.482459,-3.974931 9.482459,-3.974931'\n"
	"       id='path4268-8-1-0-9-3'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:7.86712933;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 67.328615,109.95619 c 0.332047,-0.0851 11.366482,-6.82333 11.366482,-6.82333'\n"
	"       id='path4266-4-3-5'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 80.390541,127.8306 c 7.396444,-5.03496 7.522329,-5.15125 7.522329,-5.15125'\n"
	"       id='path4268-83-0-8' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 94.727154,145.73333 c 0.251669,-0.23273 6.914866,-5.91036 6.914866,-5.91036'\n"
	"       id='path4268-8-8-0-1'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 110.50729,162.54211 c 0.25168,-0.23274 6.93475,-7.12921 6.93475,-7.12921'\n"
	"       id='path4268-8-1-6-7-3'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 127.87373,177.10455 c 0.25165,-0.23272 6.45311,-8.00463 6.45311,-8.00463'\n"
	"       id='path4268-8-1-0-9-3-5'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:7.86712933;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 147.2461,190.29335 c 0.25166,-0.23274 6.75667,-11.40623 6.75667,-11.40623'\n"
	"       id='path4266-4-3-5-3'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.91382933;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 167.80873,198.49076 c 3.85792,-8.05917 3.90848,-8.22284 3.90848,-8.22284'\n"
	"       id='path4268-83-0-8-7' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.91382933;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 188.93328,206.86491 c 0.1009,-0.32749 3.01312,-8.57817 3.01312,-8.57817'\n"
	"       id='path4268-8-8-0-1-0'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.91382933;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 210.75585,213.56964 c 0.10071,-0.3275 2.42886,-9.64537 2.42886,-9.64537'\n"
	"       id='path4268-8-1-6-7-3-7'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.91382933;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 232.83035,217.53404 c 0.10071,-0.3275 1.58401,-10.16437 1.58401,-10.16437'\n"
	"       id='path4268-8-1-0-9-3-5-2'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:7.82765865;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 255.94708,219.30571 c 0.10091,-0.32748 0.16626,-13.26657 0.16626,-13.26657'\n"
	"       id='path4266-4-3-5-3-9'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:7.99318409;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 256.54433,-218.97951 c 0.16526,5.4386 -0.44562,13.45913 -0.44562,13.45913'\n"
	"       id='path4266-0' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 233.25355,-217.91023 c 1.0548,9.13147 1.10014,9.30218 1.10014,9.30218'\n"
	"       id='path4268-2' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 210.57153,-214.03297 c 0.0907,0.34142 2.06172,9.13817 2.06172,9.13817'\n"
	"       id='path4268-8-6'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 188.1949,-208.31792 c 0.0907,0.34141 3.14193,9.76498 3.14193,9.76498'\n"
	"       id='path4268-8-1-2'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 167.08721,-200.0616 c 0.0907,0.34141 4.14882,9.77167 4.14882,9.77167'\n"
	"       id='path4268-8-1-0-94'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:7.99318409;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 364.43071,-189.21562 c -2.87254,4.62105 -6.36956,11.86488 -6.36956,11.86488'\n"
	"       id='path4266-6' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 384.039,-176.60138 c -5.49531,7.36872 -5.62021,7.49361 -5.62021,7.49361'\n"
	"       id='path4268-4' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 401.71143,-161.86395 c -0.24979,0.24979 -6.36958,6.86914 -6.36958,6.86914'\n"
	"       id='path4268-8-4'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 418.19736,-145.69024 c -0.24978,0.24979 -7.61849,6.86914 -7.61849,6.86914'\n"
	"       id='path4268-8-1-68'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 432.31034,-127.95536 c -0.24979,0.24979 -8.49277,6.36958 -8.49277,6.36958'\n"
	"       id='path4268-8-1-0-4'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:7.99318409;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 444.92458,-108.22218 c -0.24978,0.24978 -11.98977,6.61935 -11.98977,6.61935'\n"
	"       id='path4266-4-9'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 454.4877,-87.851111 c -8.14335,3.707286 -8.30813,3.754463 -8.30813,3.754463'\n"
	"       id='path4268-83-1' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 462.34478,-66.303182 c -0.32953,0.09432 -8.64149,2.841157 -8.64149,2.841157'\n"
	"       id='path4268-8-8-1'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 468.51742,-44.089535 c -0.32954,0.09432 -9.69351,2.225322 -9.69351,2.225322'\n"
	"       id='path4268-8-1-6-9'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 471.94664,-21.686438 c -0.32955,0.09432 -10.19166,1.359181 -10.19166,1.359181'\n"
	"       id='path4268-8-1-0-9-7'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:7.86712933;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 473.16036,1.7178261 c -0.32954,0.09432 -13.25642,-0.1474743 -13.25642,-0.1474743'\n"
	"       id='path4266-4-3-1'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 472.52288,24.400545 c -8.90072,-0.913801 -9.06676,-0.956285 -9.06676,-0.956285'\n"
	"       id='path4268-83-0-9' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 468.41972,46.966244 c -0.33205,-0.08513 -8.89318,-1.912909 -8.89318,-1.912909'\n"
	"       id='path4268-8-8-0-8'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 462.52653,69.255658 c -0.33206,-0.08513 -9.49005,-2.975803 -9.49005,-2.975803'\n"
	"       id='path4268-8-1-6-7-1'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 454.16992,90.322815 c -0.33203,-0.08512 -9.48246,-3.974931 -9.48246,-3.974931'\n"
	"       id='path4268-8-1-0-9-3-6'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:7.86712933;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 443.39551,111.13495 c -0.33204,-0.0851 -11.36648,-6.82334 -11.36648,-6.82334'\n"
	"       id='path4266-4-3-5-8'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 430.33358,129.00935 c -7.39643,-5.03495 -7.52232,-5.15125 -7.52232,-5.15125'\n"
	"       id='path4268-83-0-8-9' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 415.99697,146.91208 c -0.25167,-0.23272 -6.91486,-5.91036 -6.91486,-5.91036'\n"
	"       id='path4268-8-8-0-1-08'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 400.21685,163.72087 c -0.25167,-0.23275 -6.93476,-7.12923 -6.93476,-7.12923'\n"
	"       id='path4268-8-1-6-7-3-6'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.93356466;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 382.85041,178.28331 c -0.25166,-0.23272 -6.45314,-8.00463 -6.45314,-8.00463'\n"
	"       id='path4268-8-1-0-9-3-5-4'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:7.86712933;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 363.47803,191.47211 c -0.25167,-0.23274 -6.75668,-11.40623 -6.75668,-11.40623'\n"
	"       id='path4266-4-3-5-3-7'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.91382933;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 342.9154,199.66952 c -3.85793,-8.05917 -3.90849,-8.22285 -3.90849,-8.22285'\n"
	"       id='path4268-83-0-8-7-1' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.91382933;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 321.79085,208.04366 c -0.10091,-0.32748 -3.01313,-8.57817 -3.01313,-8.57817'\n"
	"       id='path4268-8-8-0-1-0-7'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.91382933;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 299.96828,214.74839 c -0.10071,-0.32749 -2.42887,-9.64537 -2.42887,-9.64537'\n"
	"       id='path4268-8-1-6-7-3-7-5'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.91382933;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 277.89378,218.7128 c -0.10071,-0.3275 -1.58401,-10.16437 -1.58401,-10.16437'\n"
	"       id='path4268-8-1-0-9-3-5-2-5'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 277.47059,-216.73149 c -1.0548,9.13149 -1.10015,9.30219 -1.10015,9.30219'\n"
	"       id='path4268-2-8' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 300.1526,-212.85422 c -0.0907,0.34141 -2.06172,9.13817 -2.06172,9.13817'\n"
	"       id='path4268-8-6-0'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 322.52923,-207.13917 c -0.0907,0.34141 -3.14193,9.76498 -3.14193,9.76498'\n"
	"       id='path4268-8-1-2-3'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:3.99659204;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none;display:inline'\n"
	"       d='m 343.63692,-198.88286 c -0.0907,0.34141 -4.14883,9.77167 -4.14883,9.77167'\n"
	"       id='path4268-8-1-0-94-5'\n"
	"       sodipodi:nodetypes='cc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:2.16094732px;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-opacity:1;display:inline'\n"
	"       d='m 224.78816,-75.278911 c 0.14328,-2.101026 -1.91002,-2.865037 -1.91002,-2.865037 "
	"0,0 -7.30582,-0.04776 -9.02485,0.04776 -2.15867,0.119939 -3.0083,2.483034 "
	"-2.86506,3.820054 0.14328,1.337015 0.99979,3.24705 2.53081,3.247042 1.76677,-10e-6 "
	"9.45462,-0.0955 9.45462,-0.0955 0,0 2.48302,0.955012 2.43528,3.581296 -0.0429,2.37085 "
	"-2.43528,3.867802 -2.43528,3.867802 l -9.55012,0.190997 c 0,0 -2.50437,-0.815419 "
	"-2.33979,-2.674037'\n"
	"       id='path4749'\n"
	"       sodipodi:nodetypes='ccssscsccc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:2.22774911px;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-opacity:1'\n"
	"       d='m 229.03371,-72.009152 0,9.106952'\n"
	"       id='path4786' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:2.28125501px;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-opacity:1'\n"
	"       d='m 238.90309,-63.555079 5.36505,-7.636841 m 0.17145,-0.661252 0.0101,9.063455 "
	"m -11.16896,-8.544825 5.19355,7.766494 m -5.37727,0.635324 0,-9.076039'\n"
	"       id='path4788'\n"
	"       sodipodi:nodetypes='cccccccc' />\n"
	"    <path\n"
	"       style='fill:#000000;fill-opacity:1;stroke-width:4;stroke-linecap:butt;stroke-miterlimit:4;"
	"stroke-dasharray:none'\n"
	"       id='path4790'\n"
	"       d='m 236.54487,-64.499812 c 0.42872,-0.110605 4.39004,-0.408064 4.13219,-0.03099 "
	"-0.20147,0.294522 -0.76605,1.490632 -0.88521,1.645345 -0.0932,0.01937 -1.74727,9.23e-4 "
	"-2.19932,-0.01179 l -1.04766,-1.602662 0,4.6e-5 z'\n"
	"       sodipodi:nodetypes='cscccc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:2.02427435px;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-opacity:1'\n"
	"       d='m 248.32622,-62.733567 -0.0439,-8.319603 7.8339,0'\n"
	"       id='path3124'\n"
	"       sodipodi:nodetypes='ccc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:1.99829602px;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-opacity:1'\n"
	"       d='m 248.35753,-66.694663 7.02087,0.04417 c 2.20784,0.132469 2.91434,-3.797461 0.48575,-4.415652'\n"
	"       id='path3126'\n"
	"       sodipodi:nodetypes='ccc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:1.99829602px;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-opacity:1'\n"
	"       d='m 260.41225,-72.081759 0,8.566365 10.64172,0'\n"
	"       id='path3128'\n"
	"       sodipodi:nodetypes='ccc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:1.99829602px;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-opacity:1;display:inline'\n"
	"       d='m 274.21117,-72.125915 0,8.566365 9.40533,0'\n"
	"       id='path3128-8'\n"
	"       sodipodi:nodetypes='ccc' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:1.99829602px;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-opacity:1'\n"
	"       d='m 274.32154,-71.154473 9.05211,0'\n"
	"       id='path3152' />\n"
	"    <path\n"
	"       style='fill:none;stroke:#000000;stroke-width:1.99829602px;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-opacity:1'\n"
	"       d='m 274.27741,-67.216965 4.63642,0'\n"
	"       id='path3154' />\n"
	"    <g\n"
	"       id='g4016'\n"
	"       transform='matrix(1.9982961,0,0,1.9982961,-5.7191058,-254.81194)'>\n"
	"      <path\n"
	"         sodipodi:nodetypes='cccc'\n"
	"         d='m 147.06,96.268898 c -0.21784,-0.416626 -0.39017,-0.536265 -0.73385,-0.676292 "
	"-0.16415,0.185294 -0.27297,0.314789 -0.53799,0.664622 0.72101,-0.0066 0.85248,0.0074 "
	"1.27184,0.01167 z'\n"
	"         id='path3936-3'\n"
	"         style='fill:#000000;fill-opacity:1;stroke-linecap:butt;"
	"stroke-linejoin:miter;display:inline' />\n"
	"      <path\n"
	"         sodipodi:nodetypes='cccc'\n"
	"         d='m 149.92048,96.258958 c 0.22119,-0.416626 0.39617,-0.536265 0.74512,-0.676292 "
	"0.16667,0.185294 0.27716,0.314789 0.54625,0.664622 -0.73208,-0.0066 -0.86557,0.0074 "
	"-1.29137,0.01167 z'\n"
	"         id='path3936-8-1'\n"
	"         style='fill:#000000;fill-opacity:1;stroke-linecap:butt;"
	"stroke-linejoin:miter;display:inline' />\n"
	"      <path\n"
	"         sodipodi:nodetypes='cc'\n"
	"         id='path3156'\n"
	"         d='m 146.49758,91.790063 3.80071,4.132155'\n"
	"         style='fill:none;stroke:#000000;stroke-width:1px;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-opacity:1;display:inline' />\n"
	"      <path\n"
	"         sodipodi:nodetypes='cc'\n"
	"         id='path3158'\n"
	"         d='m 146.67437,95.944315 3.55762,-4.132156'\n"
	"         style='fill:none;stroke:#000000;stroke-width:1px;stroke-linecap:butt;"
	"stroke-linejoin:miter;stroke-opacity:1;display:inline' />\n"
	"      <path\n"
	"         sodipodi:nodetypes='cccc'\n"
	"         d='m 146.87249,91.454023 c -0.21784,0.416626 -0.39017,0.536265 -0.73385,0.676292 "
	"-0.16415,-0.185294 -0.27297,-0.314789 -0.53799,-0.664622 0.72101,0.0066 "
	"0.85248,-0.0074 1.27184,-0.01167 z'\n"
	"         id='path3936'\n"
	"         style='fill:#000000;fill-opacity:1;stroke-linecap:butt;"
	"stroke-linejoin:miter;display:inline' />\n"
	"      <path\n"
	"         sodipodi:nodetypes='cccc'\n"
	"         d='m 149.84235,91.4874 c 0.22119,0.416626 0.39617,0.536265 0.74512,0.676292 "
	"0.16667,-0.185294 0.27716,-0.314789 0.54625,-0.664622 -0.73208,0.0066 "
	"-0.86557,-0.0074 -1.29137,-0.01167 z'\n"
	"         id='path3936-8'\n"
	"         style='fill:#000000;fill-opacity:1;stroke-linecap:butt;"
	"stroke-linejoin:miter;display:inline' />\n"
	"    </g>\n"
	"  </g>\n"
	"</svg>";


const char *G_hands_svg =
	"<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n"
	"<!-- Created with Inkscape (http://www.inkscape.org/) -->\n"
	"\n"
	"<svg\n"
	"   id='svg2837'\n"
	"   version='1.1'\n"
	"   inkscape:version='0.47 r22583'\n"
	"   width='512'\n" "   height='512'>\n" "  <g\n"
	// Here's the hour/min/second hands part of the svg data.
	//     I manually inserted this at the bottom of the Inkscape generated svg data.
	//     There's three "%.2f"s in here; one each for hour, minute, and second hands
	//     respectively. These will be expanded by sprintf() in ShowTime(), and must
	//     be specified in degrees (0-360), as that's what svg's rotate() command expects.
	//
	"  <!-- Draw hour hand-->\n"
	"  <g transform='translate(256,256) rotate(%lf 0 0)'>\n"
	"     <line x1='0' y1='0' x2='0' y2='-140' stroke='black' stroke-width='15' stroke-opacity='0.8' stroke-linecap='round' />\n"
	"  </g>\n"
	"  <!-- Draw minute hand-->\n"
	"  <g transform='translate(256,256) rotate(%lf 0 0)'>\n"
	"     <line x1='0' y1='0' x2='0' y2='-200' stroke='black' stroke-width='10' stroke-opacity='0.8' stroke-linecap='round' />\n"
	"  </g>\n"
	"  <!-- Draw minute hand disc -->\n"
	"  <g transform='translate(256,256)'>\n"
	"     <circle cx='0' cy='0' r='20' style='fill:black' />\n"
	"  </g>\n"
	"  <!-- Draw second hand-->\n"
	"  <g transform='translate(256,256) rotate(%lf 0 0)'>\n"
	"     <line x1='0' y1='20' x2='0' y2='-200' stroke='red' stroke-width='5' stroke-opacity='0.8' stroke-linecap='round' />\n"
	"  </g>\n"
	"  <!-- Draw second hand disc -->\n"
	"  <g transform='translate(256,256)'>\n"
	"     <circle cx='0' cy='0' r='7' style='fill:red' />\n"
	"  </g>\n"
	"  <!-- Draw tiny shaft -->\n"
	"  <g transform='translate(256,256)'>\n"
	"     <circle cx='0' cy='0' r='3' style='fill:black' />\n"
	"  </g>\n"
	"</svg>";

const char *G_mask_svg =
	"<svg\n"
	"   id='svg2837'\n"
	"   version='1.1'\n"
	"   inkscape:version='0.47 r22583'\n"
	"   width='512'\n"
	"   height='512'>\n"
	"  <!-- Draw mask of clock face -->\n"
	"  <g transform='translate(256,256)'>\n"
	"     <circle cx='0' cy='0' r='240' stroke='white' fill='white' stroke-width='15' />\n"
	"  </g>\n"
	"</svg>";


// Simplex clock class
//     This is the 512x512 clock face.
//
class SimplexClock: public Fl_Group {
public:
	// Show a specific time
	void ShowTime( int hour, int min, int sec, unsigned long usec = 0 ) {
		// Make a string large enough for clock's svg data plus changes
		// that sprintf() will make to change the clock hand's angles
		char *s = new char[strlen( G_clock_svg ) + 100];
		double sec_deg = ( sec / 60.0 ) * 360 + ( ( usec / 1000000.0 ) * 1 / 60 * 360.0 );	// let usecs influence sec hand
		double min_deg = ( min / 60.0 ) * 360 + ( ( sec / 60.0 ) * 1 / 60 * 360 );	// let seconds influence minute hand
		double hour_deg = ( ( hour / 12.0 ) * 360 ) + ( ( min / 60.0 ) * 5 / 60 * 360.0 );	// let minutes influence hour hand
		hour_deg = fmod( hour_deg, 360 );
		min_deg = fmod( min_deg, 360 );
		sec_deg = fmod( sec_deg, 360 );
		sprintf( s, G_hands_svg, hour_deg, min_deg, sec_deg );	// let sprintf() expand the %.2f's embedded within
		// Rebuild hands, reassign to box
		delete hands_svg;
		hands_svg = new Fl_SVG_Image( NULL, s );
		hands_svg->scale( w(), h(), 1, 1 );
		hands->image( hands_svg );
		clock_svg->scale( w(), h(), /*proportional= */ 1, /*can_expand= */ 1 );
		// No longer need copy of svg data, Fl_SVG_Image already parsed it into an image
		delete s;
		redraw();
	} // ShowTime

private:
	void Tick() {
		set_mask();
		// Get current time
		time_t tt = time( NULL );
		struct tm *now = localtime( &tt );
		struct timeval tval;
		gettimeofday( &tval, NULL );	// get usecs (analog sweep for second hand)
		// Show the time
		ShowTime( now->tm_hour, now->tm_min, now->tm_sec, rate < 1 ? tval.tv_usec : 0 );
	}

	static void Timer_CB( void *data ) {
		SimplexClock *clock = ( SimplexClock * )data;
		clock->Tick();
		Fl::repeat_timeout( clock->rate, Timer_CB, data );
	}

	void set_mask() {
		if ( !mask )
			return;
		if ( mask && mask->w() == w() && mask->h() == h() ) {
			return;
		}
		mask->resize( w(), h() );
		window()->shape( mask );
	}

	int handle( int e ) {
		// Handle dragging of clock
		// and resizing with mousewheel
		static int x, y;
		int ret = Fl_Group::handle( e );
		switch ( e ) {
			case FL_PUSH:
				x = Fl::event_x();
				y = Fl::event_y();
				return 1;
			case FL_DRAG:
				window()->position( Fl::event_x_root() - x, Fl::event_y_root() - y );
				return 1;
			case FL_MOUSEWHEEL: {
				int height = h() + 3 * Fl::event_dy();
				if ( height < 30 ) {
					height = 30;
				}
				int X, Y, W, H;
				Fl::screen_work_area( X, Y, W, H );
				if ( height > H ) {
					height = H;
				}
				window()->size( height, height );
				return 1;
			}
		}
		return ret;
	}

	void resize( int x, int y, int w, int h ) {
		Fl_Group::resize( x, y, w, h );
		Tick();
	}

public:
	SimplexClock( int X, int Y, int W, int H, bool use_mask = false ) :
		Fl_Group( X, Y, W, H ),
		clock_svg( 0 ), hands_svg( 0 ), clock( 0 ), hands( 0 ), mask( 0 ) {
		clock = new Fl_Box( X, Y, W, H );
		hands = new Fl_Box( X, Y, W, H );
		end();
		clock_svg = new Fl_SVG_Image( NULL, G_clock_svg );
		clock_svg->scale( w(), h(), /*proportional= */ 1, /*can_expand= */ 1 );
		clock->image( clock_svg );
		if ( use_mask )
			mask = new Fl_SVG_Image( NULL, G_mask_svg );
		Tick();
	}

	// Start the clock's timer ticking
	void StartClock( double rate = 0.25 ) {
		this->rate = rate;
		Fl::add_timeout( rate, Timer_CB,( void * ) this );
	}

	// Stop the clock's timer ticks
	void StopClock() {
		Fl::remove_timeout( Timer_CB, ( void * ) this );
	}
private:
	// Clock face
	Fl_SVG_Image *clock_svg;	// the svg image assigned to the clock box
	Fl_SVG_Image *hands_svg;	// the svg image assigned to the hands box
	double rate;			// timer tick rate the user specified with StartClock()
	Fl_Box *clock;
	Fl_Box *hands;
	Fl_SVG_Image *mask;
};


int main( int argc, char **argv ) {
	bool mask = false;	// default: show box
	double rate = 1.;		// default: tick seconds only
	for ( int i=1; i<argc; i++ ) {
		if ( strchr(argv[i], 'c') ) rate = 0.05;	// show a continuous second hand
		if ( strchr(argv[i], 'm') ) mask = true;	// clock masked (no box around)
	}
	Fl_Double_Window *win =	new Fl_Double_Window( 550, 550, "svg simplex clock" );
	SimplexClock *clock = new SimplexClock( 0, 0, win->w(), win->h(), mask );
	clock->StartClock( rate );	// start running clock
	win->end();
	win->resizable( win );
	win->show( argc, argv );
	return Fl::run();
}
