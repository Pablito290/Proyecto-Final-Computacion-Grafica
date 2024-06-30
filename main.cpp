//! [code]
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>

#include <unordered_map>
#include "learnopengl/shader_s.h"
#include "learnopengl/camera.h"



//This is something that stb image defines in its own include guard, so I should be able to test for it
#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <filesystem>


namespace fs = std::filesystem;
using namespace std;


string Mercurio = "C:\\Users\\pacit\\Desktop\\GraficaUSB\\LabCompiler2023_II_CG\\glfw-master\\OwnProjects\\Project_02\\Textures\\Mercury.jpg";
string VenusT = "C:\\Users\\pacit\\Desktop\\GraficaUSB\\LabCompiler2023_II_CG\\glfw-master\\OwnProjects\\Project_02\\Textures\\Venus.jpg";
string Tierra = "C:\\Users\\pacit\\Desktop\\GraficaUSB\\LabCompiler2023_II_CG\\glfw-master\\OwnProjects\\Project_02\\Textures\\Earth.jpg";
string Marte = "C:\\Users\\pacit\\Desktop\\GraficaUSB\\LabCompiler2023_II_CG\\glfw-master\\OwnProjects\\Project_02\\Textures\\Mars.jpg";
string Luna = "C:\\Users\\pacit\\Desktop\\GraficaUSB\\LabCompiler2023_II_CG\\glfw-master\\OwnProjects\\Project_02\\Textures\\Moon.jpg";
string Estrellas = "C:\\Users\\pacit\\Desktop\\GraficaUSB\\LabCompiler2023_II_CG\\glfw-master\\OwnProjects\\Project_02\\Textures\\Stars.jpg";
string AnillosT = "C:\\Users\\pacit\\Desktop\\GraficaUSB\\LabCompiler2023_II_CG\\glfw-master\\OwnProjects\\Project_02\\Textures\\AnillosS2.png";
string Saturno = "C:\\Users\\pacit\\Desktop\\GraficaUSB\\LabCompiler2023_II_CG\\glfw-master\\OwnProjects\\Project_02\\Textures\\Saturno.jpg";
string JupiterT = "C:\\Users\\pacit\\Desktop\\GraficaUSB\\LabCompiler2023_II_CG\\glfw-master\\OwnProjects\\Project_02\\Textures\\Jupiter.jpg";
string Urano = "C:\\Users\\pacit\\Desktop\\GraficaUSB\\LabCompiler2023_II_CG\\glfw-master\\OwnProjects\\Project_02\\Textures\\Urano.jpg";
string Neptuno = "C:\\Users\\pacit\\Desktop\\GraficaUSB\\LabCompiler2023_II_CG\\glfw-master\\OwnProjects\\Project_02\\Textures\\Neptuno.jpg";
string Sol = "C:\\Users\\pacit\\Desktop\\GraficaUSB\\LabCompiler2023_II_CG\\glfw-master\\OwnProjects\\Project_02\\Textures\\Sol.jpg";
string blanco = "C:\\Users\\pacit\\Desktop\\GraficaUSB\\LabCompiler2023_II_CG\\glfw-master\\OwnProjects\\Project_02\\Textures\\Blanco.jpg";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 1630;
const unsigned int SCR_HEIGHT = 820;

glm::vec3 lightPos(0.00001f, 0.00001f, 0.00001f);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
int vista2 = 0;

float deltaTime = 0.0f;	
float lastFrame = 0.0f;


class Objeto {
public:
    Objeto(const vector<float>& _vertices, const vector<unsigned int>& _indices, const string& _texturePath)
        : vertices(_vertices), indices(_indices), texturePath(_texturePath) {
        initialize();
    }

    ~Objeto() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteTextures(1, &textureID);
    }

    void initialize() {
        model = glm::mat4(1.0f);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Enlazar el buffer de vértices
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Enlazar el buffer de elementos
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Atributo de posición del vértice
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Atributo de coordenadas de textura
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Atributo de normal del vértice
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        loadTexture(texturePath);
    }


    void render(const Shader& shader, const glm::vec3& lightColor = glm::vec3(1.0f, 1.0f, 1.0f), const glm::vec3& objectColor = glm::vec3(1.0f, 0.5f, 0.31f)) const {
        shader.use();

        // Configure lighting uniforms
        shader.setVec3("objectColor", objectColor);
        shader.setVec3("lightColor", lightColor);
        shader.setVec3("lightPos", lightPos); // Directly use global variable
        shader.setVec3("viewPos", camera.Position); // Directly use global variable

        // Bind the object's texture
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Bind VAO and draw the object
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }

    void modelrender(Shader &shader){
        shader.setMat4("model", model);
        render(shader);
        shader.setMat4("model", glm::mat4(1.0f));
    }

    void renderBasic(const Shader& shader) const {
        shader.use();
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }




    void updateVertices(const vector<float>& newVertices) {
        vertices = newVertices;
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    }

    void updateIndices(const vector<unsigned int>& newIndices) {
        indices = newIndices;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }

    void setModelMatrix(const glm::mat4& newModel) {
        model = newModel;
    }

    glm::mat4 getModelMatrix() const {
        return model;
    }
    void updateTexture(const string& newTexturePath) {
        loadTexture(newTexturePath);
    }


    void rotacion_Render(Shader &shader, float angle = 0.1f, vector<float> ejes = {0.0f, 0.0f, 0.0f}) {
        model = glm::rotate(model, glm::radians(angle), glm::vec3(ejes[0], ejes[1], ejes[2]));
        shader.setMat4("model", model);
        render(shader);
        //limpiar model de shader
        shader.setMat4("model", glm::mat4(1.0f));
    }
    
    void orbita_Render(Shader &shader, float angle = 0.1f, float radio = 0.8f, vector<float> ejes = {0.0f, 0.0f, 0.0f}) {
        // Usar una matriz temporal para la transformación orbital
        glm::mat4 orbitalModel = model;  // Comenzar con la matriz de modelo actual

        // Aplicar primero la traslación y luego la rotación
        orbitalModel = glm::rotate(orbitalModel, glm::radians(angle/2), glm::vec3(ejes[0], ejes[1], ejes[2]));
        orbitalModel = glm::translate(orbitalModel, glm::vec3(radio, 0.0f, 0.0f));
        orbitalModel = glm::rotate(orbitalModel, glm::radians(angle), glm::vec3(ejes[0], ejes[1], ejes[2]));

        shader.use();
        shader.setMat4("model", orbitalModel);
        render(shader);

        // No es necesario restablecer la matriz del modelo en el shader aquí,
        // ya que cada objeto debería configurar su propia matriz de modelo antes de renderizar.
    }

    void justorbita_Render(Shader &shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float angle = 0.1f, float radio = 0.8f, vector<float> ejes = {0.0f, 0.0f, 0.0f}, float bajar =0.01f) {
    // Usar una matriz temporal para la transformación orbital
    glm::mat4 orbitalModel = getModelMatrix();  // Comenzar con la matriz de modelo actual
    orbitalModel = glm::translate(orbitalModel, glm::vec3(0.0f, bajar, 0.0f));

    // Aplicar primero la traslación y luego la rotación
    orbitalModel = glm::rotate(orbitalModel, glm::radians(angle / 2), glm::vec3(ejes[0], ejes[1], ejes[2]));
    orbitalModel = glm::translate(orbitalModel, glm::vec3(radio, 0.0f, 0.0f));

    // Alinear el objeto hacia la cámara
    glm::vec3 cameraPos = glm::vec3(glm::inverse(viewMatrix)[3]);
    glm::vec3 objPos = glm::vec3(orbitalModel[3]); // La posición del objeto está en la cuarta columna de la matriz de modelo
    glm::vec3 direction = glm::normalize(cameraPos - objPos);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(up, direction));
    up = glm::cross(direction, right);

    glm::mat4 rotation = glm::mat4(1.0f);
    rotation[0] = glm::vec4(right, 0.0f);
    rotation[1] = glm::vec4(up, 0.0f);
    rotation[2] = glm::vec4(direction, 0.0f);

    orbitalModel = glm::translate(glm::mat4(1.0f), objPos) * rotation;

    shader.use();
    shader.setMat4("model", orbitalModel);
    shader.setMat4("view", viewMatrix);
    shader.setMat4("projection", projectionMatrix);
    render(shader);
    shader.setMat4("model", glm::mat4(1.0f));
    // No es necesario restablecer la matriz del modelo en el shader aquí,
    // ya que cada objeto debería configurar su propia matriz de modelo antes de renderizar.
}




    void orbita_orbita_Render(Shader &shader, float angle = 0.1f, float radio = 0.8f, float radio2 = 0.8f, vector<float> ejes = {0.0f, 0.0f, 0.0f}) {
        // Usar una matriz temporal para la transformación orbital
        glm::mat4 orbitalModel = model;  // Comenzar con la matriz de modelo actual

        // Aplicar primero la traslación y luego la rotación
        orbitalModel = glm::rotate(orbitalModel, glm::radians(angle/2), glm::vec3(ejes[0], ejes[1], ejes[2]));
        orbitalModel = glm::translate(orbitalModel, glm::vec3(radio, 0.0f, 0.0f));

        orbitalModel = glm::rotate(orbitalModel, glm::radians(angle), glm::vec3(ejes[0], ejes[1], ejes[2]));
        orbitalModel = glm::translate(orbitalModel, glm::vec3(radio2, 0.0f, 0.0f));

        shader.use();
        shader.setMat4("model", orbitalModel);
        render(shader);

        // No es necesario restablecer la matriz del modelo en el shader aquí,
        // ya que cada objeto debería configurar su propia matriz de modelo antes de renderizar.
    }

   void orbita_rotacion_Render(Shader &shader, float angle = 0.1f, float radio = 0.8f, vector<float> ejes = {0.0f, 0.0f, 0.0f}) {
        // Usar una matriz temporal para la transformación orbital
        glm::mat4 orbitalModel = model;  // Comenzar con la matriz de modelo actual

        // Aplicar primero la traslación y luego la rotación
        orbitalModel = glm::rotate(orbitalModel, glm::radians(angle/2), glm::vec3(ejes[0], ejes[1], ejes[2]));
        orbitalModel = glm::translate(orbitalModel, glm::vec3(radio, 0.0f, 0.0f));

        orbitalModel = glm::rotate(orbitalModel, glm::radians(angle), glm::vec3(ejes[0], ejes[1], ejes[2]));
        

        shader.use();
        shader.setMat4("model", orbitalModel);
        render(shader);

        // No es necesario restablecer la matriz del modelo en el shader aquí,
        // ya que cada objeto debería configurar su propia matriz de modelo antes de renderizar.
    }

    void rotacion_Const(Shader &shader, float angle = 0.1f, vector<float> ejes = {0.0f, 0.0f, 0.0f}) {
        model = glm::rotate(glm::mat4(1.0f) , glm::radians(angle), glm::vec3(ejes[0], ejes[1], ejes[2]));
        shader.setMat4("model", model);
        //limpiar model de shader
    }




private:
    vector<float> vertices;
    vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO, textureID;
    string texturePath;
    glm::mat4 model;

    void loadTexture(const string& path) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Configurar parámetros de la textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cerr << "Failed to load texture: " << path << std::endl;
        }
        stbi_image_free(data);
    }
};


class Sphere : public Objeto {
public:
    Sphere(const string& _texturePath, float rad, float _afelio = 0, float _perielio = 0, float _paso = 0, int _resolucion=20) : Objeto({}, {}, _texturePath) {
        radius = rad;
        resolucion = _resolucion;
		raio = _perielio;
		perielio = _perielio;
		afelio = _afelio;
		flagRaio = 1;
		paso = _paso;
		angulo = 0;
		divisorA = (afelio + perielio)/2;
		divisorB = perielio * 0.8;
		centro = divisorA - perielio;
		varX = - divisorA;
		initialize();
    }

    void initialize() {
        vector<float> vertices;
        vector<unsigned int> indices;
        const int stacks = resolucion;
        const int slices = resolucion;

        for (int i = 0; i <= stacks; ++i) {
            float V = i / (float)stacks;
            float phi = V * glm::pi<float>();

            for (int j = 0; j <= slices; ++j) {
                float U = j / (float)slices;
                float theta = U * (glm::pi<float>() * 2);

                // Posición del vértice
                float x = cos(theta) * sin(phi);
                float y = cos(phi);
                float z = sin(theta) * sin(phi);

                // Coordenadas de textura
                float u = 1.0f - U;
                float v = 1.0f - V;

                // Normal del vértice
                glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));

                // Añadir posición, coordenadas de textura y normal al vector de vértices
                vertices.push_back(x * radius);
                vertices.push_back(y * radius);
                vertices.push_back(z * radius);
                vertices.push_back(u);
                vertices.push_back(v);
                vertices.push_back(normal.x);
                vertices.push_back(normal.y);
                vertices.push_back(normal.z);
            }
        }

        // Cálculo de índices para los triángulos
        for (int i = 0; i < slices * stacks + slices; ++i) {
            indices.push_back(i);
            indices.push_back(i + slices + 1);
            indices.push_back(i + slices);

            indices.push_back(i + slices + 1);
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        // Actualizar los vertices e indices en la clase base Objeto
        Objeto::updateVertices(vertices);
        Objeto::updateIndices(indices);
        Objeto::initialize();
    }


    vector<float> getCenter() const {
        return center;
    }

    void setAngle(float newAngle) {
        angle = newAngle;
    }
    float getAngle() const {
        return angle;
    }
	
	float getRot() const {
        return paso;
    }
	
    float getRaio() {
		 if (flagRaio)
			 varX += divisorA * paso/ 360;
		 else
			 varX -= divisorA * paso / 360;
		 if (varX >= divisorA && flagRaio == 1)
		 {
			 flagRaio = 0;
			 varX = divisorA;
		 }
		 if (varX <= - divisorA  && flagRaio == 0)
		 {
			 flagRaio = 1;
			 varX = - divisorA;
		 }
        return sqrt((varX + centro)* (varX + centro) + divisorB * divisorB * (1 - ((varX + centro)* (varX + centro)) /(divisorA * divisorA)));
    }



    private:
		float afelio, perielio, divisorA, divisorB, centro, varX, raio, paso, angulo;
        int resolucion, flagRaio;
        float radius;
        float angle;
        vector<float> center;   
};


class Circle : public Objeto {
public:
    Circle(const string& _texturePath, float rad, int _resolucion = 20) 
        : Objeto({}, {}, _texturePath), radius(rad), resolucion(_resolucion) {
        initialize();
    }

    void initialize() {
        vector<float> vertices;
        vector<unsigned int> indices;
        const int slices = resolucion;

        // Centro del círculo
        vertices.push_back(0.0f);  // X
        vertices.push_back(0.0f);  // Y
        vertices.push_back(0.0f);  // Z
        vertices.push_back(0.5f);  // U
        vertices.push_back(0.5f);  // V
        vertices.push_back(0.0f);  // Normal X
        vertices.push_back(0.0f);  // Normal Y
        vertices.push_back(1.0f);  // Normal Z

        // Vertices para el perímetro del círculo
        for (int i = 0; i <= slices; ++i) {
            float theta = i * 2.0f * glm::pi<float>() / slices;
            float x = cos(theta) * radius;
            float y = sin(theta) * radius;

            vertices.push_back(x);         // X
            vertices.push_back(y);         // Y
            vertices.push_back(0.0f);      // Z
            vertices.push_back((x / (2.0f * radius)) + 0.5f);  // U
            vertices.push_back((y / (2.0f * radius)) + 0.5f);  // V
            vertices.push_back(0.0f);      // Normal X
            vertices.push_back(0.0f);      // Normal Y
            vertices.push_back(1.0f);      // Normal Z
        }

        // Índices para los triángulos del círculo
        for (int i = 1; i <= slices; ++i) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        // Actualizar los vertices e indices en la clase base Objeto
        Objeto::updateVertices(vertices);
        Objeto::updateIndices(indices);
        Objeto::initialize();
    }

    void alignWithCamera() {
        glm::vec3 cameraPos = camera.Position;
        glm::vec3 objPos = glm::vec3(getModelMatrix()[3]); // La posición del objeto está en la cuarta columna de la matriz de modelo
        glm::vec3 direction = glm::normalize(cameraPos - objPos);

        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(up, direction));
        up = glm::cross(direction, right);

        glm::mat4 rotation = glm::mat4(1.0f);
        rotation[0] = glm::vec4(right, 0.0f);
        rotation[1] = glm::vec4(up, 0.0f);
        rotation[2] = glm::vec4(direction, 0.0f);

        setModelMatrix(glm::translate(glm::mat4(1.0f), objPos) * rotation);
    }
    

private:
    float radius;
    int resolucion;
};

struct Estela {
    Estela(const std::string& _texturePath, int _cantidad, float _tam, int res, float _separacion) {
        Circulo= new Circle(_texturePath, _tam, res);
        cantidad = _cantidad;
        separacion=_separacion;
    }


    void printCordenadas(){
        // cout<<
    }

    void Desfasar(Shader &shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float angle, float desfase, float raio, float razondesfase) {

        for (int i = 0; i < cantidad; i++) {
            Circulo->justorbita_Render(shader, viewMatrix, projectionMatrix, angle - (((i) *razondesfase) * desfase), raio, {0.0f, 1.0f, 0.0f}, -separacion * i);
        }
    }

// Circulos[i]->justorbita_Render(shader, viewMatrix, projectionMatrix, angle - (((i+1) *10) * desfase), raio, {0.0f, 1.0f, 0.0f});

    

    Circle* Circulo;
    int cantidad;
    float tam;
    float separacion;
};


class Rings : public Objeto {
    public:
    Rings(const string& _texturePath, float rad, float _innerRadius, int _resolucion = 20) : Objeto({}, {}, _texturePath) {
        radius = rad;
        innerRadius = _innerRadius;
        resolucion = _resolucion;
        initialize();
    }
    void initialize() {
        vector<float> vertices;
        vector<unsigned int> indices;
        const int stacks = resolucion;

        // Normal de la superficie del anillo, apuntando hacia +Z ya que está en el plano XY
        glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f); 

        for (int i = 0; i <= stacks; ++i) {
            float theta = i * 2.0f * glm::pi<float>() / stacks;
            float x = cos(theta);
            float y = sin(theta);

            // Vertices for the outer radius
            vertices.push_back(x * radius);       // X
            vertices.push_back(y * radius);       // Y
            vertices.push_back(0.0f);             // Z
            vertices.push_back((float)i / stacks); // U
            vertices.push_back(0.0f);             // V
            vertices.push_back(normal.x);         // Normal X
            vertices.push_back(normal.y);         // Normal Y
            vertices.push_back(normal.z);         // Normal Z

            // Vertices for the inner radius
            vertices.push_back(x * innerRadius);  // X
            vertices.push_back(y * innerRadius);  // Y
            vertices.push_back(0.0f);             // Z
            vertices.push_back((float)i / stacks); // U
            vertices.push_back(1.0f);             // V
            vertices.push_back(normal.x);         // Normal X
            vertices.push_back(normal.y);         // Normal Y
            vertices.push_back(normal.z);         // Normal Z
        }

        for (int i = 0; i < stacks; ++i) {
            int outer1 = i * 2 * 8;
            int inner1 = (i * 2 + 1) * 8;
            int outer2 = ((i + 1) * 2) * 8;
            int inner2 = ((i + 1) * 2 + 1) * 8;

            indices.push_back(outer1 / 8);
            indices.push_back(inner1 / 8);
            indices.push_back(outer2 / 8);

            indices.push_back(inner1 / 8);
            indices.push_back(inner2 / 8);
            indices.push_back(outer2 / 8);
        }

        Objeto::updateVertices(vertices);
        Objeto::updateIndices(indices);
        Objeto::initialize();
}



    private:
        int resolucion;
        float radius;
        float innerRadius;
        vector<float> center;
};


class ConjuntoObjetos {
    public:
    void addObjeto(shared_ptr<Objeto> objeto) {
        objetos.push_back(objeto);
    }
    void aplicarTransformacion(const glm::mat4& transformacion) {
        for (auto& objeto : objetos) {
            
        }
    }
    

    private:
    vector<shared_ptr<Objeto>> objetos;
};



void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void setupShader(Shader &shader, const glm::mat4& view, const glm::mat4& projection) {
    shader.use(); // Activa el programa shader antes de configurar los uniformes
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
}




int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    //if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    if( !gladLoadGL(glfwGetProcAddress) )
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set path to search files to run this demo
	fs::path p = fs::current_path();
	std::cout << "Separator in my system " << fs::path::preferred_separator << std::endl;
 
    std::cout << "The current path: " << p << " decomposes into:\n"
              << "root name: " << p.root_name() << '\n'
              << "root directory: " << p.root_directory() << '\n'
              << "relative path: " << p.relative_path() << '\n';
	
	// Set current project path to search files to run this demo
	int levels_path = 1;
	fs::path p_current; 
	p_current = p.parent_path();
	
	std::cout << "The current path: " << p_current << " after cd.. by " << levels_path <<" levels \n";
    

	std::string vs_path , fs_path, fs_path2, fs_path3, vs_path3;
	
	
	std::stringstream ss;
	ss << std::quoted( p_current.string() );
	std::string out;
	ss >> std::quoted(out);
	
	vs_path = out + "\\OwnProjects\\Project_02\\learnopengl\\3.3.shader.vs";
	fs_path = out + "\\OwnProjects\\Project_02\\learnopengl\\3.3.shader.fs";
    fs_path2 = out + "\\OwnProjects\\Project_02\\learnopengl\\shadernoluz.fs";
    // fs_path3 = out + "\\OwnProjects\\Project_02\\learnopengl\\line.fs";
    // vs_path3 = out + "\\OwnProjects\\Project_02\\learnopengl\\line.vs";


    // build and compile our shader zprogram
    // ------------------------------------
    //Shader ourShader("4.2.texture.vs", "4.2.texture.fs");
    Shader ourShader(vs_path.c_str(), fs_path.c_str());
    Shader NoLight(vs_path.c_str(), fs_path2.c_str());
    // Shader LineShader(vs_path3.c_str(), fs_path3.c_str());


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int cantidadEstela=250;
    float acelera = 4;
    int estelaresolucion=10;
    float tamanoEstela=0.02;
    Sphere Sun(Sol, 0.8, 0, 0, 0);
    Estela EstelaSol(blanco, cantidadEstela, tamanoEstela, estelaresolucion, 0.025f);

    Sphere Mercury(Mercurio, 0.05, 1.8f, 1.6f, 0.1f * acelera);
    Rings OrbitaMercury(blanco, 1.8f, 1.75f);
    OrbitaMercury.setModelMatrix(glm::rotate(OrbitaMercury.getModelMatrix(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

    Estela EstelaMercury(blanco, cantidadEstela, tamanoEstela, estelaresolucion, 0.023f);
    

    Sphere Venus(VenusT, 0.08, 2.5f, 2.3f, 0.2f * acelera);
    Estela EstelaVenus(blanco, cantidadEstela, tamanoEstela, estelaresolucion, 0.023f);
    
    
    Sphere Earth(Tierra, 0.1, 3.3f, 2.5f, 0.15f * acelera);
    Estela EstelaTierra(blanco, cantidadEstela, tamanoEstela, estelaresolucion, 0.02f);

    Sphere Moon(Luna, 0.03, 3.0f, 2.7f, 0.01f * acelera);
    
    Sphere Mars(Marte, 0.07, 4.0f, 3.6f, 0.02f * acelera);
    Estela EstelaMarte(blanco, cantidadEstela, tamanoEstela, estelaresolucion, 0.023f);
    
    Sphere Jupiter(JupiterT, 0.3, 5.0f, 4.5f, 0.05f * acelera);
    Estela EstelaJupiter(blanco, cantidadEstela, tamanoEstela, estelaresolucion, 0.023f);

    Sphere Saturn(Saturno, 0.2, 6.0f, 5.4f, 0.03f * acelera);
    Estela EstelaSaturno(blanco, cantidadEstela, tamanoEstela, estelaresolucion, 0.025f);
    
    Rings Anillos(AnillosT, 0.3f, 0.4f);

    Sphere Uranus(Urano, 0.15, 7.0f, 6.4f, 0.02f * acelera);
    Estela EstelaUrano(blanco, cantidadEstela, tamanoEstela, estelaresolucion, 0.025f);
    

    Sphere Neptune(Neptuno, 0.15, 8.0f, 7.3f, 0.01f * acelera);
    Estela EstelaNeptuno(blanco, cantidadEstela, tamanoEstela, estelaresolucion, 0.025f);

    Sphere Stars(Estrellas, 60.0f, 0, 0, 0, 5);
    


    vector<Sphere*> asteroides;
    for (int i = 0; i < 5; i++) {
        Sphere* asteroide = new Sphere(Luna, 0.01,4);
        asteroides.push_back(asteroide);
    }
    
    



    
    glm::mat4 modelEarth = glm::mat4(1.0f);
    modelEarth = glm::translate(modelEarth, glm::vec3(0.0f, 0.0f, 0.0f));  // Trasladar la Earth
    Earth.setModelMatrix(modelEarth);

  
    float angleMercury=0;
    float angleVenus=0;
    float angleMars=0;
    float angleJupiter=0;
    float angleSaturn=0;
    float angleUranus=0;
    float angleNeptune=0;
    float angle=0;
	
	float raioMercury=0;
    float raioVenus=0;
    float raioMars=0;
    float raioJupiter=0;
    float raioSaturn=0;
    float raioUranus=0;
    float raioNeptune=0;
	float rt = 0;

    vector<float> anglesAsteroides(asteroides.size(), 0.0f);


   for(int i=0;i<asteroides.size();i++){
    float angle = rand() % 360; // Genera un número aleatorio entre 0 y 359
    glm::mat4 model = asteroides[i]->getModelMatrix();
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    //angle = rand() % 500; 
    float radio = 4+angle*0.01f;
    model = glm::rotate(model, glm::radians(radio), glm::vec3(0.0f, 1.0f, 0.0f));
    asteroides[i]->setModelMatrix(model);
}

for(int i=0;i<asteroides.size();i++){
    float angle = rand() % 360; // Genera un número aleatorio entre 0 y 359
    anglesAsteroides[i] = angle;
}

    int verticesprueba=1;

    camera.Position.y = 5.0f;
    camera.Pitch = -80.0f;

    Anillos.setModelMatrix(glm::rotate(Anillos.getModelMatrix(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	
	glm::mat4 view = glm::mat4(1.0f), projection =  glm::mat4(1.0f);
	glm::vec3 cameraPos, cameraTarget, cameraUp;
    float radius, camX, camZ, camY;
    
    while (!glfwWindowShouldClose(window)) {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      
		raioMercury= Mercury.getRaio();
		raioVenus=Venus.getRaio();
		raioMars= Mars.getRaio();
		raioJupiter= Jupiter.getRaio();
		raioSaturn= Saturn.getRaio();
		raioUranus= Uranus.getRaio();
		raioNeptune= Neptune.getRaio();
		rt = Earth.getRaio();
		
		glViewport(10, 10, SCR_WIDTH / 2 - 15, SCR_HEIGHT - 20);
        glm::mat4 view1 = camera.GetViewMatrix();
        glm::mat4 projection1 = glm::perspective(glm::radians(camera.Zoom), (float)(SCR_WIDTH / 2 - 15) / (float)SCR_HEIGHT, 0.1f, 100.0f);
        setupShader(NoLight, view1, projection1);

        Sun.rotacion_Render(NoLight, 0.01f* acelera, {0.0f, 1.0f, 0.0f});
        EstelaSol.Desfasar(NoLight, view1, projection1, 0, 0, 0, 3);
        EstelaMercury.Desfasar(NoLight, view1, projection1, angleMercury, Mercury.getRot(), raioMercury, 7);
        EstelaVenus.Desfasar(NoLight, view1, projection1, angleVenus, Venus.getRot(), raioVenus, 3);

        EstelaTierra.Desfasar(NoLight, view1, projection1, angle, Earth.getRot() , rt,4);
        EstelaMarte.Desfasar(NoLight, view1, projection1, angleMars, Mars.getRot(), raioMars, 15);
        EstelaJupiter.Desfasar(NoLight, view1, projection1, angleJupiter, Jupiter.getRot(), raioJupiter, 5);
        EstelaSaturno.Desfasar(NoLight, view1, projection1, angleSaturn, Saturn.getRot(), raioSaturn, 6);
        EstelaUrano.Desfasar(NoLight, view1, projection1, angleUranus, Uranus.getRot(), raioUranus, 10);
        EstelaNeptuno.Desfasar(NoLight, view1, projection1, angleNeptune, Neptune.getRot(), raioNeptune, 17);

        Stars.rotacion_Render(NoLight, -0.1f, {1.0f, 0.0f, 0.0f});
        // OrbitaMercury.modelrender(NoLight);

        Anillos.orbita_Render(NoLight, -angleSaturn, Saturn.getRaio(), {0.0f, 0.0f, 1.0f});

        for (int i = 0; i < asteroides.size(); i++) {
            asteroides[i]->orbita_Render(NoLight, anglesAsteroides[i], 6.5, {1.0f, 1.0f, 0.0f});
            anglesAsteroides[i] += 0.001f;
        }

        setupShader(ourShader, view1, projection1);



		// setupShader(ourShader, camera.GetViewMatrix(), SCR_WIDTH/2 - 15, SCR_HEIGHT - 20);
        Earth.orbita_Render(ourShader,  angle, rt, {0.0f, 1.0f, 0.0f});
        Moon.orbita_orbita_Render(ourShader, angle, rt, 0.2f, {0.0f, 1.0f, 0.0f});
        Mercury.orbita_Render(ourShader, angleMercury, raioMercury, {0.1f, 1.0f, 0.0f});
        Venus.orbita_Render(ourShader, angleVenus, raioVenus, {0.0f, 1.0f, 0.0f});
        Mars.orbita_Render(ourShader, angleMars, raioMars, {0.0f, 1.0f, 0.0f});
        Jupiter.orbita_Render(ourShader, angleJupiter, raioJupiter, {0.0f, 1.0f, 0.0f});
        Saturn.orbita_Render(ourShader, angleSaturn, raioSaturn, {0.0f, 1.0f, 0.0f});
        Uranus.orbita_Render(ourShader, angleUranus, raioUranus, {0.0f, 1.0f, 0.0f});
        Neptune.orbita_Render(ourShader, angleNeptune,raioNeptune, {0.0f, 1.0f, 0.0f});
		//-----------------------------------------------------------------------------------------------------------------------
		glViewport(SCR_WIDTH / 2 + 5, 10, SCR_WIDTH / 2 - 15, SCR_HEIGHT - 20);
        radius = 8.0f;
        camX = sin(glfwGetTime()) * radius;
        camZ = cos(glfwGetTime()) * radius;
        camY = cos(glfwGetTime()) * radius;
        cameraPos = glm::vec3(camX, camY, camZ);
        cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 view2 = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        glm::mat4 projection2 = glm::perspective(glm::radians(45.0f), (float)(SCR_WIDTH / 2 - 15) / (float)SCR_HEIGHT, 0.1f, 100.0f);
        setupShader(NoLight, view2, projection2);


        Sun.rotacion_Render(NoLight, 0.01f* acelera, {0.0f, 1.0f, 0.0f});
        Stars.rotacion_Render(NoLight, -0.01f, {1.0f, 0.0f, 0.0f});
        Anillos.orbita_Render(NoLight, -angleSaturn, raioSaturn, {0.0f, 0.0f, 1.0f});
        
        for (int i = 0; i < asteroides.size(); i++) {
            asteroides[i]->orbita_Render(NoLight, anglesAsteroides[i], 4.5 ,{1.0f, 1.0f, 0.0f});
            anglesAsteroides[i] += 0.001f;
        }


        setupShader(ourShader, view2, projection2);
        Earth.orbita_Render(ourShader,  angle, rt, {0.0f, 1.0f, 0.0f});
        Moon.orbita_orbita_Render(ourShader, angle, rt, 0.2f, {0.0f, 1.0f, 0.0f});
        Mercury.orbita_Render(ourShader, angleMercury, raioMercury, {0.1f, 1.0f, 0.0f});
        Venus.orbita_Render(ourShader, angleVenus, raioVenus, {0.0f, 1.0f, 0.0f});
        Mars.orbita_Render(ourShader, angleMars, raioMars, {0.0f, 1.0f, 0.0f});
        Jupiter.orbita_Render(ourShader, angleJupiter, raioJupiter, {0.0f, 1.0f, 0.0f});
        Saturn.orbita_Render(ourShader, angleSaturn, raioSaturn, {0.0f, 1.0f, 0.0f});
        Uranus.orbita_Render(ourShader, angleUranus, raioUranus, {0.0f, 1.0f, 0.0f});
        Neptune.orbita_Render(ourShader, angleNeptune, raioNeptune, {0.0f, 1.0f, 0.0f});
		
        angle += Earth.getRot();
        angleMercury += Mercury.getRot();
        angleVenus += Venus.getRot();
        angleMars += Mars.getRot();
        angleJupiter += Jupiter.getRot();
        angleSaturn += Saturn.getRot();
        angleUranus += Uranus.getRot();
        angleNeptune += Neptune.getRot();


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
