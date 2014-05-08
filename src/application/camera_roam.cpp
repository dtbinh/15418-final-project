/**
 * @file camera_roam.cpp
 * @brief CameraRoamControl class
 *
 * @author Zeyang Li (zeyangl)
 * @author Eric Butler (edbutler)
 * @description   this file implements a free roam camera
 */

#include "application/camera_roam.hpp"

namespace _462 {

static const real_t DirectionTable[] = { 0.0, 1.0, -1.0 };
static const real_t TranslationSpeed = 2.0;
static const real_t RotationSpeed = 0.02;

void window_to_world(Vector3 *ret, Vector3 *coord, int w, int h);

CameraRoamControl::CameraRoamControl()
{
    direction[0] = DZERO;
    direction[1] = DZERO;
    direction[2] = DZERO;
    rotation = RNONE;
	for (int i = 0; i < 11; i++)
		zoom_dirs[i] = Vector3::Zero;
}

CameraRoamControl::~CameraRoamControl()
{

}

void CameraRoamControl::set_dir( bool pressed, int index, Direction newdir )
{
    // fi pressed set, otherwise only undo it if other direction is not the one being pressed
    if ( pressed )
        direction[index] = newdir;
    else if ( direction[index] == newdir )
        direction[index] = DZERO;
}

void CameraRoamControl::handle_event( const Application* app, const SDL_Event& event )
{
    int width, height;
    app->get_dimension( &width, &height );
    int newidx = -1;
    Direction newdir = DZERO;

    switch ( event.type )
    {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        switch( event.key.keysym.sym )
        {
        case SDLK_i:
            newidx = 2;
            newdir = DNEG;
            break;
        case SDLK_o:
            newidx = 2;
            newdir = DPOS;
            break;
        case SDLK_LEFT:
            newidx = 0;
            newdir = DNEG;
            break;
        case SDLK_RIGHT:
            newidx = 0;
            newdir = DPOS;
            break;
        case SDLK_DOWN:
            newidx = 1;
            newdir = DNEG;
            break;
        case SDLK_UP:
            newidx = 1;
            newdir = DPOS;
            break;
        default:
            newidx = -1;
            break;
        }

        if ( newidx != -1 ) {
            set_dir( event.key.state == SDL_PRESSED, newidx, newdir );
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        // enable rotation
        if ( event.button.button == SDL_BUTTON_WHEELUP )
		{
			Vector3 moveto;
			Vector3 input = Vector3(event.button.x, event.button.y, 0.0);
			window_to_world(&moveto, &input , width, height);

			moveto = moveto / distance(moveto, camera.default_position) * 100;
			moveto.z = -100;

			Vector3 newvect = camera.get_position() + moveto;
			if (newvect.z < 2500 && newvect.z > 1)
			{
				zoom_dirs[camera.zoom] = moveto;
				camera.zoom++;
				camera.translate(moveto);
			}
		}
        else if ( event.button.button == SDL_BUTTON_WHEELDOWN )
		{
			if (camera.zoom > 0)
			{
				camera.zoom--;
				camera.translate(zoom_dirs[camera.zoom]*-1);
			}
		}
        break;

    case SDL_MOUSEBUTTONUP:
		break;

    case SDL_MOUSEMOTION:
        if ( rotation == RPITCHYAW ) {
            camera.yaw( -RotationSpeed * event.motion.xrel );
            camera.pitch( -RotationSpeed * event.motion.yrel );
        } else if ( rotation == RROLL ) {
            camera.roll( RotationSpeed * event.motion.yrel );
        }
        break;

    default:
        break;
    }
}

void CameraRoamControl::update( real_t dt )
{
    // update the position based on keys
    // no need to update based on mouse, that's done in the event handling
    real_t dist = TranslationSpeed * dt;
    Vector3 displacement(
        DirectionTable[direction[0]],
        DirectionTable[direction[1]],
        DirectionTable[direction[2]]
    );
	Vector3 newvect = camera.get_position() + displacement*dist;
	if (newvect.z < 2500)
		camera.translate( displacement * dist );

//	std::cout << "pos " << camera.get_position() << "\n";
//	std::cout << "displ " << displacement * dist << "\n";
}

void window_to_world(Vector3 *ret, Vector3 *coord, int w, int h)
{
	Vector3 origin = Vector3(w/2.0, h/2.0, 0.0);
	*coord = *coord - origin;
	*ret = Vector3(coord->x / (w/2) * 1000, -coord->y/ (h/2) * 500, 0.0);
}

}
