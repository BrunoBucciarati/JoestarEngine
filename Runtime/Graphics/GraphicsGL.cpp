//#include <glad/glad.h>
//#include "GraphicsGL.h"
//#include <GLFW/glfw3.h>
//#include <iostream>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include "../Misc/GlobalConfig.h"
//
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    // make sure the viewport matches the new window dimensions; note that width and 
//    // height will be significantly larger than specified on retina displays.
//    glViewport(0, 0, width, height);
//}
//void processInput(GLFWwindow* window)
//{
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//    //float cameraSpeed = 0.05f; // adjust accordingly
//    //if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//    //    cam->ProcessKeyboard(FORWARD, deltaTime);
//    //if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//    //    cam->ProcessKeyboard(BACKWARD, deltaTime);
//    //if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//    //    cam->ProcessKeyboard(LEFT, deltaTime);
//    //if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//    //    cam->ProcessKeyboard(RIGHT, deltaTime);
//}
//
//void GraphicsGL::Init() {
//    glfwInit();
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//    uint32_t width = GetGlobalConfig()->GetConfig<uint32_t>("WINDOW_WIDTH");
//    uint32_t height = GetGlobalConfig()->GetConfig<uint32_t>("WINDOW_HEIGHT");
//    GLFWwindow* window = glfwCreateWindow(width, height, "JoestarEngine", NULL, NULL);
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//    glfwSetCursorPosCallback(window, mouse_callback);
//    glfwSetScrollCallback(window, scroll_callback);
//    glfwMakeContextCurrent(window);
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return;
//    }
//    int nrAttributes;
//    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
//    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
//
//
//    while (!glfwWindowShouldClose(window))
//    {
//        // input
//        // -----
//        processInput(window);
//
//        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
//        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//}
//
//void GraphicsGL::DrawTriangle() {
//
//}
//
//void GraphicsGL::DrawMesh(Mesh* mesh) {
//    if (mesh->cachedDirty) {
//        UpdateCacheMesh(mesh);
//        mesh->cachedDirty = false;
//    }
//
//    glBindVertexArray(mesh->cachedVAO);
//    glDrawElements(GL_TRIANGLES, mesh->indices_.size(), GL_UNSIGNED_INT, 0);
//    glBindVertexArray(0);
//}
//
//void GraphicsGL::UpdateCacheMesh(Mesh* mesh) {
//    if (0 == mesh->cachedVAO) {
//        GLuint VAO, VBO, EBO;
//        glGenVertexArrays(1, &VAO);
//        glGenBuffers(1, &VBO);
//        glGenBuffers(1, &EBO);
//
//        glBindVertexArray(VAO);
//        // load data into vertex buffers
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        // A great thing about structs is that their memory layout is sequential for all its items.
//        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
//        // again translates to 3/2 floats which translates to a byte array.
//        glBufferData(GL_ARRAY_BUFFER, mesh->vertices_.size() * sizeof(Vertex), &mesh->vertices_[0], GL_STATIC_DRAW);
//
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices_.size() * sizeof(unsigned int), &mesh->indices_[0], GL_STATIC_DRAW);
//
//        // set the vertex attribute pointers
//        // vertex Positions
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
//        // vertex normals
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
//        // vertex texture coords
//        glEnableVertexAttribArray(2);
//        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
//        // vertex tangent
//        glEnableVertexAttribArray(3);
//        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
//        // vertex bitangent
//        glEnableVertexAttribArray(4);
//        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
//
//        glBindVertexArray(0);
//        mesh->cachedVAO = VAO;
//    }
//}
//
//void mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//    //if (firstMouse) // ���bool������ʼʱ���趨Ϊtrue��
//    //{
//    //    lastX = xpos;
//    //    lastY = ypos;
//    //    firstMouse = false;
//    //}
//    //float xoffset = xpos - lastX;
//    //float yoffset = lastY - ypos; // ע���������෴�ģ���Ϊy�����Ǵӵײ����������������
//    //lastX = xpos;
//    //lastY = ypos;
//
//    //cam->ProcessMouseMovement(xoffset, yoffset);
//}
//
//
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    //cam->ProcessMouseScroll(yoffset);
//}
//
