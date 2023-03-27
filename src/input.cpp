#include "input.h"

// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// "g_LeftMouseButtonPressed = true" se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false; // Análogo para botão direito do mouse
bool g_MiddleMouseButtonPressed = false; // Análogo para botão do meio do mouse

// "g_WKeyPressed = true" se o usuário está com a tecla W
// pressionada no momento atual
// Análogo para as outras teclas
bool g_WKeyPressed        = false;
bool g_AKeyPressed        = false;
bool g_SKeyPressed        = false;
bool g_DKeyPressed        = false;
bool g_SpaceBarKeyPressed = false;
bool g_EnterKeyPressed    = false;

// Variável que segura qual tecla numérica o usuário pressionou por último, -1
int g_LastNumberPressed = 0;

// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()).
float g_CameraTheta = 0.0f; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y

// Variável que controla se a informação extra será mostrada na tela.
bool g_ShowInfo = true;

// Guarda se o usuário já movimentou o mouse
bool g_hasMovedMouse = false;

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // Atualizamos também a razão que define a proporção da janela (largura /
    // altura), a qual será utilizada na definição das matrizes de projeção,
    // tal que não ocorra distorções durante o processo de "Screen Mapping"
    // acima, quando NDC é mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    //
    // O cast para float é necessário pois números inteiros são arredondados ao
    // serem divididos!
    g_ScreenRatio = (float)width / height;
}

// Variáveis globais que armazenam a última posição do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, setamos a variável
        // g_LeftMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
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
        // Se o usuário pressionou o botão esquerdo do mouse, setamos a variável
        // g_RightMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
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
        // Se o usuário pressionou o botão esquerdo do mouse, setamos a variável
        // g_MiddleMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        g_MiddleMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_MiddleMouseButtonPressed = false;
    }
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
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

    // Atualizamos parâmetros da câmera com os deslocamentos
    g_CameraTheta -= 0.01f*dx;
    g_CameraPhi   -= 0.01f*dy;

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

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    /* currently unused
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f*yoffset;

    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
    */
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // Se apertar/soltar teclas WASD/space então atualiza variável global correspondente

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

    // Se o usuário apertar uma tecla numérica de 1 a 5, atualiza variável global
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

    // Se o usuário apertar a tecla H, fazemos um "toggle" da informação extra.
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        g_ShowInfo = !g_ShowInfo;
    }
}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}
