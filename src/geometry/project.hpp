/**
 * @file project.hpp
 * @brief Geometry project
 *
 * @author H. Q. Bovik (hqbovik)
 * @bug Unimplemented
 */

#ifndef _462_GEOMETRY_PROJECT_HPP_
#define _462_GEOMETRY_PROJECT_HPP_

#include "math/camera.hpp"
#include "application/opengl.hpp"
#include "objects/world.hpp"
#include <vector>


/*
   A namespace declaration. All project files use this namespace.
   Add this declaration (and its closing) to all source/headers you create.
   Note that all #includes should be BEFORE the namespace declaration.
 */
namespace _462 {

struct Triangle
{
    // index into vertex list of the 3 vertices of this triangle
    unsigned int vertices[3];
};

struct Vertex
{
    // the position of the vertex
    Vector3 position;
    // the normal of the vertex
    Vector3 normal;
    // the texture coordinate of the vertex
    Vector2 texture_coord;
	// list of adjacent vertices
	std::vector<unsigned int> *adj_v;
};
struct Face;

struct WingedEdge
{
	// start vertex index
	unsigned int start;
	// end vertex index
	unsigned int end;
	// odd vertex
	unsigned int odd_vtex;
	// right face
	Face* right_tri;
	// left face
	Face* left_tri;

};

struct Face
{
	Triangle* tri;
	bool is_sub;
	// pointer to current edge
	WingedEdge* e1;
	// pointer to predecessing edge
	WingedEdge* e2;
	// pointer to succesive edge
	WingedEdge* e3;
};

struct MeshData
{
	//texture
	GLuint texture;
    // array of vertices
    Vertex* vertices;
    // size of vertex array
    size_t num_vertices;

	// array of winged edges
	WingedEdge* w_edges;
	// size of winged edge array
	size_t num_w_edges;

    // array of triangles
    Triangle* triangles;
    // array of faces
    Face* faces;
    // size of triangle array
    size_t num_triangles;
};




class GeometryProject
{
public:

    // constructor, invoked when object is created
    GeometryProject();
    // destructor, invoked when object is destroyed
    ~GeometryProject();
	
	/*** added by elena ***/
	bool running;	//check if project just running
	void step();    //advance a step
	/********/


    // more detailed specifications for each function are in project.cpp.

    // Initialize the project, loading the mesh from the given filename.
    // Returns true on success.
    bool initialize( const Camera* camera, const MeshData* mesh, const char* texture_filename );
    // Clean up the project, free any memory, etc.
    void destroy();
    // Render the mesh using the given camera.
    void render( const Camera* camera );
    // Subdivide the mesh
    void subdivide();
	void calc_norms();
	void init_winged_edge();
	bool is_boundary(WingedEdge* w_edge);
	unsigned int end_or_start(unsigned int curr, WingedEdge* edge);
	unsigned int get_even_vtex(WingedEdge* curr, Face* face);
	void generate_odds();
	void generate_evens();
	void generate_triangles();
	void create_faces();
	unsigned int exists(unsigned int* start, unsigned int* end);



private:

    MeshData mesh;
	World* world;

	real_t text_ratio;

	Vector3* people;
	Vector3*  infected;
	int num_ppl;
    // TODO add any other private members/functions here.

    // since this has no meaningful assignment/copy, prevent the compiler from
    // automatically generating those functions
    GeometryProject( const GeometryProject& );
    GeometryProject& operator=( const GeometryProject& );
};

} /* _462 */

#endif /* _462_GEOMETRY_PROJECT_HPP_ */

