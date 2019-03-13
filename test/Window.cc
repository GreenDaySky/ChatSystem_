#include "Window.hpp"
#include<unistd.h>

int main()
{
    Window w;
    w.DrawHeader();
    w.DrawOutput();
    w.DrawOnline();
    w.DrawInput();
  
    std::string message;
    for(;;){
	w.GetStringFromInput(message);
	w.PutMessageToOutput(message);
    }

    w.Welcome();
    return 0;
}
