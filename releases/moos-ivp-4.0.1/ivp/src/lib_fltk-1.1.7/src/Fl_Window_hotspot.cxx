//
// "$Id: Fl_Window_hotspot.cxx 4288 2005-04-16 00:13:17Z mike $"
//
// Common hotspot routines for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2005 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/x.H>
#include <stdio.h>

void Fl_Window::hotspot(int X, int Y, int offscreen) {
  int mx,my;

  // Update the screen position based on the mouse position.
  Fl::get_mouse(mx,my);
  X = mx-X; Y = my-Y;

  // If offscreen is 0 (the default), make sure that the window
  // stays on the screen, if possible.
  if (!offscreen) {
#if defined(WIN32) || defined(__APPLE__)
    // These will be used by reference, so we must passed different variables
    int bt,bx,by;
    x(X);y(Y);
    Fl_X::fake_X_wm(this, X, Y, bt, bx, by);
    //force FL_FORCE_POSITION to be set in Fl_Window::resize()
    if (X==x()) x(X-1);
#else
    int scr_x, scr_y, scr_w, scr_h;
    Fl::screen_xywh(scr_x, scr_y, scr_w, scr_h);

    if (border()) {
      // Ensure border is on screen; these values are generic enough
      // to work with many window managers, and are based on KDE defaults.
      const int top = 20;
      const int left = 4;
      const int right = 4;
      const int bottom = 8;
      if (X+w()+right > scr_w-scr_x) X = scr_w-scr_x-right-w();
      if (X-left < scr_x) X = left;
      if (Y+h()+bottom > scr_h-scr_y) Y = scr_h-scr_y-bottom-h();
      if (Y-top < scr_y) Y = top;
    }
    // now insure contents are on-screen (more important than border):
    if (X+w() > scr_w-scr_x) X = scr_w-scr_x-w();
    if (X < scr_x) X = scr_x;
    if (Y+h() > scr_h-scr_y) Y = scr_h-scr_y-h();
    if (Y < scr_y) Y = scr_y;
#endif
  }

  position(X,Y);
}

void Fl_Window::hotspot(const Fl_Widget *o, int offscreen) {
  int X = o->w()/2;
  int Y = o->h()/2;
  while (o != this && o) {
    X += o->x(); Y += o->y();
    o = o->window();
  }
  hotspot(X,Y,offscreen);
}


//
// End of "$Id: Fl_Window_hotspot.cxx 4288 2005-04-16 00:13:17Z mike $".
//
