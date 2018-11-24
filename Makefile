cmd=fltk-config -g --use-images --compile

all:
	$(cmd) aa_line.cxx
	$(cmd) svg_buttons.cxx
	$(cmd) svg_clock.cxx
	$(cmd) svg_circle.cxx
	$(cmd) waiter.cxx
