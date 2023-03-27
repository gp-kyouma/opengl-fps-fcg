#include "input.h"

// Raz�o de propor��o da janela (largura/altura). Veja fun��o FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// "g_LeftMouseButtonPressed = true" se o usu�rio est� com o bot�o esquerdo do mouse
// pressionado no momento atual. Veja fun��o MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false; // An�logo para bot�o direito do mouse
bool g_MiddleMouseButtonPressed = false; // An�logo para bot�o do meio do mouse

// "g_WKeyPressed = true" se o usu�rio est� com a tecla W
// pressionada no momento atual
// An�logo para as outras teclas
bool g_WKeyPressed        = false;
bool g_AKeyPressed        = false;
bool g_SKeyPressed        = false;
bool g_DKeyPressed        = false;
bool g_SpaceBarKeyPressed = false;
bool g_EnterKeyPressed    = false;

// Vari�vel que segura qual tecla num�rica o usu�rio pressionou por �ltimo, -1
int g_LastNumberPressed = 0;

// Vari�veis que definem a c�mera em coordenadas esf�ricas, controladas pelo
// usu�rio atrav�s do mouse (veja fun��o CursorPosCallback()).
float g_CameraTheta = 0.0f; // �ngulo no plano ZX em rela��o ao eixo Z
float g_CameraPhi = 0.0f;   // �ngulo em rela��o ao eixo Y

// Vari�vel que controla se a informa��o extra ser� mostrada na tela.
bool g_ShowInfo = true;

// Guarda se o usu�rio j� movimentou o mouse
bool g_hasMovedMouse = false;

// Defini��o da fun��o que ser� chamada sempre que a janela do sistema
// operacional for redimensionada, por consequ�ncia alterando o tamanho do
// "framebuffer" (regi�o de mem�ria onde s�o armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda regi�o do framebuffer. A
    // fun��o "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa � a opera��o de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // Atualizamos tamb�m a raz�o que define a propor��o da janela (largura /
    // altura), a qual ser� utilizada na defini��o das matrizes de proje��o,
    // tal que n�o ocorra distor��es durante o processo de "Screen Mapping"
    // acima, quando NDC � mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    //
    // O cast para float � necess�rio pois n�meros inteiros s�o arredondados ao
    // serem divididos!
    g_ScreenRatio = (float)width / height;
}

// Vari�veis globais que armazenam a �ltima posi��o do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;

// Fun��o callback chamada sempre que o usu�rio aperta algum dos bot�es do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usu�rio pressionou o bot�o esquerdo do mouse, setamos a vari�vel
        // g_LeftMouseButtonPressed como true, para saber que o usu�rio est�
        // com o bot�o esquerdo pressionado.
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usu�rio soltar o bot�o esquerdo do mouse, atualizamos a
        // vari�vel abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        // Se o usu�rio pressionou o bot�o esquerdo do mouse, setamos a vari�vel
        // g_RightMouseButtonPressed como true, para saber que o usu�rio est�
        // com o bot�o esquerdo pressionado.
        g_RightMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        // Quando o usu�rio soltar o bot�o esquerdo do mouse, atualizamos a
        // vari�vel abaixo para false.
        g_RightMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        // Se o usu�rio pressionou o bot�o esquerdo do mouse, setamos a vari�vel
        // g_MiddleMouseButtonPressed como true, para saber que o usu�rio est�
        // com o bot�o esquerdo pressionado.
        g_MiddleMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        // Quando o usu�rio soltar o bot�o esquerdo do mouse, atualizamos a
        // vari�vel abaixo para false.
        g_MiddleMouseButtonPressed = false;
    }
}

// Fun��o callback chamada sempre que o usu�rio movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (!g_hasMovedMouse)
    {
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_hasMovedMouse = true;
    }

    // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
    float dx = xpos - g_LastCursorPosX;
    float dy = ypos - g_LastCursorPosY;

    // Atualizamos par�metros da c�mera com os deslocamentos
    g_CameraTheta -= 0.01f*dx;
    g_CameraPhi   -= 0.01f*dy;

    // Em coordenadas esf�ricas, o �ngulo phi deve ficar entre -pi/2 e +pi/2.
    float phimax = 3.141592f/2;
    float phimin = -phimax;

    if (g_CameraPhi > phimax)
        g_CameraPhi = phimax;

    if (g_CameraPhi < phimin)
        g_CameraPhi = phimin;

    // Atualizamos as vari�veis globais para armazenar a posi��o atual do
    // cursor como sendo a �ltima posi��o conhecida do cursor.
    g_LastCursorPosX = xpos;
    g_LastCursorPosY = ypos;
}

// Fun��o callback chamada sempre que o usu�rio movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    /* currently unused
    // Atualizamos a dist�ncia da c�mera para a origem utilizando a
    // movimenta��o da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f*yoffset;

    // Uma c�mera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela est� olhando, pois isto gera problemas de divis�o por zero na
    // defini��o do sistema de coordenadas da c�mera. Isto �, a vari�vel abaixo
    // nunca pode ser zero. Vers�es anteriores deste c�digo possu�am este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
    */
}

// Defini��o da fun��o que ser� chamada sempre que o usu�rio pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // Se o usu�rio pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // Se apertar/soltar teclas WASD/space ent�o atualiza vari�vel global correspondente

    if (key == GLFW_KEY_W)
    {
        if (action == GLFW_PRESS)
            g_WKeyPressed = true;
        if (action == GLFW_RELEASE)
            g_WKeyPressed = false;
    }
    if (key == GLFW_KEY_A)
    {
        if (action == GLFW_PRESS)
            g_AKeyPressed = true;
        if (action == GLFW_RELEASE)
            g_AKeyPressed = false;
    }
    if (key == GLFW_KEY_S)
    {
        if (action == GLFW_PRESS)
            g_SKeyPressed = true;
        if (action == GLFW_RELEASE)
            g_SKeyPressed = false;
    }
    if (key == GLFW_KEY_D)
    {
        if (action == GLFW_PRESS)
            g_DKeyPressed = true;
        if (action == GLFW_RELEASE)
            g_DKeyPressed = false;
    }
    if (key == GLFW_KEY_SPACE)
    {
        if (action == GLFW_PRESS)
            g_SpaceBarKeyPressed = true;
        if (action == GLFW_RELEASE)
            g_SpaceBarKeyPressed = false;
    }
    if (key == GLFW_KEY_ENTER)
    {
        if (action == GLFW_PRESS)
            g_EnterKeyPressed = true;
        if (action == GLFW_RELEASE)
            g_EnterKeyPressed = false;
    }

    // Se o usu�rio apertar uma tecla num�rica de 1 a 5, atualiza vari�vel global
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        g_LastNumberPressed = 0;
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        g_LastNumberPressed = 1;
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        g_LastNumberPressed = 2;
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        g_LastNumberPressed = 3;
    }
    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
    {
        g_LastNumberPressed = 4;
    }

    // Se o usu�rio apertar a tecla H, fazemos um "toggle" da informa��o extra.
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        g_ShowInfo = !g_ShowInfo;
    }
}

// Definimos o callback para impress�o de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}
