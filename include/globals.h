#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader.h>
#include <stb_image.h>
#include "utils.h"
#include "matrices.h"


struct ObjModel
{
    tinyobj::attrib_t                 attrib;
    std::vector<tinyobj::shape_t>     shapes;
    std::vector<tinyobj::material_t>  materials;

    // Este construtor l� o modelo de um arquivo utilizando a biblioteca tinyobjloader.
    // Veja: https://github.com/syoyo/tinyobjloader
    ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true)
    {
        printf("Carregando modelo \"%s\"... ", filename);

        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, basepath, triangulate);

        if (!err.empty())
            fprintf(stderr, "\n%s\n", err.c_str());

        if (!ret)
            throw std::runtime_error("Erro ao carregar modelo.");

        printf("OK.\n");
    }
};

struct SceneObject
{
    std::string  name;        // Nome do objeto
    void*        first_index; // �ndice do primeiro v�rtice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    int          num_indices; // N�mero de �ndices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    GLenum       rendering_mode; // Modo de rasteriza��o (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    GLuint       vertex_array_object_id; // ID do VAO onde est�o armazenados os atributos do modelo
    glm::vec3    bbox_min; // Axis-Aligned Bounding Box do objeto
    glm::vec3    bbox_max;
};

struct GameCow
{
    glm::vec4 pos = glm::vec4(0.0f,0.0f,0.0f, 1.0f); //cow position
    glm::mat4 model = Matrix_Identity() * Matrix_Translate(pos.x, pos.y, pos.z);
    glm::vec4 direction;
    float angle = 0; //angulo da dire��o da vaca
    bool alive = true; //if cow is alive
    int abducted = 0;
    bool safe = false;
};

std::map<std::string, SceneObject> g_VirtualScene;
// Pilha que guardar� as matrizes de modelagem.
std::stack<glm::mat4>  g_MatrixStack;

// Raz�o de propor��o da janela (largura/altura). Veja fun��o FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// �ngulos de Euler que controlam a rota��o de um dos cubos da cena virtual
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

#define INC 0.2f
#define ALPHA 0.523599
#define GROUND_SIZE 100.0f
#define WALL_DIST 10.0f //o qu�o longe do precip�cio a nave vai
#define SKY_SIZE 50.0
#define SPHERE 0
#define PLANE  2
#define COW    3
#define SHIP   4
#define POOL   5
#define BARN   6
#define NUM_COWS 40
#define ENDGAME 2
#define GAME 1

int game = GAME;
// "g_LeftMouseButtonPressed = true" se o usu�rio est� com o bot�o esquerdo do mouse
// pressionado no momento atual. Veja fun��o MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false; // An�logo para bot�o direito do mouse
bool g_MiddleMouseButtonPressed = false; // An�logo para bot�o do meio do mouse

// Vari�veis que definem a c�mera em coordenadas esf�ricas, controladas pelo
// usu�rio atrav�s do mouse (veja fun��o CursorPosCallback()). A posi��o
// efetiva da c�mera � calculada dentro da fun��o main(), dentro do loop de
// renderiza��o.
float g_CameraTheta = 0.0f; // �ngulo no plano ZX em rela��o ao eixo Z
float g_CameraPhi = 0.0f;   // �ngulo em rela��o ao eixo Y
float g_CameraDistance = 10.0f; // Dist�ncia da c�mera para a origem

//vetores tem que ser definiidos globalmente para serem acessados na fun��o de callback
glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f);
glm::vec4 ship_position      = glm::vec4(1.0f,2.5f,0.0f,1.0f);
glm::vec4 camera_lookat_l    = glm::vec4(0.0f,0.0f,g_CameraDistance,0.0f);
//glm::vec4 camera_position_c  = glm::vec4(0.0f,4.0f,-2.0f,0.0f);
glm::vec4 camera_position_c  = camera_lookat_l + ship_position;
glm::vec4 probe_position = glm::vec4(3.0f, 0.0f, 0.0f, 0.0f) + ship_position;
glm::vec4 probe_lookat = glm::vec4(0.0f,0.0f, 1.0f, 0.0f);
glm::vec4 barn_position = glm::vec4(0.0f,-1.0f,0.0f,1.0f);

//mais vari�veis globais que v�o ser �teis
glm::vec4 cowM_min;
glm::vec4 cowM_max;
glm::vec4 shipM_min;
glm::vec4 shipM_max;
glm::vec4 barnW_min;
glm::vec4 barnW_max;
glm::mat4 ship_model;
glm::vec4 probe_min = glm::vec4(-1.0f, -1.0f, -1.0f, 0.0f);
glm::vec4 probe_max = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
bool under_abduction = false;
int cow_abducted = -1;
bool probe_on = false;
int freeCows = NUM_COWS;
int abductedCows = 0;
int murderedCows = 0;
int safeCows = 0;

GameCow cows[NUM_COWS];

//model global por perfomance
glm::mat4 model = Matrix_Identity();

// Vari�vel que controla se o texto informativo ser� mostrado na tela.
bool g_ShowInfoText = true;

// Vari�veis que definem um programa de GPU (shaders). Veja fun��o LoadShadersFromFiles().
GLuint vertex_shader_id;
GLuint fragment_shader_id;
GLuint program_id = 0;
GLint model_uniform;
GLint view_uniform;
GLint projection_uniform;
GLint object_id_uniform;
GLint bbox_min_uniform;
GLint bbox_max_uniform;
GLint ship_light_uniform;

// N�mero de texturas carregadas pela fun��o LoadTextureImage()
GLuint g_NumLoadedTextures = 0;

double g_LastCursorPosX, g_LastCursorPosY;
