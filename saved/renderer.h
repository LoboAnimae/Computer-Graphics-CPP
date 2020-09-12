class Sphere
{
public:
    float *center, radius;
    Sphere(float *center1, float radius1)
    {
        center = center1;
        radius = radius1;
    }
    bool ray_intersect(float *orig, float *direction);
};

class Renderer
{
public:
    int height, width;
    unsigned char *pixels;
    Renderer(int y, int x)
    {
        height = y;
        width = x;
        pixels = new unsigned char[height * width * 3];
    }

    void point(int x, int y, unsigned char r, unsigned char g, unsigned char b);
    int *cast_ray(float *orig, float *direction, Sphere spehre);
    unsigned char *get_pixel_array();
};

