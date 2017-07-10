#include "game.h"

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula (slide 183 do
    // documento "Aula_03_Rendering_Pipeline_Grafico.pdf").
    glViewport(0, 0, width, height);

    // Atualizamos também a razão que define a proporção da janela (largura /
    // altura), a qual será utilizada na definição das matrizes de projeção,
    // tal que não ocorra distorções durante o processo de "Screen Mapping"
    // acima, quando NDC é mapeado para coordenadas de pixels. Veja slide 199
    // do documento "Aula_09_Projecoes.pdf".
    //
    // O cast para float é necessário pois números inteiros são arredondados ao
    // ser divididos!
    g_ScreenRatio = (float)width / height;
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_LeftMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_RightMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_RightMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_RightMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_MiddleMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_MiddleMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_MiddleMouseButtonPressed = false;
    }
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Abaixo executamos o seguinte: caso o botão esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o último
    // instante de tempo, e usamos esta movimentação para atualizar os
    // parâmetros que definem a posição da câmera dentro da cena virtual.
    // Assim, temos que o usuário consegue controlar a câmera.

    if (g_LeftMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos parâmetros da câmera com os deslocamentos
        g_CameraTheta -= 0.01f*dx;
        g_CameraPhi   += 0.01f*dy;

        // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
        float phimax = 3.141592f/2;
        float phimin = -phimax;

        if (g_CameraPhi > phimax)
            g_CameraPhi = phimax;

        if (g_CameraPhi < phimin)
            g_CameraPhi = phimin;

        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_RightMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!

        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_MiddleMouseButtonPressed)
    {
        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f*yoffset;

    if (g_CameraDistance < 0.0f)
        g_CameraDistance = 0.0f;
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // O código abaixo implementa a seguinte lógica:
    //   Se apertar tecla X       então g_AngleX += delta;
    //   Se apertar tecla shift+X então g_AngleX -= delta;
    //   Se apertar tecla Y       então g_AngleY += delta;
    //   Se apertar tecla shift+Y então g_AngleY -= delta;
    //   Se apertar tecla Z       então g_AngleZ += delta;
    //   Se apertar tecla shift+Z então g_AngleZ -= delta;

    float delta = 3.141592 / 16; // 22.5 graus, em radianos.

    if (key == GLFW_KEY_X)// desce a nave
    {
        //g_AngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
        if (ship_position.y <= -0.5) return;
        ship_position.y -= 0.1f;
        ship_model = Matrix_Translate(ship_position.x, ship_position.y, ship_position.z);
        if(AnotherShipInTheWall()){
            ship_position.y += 0.2f;
        }
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        g_AngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }
    if (key == GLFW_KEY_Z) //sobe a nave
    {
        //g_AngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
        if (ship_position.y >= 5.0f) return;
        ship_position.y += 0.1f;
    }



    if(!probe_on){
        int c = AllCowShip_collision();
        if(c!=-1){
                cows[c].alive = false;
                murderedCows++;
                if (cows[c].pos.y == -0.5f){
                    cows[c].pos.y -= 0.5f;
                }
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            glm::vec4 step = INC* camera_lookat_l;
            ship_position -= step;
            ship_position.y += step.y;
            ship_model = Matrix_Translate(ship_position.x, ship_position.y, ship_position.z);
            if(AnotherShipInTheWall()){  //caso a alteração anterior gere uma colisão, desfazemos ela
                ship_position += step;
                ship_position.y -= step.y;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            glm::vec4 step = INC* camera_lookat_l;
            ship_position += step;
            ship_position.y -= step.y;
            ship_model = Matrix_Translate(ship_position.x, ship_position.y, ship_position.z);
            if(AnotherShipInTheWall()){  //caso a alteração anterior gere uma colisão, desfazemos ela
                ship_position -= step;
                ship_position.y += step.y;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            glm::vec4 step = INC* crossproduct(camera_lookat_l,camera_up_vector);
            ship_position += step;
            ship_model = Matrix_Translate(ship_position.x, ship_position.y, ship_position.z);
            if(AnotherShipInTheWall()){  //caso a alteração anterior gere uma colisão, desfazemos ela
                ship_position -= step;
            }
        }                                       // A e D andam em direção a um vetor que aponta para o lado da câmera
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            glm::vec4 step = INC* crossproduct(camera_lookat_l,camera_up_vector);
            ship_position -= step;
            ship_model = Matrix_Translate(ship_position.x, ship_position.y, ship_position.z);
            if(AnotherShipInTheWall()){  //caso a alteração anterior gere uma colisão, desfazemos ela
                ship_position += step;
            }
        }
    }else{
        int c = Probe_collision();
        if(c!=-1){
            if((c>=0) && (c<NUM_COWS)){
                cows[c].alive = false;
                murderedCows++;
                if (cows[c].pos.y == -0.5f){
                    cows[c].pos.y -= 0.5f;
                }
            }
            probe_position = glm::vec4(2.0f,0.0f,0.0f,0.0f) + ship_position; //if crash to a cow come back to ship
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            probe_position -= INC* probe_lookat;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            probe_position += INC* probe_lookat;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            probe_position += INC* crossproduct(probe_lookat,camera_up_vector);
        }                                       // A e D andam em direção a um vetor que aponta para o lado da câmera
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            probe_position -= INC* crossproduct(probe_lookat,camera_up_vector);
        }

    }

    // Se o usuário apertar a tecla espaço, resetamos os ângulos de Euler para zero.
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {

       AbductCow();
    }
    // Se o usuário apertar a tecla P, utilizamos projeção perspectiva.
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        //g_UsePerspectiveProjection = true;
        probe_on = (probe_on)? false:true;
        probe_position = glm::vec4(2.0f,0.0f,0.0f,0.0f) + ship_position;
    }

    // Se o usuário apertar a tecla O, utilizamos projeção ortográfica.
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        //g_UsePerspectiveProjection = false;
    }

    // Se o usuário apertar a tecla H, fazemos um "toggle" do texto informativo mostrado na tela.
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        g_ShowInfoText = !g_ShowInfoText;
    }

    // Se o usuário apertar a tecla R, recarregamos os shaders dos arquivos "shader_fragment.glsl" e "shader_vertex.glsl".
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        LoadShadersFromFiles();
        fprintf(stdout,"Shaders recarregados!\n");
        fflush(stdout);
    }
}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}
