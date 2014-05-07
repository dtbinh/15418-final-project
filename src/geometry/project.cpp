/**
 * @file project.cpp
 * @brief Geometry project
 *
 * @author H. Q. Bovik (hqbovik)
 * @bug Unimplemented
 */

#include "geometry/project.hpp"
#include "application/opengl.hpp"
#include "application/imageio.hpp"
#include <assert.h>
#include <math.h>

#define H 500
#define W 1000
/*
   A namespace declaration. All project files use this namespace.
   Add this declaration (and its closing) to all source/headers you create.
   Note that all #includes should be BEFORE the namespace declaration.
 */
namespace _462 {

// definitions of functions for the GeometryProject class

// constructor, invoked when object is allocated
GeometryProject::GeometryProject() { }

// destructor, invoked when object is de-allocated
GeometryProject::~GeometryProject() 
{ 
	delete world;
}

/**
 * Initialize the project, doing any necessary opengl initialization.
 * @param camera An already-initialized camera.
 * @param mesh The mesh to be rendered and subdivided.
 * @param texture_filename The filename of the texture to use with the mesh.
 *  Is null if there is no texture data with the mesh or no texture filename
 *  was passed in the arguments, in which case textures should not be used.
 * @return true on success, false on error.
 */
bool GeometryProject::initialize( const Camera* camera, const MeshData* m, const char* texture_filename )
{
	/* create world */
	world = new World();
	num_ppl = 10000;
	running = false;

	
	/* initialize texture */
	unsigned char* text_array;
	int width, height;
	glGenTextures(1, &mesh.texture);
	glBindTexture(GL_TEXTURE_2D, mesh.texture);

	/* Load the texture from image */
	if (!texture_filename)
	{
		mesh.texture = NULL;
	}
	else
	{
		text_array = imageio_load_image(texture_filename, &width, &height);
		int err = gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, 
						GL_UNSIGNED_BYTE, text_array);
		std::cout << "error: " << err << "\n";
		text_ratio = (real_t)width / height;
	}
	
	/* Enable a bunch of stuff */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	/* initialize people */

	nyc = new Zone(NULL, "New York City", 100, 1213.4,true);
	int pop = nyc -> get_population();
	people = (Vector3*)calloc(pop, sizeof(Vector3));
	infected = (Vector3*)calloc(pop, sizeof(Vector3));

	step();
    return true;
}

/**
 * Clean up the project. Free any memory, etc.
 */
void GeometryProject::destroy()
{
  // TODO any cleanup code
}

/**
 * Clear the screen, then render the mesh using the given camera.
 * @param camera The logical camera to use.
 * @see scene/camera.hpp
 */
void GeometryProject::render( const Camera* camera )
{
  // TODO render code
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	/* load the identity matrix for projection */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	real_t fov = camera->get_fov_degrees();
	real_t ratio = camera->get_aspect_ratio();
	real_t near = camera->get_near_clip();
	real_t far = camera->get_far_clip();

	gluPerspective(fov,ratio,near,far);
	/* set camera position and where it points */
	Vector3 p = camera->get_position();
	Vector3 u = camera->get_up();
	Vector3 d = Vector3(0.0, 0.0, -1.0) + p;
	gluLookAt(p.x,p.y,p.z,d.x,d.y,d.z,0.0,1.0,0.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/* set up material */
	if (mesh.texture == NULL)
	{
		std::cout << "here\n";
	}
	else
	{
		glEnable(GL_TEXTURE_2D);
		glClientActiveTexture(mesh.texture);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, mesh.texture);
		glBegin (GL_QUADS);
		glTexCoord2f (0.0, 0.0);
		glVertex3f (-W, -H, 0.0);
		glTexCoord2f (1.0, 0.0);
		glVertex3f (W, -H, 0.0);
		glTexCoord2f (1.0, 1.0);
		glVertex3f (W, H, 0.0);
		glTexCoord2f (0.0, 1.0);
		glVertex3f (-W, H, 0.0);
		glEnd ();

		glDisable(GL_TEXTURE_2D);
	}

	glPointSize(3.0);
	
	glBegin(GL_POINTS);
	glColor3f(1.0,0.0,0.0);
	glVertex3f(1.15 * H, 0.016 * H, 1); //singapore
	glVertex3f(-0.82 * H, 0.4503 * H, 1); //nyc
	glVertex3f(0.10 * H, 0.53 * H, 1);	//zurich
	glVertex3f(0.346 * H, 0.33 * H, 1); // cairo
	glVertex3f(0.809 * H, 0.208 * H, 1);  //mumbai
	glVertex3f(1.55 * H, 0.39 * H, 1); //tokyo


	for (int i = 0; i < nyc->get_population(); i++)
	{
		glColor3f(infected[i].x, infected[i].y, infected[i].z);
		glVertex3f(people[i].x, people[i].y, people[i].z);
	}
	glEnd();
	glFlush();
}

/**
 * Advance a step in the simulation of the world 
 */
void GeometryProject::step()
{
	int num_sick = 0;
	Vector2 c = Vector2(-0.82 * H, 0.4503 * H); //nyc
	generate_points(people, infected, nyc, &c);
	/* randomly generate the array */
	std::cout << "Number of infected: " << num_sick << "\n";
}

/**
 * Subdivide the mesh that we are rendering using Loop subdivison.
 */
void GeometryProject::subdivide()
{
		
}

/* for all people in a zone with area (msq), update the person's position and convert the new coordinates
 * to real world coordinates 
 */
void GeometryProject::generate_points(Vector3 *coordinates, Vector3 *color, Zone* zone, Vector2 *center)
{
	int popul = zone->get_population();
	std::cout << popul << " enerate\n";
	double area = zone->get_area();
	for (int i = 0; i < popul; i++)
	{
		Person p = zone->get_person(i);
		update_person(area, &p);
		coordinates[i].x = center->x + (p.x  ); 
		coordinates[i].y = center->y + (p.y  ); 
		coordinates[i].z = 1.0;
		if (p.infected == true)
			color[i] = Vector3(0.0, 0.0, 0.0);
		else
			color[i] = Vector3(1.0, 1.0, 1.0);

	}
}

/** update a persons zone coordinates based on randomly selecting a direction and checking if 
 * the person can go to that position (if within zone's bounds 
 */
void GeometryProject::update_person(double area, Person* person)
{
	//pick a random direction
	int index = rand() % 4;
	double radius = sqrt(area/ PI);
	Vector2 current = Vector2(person->x, person->y);
	Vector2 next = current + directions[index];
	if (inArea(radius, next))
	{
		person->x = next.x;
		person->y = next.y;
	}
}

/** checks if the vector is inside the radius **/
bool GeometryProject::inArea(double radius, Vector2 vec)
{
	if (length(vec) <= radius)
		return true;
	return false;
}
/**** HELPER FUNCTIONS ****/

/* ~~~~ structure - related functions ~~~~ */

/* init_winged_edge: creates a winged edge structure from the mesh's triangles
 *					 and vertices.
 */
void GeometryProject::init_winged_edge()
{
}

/* generate_triangles: loops through all the edges, calculates all the new
 *					   triangles from odd vertices and inputs them into the mesh
 *					   triangle array
 */
void GeometryProject::generate_triangles()
{
	
}

/* create_faces: loops through all the triangles, and sets the corresponding
 *				faces structure to the triangle. Used as a separate way to keep
 *				track of what triangles have been subdivided already
 */
void GeometryProject::create_faces()
{
	}

/* ~~~~ subdivision - related functions ~~~~ */

/* generate_odds: loops through all the edges, calculates and sets all the odd
 *				  vertices of each edge.
 *
 */
void GeometryProject::generate_odds()
{

}

unsigned int GeometryProject::get_even_vtex(WingedEdge* curr, Face* face)
{
	return 0;
}
/* generate_evens: loops through all the edges, calculates and sets all the odd
 *				  vertices of each edge.
 */
void GeometryProject::generate_evens()
{
}

/* ~~~~ other helper functions ~~~~ */

/* exists: given a start and an end index to the vertices array, loop through
 *			all the existing edges and check if the edge with this start and end
 *			has already been created. Used to make sure doubles of the same edge
 *			aren't created 
 *	  Parameters:
 *		start - the pointer to the index into the vertices array for the 
 *				starting vertex
 *		end - the pointer to the index into the vertices array for the 
 *				ending vertex
 *	  Return value:
 *			the index into the edge array of the existing edge or the next
 *			available index into the edge array for creation of a new one.
 */
unsigned int GeometryProject::exists(unsigned int* start, unsigned int* end)
{
	return 0;
}

/*  end_or_start: because the order of the starting and ending vertices
 *				 in each successor or predecessor isn't guaranteed to be
 *				 consistent with each edge, whenever we want to access a vertex
 *				 of a given predecessor or a successor, we want the other vertex
 *				 it contains, not the vertex it shares with the current one
 *    Parameters:
 *		 curr - the index into the mesh vertices array that corresponds to the
 *				current vertex we're comparing against.
 *		 edge - the edge to be checked for the current vertex
 *    Return value:
 *			the index into the vertices array that corresponds to either the
 *			start or the end of the edge
 */
unsigned int GeometryProject::end_or_start(unsigned int curr, WingedEdge* edge)
{
	return 0;
	}

/*  is_boundary: checks whether the given edge is a boundary edge or not
 *    Parameters:
 *		 w_edge - the edge to be checked for boundary
 *    Return value:
 *			true if it's a boundary edge, false otherwise
 */
bool GeometryProject::is_boundary(WingedEdge* w_edge)
{
	return true;
}

/* calc_norms: Calculates the normals of the vertices and stores them into a
 *				the normals of each vertex in the vertices array
 */
void GeometryProject::calc_norms()
{
}

} /* _462 */

