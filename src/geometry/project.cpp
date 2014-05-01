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

    mesh = *m;
	int height;
	int width;
	/* initialize texture */
	unsigned char* text_array;
	glGenTextures(1, &mesh.texture);
	glBindTexture(GL_TEXTURE_2D, mesh.texture);

	/* Enable a bunch of stuff */
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Load the texture from image */
	if (!texture_filename)
	{
		mesh.texture = NULL;
	}
	else
	{
		text_array = imageio_load_image(texture_filename, &width, &height);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, 
						GL_UNSIGNED_BYTE, text_array);
	}
	
	/* Initialize winged edges */
	mesh.w_edges = (WingedEdge*)calloc(mesh.num_triangles*3,sizeof(WingedEdge));
	mesh.faces = (Face*)calloc(mesh.num_triangles,sizeof(Face));
	create_faces();
	assert(mesh.w_edges);
	/* initialize the adjacent vertices vector for each vertex */
	for (int i = 0; i < mesh.num_vertices; i++)
	{
		mesh.vertices[i].adj_v = (std::vector<unsigned int>*)
						calloc(1,sizeof(std::vector<unsigned int>));
	}
	init_winged_edge();


    // TODO opengl initialization code
    return true;
}

/**
 * Clean up the project. Free any memory, etc.
 */
void GeometryProject::destroy()
{
  // TODO any cleanup code
  free(mesh.w_edges);
  free(mesh.faces);
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
	Vector3 d = camera->get_direction() + p;
	gluLookAt(p.x,p.y,p.z,d.x,d.y,d.z,u.x,u.y,u.z);

	/* set up light */
	GLfloat light_pos[] = {p.x,p.y,p.z, 0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/* set up material */
	if (mesh.texture == NULL)
	{
		GLfloat specular[] = {1,0,0,0};
		GLfloat shininess[] = {10};
		GLfloat colors[] = {1,.1,.05,0};
		glShadeModel(GL_SMOOTH);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colors);
	}
	else
	{
		glClientActiveTexture(mesh.texture);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, mesh.texture);
		GLfloat colors[] = {0,0,0,0};
	//	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colors);
	}

	/* load mesh */
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_DOUBLE, sizeof(Vertex), mesh.vertices);
	glNormalPointer(GL_DOUBLE, sizeof(Vertex), 
				((char*)mesh.vertices) + sizeof(Vector3));
	glTexCoordPointer(2, GL_DOUBLE, sizeof(Vertex),
				((char*)mesh.vertices) + sizeof(Vector3)*2);

	glDrawElements(GL_TRIANGLES, mesh.num_triangles*3, GL_UNSIGNED_INT,
				mesh.triangles);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glFlush();
}

/**
 * Advance a step in the simulation of the world 
 */
void GeometryProject::step()
{
	
}

/**
 * Subdivide the mesh that we are rendering using Loop subdivison.
 */
void GeometryProject::subdivide()
{
	/* increase the size of the vertex array */
	int new_size = mesh.num_vertices+mesh.num_w_edges;
	mesh.vertices = (Vertex*)realloc(mesh.vertices,
			(new_size)*sizeof(Vertex));

	generate_odds();
	/* reinitialize the adjacent vertices vector for each old vertex */
	for (int i = 0; i < mesh.num_vertices; i++)
	{
		mesh.vertices[i].adj_v->clear();
	}
	/* initialize the adjacent vertices vector for each new vertex */
	for (int i = mesh.num_vertices; i < new_size; i++)
	{
		mesh.vertices[i].adj_v = (std::vector<unsigned int>*)
						calloc(1,sizeof(std::vector<unsigned int>));
	}

	/* increase the size of triangle array */
	mesh.triangles = (Triangle*)realloc(mesh.triangles, 
			mesh.num_triangles*4*sizeof(Triangle));

	generate_triangles();

	/* once the triangles were generated, create the faces */
	mesh.num_triangles *= 4;
	mesh.faces = (Face*)realloc(mesh.faces, mesh.num_triangles*sizeof(Face));
	create_faces();

	/* increase the size of winged edge array */
	mesh.w_edges = (WingedEdge*)realloc(mesh.w_edges,
			mesh.num_triangles*3*sizeof(WingedEdge));
	/*re create edges */
	init_winged_edge();

	/* once the new edges were created, use them to recalculate even vertices */
	generate_evens();
	mesh.num_vertices = new_size;

	/* update each vertex's normals */
	calc_norms();
	
}


/**** HELPER FUNCTIONS ****/

/* ~~~~ structure - related functions ~~~~ */

/* init_winged_edge: creates a winged edge structure from the mesh's triangles
 *					 and vertices.
 */
void GeometryProject::init_winged_edge()
{
	mesh.num_w_edges = 0;
	/* loop through triangles to create edges */
	for (int i = 0; i < mesh.num_triangles; i++)
	{
		int num_e = mesh.num_w_edges-1;
		/* check if the 3 triangle edges already exist in the edge array */
		unsigned int edge1 = exists(&mesh.triangles[i].vertices[0],
						&mesh.triangles[i].vertices[1]);
		unsigned int edge2 = exists(&mesh.triangles[i].vertices[1],
						&mesh.triangles[i].vertices[2]);
		unsigned int edge3 = exists(&mesh.triangles[i].vertices[2],
						&mesh.triangles[i].vertices[0]);
		/* if the edge hasn't already been created 
		 * initialize it */
		if (signed(edge1) > num_e)
		{
			/* set start and end vertices */
			mesh.w_edges[edge1].start = mesh.triangles[i].vertices[0];
			mesh.w_edges[edge1].end = mesh.triangles[i].vertices[1];
			mesh.w_edges[edge1].odd_vtex = 0;
			/* add the corresponding adjacent vertices */
			mesh.vertices[mesh.w_edges[edge1].end].adj_v->push_back(
							mesh.w_edges[edge1].start);
			mesh.vertices[mesh.w_edges[edge1].start].adj_v->push_back(
							mesh.w_edges[edge1].end);
			/* set triangle and the triangle's succ/pred pointers */
			mesh.w_edges[edge1].left_tri = &mesh.faces[i];
			mesh.w_edges[edge1].left_tri->is_sub = 0;
		}
		else
		{
			mesh.w_edges[edge1].right_tri = &mesh.faces[i];
			mesh.w_edges[edge1].right_tri->is_sub = 0;
		}
		if (signed(edge2) > num_e)
		{
			/* set start and end vertices */
			mesh.w_edges[edge2].start = mesh.triangles[i].vertices[1];
			mesh.w_edges[edge2].end = mesh.triangles[i].vertices[2];
			mesh.w_edges[edge2].odd_vtex = 0;

			mesh.vertices[mesh.w_edges[edge2].end].adj_v->push_back(
							mesh.w_edges[edge2].start);
			mesh.vertices[mesh.w_edges[edge2].start].adj_v->push_back(
							mesh.w_edges[edge2].end);
			/* set triangle and succ/pred pointers */
			mesh.w_edges[edge2].left_tri = &mesh.faces[i];
			mesh.w_edges[edge2].left_tri->is_sub = 0;
		}
		else
		{
			mesh.w_edges[edge2].right_tri = &mesh.faces[i];
			mesh.w_edges[edge2].right_tri->is_sub = 0;
		}
		if (signed(edge3) > num_e)
		{
			/* set start and end vertices */
			mesh.w_edges[edge3].start = mesh.triangles[i].vertices[2];
			mesh.w_edges[edge3].end = mesh.triangles[i].vertices[0];
			mesh.w_edges[edge3].odd_vtex = 0;

			mesh.vertices[mesh.w_edges[edge3].end].adj_v->push_back(
							mesh.w_edges[edge3].start);
			mesh.vertices[mesh.w_edges[edge3].start].adj_v->push_back(
							mesh.w_edges[edge3].end);

			/* set triangle and succ/pred pointers */
			mesh.w_edges[edge3].left_tri = &mesh.faces[i];
			mesh.w_edges[edge3].left_tri->is_sub = 0; 
		}
		else
		{
			mesh.w_edges[edge3].right_tri = &mesh.faces[i];
			mesh.w_edges[edge3].right_tri->is_sub = 0;
		}	
		/* add these three edges into the current face */
		mesh.faces[i].e1 = &mesh.w_edges[edge1];
		mesh.faces[i].e2 = &mesh.w_edges[edge2];
		mesh.faces[i].e3 = &mesh.w_edges[edge3];

	}


}

/* generate_triangles: loops through all the edges, calculates all the new
 *					   triangles from odd vertices and inputs them into the mesh
 *					   triangle array
 */
void GeometryProject::generate_triangles()
{
	int tri_index = 0; /*index into triangle array */
	for (int i = 0; i < mesh.num_triangles; i++)
	{
		/* divide the current face if hasnt been subdivided already*/
		if (!mesh.faces[i].is_sub)
		{
			/* start with any of the 3 edges */
			/* find the start, end and the odd vertex' indices into the array */
			unsigned int start = mesh.faces[i].e1->start;
			unsigned int end = mesh.faces[i].e1->end;
			unsigned int odd = mesh.faces[i].e1->odd_vtex;
			/* get the other two odd vertices */
			unsigned int pred = mesh.faces[i].e2->odd_vtex;
			unsigned int succ = mesh.faces[i].e3->odd_vtex;
			

			/* get the even vertex of this triangle
			 * index into the vertex array*/
			unsigned int even = end_or_start(start, mesh.faces[i].e3);

			/* create the 4 triangles */
	/*1*/	mesh.triangles[tri_index].vertices[0] = odd;/*odd*/
			mesh.triangles[tri_index].vertices[1] = pred; /*pre_odd*/
			mesh.triangles[tri_index].vertices[2] = succ; /*suc_odd*/
			tri_index++;
	/*2*/	mesh.triangles[tri_index].vertices[0] = odd; /*odd*/
			mesh.triangles[tri_index].vertices[1] = end;
			mesh.triangles[tri_index].vertices[2] = pred; /*pre_odd*/
			tri_index++;
	/*3*/	mesh.triangles[tri_index].vertices[0] = odd; /*odd*/
			mesh.triangles[tri_index].vertices[1] = succ; /*suc_odd*/
			mesh.triangles[tri_index].vertices[2] = start; 
			tri_index++;
	/*4*/	mesh.triangles[tri_index].vertices[0] = succ; /*suc_odd*/
			mesh.triangles[tri_index].vertices[1] = pred; /*pre_odd*/
			mesh.triangles[tri_index].vertices[2] = even;
			tri_index++;

			/* set that this face has been subdivided already */
			mesh.faces[i].is_sub = 1;
		}
	}

}

/* create_faces: loops through all the triangles, and sets the corresponding
 *				faces structure to the triangle. Used as a separate way to keep
 *				track of what triangles have been subdivided already
 */
void GeometryProject::create_faces()
{
	for (int i = 0; i < mesh.num_triangles; i++)
	{
		mesh.faces[i].tri = &mesh.triangles[i];
		mesh.faces[i].is_sub = 0;
		mesh.faces[i].e1 = NULL;
		mesh.faces[i].e2 = NULL;
		mesh.faces[i].e3 = NULL;
	}

}

/* ~~~~ subdivision - related functions ~~~~ */

/* generate_odds: loops through all the edges, calculates and sets all the odd
 *				  vertices of each edge.
 *
 */
void GeometryProject::generate_odds()
{
	for (int i = 0; i < mesh.num_w_edges; i ++)
	{
		if (mesh.w_edges[i].odd_vtex < mesh.num_vertices)
		{
			mesh.w_edges[i].odd_vtex = mesh.num_vertices+i;

			/* boundary edge case * 
			 * if it's a boundary, calculate the odd vertex using only its 
			 * start and end vertices */
			if (is_boundary(&mesh.w_edges[i]))
			{
				Vector3 a = mesh.vertices[mesh.w_edges[i].start].position;
				Vector3 b = mesh.vertices[mesh.w_edges[i].end].position;
				mesh.vertices[mesh.num_vertices+i].position = a*0.5 + b*0.5;
			}
			/* otherwise get the 4 adjacent vertices and calculate it that way */
			else
			{
				Vector3 a = mesh.vertices[mesh.w_edges[i].start].position;
				Vector3 b = mesh.vertices[mesh.w_edges[i].end].position;

				Face* left = mesh.w_edges[i].left_tri;
				Face* right = mesh.w_edges[i].right_tri;

				Vector3 c = mesh.vertices[get_even_vtex(&mesh.w_edges[i], left)].position;
				Vector3 d = mesh.vertices[get_even_vtex(&mesh.w_edges[i], right)].position;
				mesh.vertices[mesh.num_vertices+i].position = a*3.0/8 + b*3.0/8
														+ (c+d)*1.0/8;
			}

		}
	}

}

unsigned int GeometryProject::get_even_vtex(WingedEdge* curr, Face* face)
{
	if (face->e1 == curr)
		return end_or_start(curr->start, face->e3);
	else if (face->e2 == curr)
		return end_or_start(curr->start, face->e3);
	else
		return end_or_start(curr->start, face->e1);
}
/* generate_evens: loops through all the edges, calculates and sets all the odd
 *				  vertices of each edge.
 */
void GeometryProject::generate_evens()
{
	for (int i = 0; i < mesh.num_w_edges; i++)
	{
		/* since i add the odd vertices to the end of the vertex array, I
		 * know that any index prior to the addition is the original vertex */
		if (mesh.w_edges[i].start < mesh.num_vertices)
		{
			int count = mesh.vertices[mesh.w_edges[i].start].adj_v->size();

			if (!is_boundary(&mesh.w_edges[i]))
			{	
				float beta = (1.0/(count))*(5.0/8 -
						pow(3.0/8+1.0/4*cos(2*3.14159/count), 2));
				Vector3 sum = mesh.vertices[mesh.vertices[mesh.w_edges[i].start].adj_v->at(0)].position;
				for (int j = 1; j < count; j++)
				{
					unsigned int index = mesh.vertices[mesh.w_edges[i].start].adj_v->at(j);
					sum += mesh.vertices[index].position;
				}
				Vector3 result = (1 - beta*count) *
					mesh.vertices[mesh.w_edges[i].start].position + beta*sum;
				mesh.vertices[mesh.w_edges[i].start].position = result;
			}
			else
			{
				unsigned int index1 = mesh.vertices[mesh.w_edges[i].start].adj_v->at(0);
				unsigned int index2 = mesh.vertices[mesh.w_edges[i].start].adj_v->at(1);
				Vector3 sum = mesh.vertices[index1].position * 1.0/8 +
							mesh.vertices[index2].position * 1.0/8;
				Vector3 result = mesh.vertices[mesh.w_edges[i].start].position*3.0/4
								+ sum;
				mesh.vertices[mesh.w_edges[i].start].position = result;
			}
		}
	}
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
	Vector3 s = mesh.vertices[*start].position;
	Vector3 e = mesh.vertices[*end].position;
	
	for (int i = 0; i < mesh.num_w_edges; i ++)
	{
		if ((mesh.vertices[mesh.w_edges[i].start].position == e &&
				mesh.vertices[mesh.w_edges[i].end].position == s) ||
				(mesh.vertices[mesh.w_edges[i].start].position == s &&
				mesh.vertices[mesh.w_edges[i].end].position == e))
			return i;
	}
	/* update the total number of edges because if it reaches this point, the
	 * edge was not found and is going to be created anyway */
	mesh.num_w_edges++;
	return mesh.num_w_edges-1;
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
	if (curr != edge->start)
		return edge->start;
	else
		return edge->end;
}

/*  is_boundary: checks whether the given edge is a boundary edge or not
 *    Parameters:
 *		 w_edge - the edge to be checked for boundary
 *    Return value:
 *			true if it's a boundary edge, false otherwise
 */
bool GeometryProject::is_boundary(WingedEdge* w_edge)
{
	return (w_edge->right_tri == NULL || w_edge->left_tri == NULL);
}

/* calc_norms: Calculates the normals of the vertices and stores them into a
 *				the normals of each vertex in the vertices array
 */
void GeometryProject::calc_norms()
{
	Triangle* tris = mesh.triangles;
	Vertex* vtex = mesh.vertices;
	int nums_t = mesh.num_triangles;
	int nums_v = mesh.num_vertices;
	for (int i = 0; i < nums_t; i++)
	{
		/* get vectors from vertices */
		Triangle curr_tri = tris[i];
		Vector3 a = vtex[curr_tri.vertices[0]].position - 
			vtex[curr_tri.vertices[1]].position;
		Vector3 b = vtex[curr_tri.vertices[2]].position - 
			vtex[curr_tri.vertices[1]].position;

		/* calculate triangle normal */
		Vector3 norm = cross(b,a);
		norm = normalize(norm);
		/* add the normal to the corresponding vertices normals */
		for (int j = 0; j < 3; j++)
			vtex[curr_tri.vertices[j]].normal += norm;

	}
	/* normalize the normal array */
	for (int i = 0; i < nums_v; i++)
		vtex[i].normal = normalize(vtex[i].normal);
}

} /* _462 */

