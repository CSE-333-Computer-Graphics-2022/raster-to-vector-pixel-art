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
    
    // EDITABLE PARAMETERS
    std::string imageSource = "res/textures/mewsprite.png";
    char* imageSrc = (char*)imageSource.c_str();//"res/textures/ralseiprite.png";
    int pixelsPerSquare = 10;
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

        // Renderer for Visible Edges in the Voronoi Diagram
        voroni.setVoroniVisibleEdgeLines();
        ObjectRenderer visibleEdgesRenderer(voroni.getVoroniVisibleEdgeLines(), voroni.getVoroniVisibleEdgeCount() * 12 * sizeof(float),
            voroni.getVoroniGraphIndices(), voroni.getVoroniVisibleEdgeCount() * 2);
        visibleEdgesRenderer.addFloatAttrib("Position", 2);
        visibleEdgesRenderer.addFloatAttrib("Colour", 4);
        visibleEdgesRenderer.AddBuffersToVAO();
        std::cout << "NUMBER OF EDGES : " << voroni.getVoroniVisibleEdgeCount() << std::endl;

        // Adding Shaders in all the above renderers
        char vertexShaderPath[] = "res/shaders/VertexShader.shader";
        char fragmentShaderPath[] = "res/shaders/FragmentShader.shader";
        rasterRenderer.setShaders(vertexShaderPath, fragmentShaderPath);
        voroniRenderer.setShaders(vertexShaderPath, fragmentShaderPath);
        graphVerticesRenderer.setShaders(vertexShaderPath, fragmentShaderPath);
        graphEdgesRenderer.setShaders(vertexShaderPath, fragmentShaderPath);
        visibleEdgesRenderer.setShaders(vertexShaderPath, fragmentShaderPath);

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
        bool create_voronoi_diagram = false;
        bool show_voroni_diagram = false;
        bool simplify_voroni_diagram = false;
        bool show_visible_edges = false;
        bool show_t_junctions = false;

        bool diagram_simplified = false;
        bool replace_visible_edges_with_splines = false;
        bool splines_replaced = false;

        int optimisationsPerformed = 0;
        int optimisationsToPerform = 0;
        bool isOptimising = false;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);
            glClearColor(0.0f, 0.4f, 0.0f, 0.0f);

            ImGui_ImplGlfwGL3_NewFrame();

            rasterRenderer.useShader();

            // Simplify the Voronoi diagram when user asks
            //if (simplify_voroni_diagram && !diagram_simplified)
            if(isOptimising)
            {
                voroniRenderer.bindVAO();
                if (!simplify_voroni_diagram)
                {
                    voroni.simplifyVoroniGraph();
                    voroni.setVerticesSharpness();
                }
                
                //int numOptimisations = 50;
                //for (int i = 0; i < numOptimisations; i++)
                if(optimisationsPerformed < optimisationsToPerform)
                {
                    voroni.optimiseImageVertices();
                    //std::cout << "Optimised (" << optimisationsPerformed+1 << "/" << optimisationsToPerform << ")" << std::endl;
                    optimisationsPerformed++;
                }
                else
                {
                    //diagram_simplified = true;
                    optimisationsPerformed = 0;
                    optimisationsToPerform = 0;
                    isOptimising = false;
                }
                voroni.setVoroniGraphTriangles();
                voroniRenderer.updateVBO(voroni.getVoroniGraphTriangles(), voroni.getVoroniGraphTriangleCount() * sizeof(float));
                voroniRenderer.updateIBO(voroni.getVoroniGraphIndices(), voroni.getVoroniGraphTriangleCount() / 6);
                //diagram_simplified = true;
                show_voroni_diagram = true;

                voroni.setHalfEdgesVisibility();
                voroni.setVoroniVisibleEdgeLines();
                visibleEdgesRenderer.updateVBO(voroni.getVoroniVisibleEdgeLines(), voroni.getVoroniVisibleEdgeCount() * 12 * sizeof(float));
                visibleEdgesRenderer.updateIBO(voroni.getVoroniGraphIndices(), voroni.getVoroniVisibleEdgeCount() * 2);
            }
            //// Desimplify the Voronoi diagram when user asks
            //if (!simplify_voroni_diagram && diagram_simplified)
            //{
            //    voroniRenderer.bindVAO();
            //    voroni.setPixelFaces();
            //    voroni.setSubPixels();
            //    voroni.constructHalfEdges();
            //    voroni.setVoroniGraphTriangles();
            //    voroniRenderer.updateVBO(voroni.getVoroniGraphTriangles(), voroni.getVoroniGraphTriangleCount() * sizeof(float));
            //    voroniRenderer.updateIBO(voroni.getVoroniGraphIndices(), voroni.getVoroniGraphTriangleCount() / 6);
            //    diagram_simplified = false;

            //    voroni.setHalfEdgesVisibility();
            //    voroni.setVoroniVisibleEdgeLines();
            //    visibleEdgesRenderer.updateVBO(voroni.getVoroniVisibleEdgeLines(), voroni.getVoroniVisibleEdgeCount() * 12 * sizeof(float));
            //    visibleEdgesRenderer.updateIBO(voroni.getVoroniGraphIndices(), voroni.getVoroniVisibleEdgeCount() * 2);
            //}
            // Display either the input raster image or the Voronoi diagram, based on user input
            if (replace_visible_edges_with_splines)
            {
                if (!show_voroni_diagram)
                {
                    show_voroni_diagram = true;
                }
                else if (!simplify_voroni_diagram)
                {
                    simplify_voroni_diagram = true;
                }
                else if(!splines_replaced)
                {
                    voroni.replaceVisibleEdgesWithSplines();
                    voroni.setHalfEdgesVisibility();
                    voroni.setVoroniVisibleEdgeLines();
                    visibleEdgesRenderer.updateVBO(voroni.getVoroniVisibleEdgeLines(), voroni.getVoroniVisibleEdgeCount() * 12 * sizeof(float));
                    visibleEdgesRenderer.updateIBO(voroni.getVoroniGraphIndices(), voroni.getVoroniVisibleEdgeCount() * 2);

                    voroni.setVoroniGraphTriangles();
                    voroniRenderer.updateVBO(voroni.getVoroniGraphTriangles(), voroni.getVoroniGraphTriangleCount() * sizeof(float));
                    voroniRenderer.updateIBO(voroni.getVoroniGraphIndices(), voroni.getVoroniGraphTriangleCount() / 6);

                    splines_replaced = true;
                }
            }

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
            if (show_visible_edges)
            {
                visibleEdgesRenderer.drawLines();
            }

            // ImGUI Window
            {
                // DEBUG (default) WINDOW
                ImGui::Begin("Debug");
                //ImGui::Checkbox("Simplify Voroni Diagram", &simplify_voroni_diagram);
                //ImGui::Checkbox("Show Visible Edges", &show_visible_edges);
                /*ImGui::Checkbox("Replace Visible Edges with Splines", &replace_visible_edges_with_splines);*/
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();

                // FILE PATH WINDOW
                ImGui::Begin("RASTER DISPLAY CONTROLS");
                ImGui::InputText("File Path", imageSrc, sizeof(imageSrc));
                ImGui::End();

                // SIMILARITY GRAPH CONTROLS WINDOW
                ImGui::Begin("SIMILARITY GRAPH CONTROLS");
                ImGui::Checkbox("Show Similarity Graph", &show_similarity_graph);
                const char* similarityGraphButtonText = (rasterGraph.isPlanar()) ? "RESET GRAPH" : "MAKE GRAPH PLANAR";
                const char* similarityGraphBoxText = (rasterGraph.isPlanar()) ? "Similarity Graph is planar!" : "Similarity Graph is NOT planar. Press button above";
                if (ImGui::Button(similarityGraphButtonText))
                {
                    if (rasterGraph.isPlanar())
                        rasterGraph.resetGraph();
                    else
                        rasterGraph.resolveGraph();

                    graphVerticesRenderer.updateVBO(rasterGraph.getGraphVertexVertices(), rasterGraph.getGraphVertexRenderCount() * (2 + 4) * sizeof(float));
                    graphVerticesRenderer.updateIBO(rasterGraph.getGraphVertexIndices(), rasterGraph.getGraphVertexIndexCount());
                    graphEdgesRenderer.updateVBO(rasterGraph.getGraphEdgeVertices(), rasterGraph.getGraphEdgeCount() * 2 * (2 + 4) * sizeof(float));
                    graphEdgesRenderer.updateIBO(rasterGraph.getGraphEdgeIndices(), rasterGraph.getGraphEdgeCount() * 2);
                }
                ImGui::Text("");
                ImGui::Text(similarityGraphBoxText);
                ImGui::End();
                   
                // VORONOI DIAGRAM CONTROL WINDOW
                ImGui::Begin("VORONOI DIAGRAM CONTROLS");
                
                const char* isPlanarText = (rasterGraph.isPlanar()) ? "" : "Please ensure similarity graph is planar\nbefore continuing.";

                ImGui::Text(isPlanarText);
                if (ImGui::Button("CREATE VORONOI DIAGRAM"))
                {
                    if (rasterGraph.isPlanar())
                    {
                        create_voronoi_diagram = true;
                        voroni.constructHalfEdges();
                        voroni.setHalfEdgesVisibility();

                        voroni.setVoroniGraphTriangles();
                        voroni.setVoroniVisibleEdgeLines();
                        voroniRenderer.updateVBO(voroni.getVoroniGraphTriangles(), voroni.getVoroniGraphTriangleCount() * sizeof(float));
                        voroniRenderer.updateIBO(voroni.getVoroniGraphIndices(), voroni.getVoroniGraphTriangleCount() / 6);
                        visibleEdgesRenderer.updateVBO(voroni.getVoroniVisibleEdgeLines(), voroni.getVoroniVisibleEdgeCount() * 12 * sizeof(float));
                        visibleEdgesRenderer.updateIBO(voroni.getVoroniGraphIndices(), voroni.getVoroniVisibleEdgeCount() * 2);
                    }
                }
                ImGui::Text(create_voronoi_diagram ? "Voronoi diagram created" : "");

                if (ImGui::Button("SIMPLIFY VORONOI DIAGRAM"))
                {
                    simplify_voroni_diagram = true;
                    voroniRenderer.bindVAO();
                    voroni.simplifyVoroniGraph();
                    voroni.setVerticesSharpness();

                    voroni.setVoroniGraphTriangles();
                    voroniRenderer.updateVBO(voroni.getVoroniGraphTriangles(), voroni.getVoroniGraphTriangleCount() * sizeof(float));
                    voroniRenderer.updateIBO(voroni.getVoroniGraphIndices(), voroni.getVoroniGraphTriangleCount() / 6);
                    diagram_simplified = true;

                    voroni.setHalfEdgesVisibility();
                    voroni.setVoroniVisibleEdgeLines();
                    visibleEdgesRenderer.updateVBO(voroni.getVoroniVisibleEdgeLines(), voroni.getVoroniVisibleEdgeCount() * 12 * sizeof(float));
                    visibleEdgesRenderer.updateIBO(voroni.getVoroniGraphIndices(), voroni.getVoroniVisibleEdgeCount() * 2);
                }
                ImGui::Text(simplify_voroni_diagram ? "Voronoi diagram simplified." : "");
                ImGui::Checkbox("Show Voronoi Diagram", &show_voroni_diagram);

                ImGui::End();

                // SHOW VISIBLE EDGES WINDOW
                ImGui::Begin("VISIBLE EDGES");
                const char* tJunctionButtonText = show_t_junctions ? "HIDE T JUNCTIONS" : "SHOW T JUNCTIONS";
                if (ImGui::Button(tJunctionButtonText))
                {
                    show_t_junctions = !show_t_junctions;
                    voroni.setVoroniVisibleEdgeLines(show_t_junctions);

                    visibleEdgesRenderer.updateVBO(voroni.getVoroniVisibleEdgeLines(), voroni.getVoroniVisibleEdgeCount() * 12 * sizeof(float));
                    visibleEdgesRenderer.updateIBO(voroni.getVoroniGraphIndices(), voroni.getVoroniVisibleEdgeCount() * 2);
                }
                ImGui::Checkbox("Show Visible Edges", &show_visible_edges);
                ImGui::End();

                // VISIBLE EDGE OPTIMISATION WINDOW
                ImGui::Begin("OPTIMISATION");
                ImGui::Text("Number of Optimisations:");
                ImGui::InputInt("", &optimisationsToPerform, 1, 5000);
                if (ImGui::Button("OPTIMISE"))
                {
                    isOptimising = true;
                }
                ImGui::Text("");
                std::string progresstext = "Optimisation in progress... (" + std::to_string(optimisationsPerformed) + "/" + std::to_string(optimisationsToPerform) + ")";
                ImGui::Text(isOptimising ? progresstext.c_str() : "");
                ImGui::End();
            }

            ImGui::Begin("SPLINE INTERPOLATION");
            ImGui::Checkbox("Replace Visible Edges with Splines", &replace_visible_edges_with_splines);
            ImGui::End();

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