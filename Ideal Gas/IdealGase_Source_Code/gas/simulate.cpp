#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<vector>
#include<time.h>
#include<gsl/gsl_rng.h>
#include<iostream>
#include<imgui.h>
#include<imgui_impl_glfw.h>
#include<imgui_impl_opengl3.h>
#include<implot.h>
#include "gas.h"

class section
{
public:
    int x;
    int y;
    int s;

    section(int x_pos, int y_pos, int scale)
    {
        x = x_pos;
        y = y_pos;
        s = scale;
    }
};

void drawsquare(int x, int y, int s)
{   
    glBegin(GL_LINE_LOOP);
    glVertex2i(x, y);
    glVertex2i(x + s, y);
    glVertex2i(x + s, y - s);
    glVertex2i(x, y - s);
    glEnd();
}

bool insidebox(int x1, int y1, int x2, int y2, int s2)
{   
    bool yes = false;
    if (x1 > x2 && x1 < (x2 + s2) && y1 < y2 && (y1 > y2 - s2))
    {
        yes = true;
    }
    return yes;
}

const int n = 10000;

const int Width = 1400;
const int Height = 700;

int pinbox = 0;

float TempD = 300, Temp = 300;
float mass2 = 1;
float k2 = 0.00831;
float Pressure, ac_Pressure;

float Vavg_th = sqrt(8 * k2 * Temp / (3.1416 * mass2));

bool initialpos = true, initialvel = true, gravityOn = true, Pause = false;

std::vector <gas> b(n);

float color[3] = { 1, 1, 1 };

double lastime = 0.0;

void update()
{   
    double currtime = glfwGetTime();
    double delta = currtime - lastime;
    if (delta >= 0.016 && !Pause)
    {
        run(b);

        lastime = currtime;
    }
}

void reset()
{
    Temp = TempD;
    initialpos = !initialpos;
    initialvel = !initialvel;
}
int main(void)
{   
    section tl(350, 700, 350);
    section tr(700, 700, 350);
    section br(700, 350, 350);
    section bl(350, 350, 350);

    section df = tl;

    int ntl, ntr, nbl, nbr;
    double Ptl, Ptr, Pbr, Pbl;

    double mx = 1, my = 1;

    bool mouseIn = true;

    gsl_rng_env_setup();

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(Width, Height, "Ideal Gas", NULL, NULL);

    glfwSetWindowAttrib(window, GLFW_RESIZABLE, false);

    if (!window)
    {   
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glMatrixMode(GL_PROJECTION);
    glOrtho(0, Width, 0, Height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

   
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font1 = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", 20);
    ImFont* font2 = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Black.ttf", 20);
    ImGui_ImplOpenGL3_Init("#version 100");
    ImGui::StyleColorsDark();

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0., 0.021, 0.143, 1));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.231, 0.094, 0.373, 1));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.231, 0.194, 0.373, 1));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.531, 0.394, 0.573, 1));
    ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.731, 0.594, 0.873, 1));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.996, 0.761, 0.376, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.896, 0.661, 0.276, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 0.861, 0.476, 1));
    ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, ImVec4(0.531, 0.394, 0.573, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {   
        if (initialpos)
        {
            int it = 0;
            for (gas& i : b)
            {
                srand(it);
                i.p(350 + rand() % 700, rand() % 700);

                it += 1;

            }
            initialpos = !initialpos;
        }

        gravity(gravityOn);

        Pressure = n * k2 * Temp / 4;
        ac_Pressure = pinbox * k2 * Temp;

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        glPointSize(b[0].size);
        glBegin(GL_LINES);
        glVertex2f(1050, 700);
        glVertex2f(1050, 0);
        glEnd();

        glBegin(GL_LINES);
        glVertex2f(350, 700);
        glVertex2f(350, 0);
        glEnd();

      /*  glBegin(GL_LINES);
        glVertex2f(350, 699);
        glVertex2f(1050, 699);
        glEnd();

        glBegin(GL_LINES);
        glVertex2f(350, 0);
        glVertex2f(1050, 0);
        glEnd();*/

        pinbox = 0;
        ntl = 0;
        ntr = 0;
        nbl = 0;
        nbr = 0;

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        { 
            
            glfwGetCursorPos(window, &mx, &my);
                
        }

        if (mx > tl.x && mx < tl.x + tl.s && my > 0 && my < tl.y - tl.s) { df = tl; }
        else if (mx > tr.x && mx < tr.x + tr.s && my > 0 && my < tr.y - tr.s) { df = tr; }
        else if (mx > bl.x && mx < bl.x + bl.s && my > bl.y && my < bl.y + bl.s) { df = bl; }
        else if (mx > br.x && mx < br.x + br.s && my > br.y && my < br.y + br.s) { df = br;  }
        
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && mouseIn)

        {
            mx = 0; my = 0;
            mouseIn = false;
            
        }
        if (mx > tl.x && mx < tr.x + tr.s && my > 0 && my < tl.y) 
        { 
            mouseIn = true; 
        }

        if (mouseIn) drawsquare(df.x, df.y, df.s);

        glBegin(GL_POINTS); 
       
        for (gas& i : b)
        {   
            i.Temp = Temp;
            i.draw();
                        
            if (insidebox(i.x, i.y, df.x, df.y, df.s))
            {

                if (mouseIn) i.setColor(0.996, 0.761, 0.376);

                else i.setColor(color[0], color[1], color[2]);
                
                pinbox += 1;
            }
            else
            {
                i.setColor(color[0], color[1], color[2]);
            }

            if (insidebox(i.x, i.y, tl.x, tl.y, tl.s)) 
            {
                ntl += 1;
            }
            if (insidebox(i.x, i.y, tr.x, tr.y, tr.s))
            {
                ntr += 1;
            }
            if (insidebox(i.x, i.y, bl.x, bl.y, bl.s))
            {
                nbl += 1;
            }
            if (insidebox(i.x, i.y, br.x, br.y, br.s))
            {
                nbr += 1;
            }

        }
        
        
        glEnd();

        Ptl = ntl * k2 * Temp;
        Ptr = ntr * k2 * Temp;
        Pbl = nbl * k2 * Temp;
        Pbr = nbr * k2 * Temp;

        glColor3f(color[0], color[1], color[2]);

        update();
        ImGui::SetNextWindowPos(ImVec2(1050, 0));
        ImGui::SetNextWindowSize(ImVec2(350, Height));
        ImGui::Begin("Properties", NULL, ImGuiWindowFlags_NoTitleBar);
        
        ImGui::Text("");
        ImGui::Text("Type of system: Isolated");
        ImGui::Text("");

        ImGui::Text("Temperature");
        ImGui::InputFloat("Kelvin", &TempD);
        if (TempD < 100) TempD = 100;

        if (initialvel)
        {
          
            for (gas& i : b)
            {
                
                i.setVel();

            }
            initialvel = !initialvel;
        }

        ImGui::Text("");
        ImGui::Text("Colour");
        ImGui::ColorEdit3("", color);
        ImGui::Text("");
        ImGui::Checkbox("Gravity", &gravityOn);
        ImGui::Text("");

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
        if (ImGui::Button("Restart", ImVec2(330, 40)))
        {
            reset();
        }
        ImGui::PopStyleColor();
        
        ImGui::Text(""); 
        
        if (ImPlot::BeginPlot("##Histograms")) 
        {
            ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
            ImPlot::SetNextFillStyle(ImVec4(0.996, 0.761, 0.376, 1), 0.5f);
            ImPlot::PlotHistogram("", getV(), 100, 20, 1.0, ImPlotRange(), ImPlotHistogramFlags_Density);
            ImPlot::EndPlot();
        }
        ImGui::Text("                     Velocity Distribution");
        ImGui::End();
        ImGui::SetNextWindowSize(ImVec2(348, Height));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        
        ImGui::Begin("Others", NULL, ImGuiWindowFlags_NoTitleBar);
        ImGui::PushFont(font2);
        ImGui::Text("Expected Values");
        ImGui::PopFont();
        ImGui::Text("Average velocity: %f", sqrt(8 * k2 * Temp / (3.1416 * mass2)));
        ImGui::Text("Pressure in the square: %f", Pressure);
        ImGui::Text("Total pressure: %f", n * k2 * Temp);
        ImGui::Text("");
        ImGui::Text("");
        ImGui::PushFont(font2);
        ImGui::Text("Actual Values");
        ImGui::PopFont();
        ImGui::Text("Average velocity: %f", getVavg());
        ImGui::Text("Pressure in the square: %f", ac_Pressure);
        ImGui::Text("Total pressure: %f", Ptr + Ptl + Pbl + Pbr);
        ImGui::Text(""); ImGui::Text(""); ImGui::Text(""); ImGui::Text(""); ImGui::Text("");
        ImGui::Text(""); ImGui::Text(""); ImGui::Text(""); ImGui::Text(""); ImGui::Text("");
        ImGui::Text(""); ImGui::Text(""); ImGui::Text(""); ImGui::Text(""); ImGui::Text(""); 
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.731, 0.594, 0.873, 1));
        ImGui::Text("Gas constant = 0.00831");
        ImGui::Text("Mass of a particle = 1"); 
        ImGui::Text("Volume = 1");
        ImGui::PopStyleColor();
        ImGui::End();
        ImGui::Render();
       
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}