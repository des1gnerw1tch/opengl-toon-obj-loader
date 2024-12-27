/* Compilation on Linux: 
 g++ -std=c++17 ./src/*.cpp -o prog -I ./include/ -I./../common/thirdparty/ -lSDL2 -ldl
*/

#include "GraphicsProgram.hpp"
#include <iostream>


int main( int argc, char* args[] ){
    if (argc < 2)   {
        std::cout << "Incorrect # of args, need 1 arg, provide model to render" << std::endl;
        return 1;
    }
    GraphicsProgram p;
    p.Start(args[1]);
	return 0;
}
