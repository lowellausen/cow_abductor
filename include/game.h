#include "others.h"
void DrawCow(int i);
void AbductCow();
bool Bbox_collision(glm::vec4 A_min, glm::vec4 A_max, glm::vec4 B_min, glm::vec4 B_max);
int AllCowShip_collision();
void Maintain_abduction();
bool AnotherShipInTheWall();
bool CowOnBarn(int i);
void MoveCow();
int Probe_collision();

// função que desenha uma vaca
void DrawCow(int i){
    // Desenhamos  vaca
    //model = Matrix_Scale(10.0f, 1.0f, 10.0f);
    cows[i].model = Matrix_Translate(cows[i].pos.x, cows[i].pos.y, cows[i].pos.z) * Matrix_Rotate_Y(cows[i].angle);
    glUniformMatrix4fv(model_uniform, 1 , GL_FALSE , glm::value_ptr(cows[i].model));
    if(cows[i].alive && !cows[i].safe){
        glUniform1i(object_id_uniform, COW); //3 é o nÚmero da vaca
        DrawVirtualObject("cow");
    }
    else if (cows[i].pos.y == -1.0f){
        glUniform1i(object_id_uniform, POOL); //5 É O NÚMERO DO SANGUE
        DrawVirtualObject("pool");
    }
}

//Função que testa colisão entre duas bounding boxes A e B
//recebe duas bbox EM COORDENADAS GLOBAIS
bool Bbox_collision(glm::vec4 A_min, glm::vec4 A_max, glm::vec4 B_min, glm::vec4 B_max){
    return (A_min.x <= B_max.x && A_max.x >= B_min.x) &&
            (A_min.y <= B_max.y && A_max.y >= B_min.y) &&
            (A_min.z <= B_max.z && A_max.z >= B_min.z);
}


//Função que testa se há colisão da nave contra alguma vaca e retorne o índice desta vaca, -1 caso não haja
int AllCowShip_collision(){
    int i;
    glm::vec4 shipW_min = ship_model * shipM_min;
    glm::vec4 shipW_max = ship_model * shipM_max;

    glm::vec4 cowW_min;
    glm::vec4 cowW_max;
    for(i=0; i < NUM_COWS; i++){
        if(!cows[i].alive) continue;
        cowW_min = cows[i].model * cowM_min;
        cowW_max = cows[i].model * cowM_max;
        if(Bbox_collision(cowW_min, cowW_max, shipW_min,shipW_max)){
            return i;
        }
    }
    return -1;
}


void MoveCow(){
    int i;
    for (i = 0; i < NUM_COWS; i++){
        if (cows[i].abducted == 0 && !cows[i].safe && cows[i].alive){
            cows[i].pos.x += cows[i].direction.x/1.5;
            cows[i].pos.z += cows[i].direction.z/1.5;
            if (CowOnBarn(i)){
                cows[i].safe = true;
                safeCows++;
            }
        }
    }
}

//Função que retorna se a probe colidiu em algum objeto
int Probe_collision(){
    int i;

    glm::vec4 shipW_min = ship_model * shipM_min;
    glm::vec4 shipW_max = ship_model * shipM_max;

    glm::vec4 probeW_min = probe_position + probe_min;
    glm::vec4 probeW_max = probe_position + probe_max;

    if(Bbox_collision(probeW_min, probeW_max, shipW_min,shipW_max)){
            return NUM_COWS;   //colisão com a nave retorna vacas
    }

    if(Bbox_collision(probeW_min, probeW_max, barnW_min,barnW_max)){
            return NUM_COWS+2;   //colisão com o celeiro retorna vacas+2
    }

    if(probe_position.y <= -1.0) return NUM_COWS +1; //colisão com o chão retorna vacas +1

    glm::vec4 cowW_min;
    glm::vec4 cowW_max;
    for(i=0; i < NUM_COWS; i++){
        if(!cows[i].alive) continue;
        cowW_min = cows[i].model * cowM_min;
        cowW_max = cows[i].model * cowM_max;
        if(Bbox_collision(cowW_min, cowW_max, probeW_min,probeW_max)){
            return i;  //colisão com uma vaca retorna seu índice
        }
    }
    return -1;
}

bool CowShip_collision(int i){
    glm::vec4 shipW_min = ship_model * shipM_min;
    glm::vec4 shipW_max = ship_model * shipM_max;

    glm::vec4 cowW_min;
    glm::vec4 cowW_max;

    cowW_min = cows[i].model * cowM_min;
    cowW_max = cows[i].model * cowM_max;
    if(Bbox_collision(cowW_min, cowW_max, shipW_min,shipW_max)){
            return true;
    }
    return false;
}

//Função que retorna o índice da primeira vaca dentro do circulo de iluminação da nave. -1 caso nenhuma
int CowCrossHair(){
    int i;

    glm::vec4 cow_to_ship;
    for(i=0; i < NUM_COWS; i++){
        cow_to_ship = cows[i].pos - ship_position;
        if(dotproduct(cow_to_ship/norm(cow_to_ship), -camera_up_vector) >= cos(ALPHA)){
            return i;
        }
    }

    return -1;
}

//Função que intersecta a nave com as paredes
bool AnotherShipInTheWall(){
    glm::vec4 shipW_min = ship_model * shipM_min;
    glm::vec4 shipW_max = ship_model * shipM_max;
    return((ship_position.x <= -GROUND_SIZE + WALL_DIST) || (ship_position.x >= GROUND_SIZE - WALL_DIST)
           || (ship_position.z <= -GROUND_SIZE + WALL_DIST) || (ship_position.z >= GROUND_SIZE - WALL_DIST)
            || Bbox_collision(shipW_min, shipW_max, barnW_min, barnW_max));
}

void AbductCow(){
    int index;

    index = CowCrossHair();

    if (index != -1){
        cows[index].abducted = 1;
        under_abduction = true;
        cow_abducted = index;
    }
}

void Maintain_abduction(){
    bool colide = CowShip_collision(cow_abducted);
    if(colide){
            cows[cow_abducted].alive = false;
            if (cows[cow_abducted].pos.y == -0.5f){
                cows[cow_abducted].pos.y -= 0.5f;
            }
    }

    if(cows[cow_abducted].alive){
        cows[cow_abducted].pos.y += 0.01f;
    }else{
        cows[cow_abducted].abducted = 2;
        under_abduction = false;
        cow_abducted = -1;
        abductedCows++;
    }
}

//Função que retorna se alguma vaca chegou no celeiro
bool CowOnBarn(int i){
    glm::vec4 cowW_min;
    glm::vec4 cowW_max;
    cowW_min = cows[i].model * cowM_min;
    cowW_max = cows[i].model * cowM_max;
    if(Bbox_collision(cowW_min, cowW_max, barnW_min,barnW_max)){
        return true;
    }
    return false;
}
