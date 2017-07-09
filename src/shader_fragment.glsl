#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da cor de cada vértice, definidas em "shader_vertex.glsl" e
// "main.cpp".
in vec4 position_world;
in vec4 normal;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
#define SPHERE 0
#define BUNNY  1
#define PLANE  2
#define COW    3
#define SHIP   4
#define POOL   5
#define BARN   6
uniform int object_id;

uniform vec4 ship_light;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;
uniform sampler2D TextureImage3;
uniform sampler2D TextureImage4;


// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec3 color;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define AMBIENT 0.1

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    //vec4 l = normalize(vec4(1.0,4.0,0.0,0.0));
    //vec4 l = normalize(ship_light);
    vec4 p_to_l = normalize(p - ship_light);
    vec4 l_dir = vec4(0.0, -1.0, 0.0,0.0) ;
    vec4 l = normalize(ship_light - p);

    vec4 r = -l + 2*n*(dot(n,l));

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;

    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    float q=0.0; // Expoente especular para o modelo de iluminação de Phong


    if ( object_id == SPHERE )
    {
        // PREENCHA AQUI as coordenadas de textura da esfera, computadas com
        // projeção esférica EM COORDENADAS DO MODELO. Utilize como referência
        // o slide 139 do documento "Aula_20_e_21_Mapeamento_de_Texturas.pdf".
        // A esfera que define a projeção deve estar centrada na posição
        // "bbox_center" definida abaixo.

        // Você deve utilizar:
        //   função 'length( )' : comprimento Euclidiano de um vetor
        //   função 'atan( , )' : arcotangente. Veja https://en.wikipedia.org/wiki/Atan2.
        //   função 'asin( )'   : seno inverso.
        //   constante M_PI
        //   variável position_model

        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;

        vec4 max_vec = bbox_max;

        max_vec.w = 0.0;

        float rho = length(max_vec);

        vec4 p_vec = normalize(position_model - bbox_center) * rho;

        vec4 p_linha = bbox_center + p_vec;

        float theta = atan(p_linha.x, p_linha.z);

        float phi = asin(p_linha.y/rho);

        U = (theta+M_PI)/(2*M_PI);
        V = (phi + M_PI_2)/M_PI;
    }
    else if ( (object_id == BUNNY) || (object_id == COW) || (object_id == BARN) )
    {
        // PREENCHA AQUI as coordenadas de textura do coelho, computadas com
        // projeção planar XY em COORDENADAS DO MODELO. Utilize como referência
        // o slide 106 do documento "Aula_20_e_21_Mapeamento_de_Texturas.pdf".
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        // Utilize as variáveis min*/max* definidas acima para normalizar as
        // coordenadas de textura U e V dentro do intervalo [0,1]. Veja 149
        // do documento "Aula_20_e_21_Mapeamento_de_Texturas.pdf".
        U = (position_model.x - minx) / (maxx - minx);
        V = (position_model.y - miny) / (maxy - miny);
    }
    /*else if ( object_id == COW )
    {
        // PREENCHA AQUI as coordenadas de textura do coelho, computadas com
        // projeção planar XY em COORDENADAS DO MODELO. Utilize como referência
        // o slide 106 do documento "Aula_20_e_21_Mapeamento_de_Texturas.pdf".
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;


        // Utilize as variáveis min* /max* definidas acima para normalizar as
        // coordenadas de textura U e V dentro do intervalo [0,1]. Veja 149
        // do documento "Aula_20_e_21_Mapeamento_de_Texturas.pdf".
        U = (position_model.x - minx) / (maxx - minx);
        V = (position_model.y - miny) / (maxy - miny);
    }
    */
    else if ( object_id == PLANE )
    {
        // Coordenadas de textura do plano,i
        // obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;


       // U = position_model.z;  // MIRRORED_REPEAT NOT 10/10
        //V = position_model.x;

        /*if(U>=1.0f){
            U = (U - floor(U));
        }
        if(U<=0.0f){
            U = -1.0f * U;
            U = 1 - (U - floor(U));
        }
        if(V<=0.0f){
            V = -1.0f * V;
            V = 1 - (V - floor(V));
        }*/

    }
    else if ( object_id == SHIP )
    {
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        // Utilize as variáveis min*/max* definidas acima para normalizar as
        // coordenadas de textura U e V dentro do intervalo [0,1]. Veja 149
        // do documento "Aula_20_e_21_Mapeamento_de_Texturas.pdf".
        U = (position_model.x - minx) / (maxx - minx);
        V = (position_model.z - minz) / (maxz - minz);
    }
    else if(object_id == POOL){
        Kd = vec3(0.5,0.0,0.00);
        Ks = vec3(0.8,0.0,0.0);
        q = 32.0;
    }

    // Obtemos a refletância difusa a partir da leitura da imagem TextureImage0


    // Equação de Iluminação
    float lambert = max(0,dot(n,l));
    float phong = pow(max(0, dot(r,v)),q);

    vec3 ambient_light_spectrum = vec3(0.1,0.1,0.1);
    vec3 light_spectrum = vec3(1.0,1.0,1.0);

    float alpha = radians(30);

    if (dot(p_to_l, l_dir) < cos(alpha))
    {
            lambert = 0;  //se o ponto não está iluminado vamos usar apenas a luz ambiente
    }

    color = vec3(255,255,255).rgb ;

    if (object_id == SPHERE){
        vec3 Kd0 = texture(TextureImage0, vec2(U,V)).rgb;
        color = Kd0;
    }
    if (object_id == PLANE){
        vec3 Kd_grass = texture(TextureImage2, vec2(U,V)).rgb;
        color = Kd_grass * (lambert + AMBIENT);
    }
    if (object_id == COW){
        vec3 Kd_cow = texture(TextureImage3, vec2(U,V)).rgb;
        //color = Kd_cow * (lambert + 0.01);
        color = Kd_cow * (lambert*light_spectrum + AMBIENT);
    }
    if (object_id == SHIP){
        vec3 Kd_ship = texture(TextureImage4, vec2(U,V)).rgb;
        color = Kd_ship * (lambert + AMBIENT);
    }
    if(object_id == POOL){
        color = Kd * light_spectrum * lambert
          + Ks * light_spectrum * (phong+AMBIENT);
    }
    if(object_id == BARN){
        vec3 Kd1 = texture(TextureImage1, vec2(U,V)).rgb;
        color = Kd1 * (lambert + AMBIENT);
    }

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color = pow(color, vec3(1.0,1.0,1.0)/2.2);
}
