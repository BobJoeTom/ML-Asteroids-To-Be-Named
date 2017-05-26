# ML-Asteroids-To-Be-Named
/*

Copyright 2010 Etay Meiri

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Tutorial 03 - First triangle

hi brendan 
i 
love
you
-_-		
*/
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <vector>;

using namespace glm;
using namespace std;

typedef vector<vector<Joint>> network;

GLuint VBO;

class VertexArray{

};
/*
Joint Class
If wieght is -, connection is nonexistant
*/
struct Joint{
	float weight;
	bool enabled;
	int	hisMark;
};

struct Node{
	float sumWeight;
};


//TODO GRAPHICS
//	Buffers
//	Inexes, transformations and colors
class Organism{
private:
	BYTE r, g, b;
	VertexArray verArr;
	string vert, frag;

	int n;//nodes
	Node *nodes;
	network net;

	//Compile Shaders

	//Transformations

	//Input Nodes

	//Output nodes
public:
	int getN(){
		return n;
	}
	network getNet(){
		return net;
	}
	Joint getJoint(int r, int d){
		return net[r][d];
	}

	void setJoint(int r, int d, Joint input){
		net[r][d] = input;
	}

	void setJoint(int r, int d, float w, bool e, int h){
		Joint temp;
		temp.enabled = e;
		temp.weight = w;
		temp.hisMark = h;
		net[r][d] = temp;
	}

	void resizeArray(int inputN){
		n = inputN;
		net.resize(n);
		for (int i = 0; i < n; i++){
			net[i].resize(n);
		}
	}
	void update(){

	}
	void render(){

	}
	/*
	Copies Smaller Net into LArger Net where Larger net have null points
	TODO conections to same nodes:: averge weights??? if one is disabled?????
	TODO Mutations done in evolve nets or combine nets??????
	*/
	network combineNets(network a, network b){
		network result;
		if (a.size >= b.size){
			result = a;
			for (int r = 0; r < result.size; r++){
				for (int c = 0; c < result.size; c++){

					if (result[r][c].weight <= 0.0f){
						result[r][c] = b[r][c];
					}
					else{
						result[r][c].weight = (b[r][c].weight + a[r][c].weight) / 2;
					}

				}
			}
		}
		else{
			result = b;
			for (int r = 0; r < result.size; r++){
				for (int c = 0; c < result.size; c++){

					if (result[r][c].weight <= 0.0f){
						result[r][c] = a[r][c];
					}
					else{
						result[r][c].weight = (b[r][c].weight + a[r][c].weight) / 2;
					}

				}
			}
			return result;
		}
	}
	void mutateNet(){

	}
	Organism(){//Randomly Generates Simple Neural net
		
	}
	Organism(Organism parent1, Organism parent2){
		net = combineNets(parent1.getNet(), parent2.getNet());
		mutateNet();
	}
};




//+++++++++++++++++++++++++++++++++++Rendering

static void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);

	glutSwapBuffers();
}

static void InitializeGlutCallbacks()
{
	glutDisplayFunc(render);
}

static void CreateVertexBuffer()
{
	vec3 Vertices[6];
	Vertices[0] = vec3(-1.0f, -1.0f, 0.0f);
	Vertices[1] = vec3(1.0f, -1.0f, 0.0f);
	Vertices[2] = vec3(0.0f, 0.0f, 0.0f);
	Vertices[3] = vec3(-1.0f, 1.0f, 0.0f);
	Vertices[4] = vec3(1.0f, 1.0f, 0.0f);
	Vertices[5] = vec3(0.0f, 0.0f, 0.0f);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Tutorial 03");

	InitializeGlutCallbacks();

	// Must be done after glut is initialized!
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	CreateVertexBuffer();

	glutMainLoop();

	return 0;
}
