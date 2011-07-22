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
