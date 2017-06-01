#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <string>
#include <vector>
#include <fstream>

using namespace glm;
using namespace std;

GLuint VBO;
//GLuint gScaleLocation;
//TODO 

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);
	if (VertexShaderStream.is_open()) {
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, ios::in);
	if (FragmentShaderStream.is_open()) {
		string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	GLint Result = GL_FALSE;
	int InfoLogLength;
	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);
	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}
	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);
	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}
	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

mat4 applyPerspective(mat4 input) {
	mat4 view = glm::lookAt(
		glm::vec3(0, 0, 100), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	return perspective(radians(45.0f), (float)16 / (float)9, 0.1f, 150.0f) * view * input;
}
class Bullet {

};

class Player {
	GLuint vertexBuffer;
	GLuint programID;
	GLuint posistion;
	GLuint colorID;

	vec3 coords = vec3(0.0f,0.0f,0.0f);
	vec3 color = vec3(1,0,1);
	vec3 verts[3];
	mat4 model = mat4(1.0f);
	mat4 translation = mat4(1.0f);
	mat4 rotation = mat4(1.0f);
	mat4 result = mat4(1.0f);
	mat4 origin = mat4(1.0f);
public:
	float x = 0;
	float y = 0;
	float rot = 0;
	float v = 0;
	float dV=0;
	float dRot=0;
	float dX = 0;
	float dY = 0;

	void compileShaders(char *vsPath, char* fsPath) {
		programID = LoadShaders(vsPath, fsPath);
	}
	void assertUniformLocation() {
		posistion = glGetUniformLocation(programID, "posistion");
		assert(posistion != 0xFFFFFFFF);
		colorID = glGetUniformLocation(programID, "colorIn");
		assert(posistion != 0xFFFFFFFF);
		//rotation = glGetUniformLocation(programID, "rotation");
		//assert(rotation != 0xFFFFFFFF);
		//gScaleLocation = glGetUniformLocation(programID, "gScale");
		//assert(gScaleLocation != 0xFFFFFFFF);
	}
	void setProgramID(GLuint ID) {
		programID = ID;
		assertUniformLocation();
	}
	void update() {
		//coords
		coords = vec3(coords.x+ sinf(radians(rot))*v, coords.y+ cosf(radians(rot))*v, 1);
		//FIX translation matrix
		if (dV < .001 && dV > -.001) {
			v += dV;
		}
		else if (dV > .001) {
			dV = .001;
			v += dV;
		}
		else if (dV < -.001) {
			dV = -.001;
			v += dV;
		}
		if (x < -75) {
			x = 75;
		}
		else if (x > 75) {
			x = -75;
		}
		if (y > 40) {
			y = -40;
		}
		else if (y < -40) {
			y = 40;
		}
		v += dV;
		dX = -sinf(rot)*v;
		dY = cosf(rot)*v;
		x += dX;
		y += dY;
		//result = perxpective * translation * roatation * translation center * model
		/**
		origin = translate(model, vec3(0, 0, 0));
		rotation = rotate(origin, rot, vec3(0, 0, 1));
		translation = translate(rotation, vec3(x, y, 0));
		printf("Center %d,%d\n", x, y);

		result = applyPerspective(translation);
		*/
		result = applyPerspective(translate(translation, vec3(x, y, 0))*(rotate(rotation,rot,vec3(0,0,1))*translate(model,vec3(0,0,0))));
		printf("Center %F,%F\n", x, y);

		//result = applyPerspective(translate(rotate(translate(model,vec3(0,0,0)), rot, vec3(0.0f,0.0f,1.0f)), vec3(x, y, 0)));
		glUniformMatrix4fv(posistion, 1, GL_FALSE, &result[0][0]);
		glUniform3f(colorID, color.x,color.y,color.z);

	}
	void draw() {
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(0);
	}
	void render() {
		glUseProgram(programID);
		update();
		draw();
		glUseProgram(0);
	}
	Player() {}
	Player(GLuint vertexB) {
		vertexBuffer = vertexB;

		verts[0] = vec3(-1.0f, -1.0f, 0.0f);
		verts[1] = vec3(1.0f, -1.0f, 0.0f);
		verts[2] = vec3(0.0f, 1.0f, 0.0f);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
	}
	Player(GLuint vertexB, vec3 iVerts[3]) {
		vertexBuffer = vertexB;

		verts[0] = iVerts[0];
		verts[1] = iVerts[1];
		verts[2] = iVerts[2];

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
	}
};

class Triangle {
	GLuint vertexBuffer;
	GLuint programID;
	GLuint posistion;
	GLuint colorID;

	vec3 coords = vec3(0.0f, 0.0f, 0.0f);
	vec3 color = vec3(1, 1, 1);
	vec3 verts[3];
	mat4 model = mat4(1.0f);
	mat4 translation = mat4(1.0f);
	mat4 rotation = mat4(1.0f);
	mat4 result = mat4(1.0f);
	mat4 origin = mat4(1.0f);
public:
	float x = 0;
	float y = 0;
	float rot = 0;
	float v = 0;
	float dV = 0;
	float dRot = 0;
	float dX = 0;
	float dY = 0;

	void compileShaders(char *vsPath, char* fsPath) {
		programID = LoadShaders(vsPath, fsPath);
	}
	void assertUniformLocation() {
		posistion = glGetUniformLocation(programID, "posistion");
		assert(posistion != 0xFFFFFFFF);
		colorID = glGetUniformLocation(programID, "colorIn");
		assert(posistion != 0xFFFFFFFF);
		//rotation = glGetUniformLocation(programID, "rotation");
		//assert(rotation != 0xFFFFFFFF);
		//gScaleLocation = glGetUniformLocation(programID, "gScale");
		//assert(gScaleLocation != 0xFFFFFFFF);
	}
	void setProgramID(GLuint ID) {
		programID = ID;
		assertUniformLocation();
	}
	void update() {
		//coords
		coords = vec3(coords.x + sinf(radians(rot))*v, coords.y + cosf(radians(rot))*v, 1);
		//FIX translation matrix
		if (dV < .001 && dV > -.001) {
			v += dV;
		}
		else if (dV > .001) {
			dV = .001;
			v += dV;
		}
		else if (dV < -.001) {
			dV = -.001;
			v += dV;
		}

		v += dV;
		dX = -sinf(rot)*v;
		dY = cosf(rot)*v;
		x += dX;
		y += dY;
		//result = perxpective * translation * roatation * translation center * model
		/**
		origin = translate(model, vec3(0, 0, 0));
		rotation = rotate(origin, rot, vec3(0, 0, 1));
		translation = translate(rotation, vec3(x, y, 0));
		printf("Center %d,%d\n", x, y);

		result = applyPerspective(translation);
		*/
		result = applyPerspective(translate(translation, vec3(x, y, 0))*(rotate(rotation, rot, vec3(0, 0, 1))*translate(model, vec3(0, 0, 0))));
		//printf("Center %d,%d\n", x, y);
		//result = applyPerspective(translate(rotate(translate(model,vec3(0,0,0)), rot, vec3(0.0f,0.0f,1.0f)), vec3(x, y, 0)));
		glUniformMatrix4fv(posistion, 1, GL_FALSE, &result[0][0]);
		glUniform3f(colorID, color.x, color.y, color.z);

	}
	void draw() {
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(0);
	}
	void render() {
		glUseProgram(programID);
		update();
		draw();
		glUseProgram(0);
	}
	Triangle() {}
	Triangle(GLuint vertexB) {
		vertexBuffer = vertexB;

		verts[0] = vec3(-1.0f, -1.0f, 0.0f);
		verts[1] = vec3(1.0f, -1.0f, 0.0f);
		verts[2] = vec3(0.0f, 1.0f, 0.0f);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
	}
	Triangle(GLuint vertexB, vec3 iVerts[3]) {
		vertexBuffer = vertexB;

		verts[0] = iVerts[0];
		verts[1] = iVerts[1];
		verts[2] = iVerts[2];

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
	}
};

vector<Triangle> tris;
Player player;

//Node Stuff
/*
/* Joint Class If wieght is -, connection is nonexistant / 
struct Joint { float weight; bool enabled; int	hisMark; };
typedef vector<vector<Joint>> network;
struct Node { float sumWeight; };

//TODO GRAPHICS //	Buffers //	Inexes, transformations and colors
class Organism{ private: BYTE r, g, b; Triangle tri; string vert, frag;

int n;//nodes
Node *nodes;
network net;

//Compile Shaders

//Transformations

//Input Nodes

//Output nodes
public: int getN() { return n; }
		network getNet() { return net; } 
		Joint getJoint(int r, int d) { return net[r][d]; }

		void setJoint(int r, int d, Joint input) {
			net[r][d] = input;
		}

		void setJoint(int r, int d, float w, bool e, int h) {
			Joint temp;
			temp.enabled = e;
			temp.weight = w;
			temp.hisMark = h;
			net[r][d] = temp;
		}

		void resizeArray(int inputN) {
			n = inputN;
			net.resize(n);
			for (int i = 0; i < n; i++) {
				net[i].resize(n);
			}
		}
		void update() {

		}
		void render() {

		}
		/*
		Copies Smaller Net into LArger Net where Larger net have null points
		TODO conections to same nodes:: averge weights??? if one is disabled?????
		TODO Mutations done in evolve nets or combine nets??????
		/
		network combineNets(network a, network b) {
			network result;
			if (a.size >= b.size) {
				result = a;
				for (int r = 0; r < result.size; r++) {
					for (int c = 0; c < result.size; c++) {
						if (result[r][c].weight <= 0.0f) {
							result[r][c] = b[r][c];
						}
						else {
							result[r][c].weight = (b[r][c].weight + a[r][c].weight) / 2;
						}
					}
				}
			}
			else {
				result = b;
				for (int r = 0; r < result.size; r++) {
					for (int c = 0; c < result.size; c++) {
						if (result[r][c].weight <= 0.0f) {
							result[r][c] = a[r][c];
						}
						else {
							result[r][c].weight = (b[r][c].weight + a[r][c].weight) / 2;
						}
					}
				}
				return result;
			}
		}
		void mutateNet() {

		}
		Organism() {//Randomly Generates Simple Neural net

		}
		Organism(Organism parent1, Organism parent2) {
			net = combineNets(parent1.getNet(), parent2.getNet());
			mutateNet();
		}
};
///
*/
static void update() {

}

static void render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	for (int i = 0; i < tris.size(); i++) {
		tris[i].render();
	}
	
	player.render();
	glutSwapBuffers();
	//glutPostRedisplay();//Research
}

static void input(int key, int x, int y) {
	switch (key)
	{
	/*
	case GLUT_KEY_UP: printf("GLUT_KEY_LEFT %d\n", key); player.v += .01; break;
	case GLUT_KEY_DOWN: printf("GLUT_KEY_LEFT %d\n", key); player.v -= .01;   break;
	case GLUT_KEY_LEFT: printf("GLUT_KEY_LEFT %d\n", key);  player.rot -= radians(1.0f); break;
	case GLUT_KEY_RIGHT: printf("GLUT_KEY_LEFT %d\n", key); player.rot += radians(1.0f);  break;
	*/
	case GLUT_KEY_UP: printf("GLUT_KEY_LEFT %d\n", key); player.v += .0001; break;
	case GLUT_KEY_DOWN: printf("GLUT_KEY_LEFT %d\n", key); player.v -= .0001;   break;
	case GLUT_KEY_LEFT: printf("GLUT_KEY_LEFT %d\n", key);  player.rot += radians(5.0f); break;
	case GLUT_KEY_RIGHT: printf("GLUT_KEY_LEFT %d\n", key); player.rot -= radians(5.0f);  break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1600, 900);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Rendering System");

	glutIdleFunc(render);//Called every glut update/ frame
	glutDisplayFunc(render);//Only called when window is resized or moved

	glutSpecialFunc(input);

	// Must be done after glut is initialized!
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	if (GLEW_VERSION_3_2)
	{
		fprintf(stdout, "OpenGL 3.2 ready", glewGetString(GLEW_VERSION));
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	GLuint programID = LoadShaders("test.vs", "test.fs");
	
	vec3 tri1[3];
	tri1[0] = vec3(	1.0f,  1.0f, 0.0f);
	tri1[1] = vec3( 0.0f,  1.0f, 0.0f);
	tri1[2] = vec3( 0.0f,  0.0f, 0.0f);
	vec3 tri2[3];
	tri2[0] = vec3(0.0f,  0.0f, 0.0f);
	tri2[1] = vec3(1.0f, 0.0f, 0.0f);
	tri2[2] = vec3(0.0f, -1.0f, 0.0f);
	vec3 tri3[3];
	tri3[0] = vec3(-1.0f, -1.0f, 0.0f);
	tri3[1] = vec3(0.0f, -1.0f, 0.0f);
	tri3[2] = vec3(0.0f, 0.0f, 0.0f);
	
	tris.push_back(Triangle(VBO, tri1));
	tris.push_back(Triangle(VBO, tri2));
	tris.push_back(Triangle(VBO, tri3));
	
	
	//TODO INDVIDUALIZED SHADERS
	tris[0].setProgramID(programID);
	tris[1].setProgramID(programID);
	tris[2].setProgramID(programID);
	

	vec3 playTri[3];
	playTri[0] = vec3(-1.0f, -1.0f, 0.0f);
	playTri[1] = vec3(0.0f, 1.0f, 0.0f);
	playTri[2] = vec3(1.0f, -1.0f, 0.0f);

	player = Player(VBO, playTri);
	player.setProgramID(programID);

	glutMainLoop();

	return 0;
}