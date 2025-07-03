# Documentação das Funções

Este arquivo descreve de forma simples o propósito de cada função no arquivo `trab2.cpp`.

-   **`main()`**: Função principal. Inicializa o GLUT, a janela e o ambiente OpenGL, e então inicia o loop principal do jogo.

-   **`init()`**: Prepara o ambiente do jogo. Carrega todas as texturas (pinguins, peixe, gelo, céu), configura a iluminação da cena e inicializa os objetos do jogo.

-   **`initGame()`**: Inicia o jogo chamando a função para posicionar os itens no cenário.

-   **`display()`**: É a função de desenho principal. Limpa a tela e desenha toda a cena 3D, incluindo o cenário, os pinguins e os itens. Ela também gerencia as quatro viewports (câmeras) e chama a função para exibir as informações de texto 2D.

-   **`update(int value)`**: Controla a lógica do jogo que acontece ao longo do tempo. É chamada a cada segundo para atualizar o contador de tempo, a energia do pinguim filhote e verificar se o jogo terminou.

-   **`specialKeys(int key, int x, int y)`**: Captura os eventos do teclado (setas direcionais) para controlar a movimentação e a rotação da mamãe pinguim.

-   **`carregarTextura(const char *caminho)`**: Carrega um arquivo de imagem (como `.jpg`) e o converte em uma textura que o OpenGL pode usar para "pintar" os objetos 3D.

-   **`desenhaPinguim(bool ehFilhote)`**: Desenha um modelo 3D de pinguim. O parâmetro `ehFilhote` define se o pinguim desenhado será o filhote (menor) ou a mamãe (maior).

-   **`desenhaPeixe()`**: Desenha um modelo 3D de peixe.

-   **`desenhaSkybox()`**: Desenha um grande cubo ao redor de toda a cena e aplica a textura do céu em suas faces internas, criando um fundo 3D para o ambiente.

-   **`spawnItems()`**: Posiciona os peixes e os buracos em locais aleatórios no cenário no início do jogo e sempre que a mamãe pinguim alimenta o filhote.

-   **`verificaColisao()`**: Verifica se a mamãe pinguim está colidindo com algum item do jogo: um peixe para coletar, o filhote para alimentar ou um buraco para cair.

-   **`desenhaTexto(const char* texto, float x, float y)`**: Desenha texto 2D (como o tempo restante e a pontuação) diretamente na tela, sobrepondo a cena 3D.

-   **`exibeTempo()`**: Prepara e exibe todas as informações de texto na tela, como o tempo, a energia e a pontuação. Também exibe a mensagem de fim de jogo quando as condições são atendidas.
