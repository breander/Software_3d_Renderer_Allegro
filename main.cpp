#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <vector>
#include <list>
#include <math.h>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>

using namespace std;

#define KEY_SEEN 1
#define KEY_RELEASED 2

// defaults
int sWidth = 1920;
int sHeight = 1080;
float setFps = 60.0;
bool debug = false;

struct vec3d
{
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;
};

struct triangle
{
    vec3d p[3];
    float color;
};

struct mesh
{
    vector<triangle> tris;

    bool LoadFromObjectFile(string sFileName)
    {
        ifstream f(sFileName);

        if (!f.is_open())
            return false;

        vector<vec3d> verts;

        while (!f.eof())
        {
            char line[128];
            f.getline(line, 128);

            std::stringstream s;
            s << line;

            char junk;

            if (line[0] == 'v')
            {
                vec3d v;
                s >> junk >> v.x >> v.y >> v.z;
                verts.push_back(v);
            }

            if (line[0] == 'f')
            {
                int f[3];
                s >> junk >> f[0] >> f[1] >> f[2];
                tris.push_back({verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]});
            }
        }

        f.close();

        return true;
    }
};

struct mat4x4
{
    float m[4][4] = {0};
};

vec3d Matrix_MultiplyVector(mat4x4 &m, vec3d &i)
{
    vec3d v;

    v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
    v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
    v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
    v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];

    return v;
}

mat4x4 Matrix_MakeIdentify()
{
    mat4x4 matrix;

    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;

    return matrix;
}

mat4x4 Matrix_MakeRotationX(float fAngleRad)
{
    mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[1][2] = sinf(fAngleRad);
    matrix.m[2][1] = -sinf(fAngleRad);
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

mat4x4 Matrix_MakeRotationY(float fAngleRad)
{
    mat4x4 matrix;
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][2] = sinf(fAngleRad);
    matrix.m[2][0] = -sinf(fAngleRad);
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

mat4x4 Matrix_MakeRotationZ(float fAngleRad)
{
    mat4x4 matrix;
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][1] = sinf(fAngleRad);
    matrix.m[1][0] = -sinf(fAngleRad);
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

mat4x4 Matrix_MakeTranslation(float x, float y, float z)
{
    mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    matrix.m[3][0] = x;
    matrix.m[3][1] = y;
    matrix.m[3][2] = z;
    return matrix;
}

mat4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
{
    float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
    mat4x4 matrix;
    matrix.m[0][0] = fAspectRatio * fFovRad;
    matrix.m[1][1] = fFovRad;
    matrix.m[2][2] = fFar / (fFar - fNear);
    matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matrix.m[2][3] = 1.0f;
    matrix.m[3][3] = 0.0f;
    return matrix;
}

mat4x4 Matrix_MultiplyMatrix(mat4x4 &m1, mat4x4 &m2)
{
    mat4x4 matrix;
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
    return matrix;
}

vec3d Vector_Add(vec3d &v1, vec3d &v2)
{
    return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

vec3d Vector_Sub(vec3d &v1, vec3d &v2)
{
    return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

vec3d Vector_Mul(vec3d &v1, float k)
{
    return {v1.x * k, v1.y * k, v1.z * k};
}

vec3d Vector_Div(vec3d &v1, float k)
{
    return {v1.x / k, v1.y / k, v1.z / k};
}

float Vector_DotProduct(vec3d &v1, vec3d &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float Vector_Length(vec3d &v)
{
    return sqrtf(Vector_DotProduct(v, v));
}

vec3d Vector_Normalise(vec3d &v)
{
    float l = Vector_Length(v);
    return {v.x / l, v.y / l, v.z / l};
}

vec3d Vector_CrossProduct(vec3d &v1, vec3d &v2)
{
    vec3d v;

    v.x = v1.y * v2.z - v1.z * v2.y;
    v.y = v1.z * v2.x - v1.x * v2.z;
    v.z = v1.x * v2.y - v1.y * v2.x;

    return v;
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        for (int x = 1; x < argc; x++)
        {
            //Debug
            if (strcmp(argv[x], "-debug") == 0)
            {
                debug = true;
            }

            if (strcmp(argv[x], "-width") == 0)
            {
                sWidth = std::stoi(argv[x + 1]);
            }

            if (strcmp(argv[x], "-height") == 0)
            {
                sHeight = std::stoi(argv[x + 1]);
            }

            if (strcmp(argv[x], "-fps") == 0)
            {
                setFps = std::stoi(argv[x + 1]);
            }
        }
    }

    if (!al_init())
    {
        printf("couldn't initialize allegro\n");
        return 1;
    }

    if (!al_install_keyboard())
    {
        printf("couldn't initialize keyboard\n");
        return 1;
    }

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / setFps);
    if (!timer)
    {
        printf("couldn't initialize timer\n");
        return 1;
    }

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    if (!queue)
    {
        printf("couldn't initialize queue\n");
        return 1;
    }

    //al_set_new_display_option(ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST);
    //al_set_new_display_refresh_rate((int)setFps);
    ALLEGRO_DISPLAY *disp = al_create_display(sWidth, sHeight);
    if (!disp)
    {
        printf("couldn't initialize display\n");
        return 1;
    }

    al_init_font_addon();
    al_init_ttf_addon();
    ALLEGRO_FONT *font = al_load_ttf_font("../OpenSans-Regular.ttf", 32, 0); //al_create_builtin_font();
    if (!font)
    {
        printf("couldn't initialize font\n");
        return 1;
    }

    if (!al_init_primitives_addon())
    {
        printf("couldn't initialize primitives\n");
        return 1;
    }

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    mesh meshCube;
    meshCube.LoadFromObjectFile("../teapot.obj");

    mat4x4 matProj = Matrix_MakeProjection(90.0f, (float)sHeight / (float)sWidth, 0.1f, 1000.0f);

    float fTheta;
    float zPos = 3.0f;
    vec3d vCamera;
    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    al_start_timer(timer);
    double old_time = al_get_time();
    while (1)
    {
        mat4x4 matRotZ, matRotX;
        al_wait_for_event(queue, &event);

        switch (event.type)
        {
        case ALLEGRO_EVENT_TIMER:
            if (key[ALLEGRO_KEY_UP])
                zPos -= 0.05f;
            if (key[ALLEGRO_KEY_DOWN])
                zPos += 0.05f;

            if (key[ALLEGRO_KEY_ESCAPE])
                done = true;

            for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
                key[i] &= KEY_SEEN;

            redraw = true;
            break;

        case ALLEGRO_EVENT_KEY_DOWN:
            key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
            break;
        case ALLEGRO_EVENT_KEY_UP:
            key[event.keyboard.keycode] &= KEY_RELEASED;
            break;
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
            break;
        }

        if (done)
            break;

        if (redraw && al_is_event_queue_empty(queue))
        {
            // get the fps
            double new_time = al_get_time();
            double elapsed_time = new_time - old_time;
            float fps = 1.0f / elapsed_time;
            old_time = new_time;

            // black the screen
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // draw the fps
            al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Fps: %i", (int)fps);

            // keep the rotation the same regardless of fps
            fTheta += 1.0f / setFps;
            matRotZ = Matrix_MakeRotationZ(fTheta * 0.5f);
            matRotX = Matrix_MakeRotationX(fTheta);

            mat4x4 matTrans;
            matTrans = Matrix_MakeTranslation(0.0f, 0.0f, zPos);

            mat4x4 matWorld;
            matWorld = Matrix_MakeIdentify();
            matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX);
            matWorld = Matrix_MultiplyMatrix(matWorld, matTrans);

            vector<triangle> vecTrianglesToRaster;

            for (auto tri : meshCube.tris)
            {
                triangle triProjected, triTransformed;

                triTransformed.p[0] = Matrix_MultiplyVector(matWorld, tri.p[0]);
                triTransformed.p[1] = Matrix_MultiplyVector(matWorld, tri.p[1]);
                triTransformed.p[2] = Matrix_MultiplyVector(matWorld, tri.p[2]);

                vec3d normal, line1, line2;
                line1 = Vector_Sub(triTransformed.p[1], triTransformed.p[0]);
                line2 = Vector_Sub(triTransformed.p[2], triTransformed.p[0]);

                normal = Vector_CrossProduct(line1, line2);
                normal = Vector_Normalise(normal);

                vec3d vCameraRay = Vector_Sub(triTransformed.p[0], vCamera);

                if (Vector_DotProduct(normal, vCameraRay) < 0.0f)
                {
                    // lighting
                    vec3d light_direction = {0.0f, 1.0f, -1.0f};
                    light_direction = Vector_Normalise(light_direction);

                    float dp = max(0.1f, Vector_DotProduct(light_direction, normal));

                    // convert 3d to 2d
                    triProjected.p[0] = Matrix_MultiplyVector(matProj, triTransformed.p[0]);
                    triProjected.p[1] = Matrix_MultiplyVector(matProj, triTransformed.p[1]);
                    triProjected.p[2] = Matrix_MultiplyVector(matProj, triTransformed.p[2]);

                    triProjected.p[0] = Vector_Div(triProjected.p[0], triProjected.p[0].w);
                    triProjected.p[1] = Vector_Div(triProjected.p[1], triProjected.p[1].w);
                    triProjected.p[2] = Vector_Div(triProjected.p[2], triProjected.p[2].w);

                    // X/Y are inverted so put them back
                    triProjected.p[0].x *= -1.0f;
                    triProjected.p[1].x *= -1.0f;
                    triProjected.p[2].x *= -1.0f;
                    triProjected.p[0].y *= -1.0f;
                    triProjected.p[1].y *= -1.0f;
                    triProjected.p[2].y *= -1.0f;

                    // Offset verts into visible normalised space
                    vec3d vOffsetView = {1, 1, 0};
                    triProjected.p[0] = Vector_Add(triProjected.p[0], vOffsetView);
                    triProjected.p[1] = Vector_Add(triProjected.p[1], vOffsetView);
                    triProjected.p[2] = Vector_Add(triProjected.p[2], vOffsetView);

                    triProjected.p[0].x *= 0.5f * (float)sWidth;
                    triProjected.p[0].y *= 0.5f * (float)sHeight;

                    triProjected.p[1].x *= 0.5f * (float)sWidth;
                    triProjected.p[1].y *= 0.5f * (float)sHeight;

                    triProjected.p[2].x *= 0.5f * (float)sWidth;
                    triProjected.p[2].y *= 0.5f * (float)sHeight;

                    triProjected.color = dp;

                    vecTrianglesToRaster.push_back(triProjected);
                }
            }

            sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle &t1, triangle &t2)
                 {
                     float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
                     float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
                     return z1 > z2;
                 });

            for (auto &triProjected : vecTrianglesToRaster)
            {
                al_draw_filled_triangle(triProjected.p[0].x, triProjected.p[0].y,
                                        triProjected.p[1].x, triProjected.p[1].y,
                                        triProjected.p[2].x, triProjected.p[2].y,
                                        al_map_rgb_f(triProjected.color, triProjected.color, triProjected.color));

                if (debug)
                {
                    al_draw_triangle(triProjected.p[0].x, triProjected.p[0].y,
                                     triProjected.p[1].x, triProjected.p[1].y,
                                     triProjected.p[2].x, triProjected.p[2].y,
                                     al_map_rgb_f(0, 0, 0), 2.0f);
                }
            }

            al_flip_display();
            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_shutdown_primitives_addon();
    al_shutdown_font_addon();
    al_shutdown_ttf_addon();
    al_destroy_event_queue(queue);

    return 0;
}