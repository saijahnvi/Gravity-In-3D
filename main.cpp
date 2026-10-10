#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

glm::vec3 sphericalToCartesian(float r, float theta, float phi) {
    float x = r * sin(theta) * cos(phi);
    float y = r * cos(theta);
    float z = r * sin(theta) * sin(phi);
    return glm::vec3(x, y, z);
};

class FPSCamera {
public:
    glm::vec3 position{0.0f, 45.0f, 80.0f}; // Adjusted camera height and distance for wider grid view
    glm::vec3 front{0.0f, -0.5f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    glm::vec3 right{1.0f, 0.0f, 0.0f};
    glm::vec3 world_up{0.0f, 1.0f, 0.0f};

    float yaw{-90.0f};
    float pitch{-30.0f};
    
    void update_vectors() {
        glm::vec3 newfront;
        newfront.x = std::cos(glm::radians(yaw)) * (std::cos(glm::radians(pitch)));
        newfront.y = std::sin(glm::radians(pitch));
        newfront.z = (std::sin(glm::radians(yaw))) * (std::cos(glm::radians(pitch)));

        front = glm::normalize(newfront);
        right = glm::normalize(glm::cross(front, world_up));
        up = glm::normalize(glm::cross(right, front));
    }

    glm::mat4 get_view_matrix() const {
        return glm::lookAt(position, position + front, up);
    }
};

FPSCamera camera;
bool first_mouse = true;
double last_x = 640.0f;
double last_y = 340.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (first_mouse) {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }

    float xoffset = (float)(xpos - last_x);
    float yoffset = (float)(last_y - ypos);
    last_x = xpos;
    last_y = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    camera.yaw += xoffset;
    camera.pitch += yoffset;

    camera.pitch = std::max(-89.0f, std::min(camera.pitch, 89.0f));
    camera.update_vectors();
}

void process_input(GLFWwindow* window, float dt) {
    float speed = 35.0f * dt; // Parsecs per second (slightly increased movement speed for larger map)

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.position += camera.front * speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.position -= camera.front * speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.position -= camera.right * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.position += camera.right * speed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.position += camera.world_up * speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.position -= camera.world_up * speed;
}

class ball {
public:
    std::vector<float> Position;
    std::vector<float> velocity;
    float mass;
    float RADIUS = 0.5f;

    ball(std::vector<float> Position, std::vector<float> velocity, float mass, float RADIUS) {
        this->Position = Position;
        this->velocity = velocity;
        this->mass = mass;
        this->RADIUS = RADIUS;
    }

    void updatevelocity(float dt) {
        this->Position[0] += this->velocity[0] * dt;
        this->Position[1] += this->velocity[1] * dt;
    }

    void accelerate(float dt, glm::vec2 direction, float gravity) {
        this->velocity[0] += direction.x * gravity * dt;
        this->velocity[1] += direction.y * gravity * dt;
    }

    float acceleratefreespace(ball& a, ball& b, glm::vec2& outDir) {
        glm::vec2 posA(a.Position[0], a.Position[1]);
        glm::vec2 posB(b.Position[0], b.Position[1]);
        glm::vec2 delta = posB - posA;
        float distanceSq = glm::dot(delta, delta);
        float distance = std::sqrt(distanceSq);

        outDir = delta / distance;

        const float G = 300.0f; // world-space gravitational constant
        return (G * b.mass) / distanceSq;
    }

    std::vector<glm::vec3> drawcircle(float x0, float y0, int ncount, float radius) {
        float degree = 360.0f / ncount;
        std::vector<glm::vec3> temp;
        std::vector<glm::vec3> vertices;
        glm::vec3 center(0.0f, 0.0f, 0.0f);
        // assigning positions
        for (int i = 0; i <= ncount; i++) {
            float theta = i * degree;
            float x = radius * cos(glm::radians(theta));
            float y = radius * sin(glm::radians(theta));
            float z = 0.0f;
            temp.push_back(glm::vec3(x, y, z));
        }
        // triangles vertices
        for (int i = 0; i < ncount; i++) {
            vertices.push_back(center);
            vertices.push_back(temp[i]);
            vertices.push_back(temp[i + 1]);
        }

        return vertices;
    }

    std::vector<glm::vec3> drawsphere() {
        std::vector<glm::vec3> vertices;
        float stacks = 16.0f;
        float sectors = 16.0f;
        for (int i = 0; i < stacks; i++) {
            float theta1 = i / stacks * glm::pi<float>();
            float theta2 = (i + 1) / stacks * glm::pi<float>();
            for (int j = 0; j < sectors; j++) {
                float phi1 = j / sectors * 2 * glm::pi<float>();
                float phi2 = (j + 1) / sectors * 2 * glm::pi<float>();

                glm::vec3 v1 = sphericalToCartesian(RADIUS, theta1, phi1);
                glm::vec3 v2 = sphericalToCartesian(RADIUS, theta1, phi2);
                glm::vec3 v3 = sphericalToCartesian(RADIUS, theta2, phi1);
                glm::vec3 v4 = sphericalToCartesian(RADIUS, theta2, phi2);

                vertices.push_back(v1);
                vertices.push_back(v2);
                vertices.push_back(v3);

                vertices.push_back(v2);
                vertices.push_back(v4);
                vertices.push_back(v3);
            }
        }
        return vertices;
    }
};

void collision(ball& a, ball& b) {
    glm::vec2 posA(a.Position[0], a.Position[1]);
    glm::vec2 posB(b.Position[0], b.Position[1]);
    glm::vec2 delta = posA - posB;
    float distance = glm::length(delta);
    float mindist = a.RADIUS + b.RADIUS;

    // Check collision and prevent division by zero
    if (distance < mindist && distance > 0.0001f) {
        glm::vec2 normal = delta / distance;
        float overlap = mindist - distance;
        float m = overlap * 0.5f;

        // Correctly push ball A forward along normal and ball B backward
        a.Position[0] += normal.x * m;
        a.Position[1] += normal.y * m;
        b.Position[0] -= normal.x * m;
        b.Position[1] -= normal.y * m;

        glm::vec2 velA(a.velocity[0], a.velocity[1]);
        glm::vec2 velB(b.velocity[0], b.velocity[1]);
        glm::vec2 relvel = velA - velB;

        float velAlongNormal = glm::dot(relvel, normal);
        if (velAlongNormal > 0) return;

        float e = 0.9f;
        float j = -(1.0f + e) * velAlongNormal / ((1.0f / a.mass) + (1.0f / b.mass));

        glm::vec2 impulse = j * normal;
        a.velocity[0] += (1.0f / a.mass) * impulse.x;
        a.velocity[1] += (1.0f / a.mass) * impulse.y;
        b.velocity[0] -= (1.0f / b.mass) * impulse.x;
        b.velocity[1] -= (1.0f / b.mass) * impulse.y;
    }
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type); 
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        std::cout << "Failed to compile " 
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

std::vector<float> creategrid(float size, float divisions, const std::vector<ball>& objs) {
    std::vector<float> vertices;
    float halfsize = size / 2.0f;
    float step = size / divisions;

    // lines parallel to x axis
    for (int zstep = 0; zstep <= (int)divisions; zstep++) {
        float z = -halfsize + zstep * step;
        for (int xstep = 0; xstep < (int)divisions; xstep++) {
            float xstart = -halfsize + xstep * step;
            float xend = xstart + step;

            vertices.push_back(xstart); vertices.push_back(0.0f); vertices.push_back(z); 
            vertices.push_back(xend);   vertices.push_back(0.0f); vertices.push_back(z); 
        }
    }

    // lines parallel to z axis
    for (int xstep = 0; xstep <= (int)divisions; xstep++) {
        float x = -halfsize + xstep * step;
        for (int zstep = 0; zstep < (int)divisions; zstep++) {
            float zstart = -halfsize + zstep * step;
            float zend = zstart + step;

            vertices.push_back(x); vertices.push_back(0.0f); vertices.push_back(zstart); 
            vertices.push_back(x); vertices.push_back(0.0f); vertices.push_back(zend); 
        }
    }

    // Dynamic Spacetime Curvature Deformation Loop// 2. Warp Grid Vertices Only (Spacetime Bending)
    const float depthScale = 0.12f;
    const float epsilon = 3.0f; // Prevent division by zero near body centers

    for (size_t i = 0; i < vertices.size(); i += 3) {
        float gx = vertices[i];
        float gz = vertices[i + 2];

        float totalDisplacement = 0.0f;

        for (size_t idx = 0; idx < objs.size(); ++idx) {
            float dx = gx - objs[idx].Position[0];
            float dz = gz - objs[idx].Position[1]; // 2D orbital (X, Y) maps to 3D grid (X, Z)
            float distSq = dx * dx + dz * dz;

            // Visual multiplier boost for planets so their warp well is distinct
            float visualMass = (idx == 0) ? objs[idx].mass : objs[idx].mass * 40.0f; 

            float dip = (visualMass * 8.0f) / std::sqrt(distSq + epsilon * epsilon);
            totalDisplacement -= dip;
        }

        // Apply deformation downwards along the vertical Y-axis
        vertices[i + 1] = totalDisplacement * depthScale;
    }

    return vertices;
}

int main() {
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Gravity In 3D", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Error initializing glew" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    camera.update_vectors();

    glfwSetCursorPosCallback(window, mouse_callback); // mouse input function
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    {
        std::vector<ball> objs = {// ball(position, velocity, mass, radius)// Sun fixed at origin
    ball({0.0f, 0.0f}, {0.0f, 0.0f}, 1200.0f, 35.0f),

    // Earth in Farther Circular Orbit: r = 60.0, v = sqrt(G * M / r) = sqrt(300 * 1200 / 60) = 77.45967f
    ball({60.0f, 0.0f}, {0.0f, 77.45967f}, 5.0f, 5.0f),ball({120.0f, 0.0f}, {0.0f, 54.772255f}, 1.0f, 7.0f)
    ,ball({150.0f, 0.0f}, {0.0f, 48.98979f}, 1.0f, 9.0f)
    ,ball({180.0f, 0.0f}, {0.0f, 44.721359f}, 10.0f, 11.0f)

        };

        // 3D Shader for Grid & Objects
        std::string vertexShader = R"glsl(
            #version 330 core
            layout(location = 0) in vec3 position;
            uniform mat4 u_MVP;
            uniform vec3 u_Offset;
            void main(){
                vec3 finalPos = position + u_Offset; // for the balls & grid offset
                gl_Position = u_MVP * vec4(finalPos, 1.0); // for the grid
            })glsl";

        std::string fragmentShader = R"glsl( // rawstring is used
            #version 330 core
            out vec4 FragColor;
            uniform vec4 u_Color;
            void main()
            {
               FragColor = u_Color; 
            })glsl";

        unsigned int shader = CreateShader(vertexShader, fragmentShader);

        // Upload 3D Grid Geometry Buffer (Expanded to 240.0f size x 120 divisions)
        std::vector<float> gridVertices = creategrid(500.0f, 200.0f, objs);
        unsigned int gridVAO, gridVBO;
        glGenVertexArrays(1, &gridVAO);
        glGenBuffers(1, &gridVBO);
        glBindVertexArray(gridVAO);
        glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
        glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        // Separate VAO/VBO for rendering dynamic 2D balls
        unsigned int ballVAO, ballVBO;
        glGenVertexArrays(1, &ballVAO);
        glGenBuffers(1, &ballVBO);

        float lastFrame = (float)glfwGetTime();

        while (!glfwWindowShouldClose(window)) {
            float currentFrame = (float)glfwGetTime();
            float dt = std::min(currentFrame - lastFrame, 0.016f); // Clamp dt to ~60 FPS equivalent
            lastFrame = currentFrame;

            glfwPollEvents();
            process_input(window, dt);

            // Fixed sub-stepping for numerical stability
            const int subSteps = 8;
            float subDt = dt / (float)subSteps;

            for (int step = 0; step < subSteps; ++step) {
                // 1. Calculate accelerations & update velocities (Symplectic Euler - Pass 1)
                for (size_t i = 0; i < objs.size(); ++i) {
                    for (size_t j = 0; j < objs.size(); ++j) {
                        if (i != j) {
                            glm::vec2 dir;
                            float gravity = objs[i].acceleratefreespace(objs[i], objs[j], dir);
                            objs[i].accelerate(subDt, dir, gravity);
                        }
                    }
                }

                // 2. Update positions using NEW velocities (Symplectic Euler - Pass 2)
                for (size_t i = 0; i < objs.size(); ++i) {
                    if (i == 0) continue;
                    objs[i].updatevelocity(subDt);
                }

                // 3. Resolve collisions
                for (size_t i = 0; i < objs.size(); ++i) {
                    for (size_t j = i + 1; j < objs.size(); ++j) {
                        collision(objs[i], objs[j]);
                    }
                }
            }

            // Dynamic grid update pass
            gridVertices = creategrid(500.0f, 200.0f, objs);
            glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, gridVertices.size() * sizeof(float), gridVertices.data());

            glClearColor(0.0f, 0.0f, 0.02f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(shader);

            // Compute View-Projection Transformation
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            float aspect = (height > 0) ? (float)width / (float)height : 1.777f;
            glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 500.0f);
            glm::mat4 view = camera.get_view_matrix();
            glm::mat4 MVP = projection * view; // Model matrix is Identity

            glUniformMatrix4fv(glGetUniformLocation(shader, "u_MVP"), 1, GL_FALSE, glm::value_ptr(MVP));

            // Draw 3D Ground Grid
            glUniform3f(glGetUniformLocation(shader, "u_Offset"), 0.0f, 0.0f, 0.0f);
            glUniform4f(glGetUniformLocation(shader, "u_Color"), 0.3f, 0.6f, 1.0f, 0.6f);
            glBindVertexArray(gridVAO);
            glDrawArrays(GL_LINES, 0, (GLsizei)(gridVertices.size() / 3));

            // Draw 3D Balls in 3D Space
            glBindVertexArray(ballVAO);

            for (auto& obj : objs) {
                std::vector<glm::vec3> circleVerts = obj.drawsphere();

                glBindBuffer(GL_ARRAY_BUFFER, ballVBO);
                glBufferData(GL_ARRAY_BUFFER, circleVerts.size() * sizeof(glm::vec3), circleVerts.data(), GL_DYNAMIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

               

                glUniform3f(glGetUniformLocation(shader, "u_Offset"), obj.Position[0], 0.0f, obj.Position[1]);
                glUniform4f(glGetUniformLocation(shader, "u_Color"), 1.0f, 1.0f, 1.0f, 1.0f);
                glDrawArrays(GL_TRIANGLES, 0, (GLsizei)circleVerts.size());
            }

            glBindVertexArray(0);
            glfwSwapBuffers(window);
        }

        glDeleteProgram(shader);
        glDeleteVertexArrays(1, &gridVAO);
        glDeleteBuffers(1, &gridVBO);
        glDeleteVertexArrays(1, &ballVAO);
        glDeleteBuffers(1, &ballVBO);
    }
    glfwTerminate();
    return 0;
}