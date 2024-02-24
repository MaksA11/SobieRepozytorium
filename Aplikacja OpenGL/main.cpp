#include <GLFW/glfw3.h>
#include <conio.h>
#include <chrono>
#include <thread>
#include <cmath>

float toOpenGLCoord(int arrayCoord, char axis);
void start(), update(), key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

class object
{
    private:
        int x, y, length;

    public:
        object(int x, int y, int length)
        {
            this->x = x;
            this->y = y;
            this->length = length;
        }

        void setPosition(int x, int y)
        {
            this->x = x;
            this->y = y;
        }
        void changeLength(int amount)
        {
            length += amount;
        }

        int getX()
        {
            return this->x;
        }
        int getY()
        {
            return this->y;
        }
        int getLength()
        {
            return this->length;
        }

        float getOpenGLX()
        {
            return toOpenGLCoord(x, 'x');
        }
        float getOpenGLY()
        {
            return toOpenGLCoord(y, 'y');
        }

        void display(float r, float g, float b, float size)
        {
            glBegin(GL_QUADS);
            glColor3f(r, g, b);
            glVertex2f(toOpenGLCoord(x, 'x') - (size / 2), toOpenGLCoord(y, 'y') + (size / 2));
            glVertex2f(toOpenGLCoord(x, 'x') + (size / 2), toOpenGLCoord(y, 'y') + (size / 2));
            glVertex2f(toOpenGLCoord(x, 'x') + (size / 2), toOpenGLCoord(y, 'y') - (size / 2));
            glVertex2f(toOpenGLCoord(x, 'x') - (size / 2), toOpenGLCoord(y, 'y') - (size / 2));
            glEnd();
        }
};

int WinMain(void)
{
    GLFWwindow* window;

    if(!glfwInit())
        return -1;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(680, 680, "Snake", NULL, NULL);
    if(!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    start();

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        update();
        std::this_thread::sleep_for(std::chrono::milliseconds(120));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

/*==============================================================================================*/

object snake = object(10, 10, 2);
object point = object(10, 10, 0);

int scene[20][20];
int direction = 0;

bool borders = false;

void drawScene(), randomizePointPosition(), displaySnakeSegment(float x, float y, float size, float r, float g, float b);

void start()
{
    for(int y = 0; y < 20; y++)
    {
        for(int x = 0; x < 20; x++)
        {
            scene[x][y] = 0;
        }
    }

    direction = 0;
    snake = object(10, 10, 2);
    randomizePointPosition();
}

void update()
{
    switch(direction)
    {
        case 1:
            snake.setPosition(snake.getX(), snake.getY() - 1);
            break;
        case 2:
            snake.setPosition(snake.getX(), snake.getY() + 1);
            break;
        case 3:
            snake.setPosition(snake.getX() - 1, snake.getY());
            break;
        case 4:
            snake.setPosition(snake.getX() + 1, snake.getY());
            break;
    }

    if(snake.getX() > 19)
    {
        if(borders)
            start();
        else
            snake.setPosition(0, snake.getY());
    }
    if(snake.getX() < 0)
    {
        if(borders)
            start();
        else
            snake.setPosition(19, snake.getY());
    }
    if(snake.getY() > 19)
    {
        if(borders)
            start();
        else
            snake.setPosition(snake.getX(), 0);
    }
    if(snake.getY() < 0)
    {
        if(borders)
            start();
        else
            snake.setPosition(snake.getX(), 19);
    }

    if(snake.getX() == point.getX() && snake.getY() == point.getY())
    {
        randomizePointPosition();
        snake.changeLength(1);
    }

    if(scene[snake.getX()][snake.getY()] != 0 && scene[snake.getX()][snake.getY()] != snake.getLength() - 1)
        start();

    scene[snake.getX()][snake.getY()] = snake.getLength();

    for(int y = 0; y < 20; y++)
    {
        for(int x = 0; x < 20; x++)
        {
            if(scene[x][y] > 0)
                scene[x][y]--;
        }
    }

    drawScene();
}

void drawScene()
{
    snake.display(0.14f, 0.47f, 0.03f, 0.1f);
    for(int y = 0; y < 20; y++)
    {
        for(int x = 0; x < 20; x++)
        {
            if(scene[x][y] != 0)
                displaySnakeSegment(toOpenGLCoord(x, 'x'), toOpenGLCoord(y, 'y'), 0.1f, 0.14f, 0.47f, 0.03f);
        }
    }

    point.display(1.0f, 0, 0, 0.075f);
}

void randomizePointPosition()
{
    srand(static_cast<unsigned>(time(0)));

    int x, y;

    do {
        x = rand() % 19 + 1;
        y = rand() % 19 + 1;
    } while(scene[x][y] != 0);

    point.setPosition(x, y);
}

void displaySnakeSegment(float x, float y, float size, float r, float g, float b)
{
    glBegin(GL_QUADS);
    glColor3f(r, g, b);
    glVertex2f(x - (size / 2), y + (size / 2));
    glVertex2f(x + (size / 2), y + (size / 2));
    glVertex2f(x + (size / 2), y - (size / 2));
    glVertex2f(x - (size / 2), y - (size / 2));
    glEnd();
}

float toOpenGLCoord(int arrayCoord, char axis)
{
    if(axis == 'x')
    {
        if(arrayCoord < 10)
            return ((float)arrayCoord / 10.0f) - 0.95f;
        else
            return (((float)arrayCoord - 10.0f) / 10.0f) + 0.05f;
    }
    else if(axis == 'y')
    {
        if(arrayCoord < 10)
            return -1 * (((float)arrayCoord / 10.0f) - 0.95f);
        else
            return -1 * ((((float)arrayCoord - 10.0f) / 10.0f) + 0.05f);
    }
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        if(direction != 2)
            direction = 1;
    }
    else if(key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        if(direction != 1)
            direction = 2;
    }
    else if(key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        if(direction != 4)
            direction = 3;
    }
    else if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        if(direction != 3)
            direction = 4;
    }

    if(key == GLFW_KEY_B && action == GLFW_PRESS)
        borders = !borders;

    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        exit(0);
}