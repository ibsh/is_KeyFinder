/*************************************************************************

  Copyright 2011 Ibrahim Sha'ath

  This file is part of KeyFinder.

  KeyFinder is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  KeyFinder is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with KeyFinder.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#include "windowfunctions.h"

WindowFunction::WindowFunction(){
  pi = (4 * atan(1.0));
  //pi = 3.14159265358979323846264338327950288419717;
}

WindowFunction* WindowFunction::getWindowFunction(char c){
  if(c == 'n'){
    return new HannWindow();
  }else if(c == 'b'){
    return new BlackmanWindow();
  }else{
    return new HammingWindow();
  }
}

// subclasses

float HannWindow::window(int n, int N)const{
  return 0.5 * (1.0 - cos((2 * pi * n)/(N-1)));
}

float HammingWindow::window(int n, int N)const{
  return 0.54 - (0.46 * cos((2 * pi * n)/(N-1)));
}

float BlackmanWindow::window(int n, int N)const{
  return 0.42 - (0.5 * cos((2 * pi * n)/(N-1))) + (0.08 * cos((4 * pi * n)/(N-1)));
}
