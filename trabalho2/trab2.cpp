/**
 * trabalho-2.cpp
 *
 * Esse codigo implementa a versao 3D do jogo do pinguim desenvolvido no trabalho 1.
 *
 * Alunos:
 * Arthur Fernandes,
 * Maria Eduarda Silva,
 * Samira Magalhaes
 *
 * Principais funcionalidades:
 * - Modelo 3D para pinguins e peixes utilizando formas do GLUT.
 * - Esquema de controle para navegacao 3D (rotacao e movimentacao para frente/tras).
 * - Exibicao em quatro viewports (topo, frontal, de lado e perspectiva livre).
 * - A camera de todos os viewports segue a mamae pinguim.
 * - Logica do jogo adaptada para um ambiente 3D, incluindo deteccao de colisao,
coleta de peixes e mecanica de alimentacao ao pinguim filhote.
 * - Iluminacao simples habilitada.
 */

#define GL_SILENCE_DEPRECATION
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdio>
using namespace std;



// Adiciona suporte a stbi_load
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PI 3.141592654

int tempoTotal = 300; // 5 minutos
int energiaPinguimzinho = 60; // 1 minuto
int contadorTempo = 0;
bool jogoAtivo = true;
char mensagemFimJogo[50] = "";
int pontos = 0;

// --- Variaveis Mamae Pinguim ---
float pinguimX = 0.0f, pinguimY = 0.5f, pinguimZ = 5.0f; // Start in front of the baby
float pinguimRotacao = 0.0f; // Rotation angle around the Y-axis
bool carregandoPeixe = false;

// --- Variaveis Bebe Pinguim ---
const float pinguimzinhoX = 0.0f, pinguimzinhoY = 0.35f, pinguimzinhoZ = 0.0f;

// --- Variaveis Peixe ---
struct Peixe {
    float x, y, z;
    bool peixeCapturado;
};
std::vector<Peixe> peixes(4);

// --- Variaveis Buraco ---
struct Buraco {
    float x, z, raio;
};
std::vector<Buraco> buracos(3);

// --- Variaveis de Textura ---
GLuint texturaPinguimCorpo = 0, texturaCeu = 0, texturaGelo = 0, texturaPinguimBarriga = 0, texturaPeixe = 0;
// --- NOVO: Array para as texturas do Skybox ---
GLuint texturasCeu[6];
GLUquadric* quadric = nullptr;

// --- Funcoes ---
void desenhaPinguim(bool ehFilhote);
void desenhaPeixe();
void setupViewports();
void update(int value);
void verificaColisao();
void spawnItems();
GLuint carregaTextura(const char* caminho);

/**
 * @brief desenha um modelo de pinguim usando esferas.
 * @param ehFilhote true, desenha uma versao menor.
 */
GLuint carregarTextura(const char *caminho){
    int w, h, ch;

    unsigned char *img = stbi_load(caminho, &w, &h, &ch, 3);

    if (!img)
    {
        printf("Falha %s\n", caminho);
        exit(1);
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);

    stbi_image_free(img);

    return tex;
}

// --- NOVO: Função para desenhar o Skybox ---
void desenhaSkybox() {
    glPushMatrix();

    // O Skybox deve estar centralizado na câmera para dar a ilusão de infinito.
    // Como a câmera segue o pinguim, usaremos a posição dele.
    glTranslatef(pinguimX, pinguimY, pinguimZ);

    glPushAttrib(GL_ENABLE_BIT); // Salva os estados atuais de enable/disable
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);   // O céu não deve ser afetado pela luz da cena
    glDisable(GL_DEPTH_TEST); // O céu deve ser sempre desenhado no fundo

    glColor3f(1.0, 1.0, 1.0); // Cor branca para não tingir a textura

    float s = 75.0f; // Tamanho do Skybox (deve ser grande)

    // Face Direita (+X)
    glBindTexture(GL_TEXTURE_2D, texturasCeu[0]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(s, -s, -s);
        glTexCoord2f(1, 0); glVertex3f(s, -s, s);
        glTexCoord2f(1, 1); glVertex3f(s, s, s);
        glTexCoord2f(0, 1); glVertex3f(s, s, -s);
    glEnd();

    // Face Esquerda (-X)
    glBindTexture(GL_TEXTURE_2D, texturasCeu[1]);
    glBegin(GL_QUADS);
        glTexCoord2f(1, 0); glVertex3f(-s, -s, -s);
        glTexCoord2f(1, 1); glVertex3f(-s, s, -s);
        glTexCoord2f(0, 1); glVertex3f(-s, s, s);
        glTexCoord2f(0, 0); glVertex3f(-s, -s, s);
    glEnd();

    // Face Superior (+Y)
    glBindTexture(GL_TEXTURE_2D, texturasCeu[2]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex3f(-s, s, -s);
        glTexCoord2f(0, 0); glVertex3f(-s, s, s);
        glTexCoord2f(1, 0); glVertex3f(s, s, s);
        glTexCoord2f(1, 1); glVertex3f(s, s, -s);
    glEnd();

    // Face Inferior (-Y)
    glBindTexture(GL_TEXTURE_2D, texturasCeu[3]);
    glBegin(GL_QUADS);
        glTexCoord2f(1, 1); glVertex3f(s, -s, -s);
        glTexCoord2f(0, 1); glVertex3f(-s, -s, -s);
        glTexCoord2f(0, 0); glVertex3f(-s, -s, s);
        glTexCoord2f(1, 0); glVertex3f(s, -s, s);
    glEnd();
    
    // Face Frontal (-Z)
    glBindTexture(GL_TEXTURE_2D, texturasCeu[4]);
    glBegin(GL_QUADS);
        glTexCoord2f(1, 0); glVertex3f(s, -s, -s);
        glTexCoord2f(1, 1); glVertex3f(s, s, -s);
        glTexCoord2f(0, 1); glVertex3f(-s, s, -s);
        glTexCoord2f(0, 0); glVertex3f(-s, -s, -s);
    glEnd();

    // Face Traseira (+Z)
    glBindTexture(GL_TEXTURE_2D, texturasCeu[5]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(s, -s, s);
        glTexCoord2f(1, 0); glVertex3f(-s, -s, s);
        glTexCoord2f(1, 1); glVertex3f(-s, s, s);
        glTexCoord2f(0, 1); glVertex3f(s, s, s);
    glEnd();

    glPopAttrib(); // Restaura os estados de enable/disable
    glPopMatrix();
}

void desenhaPinguim(bool ehFilhote) {
    glPushMatrix();
    float escala = ehFilhote ? 0.5f : 1.0f;
    glScalef(escala, escala, escala);

    // --- LÓGICA DE TEXTURA ADICIONADA ---
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f); // Cor base branca para não tingir as texturas

    // corpo (texturizado com pelos escuros)
    glBindTexture(GL_TEXTURE_2D, texturaPinguimCorpo);
    glPushMatrix();
    glScalef(1.0f, 1.2f, 1.0f);
    // Substitui glutSolidSphere por gluSphere para aplicar a textura
    gluSphere(quadric, 0.5, 20, 20);
    glPopMatrix();

    // barriga (texturizada com pelos claros)
    glBindTexture(GL_TEXTURE_2D, texturaPinguimBarriga);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.3f);
    glScalef(0.7f, 0.9f, 0.7f);
    // Substitui glutSolidSphere por gluSphere
    gluSphere(quadric, 0.4, 20, 20);
    glPopMatrix();

    // cabeça
    // Usa a mesma textura do corpo do pinguim
    glBindTexture(GL_TEXTURE_2D, texturaPinguimCorpo);
    glPushMatrix();
    glTranslatef(0.0f, 0.7f, 0.0f);
    // Usa gluSphere para que a textura possa ser aplicada
    gluSphere(quadric, 0.3, 20, 20);
    glPopMatrix();

    // Desabilita texturas para as partes que não serão texturizadas
    glDisable(GL_TEXTURE_2D);
    // --- FIM DA LÓGICA DE TEXTURA ---


    // bico (laranja)
    glColor3f(1.0f, 0.65f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.7f, 0.25f);
    glScalef(1.0, 1.0, 1.5);
    glutSolidCone(0.1, 0.3, 10, 2);
    glPopMatrix();

    // olhos (branco)
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.1f, 0.8f, 0.25f);
    glutSolidSphere(0.05, 10, 10);
    glTranslatef(0.2f, 0.0f, 0.0f);
    glutSolidSphere(0.05, 10, 10);
    glPopMatrix();

    glPopMatrix();
}

/**
 * @brief desenha um modelo de peixe usando elipsoides e cones.
 */
void desenhaPeixe() {
    glPushMatrix();
    glScalef(0.4f, 0.4f, 0.4f);

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para não tingir a textura
    glBindTexture(GL_TEXTURE_2D, texturaPeixe);

    // Corpo (elipsoide texturizado)
    glPushMatrix();
    glScalef(1.5f, 0.8f, 0.5f);
    // Substitui glutSolidSphere por gluSphere para aplicar a textura
    gluSphere(quadric, 0.5, 15, 15);
    glPopMatrix();

    // Cauda (cone texturizado)
    glPushMatrix();
    glTranslatef(-0.8f, 0.0f, 0.0f);
    glRotatef(90, 0, 1, 0);
    // Substitui glutSolidCone por gluCylinder para aplicar a textura
    // gluCylinder(quadric, raioBase, raioTopo, altura, fatias, pilhas)
    gluCylinder(quadric, 0.3, 0.0, 0.5, 10, 2);
    glPopMatrix();

    // Desabilita a textura ao final do desenho do objeto
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

/**
 * @brief Inicializa a posicao dos itens.
 */
void initGame() {
    spawnItems();
}

/**
 * @brief Respawna peixes e buracos aleatoriamente no gelo.
 */
void spawnItems() {
    // Spawn peixe
    for (auto& peixe : peixes) {
        peixe.x = (rand() % 200 - 100) / 10.0f; // -10 to 10
        peixe.z = (rand() % 200 - 100) / 10.0f; // -10 to 10
        peixe.y = 0.1f;
        peixe.peixeCapturado = false;
    }
    // Spawn buracos
    for (auto& buraco : buracos) {
        buraco.x = (rand() % 160 - 80) / 10.0f; // -8 to 8
        buraco.z = (rand() % 160 - 80) / 10.0f; // -8 to 8

        if((buraco.x == 0.0 && buraco.z == 0.5) || (buraco.x == 0.0 && buraco.z == 0.0)){
            while((buraco.x == 0.0 && buraco.z == 0.5) || (buraco.x == 0.0 && buraco.z == 0.0)){
                buraco.x = (rand() % 160 - 80) / 10.0f;
                buraco.z = (rand() % 160 - 80) / 10.0f;
            }
           
        }

        buraco.raio = 1.0f + (rand() % 5) / 10.0f; // 1.0 to 1.5
    }
}

/**
 * @brief Desenha texto 2D na tela.
 * @param texto - a string com a mensagem.
 * @param x - coordenada x na janela.
 * @param y - coordenada y na janela.
 */
void desenhaTexto(const char* texto, float x, float y, float color[]) {
   glPushMatrix();
        int W = glutGet(GLUT_WINDOW_WIDTH);
        int H = glutGet(GLUT_WINDOW_HEIGHT);

        // Garante que o texto seja desenhado na janela inteira, não apenas na última viewport
        glViewport(0, 0, W, H);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, W, 0, H);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // desabilita 3D que interfere com a renderizacao do texto 2D
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);

        glColor3f(color[0], color[1], color[2]); // Texto branco para melhor visibilidade
        glRasterPos2f(x, y);
        for (const char* c = texto; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }

        // Reabilita 3D
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
   glPopMatrix();
}
/**
 * @brief Mostra o tempo de jogo, energia e pontuacao em 2D.
 */
void exibeTempo() {
    char tempo[50];
    char energia[50];
    char pontuacao[50];

    sprintf(tempo, "Tempo: %d:%02d", (tempoTotal - contadorTempo) / 60, (tempoTotal - contadorTempo) % 60);
    sprintf(energia, "Energia: %d", energiaPinguimzinho);
    sprintf(pontuacao, "Pontos: %d", pontos);

    int W = glutGet(GLUT_WINDOW_WIDTH);
    int H = glutGet(GLUT_WINDOW_HEIGHT);

    float x = (W - 190) / 2.0f;
    float y = H / 2.0f;
    float padding = 30.0f;
    float alturaLinha = 18.0f;
     glViewport(0, 0, W, H);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, W, 0, H);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Desenha o fundo semi-transparente
        glColor4f(1.0f, 1.0f, 1.0f, 0.7f);
        glRectf(x - padding, y - (alturaLinha / 2.0f) - padding, x + 200 + padding, y + alturaLinha + padding);


    float corPreta[] = {0.0f, 0.0f, 0.0f};
    float corbranca[] = {1.0f, 1.0f, 1.0f};
    desenhaTexto(tempo, 385, H - 390, corPreta);
    desenhaTexto(energia, 525, H -390, corPreta);
    desenhaTexto(pontuacao, 460, H - 420, corPreta);

    // Mensagem de fim de jogo
    if (!jogoAtivo && mensagemFimJogo[0] != '\0') {
        // Calcular a largura da mensagem para centralizar
        int larguraTexto = 0;
        for (const char* c = mensagemFimJogo; *c != '\0'; c++) {
            larguraTexto += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
        }

        float x = (W - larguraTexto) / 2.0f;
        float y = H / 2.0f;
        float padding = 20.0f;
        float alturaLinha = 18.0f;

        // Configura a viewport e projeção para desenhar o fundo
        glViewport(0, 0, W, H);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, W, 0, H);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Desenha o fundo semi-transparente
        glColor3f(0.0f, 0.0f, 0.0f);
        glRectf(x - padding, y - (alturaLinha / 2.0f) - padding, x + larguraTexto + padding, y + alturaLinha + padding);

        // Restaura estados
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        // Desenha o texto da mensagem de fim de jogo por cima do fundo
        desenhaTexto(mensagemFimJogo, x, y,corbranca);
    }
}

/**
 * @brief Desenha a cena e seta as viewports.
 */
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int W = glutGet(GLUT_WINDOW_WIDTH);
    int H = glutGet(GLUT_WINDOW_HEIGHT);

    // Definicoes das viewports
    int viewports[4][4] = {
        {0, H / 2, W / 2, H / 2},     // topo esq: visao do topo - Y
        {W / 2, H / 2, W / 2, H / 2}, // topo dir: posicao livre
        {0, 0, W / 2, H / 2},         // baixo esq: visao de frente - Z
        {W / 2, 0, W / 2, H / 2}      // baixo dir: visao de lado - X
    };

    // --- Desenha cena em cada Viewport ---
    for(int i = 0; i < 4; ++i) {
        glViewport(viewports[i][0], viewports[i][1], viewports[i][2], viewports[i][3]);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        // --- MODIFICADO: Aumenta a distância de visão para o Skybox caber ---
        gluPerspective(60.0, (double)viewports[i][2] / (double)viewports[i][3], 1.0, 200.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Seta a camera na viewport atual, sempre com o foco na mamae pinguim
        switch(i) {
            case 0: // visao do topo
                gluLookAt(pinguimX, 20.0, pinguimZ,  pinguimX, 0.0, pinguimZ,  0.0, 0.0, -1.0);
                break;
            case 1: // posicao livre
                gluLookAt(pinguimX + 5.0, pinguimY + 5.0, pinguimZ + 5.0,  pinguimX, pinguimY, pinguimZ,  0.0, 1.0, 0.0);
                break;
            case 2: // visao de frente
                gluLookAt(pinguimX, pinguimY + 2.0, pinguimZ + 10.0,  pinguimX, pinguimY, pinguimZ,  0.0, 1.0, 0.0);
                break;
            case 3: // visao de lado
                gluLookAt(pinguimX + 10.0, pinguimY + 2.0, pinguimZ,  pinguimX, pinguimY, pinguimZ,  0.0, 1.0, 0.0);
                break;
        }

        desenhaSkybox();

        // --- NOVO CÓDIGO para o gelo ---
        glEnable(GL_TEXTURE_2D); // Habilita o uso de texturas 2D
        glBindTexture(GL_TEXTURE_2D, texturaGelo); // Ativa a textura do gelo

        glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para não alterar a cor da textura
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.0f); // O chão fica na altura y=0
            float tamanho = 12.5f;
            float repeticao = 5.0f; // A textura vai se repetir 5 vezes no chão
            glBegin(GL_QUADS);
                glNormal3f(0.0, 1.0, 0.0); // Normal apontando para cima
                glTexCoord2f(0.0, 0.0); glVertex3f(-tamanho, 0.0, -tamanho);
                glTexCoord2f(repeticao, 0.0); glVertex3f(tamanho, 0.0, -tamanho);
                glTexCoord2f(repeticao, repeticao); glVertex3f(tamanho, 0.0, tamanho);
                glTexCoord2f(0.0, repeticao); glVertex3f(-tamanho, 0.0, tamanho);
            glEnd();
        glPopMatrix();
        glDisable(GL_TEXTURE_2D); // Desabilita para não afetar outros objetos

        // buracos
        glColor3f(0.1f, 0.1f, 0.4f); // azul escuro
        for (const auto& buraco : buracos) {
                glPushMatrix();
                glTranslatef(buraco.x, 0.01f, buraco.z);
                glRotatef(90, 1, 0, 0);
                gluDisk(gluNewQuadric(), 0, buraco.raio, 20, 1);
            glPopMatrix();
        }

        // mamae pinguim
        glPushMatrix();
            glTranslatef(pinguimX, pinguimY, pinguimZ);
            glRotatef(pinguimRotacao, 0.0f, 1.0f, 0.0f);
            desenhaPinguim(false);
            // desenha peixe no bico se estiver carregando
            if (carregandoPeixe) {
                glPushMatrix();
                glTranslatef(0.0f, 0.5f, 0.8f);
                desenhaPeixe();
                glPopMatrix();
            }
        glPopMatrix();

        // pinguimzinho
        glPushMatrix();
            glTranslatef(pinguimzinhoX, pinguimzinhoY, pinguimzinhoZ);
            desenhaPinguim(true);
        glPopMatrix();

        // peixes
        for (const auto& peixe : peixes) {
            if (!peixe.peixeCapturado) {
                glPushMatrix();
                    glTranslatef(peixe.x, peixe.y, peixe.z);
                    desenhaPeixe();
                glPopMatrix();
            }
        }
    }

    exibeTempo();
    glutSwapBuffers();
}

/**
 * @brief Movimentacao do pinguim a partir das setas do teclado.
 * @param key - tecla pressionada
 */
void specialKeys(int key, int x, int y) {
     if (!jogoAtivo) return;
    float velocidadeMovimento = 0.3f;
    float velocidadeRotacao = 5.0f;

    switch (key) {
        case GLUT_KEY_UP: // anda em frente
            if(pinguimX <=12.5){pinguimX += velocidadeMovimento * sin(pinguimRotacao * PI / 180.0f);};
            if(pinguimY <=12.5){pinguimZ += velocidadeMovimento * cos(pinguimRotacao * PI / 180.0f);};

            break;
        case GLUT_KEY_DOWN: // anda para tras
             if(pinguimX >=-12.5){pinguimX -= velocidadeMovimento * sin(pinguimRotacao * PI / 180.0f);}
             if(pinguimZ >=-12.5){pinguimZ -= velocidadeMovimento * cos(pinguimRotacao * PI / 180.0f);}
            break;
        case GLUT_KEY_LEFT: // rotaciona para a esq
            pinguimRotacao += velocidadeRotacao;
            break;
        case GLUT_KEY_RIGHT: // rotaciona para a dir
            pinguimRotacao -= velocidadeRotacao;
            break;
    }
    glutPostRedisplay();
}

/**
 * @brief Verifica colisao (com peixe, com filhote e com buraco).
 */
void verificaColisao() {
    // colisao com peixe
    if (!carregandoPeixe) {
        for (auto& peixe : peixes) {
            if (!peixe.peixeCapturado) {
                float dx = pinguimX - peixe.x;
                float dz = pinguimZ - peixe.z;
                if (sqrt(dx*dx + dz*dz) < 1.0f) {
                    peixe.peixeCapturado = true;
                    carregandoPeixe = true;
                    break;
                }
            }
        }
    }

    // colisao com filhote
    if (carregandoPeixe) {
        float dx = pinguimX - pinguimzinhoX;
        float dz = pinguimZ - pinguimzinhoZ;
        if (sqrt(dx*dx + dz*dz) < 1.5f) {
            carregandoPeixe = false;
            energiaPinguimzinho = 60; // adiciona 1min de energia no pinguimZinho
            pontos++;
            spawnItems(); // respawna itens
        }
    }

    // colisao com buraco
    for (const auto& buraco : buracos) {
        float dx = pinguimX - buraco.x;
        float dz = pinguimZ - buraco.z;
        if (sqrt(dx*dx + dz*dz) < buraco.raio) {
            // Reseta a posicao do pinguim
            pinguimX = 0.0f;
            pinguimZ = 5.0f;
            pinguimRotacao = 0.0f;
            carregandoPeixe = false;
        }
    }
}

/**
 * @brief Loop princiapl do jogo que atualiza status e verifica condicoes de vencer/perder.
 */
void update(int value) {
    if (jogoAtivo){
        contadorTempo++;
        energiaPinguimzinho--;
        
        verificaColisao();
        
        if(energiaPinguimzinho <=0 || contadorTempo >= tempoTotal) { 
            jogoAtivo = false;
            if(energiaPinguimzinho <= 0) {
                sprintf(mensagemFimJogo, "Fim de Jogo! :(");
            } else {
                sprintf(mensagemFimJogo, "Parabens! Voce ganhou! :)");
            }
        }
        std::cout << "Tempo de jogo: " << contadorTempo << "\nVida do pinguim: " << energiaPinguimzinho << std::endl;

        glutPostRedisplay();
        glutTimerFunc(1000, update, 0); // intervalo de 1s
    }
}

/**
 * @brief Inicializa contexto OpenGL.
 */
void init() {
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f); // fundo: ceu azul

    texturaGelo = carregarTextura("textura/neve.jpg");
    // Carrega as texturas do pinguim
    texturaPinguimCorpo = carregarTextura("textura/pinguim.jpg");
    texturaPinguimBarriga = carregarTextura("textura/barriga.jpg");

    // --- NOVO: Carrega as 6 texturas do Skybox ---
    texturasCeu[0] = carregarTextura("textura/ceu.jpg");
    texturasCeu[1] = carregarTextura("textura/ceu.jpg");
    texturasCeu[2] = carregarTextura("textura/ceu.jpg");
    texturasCeu[3] = carregarTextura("textura/ceu.jpg");
    texturasCeu[4] = carregarTextura("textura/ceu.jpg");
    texturasCeu[5] = carregarTextura("textura/ceu.jpg");

    // Carrega a textura do peixe
    texturaPeixe = carregarTextura("textura/peixe.jpg");

    // ---- NOVO CÓDIGO ----
    quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE); // Essencial: Habilita a geração de coordenadas de textura

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Luz branca simples
    GLfloat light_pos[] = { 10.0f, 20.0f, 10.0f, 1.0f };
    GLfloat ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    glEnable(GL_COLOR_MATERIAL); // Usa glColor para setar propriedade dos materiais
    srand(time(0));
    initGame();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Trabalho 2: Pinguim 3D");

    init();
    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(1000, update, 0); // Comeca o loop de jogo

    glutMainLoop();

    if(quadric) {
        gluDeleteQuadric(quadric);
    }

    return 0;
}
