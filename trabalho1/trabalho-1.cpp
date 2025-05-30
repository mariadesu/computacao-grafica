#include <GL/glut.h>
#include <cmath>
#include <bits/stdc++.h>

using namespace std;

float PI = 3.14;

//  tempo de jogo
int tempoTotal = 300; // 5 minutos
int energiaPinguim = 60; // 1 minuto
int contadorTempo = 0;
bool jogoAtivo = true;
char mensagemFimJogo[50] = "";
int pontos = 0;

//  peixes
float peixeX[4] = {5, 2, 9, 7};  //x dos peixes
float peixeY[4] = {-5.5, -7, -3.5, -8.5}; //y dos peixes
float peixeVelocidade[4] = {0.05, 0.07, 0.04, 0.06};
int direcaoPeixe[4] = {1, 1, -1, 1};  // 1 para direita -1 para esquerda

// pinguim + peixe
bool carregandoPeixe = false;
int peixeCarregado = -1;

//  passaro
float passaroX = -10;
float passaroY = 10;
float passaroVelocidade = 0.07;
int direcaoPassaro = 1;
int passaroMergulho = -9 + rand() % 9; // pontos de mergulho: x entre -9 e -1

vector<pair<double, double>> gerarMergulho(double x0, double y0);
vector<pair<double, double>> pontosMergulho;
int indice = 0;
int mergulhando = 0;

//  pinguim
float pinguimX = 0.0;
float pinguimY = 0.0;
int direcaoPinguim = 0;
int pinguimVida = 3;

void init(void);
void display(void);
void atualizaPeixes(int value);
void atualizaTempo(int value);
void atualizaPassaro(int value);

void init(void)
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-13.75, 13.75, -10, 10, -10, 10);

    // timer de tempo
    glutTimerFunc(1000, atualizaTempo, 0);

    // timer para animar os peixinhos
    glutTimerFunc(30, atualizaPeixes, 0);

    // timer para animar o passaro
    glutTimerFunc(30, atualizaPassaro, 0);
}

//  formas
void triangle(){
    glBegin(GL_POLYGON);
        glVertex3f(-1,-1,0);
        glVertex3f(0,1,0);
        glVertex3f(1,-1,0);
    glEnd();

    glColor3f(0, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-1, -1);
        glVertex2f(0, 1);
        glVertex2f(1, -1);
    glEnd();
}

void square(){
    glBegin(GL_POLYGON);
        glVertex3f(-1,-1,0);
        glVertex3f(1,-1,0);
        glVertex3f(1,1,0);
        glVertex3f(-1,1,0);
    glEnd();

    glColor3f(0, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-1, -1);
        glVertex2f(1, -1);
        glVertex2f(1, 1);
        glVertex2f(-1, 1);
    glEnd();
}

void circle(double size){
    glBegin(GL_POLYGON);
    for(int d = 0; d < 32; d++){
        double angulo = (2.0*PI/32.0)*d;
        glVertex3f(size*cos(angulo),size*sin(angulo),0);
    }
    glEnd();

    glColor3f(0, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    for(int d = 0; d < 32; d++){
        double angulo = (2.0*PI/32.0)*d;
        glVertex3f(size*cos(angulo),size*sin(angulo),0);
    }
    glEnd();
}

//  cenario
void chao(){
    glColor3f(0.5, 0.7, 1);
    glBegin(GL_POLYGON);
        glVertex3f(-15,-10,0);
        glVertex3f(15,-10,0);
        glVertex3f(15,-2,0);
        glVertex3f(-15,-2,0);
    glEnd();
} 

void mar(){
    glColor3f(0, 0, 1);
    glBegin(GL_POLYGON);
        glVertex3f(0,-10,0);
        glVertex3f(15,-10,0);
        glVertex3f(15,-2,0);
        glVertex3f(0,-2,0);
    glEnd();
}

void ceu(){
    glColor3f(0.8, 0.9, 1);
    glBegin(GL_POLYGON);
        glVertex3f(-15,-2,0);
        glVertex3f(15,-2,0);
        glVertex3f(15,10,0);
        glVertex3f(-15,10,0);
    glEnd();
}

void iceberg(){
    glColor3f(1, 1, 1);
    glBegin(GL_POLYGON);
        glVertex3f(-15,-2,0);
        glVertex3f(15,-2,0);
        glVertex3f(15,2,0);
        glVertex3f(7,1,0);
        glVertex3f(5,5,0);
        glVertex3f(0,0,0);
        glVertex3f(-6,4,0);
        glVertex3f(-8,1,0);
        glVertex3f(-15,5,0);
    glEnd();
}

//  animais
void pinguim(){
    //cabeça
    glColor3f(0.6, 0.6, 0.9);
    glPushMatrix();
    circle(1);
    glPopMatrix();
    //olho
    glColor3f(0, 0, 0);
    glPushMatrix();
    glTranslatef(-0.2,0.1,0);
    glScalef(0.5,1,1);
    circle(0.3);
    glPopMatrix();
    //bico
    glColor3f(1, 0.5, 0.3);
    glPushMatrix();
    glTranslatef(1,0,0);
    glScalef(0.5,0.5,1);
    glRotatef(45,1,1,1);
    triangle();
    glPopMatrix();

    //corpo
    glColor3f(0.6, 0.6, 0.9);
    glPushMatrix();
    glTranslatef(0,-3,0);
    glScalef(0.5,1,1);
    circle(2);
    glPopMatrix();
    //barriga
    glColor3f(1, 1, 1);
    glPushMatrix();
    glTranslatef(0.5,-3,0);
    glScalef(0.5,1.5,1);
    circle(1);
    glPopMatrix();

    //pes
    glColor3f(1, 0.5, 0.3);
    glPushMatrix();
    glTranslatef(-0.6,-5,0);
    glScalef(0.5,0.5,1);
    triangle();
    glPopMatrix();
    glColor3f(1, 0.5, 0.3);
    glPushMatrix();
    glTranslatef(0.6,-5,0);
    glScalef(0.5,0.5,1);
    triangle();
    glPopMatrix();
}

void pinguimFilho(){
    glPushMatrix();
    glScalef(0.3,0.3,1);
    pinguim();
    glPopMatrix();
}

void pinguimPai(){
    glPushMatrix();
    glScalef(0.65, 0.65, 1);
    pinguim();
    glPopMatrix();
}

//peixe
void peixe(int invertido) {
    glPushMatrix();
    glScalef(0.3, 0.3, 1);
    if(invertido) {
        glScalef(-1, 1, 1); 
    }
    
    // corpo 
    glPushMatrix();
    glScalef(1.5, 1.0, 1); // achatadinho
    glColor3f(1.0, 0.6, 0.4);
    circle(1);
    glPopMatrix();
    // cauda
    glColor3f(0.9, 0.4, 0.2);
    glPushMatrix();
    glTranslatef(-2, 0, 0);
    glRotatef(-90, 0, 0, 1);
    glScalef(0.6, 0.6, 1);
    triangle();
    glPopMatrix();
    glPopMatrix();
}

bool colisaoPinguimPassaro(float x1, float y1, float x2, float y2, float raio) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return (dx*dx + dy*dy) < (raio*raio);
}

void atualizaPassaro(int value) {
    if (!jogoAtivo) return;
    
    if (colisaoPinguimPassaro(pinguimX, pinguimY, passaroX, passaroY, 2)) {
        // se o pinguim colidir com o passaro, o jogo acaba
        pinguimVida--;
        carregandoPeixe = false;
        pinguimX = -13; // reposiciona o pinguim
        if(pinguimVida <= 0) { 
            jogoAtivo = false;
            if(pinguimVida <= 0) {
                sprintf(mensagemFimJogo, "Fim de Jogo! :(");
            } else {
                sprintf(mensagemFimJogo, "Parabens! Voce ganhou! :)");
            }
        }
    }

    if(pontosMergulho.empty()){
        pontosMergulho = gerarMergulho(passaroMergulho, passaroY);
    }
   
   if(passaroX > 2) {direcaoPassaro = -1; passaroMergulho = -6 + rand() % 6; pontosMergulho = gerarMergulho(passaroMergulho, passaroY);}
   if(passaroX < -9) {direcaoPassaro = 1; passaroMergulho = -6 + rand() % 6;  pontosMergulho = gerarMergulho(passaroMergulho, passaroY);}
   
   if((passaroX > (passaroMergulho-1) && passaroX < (passaroMergulho+1) && direcaoPassaro == 1) || 
      (passaroX < (passaroMergulho+1) && passaroX > (passaroMergulho-1) && direcaoPassaro == -1) || 
      mergulhando == 1){
       mergulhando = 1;
       passaroX = pontosMergulho[indice].first;
       passaroY = pontosMergulho[indice].second;
       indice++;
       if (indice>=pontosMergulho.size()){
        indice = 0;
        mergulhando = 0;
        passaroY = 2 + rand() % 9; // gera nova altura
       }
    }
    else{
        passaroX += passaroVelocidade * direcaoPassaro;
    }

    glutPostRedisplay();
    glutTimerFunc(30, atualizaPassaro, 0);
}


bool colisaoPinguimPeixe(float x1, float y1, float x2, float y2, float raio) {
    float dx = x1 - x2;
    float dy = y1 - y2;

    return (dx*dx + dy*dy) < (raio*raio);
}

void atualizaPeixes(int value) {
    if (!jogoAtivo) return;

    //verifica para captura
    if (!carregandoPeixe) {
        for(int i = 0; i < 4; i++) {
            if (colisaoPinguimPeixe(pinguimX, pinguimY, peixeX[i], peixeY[i], 1.5)) {
                carregandoPeixe = true;
                peixeCarregado = i;
                
                // nasce novo peixe
                peixeX[i] = rand() % 6 + 7;  // novo X
                peixeY[i] = -3 - (rand() % 6); // novo Y
                direcaoPeixe[i] = (rand() % 2) ? 1 : -1; // nova direção

                break;
            }
        }
    }
    
    //atualiza posição dos peixes
    for(int i = 0; i < 4; i++) {
        if (carregandoPeixe && i == peixeCarregado) continue;
        
        peixeX[i] += peixeVelocidade[i] * direcaoPeixe[i];
        
        if(peixeX[i] > 13) direcaoPeixe[i] = -1;
        if(peixeX[i] < 0.5) direcaoPeixe[i] = 1;
    }
    
    glutPostRedisplay();
    glutTimerFunc(30, atualizaPeixes, 0);
}

void pinguimComPeixe() {
    glPushMatrix();
    glScalef(0.65, 0.65, 1);
    pinguim();
    glPopMatrix();
    
    // peixinho na boca do pinguim
    glPushMatrix();
    glTranslatef(0.75, -0.3, 0);
    glScalef(0.7, 0.7, 1);
    peixe(0);
    glPopMatrix();
}

void passaro() {
    glPushMatrix();
    glScalef(2, 1.5, 1);
    
    glColor3f(0.0f, 0.0f, 0.0f);  
    glLineWidth(14); 
    glBegin(GL_LINES);
        glVertex2f(-0.5f, 0.0f);
        glVertex2f(0.0f, -0.5f); 
        glVertex2f(0.0f, -0.5f);
        glVertex2f(0.5f, 0.0f);
    glEnd();

    glColor3f(1.0f, 0.6f, 0.6f);
    glLineWidth(10);  
    glBegin(GL_LINES);
        glVertex2f(-0.5f, 0.0f);
        glVertex2f(0.0f, -0.5f); 
        glVertex2f(0.0f, -0.5f);
        glVertex2f(0.5f, 0.0f);
    glEnd();

    glPopMatrix();
}

// função para atualizar o tempo
void atualizaTempo(int value){
    if (jogoAtivo){
        contadorTempo++;
        energiaPinguim--;

        if(energiaPinguim <=0 || contadorTempo >= tempoTotal) { 
            jogoAtivo = false;
            if(energiaPinguim <= 0) {
                sprintf(mensagemFimJogo, "Fim de Jogo! :(");
            } else {
                sprintf(mensagemFimJogo, "Parabens! Voce ganhou! :)");
            }
        }
        
        glutPostRedisplay();
        glutTimerFunc(1000, atualizaTempo, 0); //chama a função após 1 segundo
    }
}

// função de exibição na tela
void exibeTempo() {
    char tempo[50];
    char energia[50];
    char vidas[50];
    char pontuacao[50];

    sprintf(tempo, "Tempo: %d:%02d", (tempoTotal-contadorTempo)/60, (tempoTotal-contadorTempo)%60);
    sprintf(energia, "Energia: %d", energiaPinguim);
    sprintf(vidas, "Vidas: %d", pinguimVida);
    sprintf(pontuacao, "Pontos: %d", pontos);

    glColor3f(0, 0, 0);
    glRasterPos2f(-13, 9);
    for (char* c = tempo; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    glRasterPos2f(-13, 8);
    for (char* c = energia; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glRasterPos2f(-13, 7);
    for (char* c = vidas; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glRasterPos2f(9, 9);
    for (char* c = pontuacao; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// função para gerar o mergulho do passaro
vector<pair<double, double>> gerarMergulho(double x0, double y0) {
    vector<pair<double, double>> pontos;
    double x1 = x0 + 3;
    double yV = 0;
    int steps = 30;

    if(direcaoPassaro == -1) {x1 = x0 - 6;} // se o passaro estiver indo para a esquerda, muda o x1

    // Vértice: no meio de x0 e x1, com y = yV
    double h = (x0 + x1) / 2.0;
    double k = yV;

    // Usamos o ponto (x0, y0) para descobrir o coeficiente 'a'
    double a = (y0 - k) / ((x0 - h) * (x0 - h));

    for (int i = 0; i <= steps; i++) {
        double t = i / (double)steps;
        double x = x0 + t * (x1 - x0);
        double y = a * (x - h) * (x - h) + k;
        pontos.push_back({x, y});
    }
    
    return pontos;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    //  cenario
    chao();
    mar();
    ceu();
    iceberg();

    //  personagens

    //pinguim filho
    glPushMatrix();
    glTranslatef(-12,-0.5,0);
    pinguimFilho();
    glPopMatrix();

    //movimentacao do pinguim pai
    //impede que o pinguim saia para fora da tela
    if (pinguimX < -13){
        pinguimX = -13;
        pinguimY = 1.5;
    }
    else if (pinguimX > 13){
        pinguimX = 13;
    }
    else if (pinguimX >= -13 && pinguimX <= 0){ //terra
        //pinguim so se move em x em terra
        pinguimY = 1.5;
    }
    else if (pinguimX >= 0 && pinguimX <= 13){ //agua
        if (pinguimY > -1.5){
            pinguimY = -1.5;
        }
        else if (pinguimY < -8.5){
            pinguimY = -8.5;
        }
    }

    // desenha o pinguim pai
    glPushMatrix();
    glTranslatef(pinguimX, pinguimY, 0);
    if (carregandoPeixe && pinguimX != -10) {
        if (direcaoPinguim == -1) {
            glScalef(-1, 1, 1);
        }
        if (pinguimX >= 2 && pinguimX <= 9) {
            glRotatef(-90, 0, 0, 1);
        }
        pinguimComPeixe();
    } 
    else if (carregandoPeixe && pinguimX == -10) {
        if (direcaoPinguim == -1) {
            glScalef(-1, 1, 1);
        }
        if (pinguimX >= 2 && pinguimX <= 9) {
            glRotatef(-90, 0, 0, 1);
        }
        carregandoPeixe = false;
        energiaPinguim = 60; // +1 minuto
        pontos++; // ganha 1 ponto
        pinguimPai();
    }
    else if (carregandoPeixe == false){
        if (direcaoPinguim == -1) {
            glScalef(-1, 1, 1);
        }
        if (pinguimX >= 2 && pinguimX <= 9) {
            glRotatef(-90, 0, 0, 1);
        }
        pinguimPai();
    }
    glPopMatrix();

    //peixes (que nao foram pegos)
    for(int i = 0; i < 4; i++) {
        if (!carregandoPeixe || i != peixeCarregado) {
            glPushMatrix();
            glTranslatef(peixeX[i], peixeY[i], 0);
            peixe(direcaoPeixe[i] == -1);
            glPopMatrix();
        }
    }

    //passaro
    glPushMatrix();
    glTranslatef(passaroX, passaroY, 0);
    passaro();
    glPopMatrix();

    //mostra o tempo na tela
    exibeTempo();

    //mensagem de fim de jogo
    if(!jogoAtivo && mensagemFimJogo[0] != '\0') {
        glColor3f(1, 0, 0); // vermelho
        glRasterPos2f(-1, 0);
        
        for(char* c = mensagemFimJogo; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }    

    glFlush();
}

void specialKeys(int key, int x, int y) {
    if (!jogoAtivo) return;

    switch (key) {
        case GLUT_KEY_LEFT:
            direcaoPinguim = -1;
            pinguimX -= 0.5;
            break;
        case GLUT_KEY_RIGHT:
            direcaoPinguim = 1;
            pinguimX += 0.5;
            break;
        case GLUT_KEY_UP:
            pinguimY += 0.5;
            break;
        case GLUT_KEY_DOWN:
            pinguimY -= 0.5;
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    srand(time(0));
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize (1100, 800);
    glutInitWindowPosition (200, 0);
    glutCreateWindow ("Trabalho 1: Maria Eduarda & Samira");

    init();
    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys);
    glutMainLoop();

    return 0;  
}
