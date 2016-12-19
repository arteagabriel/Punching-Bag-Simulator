/* Lab 6 base code - transforms using matrix stack built on glm 
	CPE 471 Cal Poly Z. Wood + S. Sueda
*/
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

/* to use glee */
#define GLEE_OVERWRITE_GL_FUNCTIONS
#include "glee.hpp"

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from
shared_ptr<Program> prog;
shared_ptr<Shape> shape;
shared_ptr<Shape> cubeShape;
shared_ptr<Shape> smoothShape;
shared_ptr<Shape> bunny;
shared_ptr<Shape> bag;
shared_ptr<Shape> rope;

vector<float> handBuf;
vector<float> footBuf;
vector<float> bagBuf;
vec3 handMin;
vec3 handMax;
vec3 footMin;
vec3 footMax;
vec3 bagMin;
vec3 bagMax;

int g_width, g_height;
float movement;
float sTheta;
float theta;
float phi;
float kickTheta = 0;
float punchTheta = 0;
bool kickBag = false;
bool punchBag = false;
bool moveBool;
vec3 movePerson = vec3(0, 0, 0);
vec3 lookAtV;
vec3 Eye;
vec3 w;
vec3 u;
vec3 v;

struct rotate_body {
    float uniform;
    float hip;
    float torso;
    float lower_arm;
    float upper_arm;
    float lower_leg;
    float upper_leg;
    bool uniform_end;
    bool uniform_start;
    bool hip_start;
    bool hip_end;
    bool torso_start;
    bool torso_end;
    bool lower_arm_start;
    bool lower_arm_end;
    bool upper_arm_start;
    bool upper_arm_end;
    bool upper_leg_end;
    bool upper_leg_start;
    bool lower_leg_start;
    bool lower_leg_end;
} kick, punch;

static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

//helper function to set materials
void SetMaterial(int i) {

  switch (i) {
    case 0: //shiny blue plastic
      glUniform3f(prog->getUniform("MatAmb"), 0.02, 0.04, 0.2);
      glUniform3f(prog->getUniform("MatDif"), 0.0, 0.16, 0.9);
      glUniform3f(prog->getUniform("MatSpec"), 0.14, 0.2, 0.8);
      glUniform1f(prog->getUniform("shine"), 120.0);
        break;
    case 1: // flat grey
      glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
      glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
      glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
      glUniform1f(prog->getUniform("shine"), 4.0);
      break;
    case 2: //brass
      glUniform3f(prog->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
      glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
      glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
      glUniform1f(prog->getUniform("shine"), 27.9);
        break;
    case 3: //copper
      glUniform3f(prog->getUniform("MatAmb"), 0.1913, 0.0735, 0.0225);
      glUniform3f(prog->getUniform("MatDif"), 0.7038, 0.27048, 0.0828);
      glUniform3f(prog->getUniform("MatSpec"), 0.257, 0.1376, 0.08601);
      glUniform1f(prog->getUniform("shine"), 12.8);
        break;
    case 4: //floor green
      glUniform3f(prog->getUniform("MatAmb"), 0.23, 0.6, 0.24);
      glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
      glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
      glUniform1f(prog->getUniform("shine"), 4.0);
      break;
    case 5: //person color
      glUniform3f(prog->getUniform("MatAmb"), 0.64, 0.16, 0.16);
      glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
      glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
      glUniform1f(prog->getUniform("shine"), 27.9);
        break;
    case 6: // snowman with ears
      glUniform3f(prog->getUniform("MatAmb"), 0.9, 0.9, 0.9);
      glUniform3f(prog->getUniform("MatDif"), 0.7038, 0.27048, 0.0828);
      glUniform3f(prog->getUniform("MatSpec"), 0.257, 0.1376, 0.08601);
      glUniform1f(prog->getUniform("shine"), 12.8);
        break;
  }
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
    else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        Eye -= w * 0.5f;
        lookAtV -= w * 0.5f;
    }
    else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        Eye -= u * 0.5f;
        lookAtV -= u * 0.5f;
    }
    else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        Eye += w * 0.5f;
        lookAtV += w * 0.5f;
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        Eye += u * 0.5f;
        lookAtV += u * 0.5f;
    }
    else if (key == GLFW_KEY_K && action == GLFW_PRESS) {
        // initiate kick animation
        kick.upper_leg_start = true;
        kick.lower_leg_start = true;
        /* kickBag = true; */
    }
    else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        // initiate punch animation
        punch.torso_start = true; 
        punch.upper_arm_start = true;
        punch.lower_arm_start = true;
        /* punchBag = true; */
    }
    else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        moveBool = !moveBool;
        movePerson = moveBool ? vec3(-1.5, 0, 1.5) : vec3(0, 0, 0);
    }
}

/* static void mouse_callback(GLFWwindow *window, int button, int action, int mods) */
/* { */
/*    double posX, posY; */
/*    if (action == GLFW_PRESS) { */
/*       glfwGetCursorPos(window, &posX, &posY); */
/*       cout << "Pos X " << posX <<  " Pos Y " << posY << endl; */
/* 	} */
/* } */

static void scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
    theta += ( ( (-x_offset / 100) / normalize((double)g_width) ) * M_PI );
    float p = ( ( (y_offset / 100) / normalize((double)g_height) ) * M_PI );

    if (phi + p < 1.6 && phi + p > -1.6) {
        phi += p; 
    }
}

static void resize_callback(GLFWwindow *window, int width, int height) {
   g_width = width;
   g_height = height;
   glViewport(0, 0, width, height);
}

static void getBoundingBoxes() {
    for (size_t v = 0; v < handBuf.size() / 3; v++) {
        if(handBuf[3*v+0] < handMin.x) handMin.x = handBuf[3*v+0];
        if(handBuf[3*v+0] > handMax.x) handMax.x = handBuf[3*v+0];

        if(handBuf[3*v+1] < handMin.y) handMin.y = handBuf[3*v+1];
        if(handBuf[3*v+1] > handMax.y) handMax.y = handBuf[3*v+1];

        if(handBuf[3*v+2] < handMin.z) handMin.z = handBuf[3*v+2];
        if(handBuf[3*v+2] > handMax.z) handMax.z = handBuf[3*v+2];
    }

    for (size_t v = 0; v < bagBuf.size() / 3; v++) {
        if(bagBuf[3*v+0] < bagMin.x) bagMin.x = bagBuf[3*v+0];
        if(bagBuf[3*v+0] > bagMax.x) bagMax.x = bagBuf[3*v+0];

        if(bagBuf[3*v+1] < bagMin.y) bagMin.y = bagBuf[3*v+1];
        if(bagBuf[3*v+1] > bagMax.y) bagMax.y = bagBuf[3*v+1];

        if(bagBuf[3*v+2] < bagMin.z) bagMin.z = bagBuf[3*v+2];
        if(bagBuf[3*v+2] > bagMax.z) bagMax.z = bagBuf[3*v+2];
    }

    for (size_t v = 0; v < footBuf.size() / 3; v++) {
        if(footBuf[3*v+0] < footMin.x) footMin.x = footBuf[3*v+0];
        if(footBuf[3*v+0] > footMax.x) footMax.x = footBuf[3*v+0];

        if(footBuf[3*v+1] < footMin.y) footMin.y = footBuf[3*v+1];
        if(footBuf[3*v+1] > footMax.y) footMax.y = footBuf[3*v+1];

        if(footBuf[3*v+2] < footMin.z) footMin.z = footBuf[3*v+2];
        if(footBuf[3*v+2] > footMax.z) footMax.z = footBuf[3*v+2];
    }
}


static void init()
{
	GLSL::checkVersion();

    kick.uniform = 0;
    kick.hip = 0;
    kick.torso = 0;
    kick.lower_leg = 0;
    kick.upper_leg = 0;
    kick.uniform_end = false;
    kick.uniform_start = false;
    kick.hip_start = false;
    kick.hip_end = false;
    kick.torso_start = false;
    kick.torso_end = false;
    punch.lower_arm_start = false;
    punch.lower_arm_end = false;
    punch.upper_arm_start = false;
    punch.upper_arm_end = false;
    kick.lower_leg_start = false;
    kick.lower_leg_end = false;
    kick.upper_leg_start = false;
    kick.upper_leg_end = false;

    punch.uniform = 0;
    punch.hip = 0;
    punch.torso = 0;
    punch.lower_arm = 0;
    punch.upper_arm = 0;
    punch.uniform_end = false;
    punch.uniform_start = false;
    punch.hip_start = false;
    punch.hip_end = false;
    punch.torso_start = false;
    punch.torso_end = false;
    punch.lower_arm_start = false;
    punch.lower_arm_end = false;
    punch.upper_arm_start = false;
    punch.upper_arm_end = false;
    punch.lower_leg_start = false;
    punch.lower_leg_end = false;
    punch.upper_leg_start = false;
    punch.upper_leg_end = false;

    kickTheta = 0;
    punchTheta = 0;
    moveBool = false;
    movement = 0;
    phi = 0;
    theta = M_PI;
    lookAtV = vec3(0, 0, 0);
    w = vec3(0, 0, 0);
    u = vec3(0, 0, 0);
    v = vec3(0, 0, 0);
	// Set background color.
	glClearColor(.64f, .64f, 1.0f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	// Initialize mesh.
    smoothShape = make_shared<Shape>();
    smoothShape->loadMesh(RESOURCE_DIR + "smoothsphere.obj");
    smoothShape->resize();
    smoothShape->init();

    cubeShape = make_shared<Shape>();
    cubeShape->loadMesh(RESOURCE_DIR + "cube.obj");
    cubeShape->resize();
    cubeShape->init();
    handBuf = cubeShape->getPositions();
    footBuf = cubeShape->getPositions();

    bag = make_shared<Shape>();
    bag->loadMesh(RESOURCE_DIR + "cylinder.obj");
    bag->resize();
    bag->init();
    bagBuf = cubeShape->getPositions();

    rope = make_shared<Shape>();
    rope->loadMesh(RESOURCE_DIR + "smoothsphere.obj");
    rope->resize();
    rope->init();

	// Initialize the GLSL program.
	prog = make_shared<Program>();
	prog->setVerbose(true);
	prog->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	prog->init();
	prog->addUniform("P");
	prog->addUniform("M");
    prog->addUniform("V");
    prog->addUniform("MatAmb");
    prog->addUniform("MatDif");
    prog->addUniform("MatSpec");
    prog->addUniform("shine");
	prog->addAttribute("vertPos");
	prog->addAttribute("vertNor");

    getBoundingBoxes();
}


static void render()
{
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use the matrix stack for Lab 6
   float aspect = width/(float)height;

   // Create the matrix stacks - please leave these alone for now
   auto P = make_shared<MatrixStack>();
   auto MV = make_shared<MatrixStack>();
   // Apply perspective projection.
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 100.0f);

   float radius = 1;
   vec3 gaze = lookAtV - Eye;
   vec3 up = vec3(0, 1, 0);
   w = -gaze / length(gaze);
   u = cross(up, w) / length(cross(up, w));
   v = cross(w, u);

   lookAtV.x = radius * cos(phi) * cos(theta) + Eye.x;
   lookAtV.y = radius * sin(phi) + Eye.y;
   lookAtV.z = radius * cos(phi) * cos (M_PI/2 - theta) + Eye.z;
   mat4 V = lookAt(Eye, lookAtV, up);

	// Draw a stack of cubes with indiviudal transforms 
	prog->bind();
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));

    // floor
    MV->pushMatrix();
     MV->loadIdentity();
     MV->translate(vec3(0, -1, 0));
     MV->scale(vec3(100, 0.1, 100));
     SetMaterial(4);
     glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
     glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
     cubeShape->draw(prog);
    MV->popMatrix();

    // Person
    MV->pushMatrix();
     MV->loadIdentity();
     MV->translate(vec3(-3.5 + movePerson.x, 0.3 + movePerson.y, 0.5 + movePerson.z));
     MV->rotate(M_PI - M_PI / 6, vec3(0, 1, 0));
     MV->scale(vec3(0.3, 0.3, 0.3));
     SetMaterial(2);

     // torso
     MV->pushMatrix();
      MV->translate(vec3(0, 0.25, 0));
      MV->rotate(-punch.torso, vec3(0, 1, 0));

      // right arm
      MV->pushMatrix();
		//place at shoulder
		MV->translate(vec3(1, 1, 0));
		//rotate shoulder joint
        MV->rotate(-M_PI / 4, vec3(0, 0, 1));
        MV->rotate(-punch.lower_arm, vec3(0, 1, 0));
        //move to shoulder joint
        MV->translate(vec3(0.75, 0, 0));
        // add second part of arm
        MV->pushMatrix();
         // place at elbow
         MV->translate(vec3(1, 0, 0));
         //rotate elbow joint
         MV->rotate(-M_PI / 4, vec3(0, 0, 1));
         MV->rotate(-punch.lower_arm, vec3(0, 1, 0));
         //move to shoulder joint
         //move to elbow joint
         MV->translate(vec3(0.5, 0, 0));
         //add hand
         MV->pushMatrix();
          MV->translate(vec3(0.5, 0, 0));
          MV->scale(vec3(0.35, 0.35, 0.35));
          glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
          glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
          cubeShape->draw(prog);
         MV->popMatrix();

         //non-uniform scale
         MV->scale(vec3(0.75, 0.25, 0.25));
         glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
         glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
         cubeShape->draw(prog);

        MV->popMatrix();
        //non-uniform scale
        MV->scale(vec3(0.75, 0.25, 0.25));
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
        cubeShape->draw(prog);

      MV->popMatrix();
      // end right arm

      // left arm
      MV->pushMatrix();
		//place at shoulder
		MV->translate(vec3(-1, 1, 0));
		//rotate shoulder joint
        MV->rotate(M_PI / 4, vec3(0, 0, 1));
        //move to shoulder joint
        MV->translate(vec3(-0.75, 0, 0));
        // add second part of arm
        MV->pushMatrix();
         // place at elbow
         MV->translate(vec3(-1, 0, 0));
         //rotate elbow joint
         MV->rotate(M_PI / 4, vec3(0, 0, 1));
         //move to shoulder joint
         //move to elbow joint
         MV->translate(vec3(-0.5, 0, 0));
         //add hand
         MV->pushMatrix();
          MV->translate(vec3(-0.5, 0, 0));
          MV->scale(vec3(0.35, 0.35, 0.35));
          glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
          glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
          cubeShape->draw(prog);
         MV->popMatrix();

         //non-uniform scale
         MV->scale(vec3(0.75, 0.25, 0.25));
         glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
         glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
         cubeShape->draw(prog);

        MV->popMatrix();
        //non-uniform scale
        MV->scale(vec3(0.75, 0.25, 0.25));
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
        cubeShape->draw(prog);

      MV->popMatrix();
      // end left arm

      // head
      MV->pushMatrix();
       MV->translate(vec3(0, 1.5, 0)); 
       MV->scale(vec3(0.5, 0.5, 0.5));
       glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
       glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
       cubeShape->draw(prog);
      MV->popMatrix();
      // end head
      
      MV->scale(vec3(1.25, 0.75, 1));
      glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
      glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
      cubeShape->draw(prog);
     MV->popMatrix();
      
      // right leg 
      MV->pushMatrix();
		//place at hip 
		MV->translate(vec3(1, -1, 0));
		//rotate hip joint
        MV->rotate(-M_PI / 2, vec3(0, 0, 1));
        /* MV->rotate(M_PI / 3, vec3(0, 1, 0)); */
        //move to hip joint
        MV->translate(vec3(0.75, 0, 0));
        // add second part of leg 
        MV->pushMatrix();
         // place at knee 
         MV->translate(vec3(1, 0, 0));
         //rotate knee joint
         /* MV->rotate(-M_PI / 4, vec3(0, 1, 0)); */
         MV->translate(vec3(0.5, 0, 0));
         //add right foot
         MV->pushMatrix();
          MV->translate(vec3(0.5, 0, 0));
          MV->scale(vec3(0.35, 0.35, 0.35));
          glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
          glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
          cubeShape->draw(prog);
         MV->popMatrix();

         //non-uniform scale
         MV->scale(vec3(0.75, 0.25, 0.25));
         glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
         glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
         cubeShape->draw(prog);

        MV->popMatrix();
        //non-uniform scale
        MV->scale(vec3(0.75, 0.25, 0.25));
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
        cubeShape->draw(prog);

      MV->popMatrix();
      // end right leg 
      
      // left leg 
      MV->pushMatrix();
		//place at hip 
		MV->translate(vec3(-1, -1, 0));
		//rotate hip joint
        MV->rotate(M_PI / 2, vec3(0, 0, 1));
        MV->rotate(kick.upper_leg, vec3(0, 1, 0));
        //move to hip joint
        MV->translate(vec3(-0.75, 0, 0));
        // add second part of leg 
        MV->pushMatrix();
         // place at knee 
         MV->translate(vec3(-1, 0, 0));
         //rotate knee joint
         MV->rotate(kick.lower_leg, vec3(0, -1, 0));
         MV->translate(vec3(-0.5, 0, 0));
         //add left foot
         MV->pushMatrix();
          MV->translate(vec3(-0.5, 0, 0));
          MV->scale(vec3(0.35, 0.35, 0.35));
          glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
          glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
          cubeShape->draw(prog);
         MV->popMatrix();

         //non-uniform scale
         MV->scale(vec3(0.75, 0.25, 0.25));
         glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
         glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
         cubeShape->draw(prog);

        MV->popMatrix();
        //non-uniform scale
        MV->scale(vec3(0.75, 0.25, 0.25));
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
        cubeShape->draw(prog);

      MV->popMatrix();
      // end left leg 

     MV->translate(vec3(0, -0.75, 0));
     MV->scale(vec3(1.25, 0.25, 1));
     glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
     glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
     cubeShape->draw(prog);

    MV->popMatrix();

    // Punching bag
    MV->pushMatrix();
     MV->loadIdentity();
     MV->translate(vec3(-2.75, 1.5, -0.25));
     MV->rotate(punchTheta, vec3(0, 0, 1)); 
     MV->rotate(kickTheta, vec3(1, 0, 0)); 
     MV->scale(vec3(1, 1, 1));

     // rope 1
     MV->pushMatrix();
      MV->translate(vec3(0, -0.25, 0.1));
      MV->rotate(M_PI / 3, vec3(1, 0, 0));
      MV->translate(vec3(0, 0, 0));
      MV->scale(vec3(0.01, 0.01, 0.25));
      SetMaterial(3);
      glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
      glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
     rope->draw(prog);
     MV->popMatrix();

     // rope 2
     MV->pushMatrix();
      MV->translate(vec3(0, -0.25, -0.1));
      MV->rotate(-M_PI / 3, vec3(1, 0, 0));
      MV->translate(vec3(0, 0, 0));
      MV->scale(vec3(0.01, 0.01, 0.25));
      SetMaterial(3);
      glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
      glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
     rope->draw(prog);
     MV->popMatrix();

     // rope 3
     MV->pushMatrix();
      MV->translate(vec3(-0.1, -0.25, 0));
      MV->rotate(M_PI / 2, vec3(0, 1, 0));
      MV->rotate(-M_PI / 3, vec3(1, 0, 0));
      MV->translate(vec3(0, 0, 0));
      MV->scale(vec3(0.01, 0.01, 0.25));
      SetMaterial(3);
      glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
      glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
     rope->draw(prog);
     MV->popMatrix();

     // rope 4
     MV->pushMatrix();
      MV->translate(vec3(0.1, -0.25, 0));
      MV->rotate(M_PI / 2, vec3(0, 1, 0));
      MV->rotate(M_PI / 3, vec3(1, 0, 0));
      MV->translate(vec3(0, 0, 0));
      MV->scale(vec3(0.01, 0.01, 0.25));
      SetMaterial(3);
      glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
      glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
     rope->draw(prog);
     MV->popMatrix();

     // punching bag
     MV->pushMatrix();
      MV->translate(vec3(0, -1.3, 0));
      MV->rotate(M_PI / 2, vec3(1, 0, 0));
      MV->scale(vec3(0.3, 0.3, 0.95));
      SetMaterial(1);
      glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
      glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    bag->draw(prog);
    MV->popMatrix();

     MV->scale(vec3(0.1, 0.1, 0.1));
     SetMaterial(2);
     glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
     glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
     smoothShape->draw(prog);
    MV->popMatrix();


	prog->unbind();

    // Pop matrix stacks.
    P->popMatrix();

    // animate kick
    if (kick.upper_leg_start) {
        if (kick.upper_leg < M_PI / 2 && !kick.upper_leg_end) {
            kick.upper_leg += 0.1;
        } else {
            if (kick.lower_leg_start) {
                if (kick.lower_leg < M_PI / 3 && !kick.lower_leg_end)
                    kick.lower_leg += 0.1;
                else {
                    kick.lower_leg_end = true;

                    if (kick.lower_leg > 0)
                        kick.lower_leg -= 0.1;
                    else {
                        kick.lower_leg = 0;
                        kick.lower_leg_start = false;
                        kick.lower_leg_end = false;
                    }
                }
            }

            kick.upper_leg_end = true;

            if (kick.upper_leg > 0) {
                kick.upper_leg -= 0.1;

                if (kick.lower_leg_start) {
                    if (kick.lower_leg < M_PI / 3 && !kick.lower_leg_end)
                        kick.lower_leg += 0.1;
                    else {
                        kick.lower_leg_end = true;

                        if (kick.lower_leg > 0)
                            kick.lower_leg -= 0.1;
                        else {
                            kick.lower_leg = 0;
                            kick.lower_leg_start = false;
                            kick.lower_leg_end = false;
                        }
                    }
                }
            } else {
                kick.upper_leg = 0;
                kick.upper_leg_start = false;
                kick.upper_leg_end = false;
                kickBag = true;
            }
        }
    }

    if (punch.torso_start) {
        if (punch.torso < M_PI / 4 && !punch.torso_end) {
            punch.torso += 0.1;
        } else {
            if (punch.lower_arm_start) {
                if (punch.lower_arm < M_PI / 2 && !punch.lower_arm_end) {
                    punch.lower_arm += 0.1;
                } else {
                    punch.lower_arm_end = true;

                    if (punch.lower_arm > 0) {
                        punch.lower_arm -= 0.1;
                    } else {
                        punch.lower_arm = 0;
                        punch.lower_arm_start = false;
                        punch.lower_arm_end = false;
                    }
                }
            }

            punch.torso_end = true;

            if (punch.torso > 0) {
                punch.torso -= 0.1;

                if (punch.lower_arm_start) {
                    if (punch.lower_arm < M_PI / 2 && !punch.lower_arm_end) {
                        punch.lower_arm += 0.1;
                    } else {
                        punch.lower_arm_end = true;

                        if (punch.lower_arm > 0) {
                            punch.lower_arm -= 0.1;
                        } else {
                            punch.lower_arm = 0;
                            punch.lower_arm_start = false;
                            punch.lower_arm_end = false;
                        }
                    }
                }
            } else {
                punch.torso = 0;
                punch.torso_start = false;
                punch.torso_end = false;
                punchBag = true;
            }
        }
    }

    if (punch.lower_arm_start) {
        if (punch.lower_arm < M_PI / 2 && !punch.lower_arm_end) {
            punch.lower_arm += 0.1;
        } else {
            punch.lower_arm_end = true;

            if (punch.lower_arm > 0) {
                punch.lower_arm -= 0.1;
            } else {
                punch.lower_arm = 0;
                punch.lower_arm_start = false;
                punch.lower_arm_end = false;
            }
        }
    }

    if (punchBag) {
        if (punchTheta < M_PI / 4)
            punchTheta += 0.1;
        else {
            punchBag = false;
        }
    }

    if (!punchBag && punchTheta > 0) {
        punchTheta -= 0.1;
    }

    if (kickBag) {
        if (kickTheta < M_PI / 3)
            kickTheta += 0.1;
        else
            kickBag = false;
    }

    if (!kickBag && kickTheta > 0) {
        kickTheta -= 0.1;
    }

    if (kickTheta < 0)
        kickTheta = 0;
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
   //request the highest possible version of OGL - important for mac
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "Gabriel Arteaga", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	//weird bootstrap of glGetError
   glGetError();
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
   cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
   //set the mouse call back
   /* glfwSetMouseButtonCallback(window, mouse_callback); */
   //set the window resize call back
   glfwSetFramebufferSizeCallback(window, resize_callback);
   
   glfwSetScrollCallback(window, scroll_callback);

	// Initialize scene. Note geometry initialized in init now
	init();

	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

