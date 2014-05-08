
#include "application/application.hpp"
#include "application/camera_roam.hpp"
#include "geometry/project.hpp"
#include "scene/mesh.hpp"

namespace _462 {


#define DEBUG

static const int WIDTH = 1024;
static const int HEIGHT = 512;
static const real_t FPS = 60.0;
static const char* TITLE = "15462 Project - Geometry";

} /* _462 */

using namespace _462;

static void print_usage( const char* progname )
{
    std::cout << "Usage : "
        << progname << " <mesh filename> [texture filename]\n";
}

int main( int argc, char* argv[] )
{
   // GeometryApplication app;

    // parse arguments
  /*  if ( argc < 2 || argc > 3 ) {
        print_usage( argv[0] );
        return 2;
    }

    app.mesh_filename = argv[1];
    app.texture_filename = argc > 2 ? argv[2] : 0;
	std::cout << "main " << app.texture_filename << "\n";
    // start a new application
    return Application::start_application( &app, WIDTH, HEIGHT, FPS, TITLE );
	*/
	double start, end;

	start = CycleTimer::currentSeconds();
	World* world = new World();
	end = CycleTimer::currentSeconds();
#ifdef DEBUG
	printf("Full Initialization took: %.3f ms\n", 1000.f * (end-start));
#endif

	bool running = true;
	/***** main run loop *****/
	while (running) {

		std::string opt;
		std::cout << "press s + enter to step, x + enter to stop running\n";
		getline(std::cin, opt);

		if (opt.compare("s") == 0) {
			start = CycleTimer::currentSeconds();
			world->step();
			end = CycleTimer::currentSeconds();
#ifdef DEBUG
			printf("Step took: %.3f ms\n", 1000.f * (end-start));
#endif
		}
		else if (opt.compare("x") == 0 )
			running = false;
	}

}

