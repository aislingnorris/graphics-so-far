
//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "maths_funcs.h"
#include "text.h"

#include <stdlib.h>     
#include <time.h>      


//#include <GLFW/glfw3.h>

// Assimp includes

#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

//text
const char* Timage = "C:/Users/Aisling/Documents/Visual Studio 2012/Projects/Project1/Project1/freemono.png";
const char* Tmeta = "C:/Users/Aisling/Documents/Visual Studio 2012/Projects/Project1/Project1/freemono.meta";


//text time id 
int time_id = -1;


/*----------------------------------------------------------------------------
                   MESH TO LOAD
  ----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here

#define TONGUE_MESH "C:/Users/Aisling/Documents/Visual Studio 2012/Projects/Project1/Project1/models/lick1.obj"
#define HALF_LICK_MESH "C:/Users/Aisling/Documents/Visual Studio 2012/Projects/Project1/Project1/models/lick2.obj"
#define FULL_LICK_MESH "C:/Users/Aisling/Documents/Visual Studio 2012/Projects/Project1/Project1//models/lick3.obj"

#define PLANE_MESH "C:/Users/Aisling/Documents/Visual Studio 2012/Projects/Project1/Project1/models/plane.obj"

#define PLAYER_MESH "C:/Users/Aisling/Documents/Visual Studio 2012/Projects/Project1/Project1/models/player.obj"

#define LOLIPOP_MESH "C:/Users/Aisling/Documents/Visual Studio 2012/Projects/Project1/Project1/models/lolipop.obj"
#define SWEET_MESH "C:/Users/Aisling/Documents/Visual Studio 2012/Projects/Project1/Project1/models/wrapped_sweet.obj"
#define JELLY_MESH "C:/Users/Aisling/Documents/Visual Studio 2012/Projects/Project1/Project1/models/chcnpl.obj"


#define APPLE_MESH "C:/Users/Aisling/Documents/Visual Studio 2012/Projects/Project1/Project1/models/apple.obj"
#define BANANA_MESH "C:/Users/Aisling/Documents/Visual Studio 2012/Projects/Project1/Project1/models/banana.obj"
#define ORANGE_MESH "C:/Users/Aisling/Documents/Visual Studio 2012/Projects/Project1/Project1/models/Orange.obj"


/*----------------------------------------------------------------------------
  ----------------------------------------------------------------------------*/

int NUM_MESHES = 10;

std::vector<float>* g_vp = new std::vector<float>[NUM_MESHES];
std::vector<float>* g_vn = new std::vector<float>[NUM_MESHES];
std::vector<float>* g_vt = new std::vector<float>[NUM_MESHES];


int* g_point_count = new int[NUM_MESHES];

GLuint VAOs[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;
GLuint shaderProgramID;

float xxxx = 0.0f;
float zzzz= 0.0f;

unsigned int mesh_vao = 0;
int width = 1200;
int height = 1000;

float beginning_cam_z;

float camerax = 10;
float cameray = 196;
float cameraz = -288.0f;

bool first_go = true;
bool start = true;

float sweet_locations[30][2];
bool sweet_uneaten[30];

float bushes_locations[30][2];

vec3 plane_location = vec3 (0.0f, 0.0f, 0.0f);
vec3 tongue_location = vec3(8.0f, -1.0f, -270.0f);


//max_bounds LBC(-70,0-70) RBC (70,0-70) LFC (-70,0,70) RFC (70,0,70)

int runner = 0;
int cut_scene = 0;

GLuint loc1, loc2, loc3;
GLfloat rotate_y = 0.0f;




#pragma region MESH LOADING
/*----------------------------------------------------------------------------
                   MESH LOADING FUNCTION
  ----------------------------------------------------------------------------*/

bool load_mesh (const char* file_name, int model_number) {
  const aiScene* scene = aiImportFile (file_name, aiProcess_Triangulate); // TRIANGLES!
        //fprintf (stderr, "ERROR: reading mesh %s\n", file_name);
  if (!scene) {
    fprintf (stderr, "ERROR: reading mesh %s\n", file_name);
    return false;
  }
  printf ("  %i animations\n", scene->mNumAnimations);
  printf ("  %i cameras\n", scene->mNumCameras);
  printf ("  %i lights\n", scene->mNumLights);
  printf ("  %i materials\n", scene->mNumMaterials);
  printf ("  %i meshes\n", scene->mNumMeshes);
  printf ("  %i textures\n", scene->mNumTextures);
  
  g_point_count[model_number] = 0;

  for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
    const aiMesh* mesh = scene->mMeshes[m_i];
    printf ("    %i vertices in mesh\n", mesh->mNumVertices);
    g_point_count[model_number] += mesh->mNumVertices;
    for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
      if (mesh->HasPositions ()) {
        const aiVector3D* vp = &(mesh->mVertices[v_i]);
        //printf ("      vp %i (%f,%f,%f)\n", v_i, vp->x, vp->y, vp->z);
        g_vp[model_number].push_back (vp->x);
        g_vp[model_number].push_back (vp->y);
        g_vp[model_number].push_back (vp->z);
      }
      if (mesh->HasNormals ()) {
        const aiVector3D* vn = &(mesh->mNormals[v_i]);
        //printf ("      vn %i (%f,%f,%f)\n", v_i, vn->x, vn->y, vn->z);
        g_vn[model_number].push_back (vn->x);
        g_vn[model_number].push_back (vn->y);
        g_vn[model_number].push_back (vn->z);
      }
      if (mesh->HasTextureCoords (0)) {
        const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
        //printf ("      vt %i (%f,%f)\n", v_i, vt->x, vt->y);
        g_vt[model_number].push_back (vt->x);
        g_vt[model_number].push_back (vt->y);
      }
      if (mesh->HasTangentsAndBitangents ()) {
        // NB: could store/print tangents here
      }
    }
  }
  
  aiReleaseImport (scene);
  return true;
}

#pragma endregion MESH LOADING

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS

// Create a NULL-terminated string by reading the provided file
char* readShaderSource(const char* shaderFile) {   
    FILE* fp = fopen(shaderFile, "rb"); //!->Why does binary flag "RB" work and not "R"... wierd msvc thing?

    if ( fp == NULL ) { return NULL; }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);
    char* buf = new char[size + 1];
    fread(buf, 1, size, fp);
    buf[size] = '\0';

    fclose(fp);

    return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }
	const char* pShaderSource = readShaderSource( pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
    glCompileShader(ShaderObj);
    GLint success;
	// check for shader related errors using glGetShaderiv
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }
	// Attach the compiled shader object to the program object
    glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders()
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
    shaderProgramID = glCreateProgram();
    if (shaderProgramID == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

	// Create two shader objects, one for the vertex, and one for the fragment shader
    AddShader(shaderProgramID, "../Shaders/simpleVertexShader.txt", GL_VERTEX_SHADER);
    AddShader(shaderProgramID, "../Shaders/simpleFragmentShader.txt", GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };
	// After compiling all shader objects and attaching them to the program, we can finally link it
    glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
    glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
    glUseProgram(shaderProgramID);
	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS

void generateObjectBufferMesh(char* file_path, int model_number) {
/*----------------------------------------------------------------------------
                   LOAD MESH HERE AND COPY INTO BUFFERS
  ----------------------------------------------------------------------------*/

	//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
	//Might be an idea to do a check for that before generating and binding the buffer.

	load_mesh (file_path, model_number);
	unsigned int vp_vbo = 0;
	loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2 = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc3 = glGetAttribLocation(shaderProgramID, "vertex_texture");

	glGenBuffers (1, &vp_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vp_vbo);
	glBufferData (GL_ARRAY_BUFFER, g_point_count[model_number] * 3 * sizeof (float), &g_vp[model_number][0], GL_STATIC_DRAW);
	unsigned int vn_vbo = 0;
	glGenBuffers (1, &vn_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vn_vbo);
	glBufferData (GL_ARRAY_BUFFER, g_point_count[model_number] * 3 * sizeof (float), &g_vn[model_number][0], GL_STATIC_DRAW);

//	This is for texture coordinates which you don't currently need, so I have commented it out
//	unsigned int vt_vbo = 0;
//	glGenBuffers (1, &vt_vbo);
//	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
//	glBufferData (GL_ARRAY_BUFFER, g_point_count * 2 * sizeof (float), &g_vt[0], GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &VAOs[model_number]);

	glBindVertexArray (VAOs[model_number]);

	glEnableVertexAttribArray (loc1);
	glBindBuffer (GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer (loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray (loc2);
	glBindBuffer (GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer (loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

//	This is for texture coordinates which you don't currently need, so I have commented it out
//	glEnableVertexAttribArray (loc3);
//	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
//	glVertexAttribPointer (loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	
}


#pragma endregion VBO_FUNCTIONS

void collision_detection(){
	
	for(int i = 0; i < 24; i ++){
		if(sweet_uneaten[i] == true){
			if(xxxx >= (sweet_locations[i][0] - 5) && xxxx <= (sweet_locations[i][0] + 5)
				&& zzzz >= (sweet_locations[i][1] - 5) && zzzz <= (sweet_locations[i][1] + 5)){
					sweet_uneaten[i] = false;
			}
		}
	}
}

float random_poisition(){
   if (start) 
   {  
      srand(time(NULL)); //seeding for the first time only!
      start = false;
   }
   float p = (float) ((-100) + rand() % (100 - (-100)));

   return p;

}

void draw_player(int matrix_location, int view_mat_location, int proj_mat_location, mat4 plane) {

	//bushes
	mat4 model_player = identity_mat4 ();	
	model_player = translate (model_player, vec3(xxxx, 5, zzzz));
	mat4 play = model_player * plane;

	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, play.m);

	glBindVertexArray(VAOs[4]);
	glDrawArrays (GL_TRIANGLES, 0, g_point_count[4]);


}

void draw_lolipops(int index, int matrix_location, int view_mat_location, int proj_mat_location, mat4 plane) {

	//lolipop
	mat4 model_lolipop = identity_mat4 ();	
	model_lolipop = translate (model_lolipop, vec3(sweet_locations[index][0], 0, sweet_locations[index][1])); 
	//model_lolipop = scale(model_lolipop, vec3(0.5,0.5,0.5));
	mat4 loli = model_lolipop * plane;

	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, loli.m);

	glBindVertexArray(VAOs[5]);
	glDrawArrays (GL_TRIANGLES, 0, g_point_count[5]);


}

void draw_sweets(int index, int matrix_location, int view_mat_location, int proj_mat_location, mat4 plane) {

	//sweets
	mat4 model_sweets = identity_mat4 ();	
	model_sweets = rotate_y_deg(model_sweets, 10 * index);
	model_sweets = translate (model_sweets, vec3(sweet_locations[index][0], 0, sweet_locations[index][1])); 
	//model_sweets = scale(model_sweets, vec3(0.5,0.5,0.5));
	mat4 swe = model_sweets * plane;

	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, swe.m);

	glBindVertexArray(VAOs[6]);
	glDrawArrays (GL_TRIANGLES, 0, g_point_count[6]);


}

void draw_jellies(int index, int matrix_location, int view_mat_location, int proj_mat_location, mat4 plane) {

	//jelies
	mat4 model_jellies = identity_mat4 ();	
	model_jellies = translate (model_jellies, vec3(sweet_locations[index][0], 0, sweet_locations[index][1])); 
	//model_sweets = scale(model_sweets, vec3(0.5,0.5,0.5));
	mat4 jell = model_jellies * plane;

	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, jell.m);

	glBindVertexArray(VAOs[7]);
	glDrawArrays (GL_TRIANGLES, 0, g_point_count[7]);


}

void draw_half_lick(int matrix_location, int view_mat_location, int proj_mat_location, mat4 global){
	

	mat4 model_half_lick = identity_mat4 ();
	model_half_lick = translate (model_half_lick, vec3 (0,0,0));
	//model_half_lick = scale(model_half_lick, vec3 (5, 5, 5));
	mat4 tong = model_half_lick * global;

	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, tong.m);

	glBindVertexArray(VAOs[1]);
	glDrawArrays (GL_TRIANGLES, 0, g_point_count[1]);

}

void draw_full_lick(int matrix_location, int view_mat_location, int proj_mat_location, mat4 global){
	
	mat4 model_full_lick = identity_mat4 ();
	model_full_lick = translate (model_full_lick, vec3 (0,0,0));
	//model_full_lick = scale(model_full_lick, vec3 (5, 5, 5));
	mat4 tong = model_full_lick * global;

	glUniformMatrix4fv (matrix_location, 1, GL_FALSE, tong.m);

	glBindVertexArray(VAOs[2]);
	glDrawArrays (GL_TRIANGLES, 0, g_point_count[2]);

}

void display(){

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor (1.0f, 0.5f, 0.5f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram (shaderProgramID);


	//text
	double t = (double)glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	char tmp[256];
	sprintf(tmp, "The time is: %f\n", t);
	update_text(time_id, tmp);
	float r = fabs(sinf(t));
	float g = fabs(sinf(t + 1.57f));
	change_text_colour(time_id, r, g, 0.0f, 1.0f);


	//draw_texts ();

	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation (shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation (shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation (shaderProgramID, "proj");


	mat4 cam = look_at(vec3(camerax, cameray, cameraz), vec3 (10,0,0), vec3(0,1,0));

	//cout << camerax << "," << cameray << "," << cameraz << endl;

	//plane
		mat4 persp_proj_plane = perspective(45.0, (float)width/(float)height, 0.1, 1000.0);
		mat4 model_plane = identity_mat4 ();
		model_plane = scale(model_plane, vec3(3, 3, 3));
		model_plane = translate (model_plane, plane_location);

		glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, persp_proj_plane.m);
		glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, cam.m);
		glUniformMatrix4fv (matrix_location, 1, GL_FALSE, model_plane.m);

		glBindVertexArray(VAOs[3]);

	//tongue & animations
	if(runner < 3750){
		cameray = 0.0f;

		//tongue
		mat4 model_tongue = identity_mat4 ();
		model_tongue = translate (model_tongue, tongue_location);
		mat4 tong = model_tongue;

		glUniformMatrix4fv (matrix_location, 1, GL_FALSE, tong.m);
		glBindVertexArray(VAOs[0]);

		if((runner < 250) || (runner >=750 && runner < 1000) || (runner >= 1500 && runner < 1750)|| (runner >= 2250 && runner < 2500)|| (runner >= 3000 && runner < 3250)){
			glDrawArrays (GL_TRIANGLES, 0, g_point_count[0]);
			runner++;
		}
		if((runner >= 250 && runner < 500) || (runner >= 1000 && runner < 1250)|| (runner >= 1750 && runner < 2000)|| (runner >= 2500 && runner < 2750)|| (runner >= 3250 && runner < 3500)){
			draw_half_lick(matrix_location, view_mat_location, proj_mat_location, tong);
			runner++;
		}
		if ((runner >= 500 && runner < 750) || (runner >= 1250 && runner < 1500)|| (runner >= 2000 && runner < 2250)|| (runner >= 2750 && runner < 3000)|| (runner >= 3500 && runner < 3750)){
			draw_full_lick(matrix_location, view_mat_location, proj_mat_location, tong);
			runner++;
		}

		//lolipop
		mat4 model_lolipop = identity_mat4 ();	
		model_lolipop = translate (model_lolipop, vec3(3.0f ,-1.0f ,0.0f)); 
		//model_lolipop = scale(model_lolipop, vec3 (1.5f,1.5f,1.5f));
		mat4 loli = model_lolipop * tong;

		glUniformMatrix4fv (matrix_location, 1, GL_FALSE, loli.m);
		glBindVertexArray(VAOs[5]);
		glDrawArrays (GL_TRIANGLES, 0, g_point_count[5]);
		
	}else if  (runner >= 3750 && runner < 4800){
		
		runner++;

		if(cameray < 196.0f){
			cameray+= 0.2;
		}
		glDrawArrays (GL_TRIANGLES, 0, g_point_count[3]);
		
	}else{

		collision_detection();

		glDrawArrays (GL_TRIANGLES, 0, g_point_count[3]);

		draw_player(matrix_location, view_mat_location, proj_mat_location, model_plane);

		//lolis
		for(int i = 0; i < 8; i++){
			if(sweet_uneaten[i] == true){
				draw_lolipops(i, matrix_location, view_mat_location, proj_mat_location, model_plane);
			}
		}

		//sweets
		for(int j = 8; j < 16; j++){ 
			if(sweet_uneaten[j] == true){
				draw_sweets(j, matrix_location, view_mat_location, proj_mat_location, model_plane);
			}
		}

		//jellies
		for(int k = 16; k < 24; k++){
			if(sweet_uneaten[k] == true){
				draw_jellies(k, matrix_location, view_mat_location, proj_mat_location, model_plane);
			}
		}
	}

	glutPostRedisplay();
    glutSwapBuffers();
}

void updateScene() {	

	// Placeholder code, if you want to work with framerate
	// Wait until at least 16ms passed since start of last frame (Effectively caps framerate at ~60fps)
	static DWORD  last_time = 0;
	DWORD  curr_time = timeGetTime();
	float  delta = (curr_time - last_time) * 0.001f;
	if (delta > 0.03f)
		delta = 0.03f;
	last_time = curr_time;


	// Draw the next frame
	glutPostRedisplay();
}

void init()
{
	// Set up the shaders
	GLuint shaderProgramID = CompileShaders();
	// load mesh into a vertex buffer array


    generateObjectBufferMesh(TONGUE_MESH, 0);
	generateObjectBufferMesh(HALF_LICK_MESH, 1);
	generateObjectBufferMesh(FULL_LICK_MESH, 2);

	generateObjectBufferMesh(PLANE_MESH, 3);
	generateObjectBufferMesh(PLAYER_MESH, 4);

	generateObjectBufferMesh(LOLIPOP_MESH, 5);
	generateObjectBufferMesh(SWEET_MESH, 6);
	generateObjectBufferMesh(JELLY_MESH, 7);

}

void keypress(unsigned char key, int x, int y) {

	if(key == 27){
		exit(0);
	}
	if(key == 'g'){
		runner = 4000;
	}

	if(runner >= 4800){
		switch (key){
		//camera controls
			case 'a' :
				camerax += 1; break;
			case 'd' :
				camerax -= 1; break;
			case 'w' :
				cameraz += 1; break;
			case 's' :
				cameraz -= 1; break;
			case 'q' :
				cameray += 1; break;
			case 'e' :
				cameray -= 1; break;

			//bound to stop tongue going out of plane
			//RBC tongue is at -113, 0, 112
			//BLC tongue is at 121, 0, 112
			//FLC tongue is at 121, 0, -122
			//FRC tongue is at -113, 0, -122

			case 'j' :
				if(xxxx <= 121){
					xxxx += 2; 
				}
				break;
			case 'l' :
				if(xxxx >= -113){
					xxxx -= 2;
				}
				break;
			case 'i' :
				if(zzzz <= 122){
					zzzz += 2; 
				}
				break;
			case 'k' :
				if(zzzz >= -122){
					zzzz -= 2;
				}
				break;
		}
	}

}

int main(int argc, char** argv){

	// Set up the window
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("EAT DA SWEEEEEEEEETS");

	if(first_go){
		first_go = false;
		 
		for(int index = 0; index < 30; index++){

			sweet_locations[index][0] = random_poisition();
			sweet_locations[index][1] = random_poisition();

			bushes_locations[index][0] = random_poisition();
			bushes_locations[index][1] = random_poisition();

			sweet_uneaten[index] = true;
		}
	}

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);


	 // A call to glewInit() must be done after glut is initialized!
    GLenum res = glewInit();
	// Check for any errors
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }

	// initialise font, load from files
	if (!init_text_rendering(Timage, Tmeta, width, height)) {
		fprintf(stderr, "ERROR init text rendering\n");
		return 1;
	}
	printf ("adding text...\n");
	int hello_id = add_text (
		"Hello\nis it me you're looking for?",
		-0.9f, 0.5f, 35.0f, 0.5f, 0.5f, 1.0f, 1.0f);
	
	//*x,y,size,r,g,b,a
	time_id = add_text (
		"The time is:",
		-1.0f, 1.0f, 40.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	move_text (time_id, -1.0f, 1.0f);

	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
    return 0;
}










