/********************************************************************************************************
 * CSE333: Computer Graphics
 * Course Project: Raster to Vector Conversion in Pixel Art Images
 * 
 * by Shreyas Gupta
 * Roll No.: 2020131
 * 
 ********************************************************************************************************
 */


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "ObjectRenderer.h"
#include "RasterSprite.h"
#include "SimilarityGraph.h"
#include "VoroniGraph.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

int main(void)
{
    // *******************************************************************************
    
    // SCALABLE PARAMETERS
    std::string imageSource = "res/textures/report_demo.png";
    int pixelsPerSquare = 1;
    int imageScale = 1500;

    // *******************************************************************************

    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    int windowWidth = 1440;
    int windowHeight = 800;

    window = glfwCreateWindow(windowWidth, windowHeight, "Raster to Vector", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK)
        std::cout << "Error initialising GLEW" << std::endl;

    {
        RasterSprite raster(imageSource, windowWidth, windowHeight, pixelsPerSquare, imageScale);
        SimilarityGraph rasterGraph(raster, 0.112);
        VoroniGraph voroni(&raster, &rasterGraph);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Renderer for the raster pixels
        // For a sharper display of the input, each pixel is rendered with separate triangles (instead of a texture)
        ObjectRenderer rasterRenderer(raster.getRasterTriangles(), raster.getRasterVertexCount() * 6 * sizeof(float),
            raster.getRasterIndices(), raster.getRasterIndexCount());
        rasterRenderer.addFloatAttrib("Position", 2);
        rasterRenderer.addFloatAttrib("Colour", 4);
        rasterRenderer.AddBuffersToVAO();

        // Renderer for the VORONOI Diagram
        ObjectRenderer voroniRenderer(voroni.getVoroniGraphTriangles(), voroni.getVoroniGraphTriangleCount() * sizeof(float),
            voroni.getVoroniGraphIndices(), voroni.getVoroniGraphTriangleCount() / 6);
        voroniRenderer.addFloatAttrib("Position", 2);
        voroniRenderer.addFloatAttrib("Colour", 4);
        voroniRenderer.AddBuffersToVAO();

        // Renderer for Vertices of the similarity graph
        float* graphVertices = rasterGraph.getGraphVertexVertices();
        unsigned int* graphIndices = rasterGraph.getGraphVertexIndices();
        ObjectRenderer graphVerticesRenderer(graphVertices, rasterGraph.getGraphVertexRenderCount() * (2 + 4) * sizeof(float),
            graphIndices, rasterGraph.getGraphVertexIndexCount());
        graphVerticesRenderer.addFloatAttrib("Position", 2);
        graphVerticesRenderer.addFloatAttrib("Colour", 4);
        graphVerticesRenderer.AddBuffersToVAO();

        // Renderer for Edges of the similarity graph
        ObjectRenderer graphEdgesRenderer(rasterGraph.getGraphEdgeVertices(), rasterGraph.getGraphEdgeCount() * 2 * (2 + 4) * sizeof(float),
            rasterGraph.getGraphEdgeIndices(), rasterGraph.getGraphEdgeCount() * 2);
        graphEdgesRenderer.addFloatAttrib("Position", 2);
        graphEdgesRenderer.addFloatAttrib("Colour", 4);
        graphEdgesRenderer.AddBuffersToVAO();

        // Adding Shaders in all the above renderers
        char vertexShaderPath[] = "res/shaders/VertexShader.shader";
        char fragmentShaderPath[] = "res/shaders/FragmentShader.shader";
        rasterRenderer.setShaders(vertexShaderPath, fragmentShaderPath);
        voroniRenderer.setShaders(vertexShaderPath, fragmentShaderPath);
        graphVerticesRenderer.setShaders(vertexShaderPath, fragmentShaderPath);
        graphEdgesRenderer.setShaders(vertexShaderPath, fragmentShaderPath);

        // Derefencing all shaders & buffers
        glUseProgram(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // ImGui Setup
        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();

        // Variables to control the stage of depixelisation shown (modified by the imgui window)
        bool show_similarity_graph = false;
        bool show_voroni_diagram = false;
        bool simplify_voroni_diagram = false;
        bool diagram_simplified = false;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);
            glClearColor(0.0f, 0.4f, 0.0f, 0.0f);

            ImGui_ImplGlfwGL3_NewFrame();

            rasterRenderer.useShader();

            // Simplify the Voronoi diagram when user asks
            if (simplify_voroni_diagram && !diagram_simplified)
            {
                voroniRenderer.bindVAO();
                voroni.simplifyVoroniGraph();
                voroni.setVoroniGraphTriangles();
                voroniRenderer.updateVBO(voroni.getVoroniGraphTriangles(), voroni.getVoroniGraphTriangleCount() * sizeof(float));
                voroniRenderer.updateIBO(voroni.getVoroniGraphIndices(), voroni.getVoroniGraphTriangleCount() / 6);
                diagram_simplified = true;
                show_voroni_diagram = true;
            }
            // Desimplify the Voronoi diagram when user asks
            if (!simplify_voroni_diagram && diagram_simplified)
            {
                voroniRenderer.bindVAO();
                voroni.setPixelFaces();
                voroni.setSubPixels();
                voroni.constructHalfEdges();
                voroni.setVoroniGraphTriangles();
                voroniRenderer.updateVBO(voroni.getVoroniGraphTriangles(), voroni.getVoroniGraphTriangleCount() * sizeof(float));
                voroniRenderer.updateIBO(voroni.getVoroniGraphIndices(), voroni.getVoroniGraphTriangleCount() / 6);
                diagram_simplified = false;
            }
            // Display either the input raster image or the Voronoi diagram, based on user input
            if(show_voroni_diagram)
            {
                voroniRenderer.drawTriangles();
            }
            else
            {
                rasterRenderer.drawTriangles();
                simplify_voroni_diagram = false;
            }
            
            if (show_similarity_graph)
            {
                graphEdgesRenderer.drawLines();
                graphVerticesRenderer.drawTriangles();
            }

            // ImGUI Window
            {
                ImGui::Text("RASTER DISPLAY CONTROLS");
                ImGui::Checkbox("Show Similarity Graph", &show_similarity_graph);
                ImGui::Checkbox("Show Voroni Diagram", &show_voroni_diagram);
                ImGui::Checkbox("Simplify Voroni Diagram", &simplify_voroni_diagram);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            // Render ImGui UI
            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
            /* Swap front and back buffers */
            glfwSwapBuffers(window);
            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}