#include "callbacks.h"


// Estrutura que representa um modelo geométrico carregado a partir de um
// arquivo ".obj". Veja https://en.wikipedia.org/wiki/Wavefront_.obj_file .






// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.


// Definimos uma estrutura que armazenará dados necessários para renderizar
// cada objeto da cena virtual.





// Abaixo definimos variáveis globais utilizadas em várias funções do código.

// A cena virtual é uma lista de objetos nomeados, guardados em um dicionário
// (map).  Veja dentro da função BuildTrianglesAndAddToVirtualScene() como que são incluídos
// objetos dentro da variável g_VirtualScene, e veja na função main() como
// estes são acessados.




int main(int argc, char* argv[])
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com a OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Pedimos apra utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas da OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional
    GLFWwindow* window;
    window = glfwCreateWindow(800, 600, "INF01047", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botões do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas pela OpenGL 3.3, utilizando a
    // biblioteca GLAD. Veja
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, 800, 600); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slide 216 do documento
    // "Aula_03_Rendering_Pipeline_Grafico.pdf".
    //
    LoadShadersFromFiles();

    // Carregamos n imagens para serem utilizadas como textura
    LoadTextureImage("../../data/tc-earth_daymap_surface.jpg");      // TextureImage0
    LoadTextureImage("../../data/barn.jpg");                        // TextureImage1
    LoadTextureImage("../../data/grass.jpg");                        // TextureImage2
    LoadTextureImage("../../data/cowT.jpg");                          // TextureImage3
    LoadTextureImage("../../data/ufoT.png");                          // TextureImage4



    // Construímos a representação de objetos geométricos através de malhas de triângulos
    ObjModel spheremodel("../../data/sphere.obj");
    ComputeNormals(&spheremodel);
    BuildTrianglesAndAddToVirtualScene(&spheremodel);

    ObjModel planemodel("../../data/plane.obj");
    ComputeNormals(&planemodel);
    BuildTrianglesAndAddToVirtualScene(&planemodel);

    ObjModel cowmodel("../../data/cow.obj");
    ComputeNormals(&cowmodel);
    BuildTrianglesAndAddToVirtualScene(&cowmodel);

    ObjModel shipmodel("../../data/ship.obj");
    ComputeNormals(&shipmodel);
    BuildTrianglesAndAddToVirtualScene(&shipmodel);

    ObjModel poolmodel("../../data/bloodpool.obj");
    ComputeNormals(&poolmodel);
    BuildTrianglesAndAddToVirtualScene(&poolmodel);

    ObjModel barnmodel("../../data/barn.obj");
    ComputeNormals(&barnmodel);
    BuildTrianglesAndAddToVirtualScene(&barnmodel);

    glm::vec3 cowB_min = g_VirtualScene["cow"].bbox_min; //bbox de modelo das vacas
    glm::vec3 cowB_max = g_VirtualScene["cow"].bbox_max;
    glm::vec3 shipB_min = g_VirtualScene["ship"].bbox_min; //bbox de modelo na nave
    glm::vec3 shipB_max = g_VirtualScene["ship"].bbox_max;
    glm::vec3 barnB_min = g_VirtualScene["barn"].bbox_min; //bbox ddo modelo do celeiro
    glm::vec3 barnB_max = g_VirtualScene["barn"].bbox_max;

    glm::mat4 barn_model = Matrix_Translate(barn_position.x, barn_position.y, barn_position.z);

    cowM_min = glm::vec4(cowB_min.x, cowB_min.y, cowB_min.z, 1.0f);
    cowM_max = glm::vec4(cowB_max.x, cowB_max.y, cowB_max.z, 1.0f);
    shipM_min = glm::vec4(shipB_min.x, shipB_min.y, shipB_min.z, 1.0f);
    shipM_max = glm::vec4(shipB_max.x, shipB_max.y, shipB_max.z, 1.0f);
    barnW_min = barn_model * glm::vec4(barnB_min.x, barnB_min.y, barnB_min.z, 1.0f);
    barnW_max = barn_model * glm::vec4(barnB_max.x, barnB_max.y, barnB_max.z, 1.0f);


    if ( argc > 1 )
    {
        ObjModel model(argv[1]);
        BuildTrianglesAndAddToVirtualScene(&model);
    }

    // Inicializamos o código para renderização de texto.
    TextRendering_Init();

    // Habilitamos o Z-buffer. Veja slides 46 à 66 do documento "Aula_13_Clipping_and_Culling.pdf".
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 22 à 33 do documento "Aula_13_Clipping_and_Culling.pdf".
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);



    // Variáveis auxiliares utilizadas para chamada à função
    // TextRendering_ShowModelViewProjection(), armazenando matrizes 4x4.
    glm::mat4 the_projection;
    glm::mat4 the_model;
    glm::mat4 the_view;


    //create cows on random positions
    float randomPosX;
    float randomPosZ;
    int negativeZ;
    int negativeX;
    float X;
    float Z;
    for(int i=0; i<NUM_COWS;i++){
        randomPosX = rand() % 100 + 1;
        negativeX = rand() % 2;
        randomPosZ = rand() % 100 + 1;
        negativeZ = rand() % 2;
        if (negativeX == 0)
            X = 0 - randomPosX;
        else
            X = randomPosX;

        if (negativeZ == 0)
            Z = 0 - randomPosZ;
        else
            Z = randomPosZ;
        cows[i].pos = glm::vec4(X, -0.5f, Z, 1.0f);

    }

    glm::vec4 cow_lookat = glm::vec4(1.0f,0.0f,0.0f,0.0f);
    float cosa = 0;
    glm::vec4 cow_y = glm::vec4(0.0f, 0.5f,0.0f, 0.0f);
    for(int i=0; i<NUM_COWS;i++){
        cows[i].direction = normalize(barn_position - (cows[i].pos - cow_y));
        cosa = dot(cow_lookat, cows[i].direction);   //tentativa de virar as vacas para o celeiro
        cows[i].angle = (cows[i].pos.z <=0)? -acos(cosa): acos(cosa);
    }

    double t0 = glfwGetTime();
    double t1;
    double t;

    // Ficamos em loop, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {
        // Aqui executamos as operações de renderização

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e também resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vértice e fragmentos)
        glUseProgram(program_id);

        // Computamos a posição da câmera utilizando coordenadas esféricas.  As
        // variáveis g_CameraDistance, g_CameraPhi, e g_CameraTheta são
        // controladas pelo mouse do usuário. Veja as funções CursorPosCallback()
        // e ScrollCallback().
        if(!probe_on){
            camera_lookat_l.y = g_CameraDistance * sin(g_CameraPhi);
            camera_lookat_l.z = g_CameraDistance * cos(g_CameraPhi)*cos(g_CameraTheta);
            camera_lookat_l.x = g_CameraDistance * cos(g_CameraPhi)*sin(g_CameraTheta);

            camera_position_c = ship_position + camera_lookat_l;

            camera_lookat_l = camera_lookat_l/norm(camera_lookat_l);
        }else{
            probe_lookat.y = sin(g_CameraPhi);
            probe_lookat.z = cos(g_CameraPhi)*cos(g_CameraTheta);
            probe_lookat.x = cos(g_CameraPhi)*sin(g_CameraTheta);

            //probe_lookat = probe_lookat/norm(probe_lookat);
        }

        // Abaixo definimos as varáveis que efetivamente definem a câmera virtual.
        // Veja slide 159 do documento "Aula_08_Sistemas_de_Coordenadas.pdf".
            glm::vec4 camera_view_vector = (probe_on)? -probe_lookat:-camera_lookat_l; // Vetor "view", sentido para onde a câmera está virada


        // Computamos a matriz "View" utilizando os parâmetros da câmera para
        // definir o sistema de coordenadas da câmera.  Veja slide 162 do
        // documento "Aula_08_Sistemas_de_Coordenadas.pdf".
        glm::mat4 view = Matrix_Camera_View((probe_on)? probe_position:camera_position_c, camera_view_vector, camera_up_vector);

        // Agora computamos a matriz de Projeção.
        glm::mat4 projection;

        // Note que, no sistema de coordenadas da câmera, os planos near e far
        // estão no sentido negativo! Veja slides 180-183 do documento
        // "Aula_09_Projecoes.pdf".
        float nearplane = -0.1f;  // Posição do "near plane"
        float farplane  = -100.0f; // Posição do "far plane"

        float field_of_view = 3.141592 / 3.0f;
        projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);

        model = Matrix_Identity(); // Transformação identidade de modelagem

        // Enviamos as matrizes "view" e "projection" para a placa de vídeo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));



        // Desenhamos o modelo da esfera
        // DESATIVANDO CULLING PARA PODER VER DENTRO DA ESFERA!! "ENVIRONMENT MAPPING"
        glDisable(GL_CULL_FACE);


        if (!probe_on){
            model = Matrix_Translate(camera_position_c.x, camera_position_c.y, camera_position_c.z);
        }else{
            model = Matrix_Translate(probe_position.x, probe_position.y, probe_position.z);
        }
        model = model * Matrix_Scale(SKY_SIZE, SKY_SIZE, SKY_SIZE);
        glUniformMatrix4fv(model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(object_id_uniform, SPHERE);
        DrawVirtualObject("sphere");


        //ship
        // definimos uma fonte de luz sob a nave
        glUniform4f(ship_light_uniform, ship_position.x, ship_position.y, ship_position.z, 1.0f);
        ship_model = Matrix_Translate(ship_position.x, ship_position.y, ship_position.z);
        glUniformMatrix4fv(model_uniform, 1 , GL_FALSE , glm::value_ptr(ship_model));
        glUniform1i(object_id_uniform, SHIP);
        DrawVirtualObject("ship");

        glEnable(GL_CULL_FACE);


        // Desenhamos o plano do chão
        model = Matrix_Scale(GROUND_SIZE, 1.0f, GROUND_SIZE);
        model = model * Matrix_Translate(0.0f,-1.1f,0.0f);
        glUniformMatrix4fv(model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(object_id_uniform, PLANE);
        DrawVirtualObject("plane");

        //desenhamos o celeiro
        glUniformMatrix4fv(model_uniform, 1 , GL_FALSE , glm::value_ptr(barn_model));
        glUniform1i(object_id_uniform, BARN);
        DrawVirtualObject("barn");

        // desenhamos as vacas
        if(under_abduction){
            Maintain_abduction();
        }

        for(int i=0; i<NUM_COWS;i++){
            DrawCow(i);
        }

        t1 = glfwGetTime();
        t = t1 - t0;
        if (t > 0.3){
            MoveCow();
            t0 = t1;
        }

        // Imprimimos na tela os ângulos de Euler que controlam a rotação do
        // terceiro cubo.
        TextRendering_ShowInfo(window);


        // Imprimimos na tela informação sobre o número de quadros renderizados
        // por segundo (frames per second).
        TextRendering_ShowFramesPerSecond(window);

        // O framebuffer onde a OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        glfwSwapBuffers(window);

        // Verifica com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();

    }

    // Finalizamos os recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}

