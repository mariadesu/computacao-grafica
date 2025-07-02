#TO DO 
1. Ajustar movimentação do pinguim para que ap entregar o peixe o jogo nao resete.
2. evitar que crie buracos no centro de respawn
3. adicionar visualizacao do tempo de jogo 
4. OK - tempo de jogo em 5 min
5. inserir 3 texturas na cena

#questoes bonus
1. o pinguim move as asas para frente e para tras ao andar
2. No lugar de desenhar um “chão” para a movimentação da aranha, utiliza-se o conceito de skybox, em que seis
imagens distintas devem ser dispostas de maneira a formar um cubo. Para essa textura, utilize somente cubemap para
definição dos parâmetros (cujas constantes são definidas por GL_TEXTURE _CUBE_MAP).

#funcoes

  * `drawPenguin(bool isBaby)`: This function is responsible for rendering a penguin on the screen. It uses basic 3D shapes
     (spheres and a cone) to construct the penguin's body, head, beak, and eyes. It can draw either a full-sized mother penguin
     or a smaller baby penguin based on the isBaby parameter.


   * `drawFish()`: This function renders a single fish, which the mother penguin collects. It uses a scaled sphere (to look like
     a body) and a cone (for the tail) to create the fish model.


   * `initGame()`: This function sets up the initial state of the game by calling spawnItems() to place the fish and holes in
     their starting positions.

   * `spawnItems()`: This function is called to randomly place the collectible fish and the hazardous holes on the ice surface.
     It ensures that each time you play, their locations are different.


   * `display()`: This is the main drawing function for the entire game. It clears the screen and then draws every object in the
     scene: the ice plate, the holes, the mother penguin, the baby penguin, and any fish that haven't been caught yet. It also
     handles setting up the camera's point of view.


   * `specialKeys(int key, int x, int y)`: This function handles the player's controls. It listens for arrow key presses (UP,
     DOWN, LEFT, RIGHT) to move the mother penguin forward and backward and to rotate her left and right in the 3D space.


   * `checkCollisions()`: This function contains the core game logic for interactions. It constantly checks the distance between
     the mother penguin and other objects:
       * Fish: If the penguin gets close enough to a fish, it "catches" it.
       * Baby Penguin: If the penguin is carrying a fish and gets close to the baby, it "feeds" the baby, which increases the
         baby's energy and the player's score.
       * Holes: If the penguin moves over a hole, it "falls in" and its position is reset to the starting point.


   * `update(int value)`: This function acts as the main game loop, running once every second. It decrements the game timer and
     the baby's energy level, calls checkCollisions() to process game events, and checks for the win/loss conditions (either the
     baby runs out of energy or the main game timer runs out).

   * `init()`: This function performs the initial setup for the OpenGL rendering environment. It sets the background color,
     enables 3D depth, and configures a simple lighting source to make the scene look more realistic.


   * `main(int argc, char argv)**: This is the entry point of the program. It initializes the GLUT library, creates the game 
     window, and registers all the other functions to handle displaying graphics (display), keyboard input (specialKeys), and the 
     main game loop (update`).
