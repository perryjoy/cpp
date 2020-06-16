#include <iostream>
#include "my_graphics.h"




int main(int argc, char* args[]) // end me pls
{
 
      line *a = new line(r_coordinates(), 0.1, 2, r_coordinates(-0.1, -0.1), 1, 0.3);
      scene S(1000, 1000);
      S.PushObject(a); // FIND OUT WHATS HAPPENIN
      

      while (1)
      {
        S.DrawScene();
      }
  

  //std::cin.get();
  return 0;
}