/**
* On dictionaries, it's always x = [0], y = [1], z = [2]
* On colors, it's always r = [2], g = [1], b = [0]
**/

#include <iostream>
#include <vector>
#include <cmath>
#include <stdarg.h>
#include <iostream>
#include <fstream>

#define BYTES_PER_PIXEL 3
#define FILE_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define PI 3.141516

double maxf(double arg1, double arg2)
{
   if (arg1 >= arg2)
      return arg1;
   else
      return arg2;
}

struct currentIntercept
{
   double distance, point[3], normal[3];
} currentInterceptValues;

struct currentMaterial
{
   unsigned char diffuse[3];
   double albedo[3], spec;
} currentMaterialValues;

struct currentLight
{
   double position[3], intensity;
} currentLightValues;

struct currentSpecular
{
   unsigned char specular[3];
   unsigned char backgroundColor[3] = {255, 255, 255};
} currentSpecularValues;

struct materialstruct
{
   unsigned char diffuse[3];
   double albedo[3];
   int spec;
};

double *sum(double *v0, double *v1)
{
   /**
    * How to receive:
    * int * [VARIABLE] = FUNCTION
    **/
   double *vector1 = v0, *vector2 = v1; // This is the memory address of the x value
   static double result[3];
   int i;
   for (i = 0; i < 3; i++)
   {
      result[i] = *(vector1 + i) + *(vector2 + i);
   }
   return result;
}

double *sub(double *v0, double *v1)
{
   double *vector1 = v0, *vector2 = v1; // This is the memory address of the x value
   static double result[3];
   int i;
   for (i = 0; i < 3; i++)
   {
      result[i] = *(vector1 + i) - *(vector2 + i);
   }
   return result;
}

double *mul(double *v0, double k)
{
   double *vector1 = v0; // This is the memory address of the x value
   static double result[3];
   int i;
   for (i = 0; i < 3; i++)
   {
      result[i] = *(vector1 + i) * k;
   }
   return result;
}

double dot(double *v0, double *v1)
{
   double vector1[3] = {*(v0), *(v0 + 1), *(v0 + 2)}, vector2[3] = {*(v1), *(v1 + 1), *(v1 + 2)}; // This is the memory address of the x value
   double result = (vector1[0] * vector2[0]) + (vector1[1] * vector2[1]) + (vector1[2] * vector2[2]);
   return result;
}

double *cross(double *v0, double *v1)
{
   double *vector1 = v0, *vector2 = v1, i;
   static double result[3];
   result[0] = (*(vector1 + 1) * *(vector2 + 2)) - (*(vector1 + 2) * *(vector2 + 1));
   result[1] = (*(vector1 + 2) * *(vector2 + 0)) - (*(vector1 + 0) * *(vector2 + 2));
   result[2] = (*(vector1 + 0) * *(vector2 + 1)) - (*(vector1 + 1) * *(vector2 + 0));

   return result;
}

double length(double *v0)
{
   double *pointer = v0;
   double x = *(pointer), y = *(pointer + 1), z = *(pointer + 2);
   x = pow(x, 2);
   y = pow(y, 2);
   z = pow(z, 2);
   double result = pow((x + y + z), 0.5);
   return result;
}

double *norm(double *v0)
{
   double size = length(v0);
   static double result[3];
   if (!size)
   {
      for (int i = 0; i < 3; i++)
      {
         result[i] = 0;
      }
      return result;
   }
   double *pointer = v0;
   for (int i = 0; i < 3; i++)
   {
      result[i] = *(pointer + i) / size;
   }
   return result;
}

double *reflect(double *I, double *N)
{
   static double *result = norm(sub(I, mul(N, 2 * dot(I, N))));
   return result;
}

class Intersect
{
public:
   double distance, point, normal;
};

class Light
{
public:
   double *position, intensity;
};

class Sphere
{

public:
   double *center, radius;
   materialstruct material;

   Sphere(double *center1, double radius1, materialstruct material1)
   {
      center = center1;
      radius = radius1;
      material = material1;
   }
   Sphere() {}
   bool ray_intersect(double *orig, double *direction)
   {
      double *L = sub(center, orig);
      double tca = dot(L, direction);
      double l = length(L);
      double d2 = pow(l, 2) - pow(tca, 2);
      if (d2 > pow(radius, 2))
      {
         return false;
      }

      double thc = sqrtf(pow(radius, 2) - d2);
      double t0 = tca - thc;
      double t1 = tca + thc;

      if (t0 < 0)
      {
         t0 = t1;
      }
      if (t0 < 0)
      {
         return false;
      }

      double *hit = sum(orig, mul(direction, t0));
      double *normal = norm(sub(hit, center));
      currentInterceptValues.distance = t0;
      currentInterceptValues.point[0] = *hit;
      currentInterceptValues.point[1] = *(hit + 1);
      currentInterceptValues.point[2] = *(hit + 2);
      currentInterceptValues.normal[0] = *(normal);
      currentInterceptValues.normal[1] = *(normal + 1);
      currentInterceptValues.normal[2] = *(normal + 2);
      return true;
   }
};

struct scenestruct
{
   Sphere objects[3];
} currentSceneValues;

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

   void generateBMP(char *imageFileName)
   {

      // for (int i = 0; i < width * height * 3; i += 2)
      // {
      //    std::cout << (int)pixels[i] <<" " << (int)pixels[i + 1] << " " << (int)pixels[i + 2] << std::endl;
      // }

      unsigned char finalpixels[height][width][3];
      for (int i = 0, counter = 0; i < height; i++)
      {
         for (int j = 0; j < width; j++)
         {
            for (int k = 0; k < 3; k++, counter++)
            {
               finalpixels[i][j][k] = pixels[counter];
            }
         }
      }

      generateBitmapImage((unsigned char *)finalpixels, imageFileName);
   }

   void generateBitmapImage(unsigned char *image, char *imageFileName)
   {
      int widthInBytes = width * BYTES_PER_PIXEL;

      unsigned char padding[3] = {0, 0, 0};
      int paddingSize = (4 - (widthInBytes) % 4) % 4;

      int stride = (widthInBytes) + paddingSize;

      FILE *imageFile = fopen(imageFileName, "wb");

      unsigned char *fileHeader = createBitmapFileHeader(height, stride);
      fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

      unsigned char *infoHeader = createBitmapInfoHeader(height, width);
      fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

      int i;
      for (i = 0; i < height; i++)
      {
         fwrite(image + (i * widthInBytes), BYTES_PER_PIXEL, width, imageFile);
         fwrite(padding, 1, paddingSize, imageFile);
      }

      fclose(imageFile);
   }

   unsigned char *createBitmapInfoHeader(int height, int width)
   {
      static unsigned char infoHeader[] = {
          0, 0, 0, 0, /// header size
          0, 0, 0, 0, /// image width
          0, 0, 0, 0, /// image height
          0, 0,       /// number of color planes
          0, 0,       /// bits per pixel
          0, 0, 0, 0, /// compression
          0, 0, 0, 0, /// image size
          0, 0, 0, 0, /// horizontal resolution
          0, 0, 0, 0, /// vertical resolution
          0, 0, 0, 0, /// colors in color table
          0, 0, 0, 0, /// important color count
      };

      infoHeader[0] = (unsigned char)(INFO_HEADER_SIZE);
      infoHeader[4] = (unsigned char)(width);
      infoHeader[5] = (unsigned char)(width >> 8);
      infoHeader[6] = (unsigned char)(width >> 16);
      infoHeader[7] = (unsigned char)(width >> 24);
      infoHeader[8] = (unsigned char)(height);
      infoHeader[9] = (unsigned char)(height >> 8);
      infoHeader[10] = (unsigned char)(height >> 16);
      infoHeader[11] = (unsigned char)(height >> 24);
      infoHeader[12] = (unsigned char)(1);
      infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL * 8);

      return infoHeader;
   }
   unsigned char *createBitmapFileHeader(int height, int stride)
   {
      int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

      static unsigned char fileHeader[] = {
          0, 0,       /// signature
          0, 0, 0, 0, /// image file size in bytes
          0, 0, 0, 0, /// reserved
          0, 0, 0, 0, /// start of pixel array
      };

      fileHeader[0] = (unsigned char)('B');
      fileHeader[1] = (unsigned char)('M');
      fileHeader[2] = (unsigned char)(fileSize);
      fileHeader[3] = (unsigned char)(fileSize >> 8);
      fileHeader[4] = (unsigned char)(fileSize >> 16);
      fileHeader[5] = (unsigned char)(fileSize >> 24);
      fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

      return fileHeader;
   }

   void point(int x, int y, int r, int g, int b)
   {
      try
      {
         pixels[x * y] = b;
         pixels[x * y + 1] = g;
         pixels[x * y + 2] = r;
      }
      catch (int e)
      {
      }
   };

   bool scene_intersect(double *orig, double *direction)
   {
      double zbuffer = INFINITY;
      materialstruct material;
      bool returnstatus = false;

      for (int i = 0; i < 3; i++)
      {

         bool hit = currentSceneValues.objects[i].ray_intersect(orig, direction);
         if (hit)
         {
            if (currentInterceptValues.distance < zbuffer)
            {
               zbuffer = currentInterceptValues.distance;
               material = currentSceneValues.objects[i].material;
               currentMaterialValues.albedo[0] = material.albedo[0];
               currentMaterialValues.albedo[1] = material.albedo[1];
               currentMaterialValues.diffuse[0] = material.diffuse[0];
               currentMaterialValues.diffuse[1] = material.diffuse[1];
               currentMaterialValues.diffuse[2] = material.diffuse[2];
               currentMaterialValues.spec = material.spec;
               returnstatus = true;
            }
         }
      }
      return returnstatus;
   }

   void cast_ray(double *orig, double *direction)
   {
      if (!scene_intersect(orig, direction))
      {
         currentSpecularValues.specular[0] = currentSpecularValues.backgroundColor[0];
         currentSpecularValues.specular[1] = currentSpecularValues.backgroundColor[1];
         currentSpecularValues.specular[2] = currentSpecularValues.backgroundColor[2];
      }
      // TODO: ADD LIGHT TO THE CONSTRUCTOR?

      double *light_temp = norm(sub(currentLightValues.position, currentInterceptValues.point));
      double light_dir[3] = {*light_temp, *(light_temp + 1), *(light_temp + 2)};
      double intensity;
      if (dot(light_dir, currentInterceptValues.normal) > 0)
      {
         printf("reeeeeeeeeeeeeeeeeeeeeeeeeeeee");
      }
      intensity = -dot(light_dir, currentInterceptValues.normal);
      intensity = maxf(0, intensity);
      intensity *= currentLightValues.intensity;
      double *reflection = reflect(light_dir, currentInterceptValues.normal);

      double specular_intensity /**= currentLightValues.intensity * (pow(maxf(0, -dot(reflection, direction)), currentMaterialValues.spec))**/;
      specular_intensity = -dot(reflection, direction);
      specular_intensity = maxf(0, specular_intensity);
      specular_intensity = (specular_intensity, currentMaterialValues.spec);
      specular_intensity *= currentLightValues.intensity;
      double diffuse[3];
      diffuse[0] = (int)currentMaterialValues.diffuse[0] * intensity * currentMaterialValues.albedo[0];
      diffuse[1] = (int)currentMaterialValues.diffuse[1] * intensity * currentMaterialValues.albedo[0];
      diffuse[2] = (int)currentMaterialValues.diffuse[2] * intensity * currentMaterialValues.albedo[0];
      unsigned char white = 255;
      currentSpecularValues.specular[0] = white * specular_intensity * currentMaterialValues.albedo[1];
      currentSpecularValues.specular[1] = white * specular_intensity * currentMaterialValues.albedo[1];
      currentSpecularValues.specular[2] = white * specular_intensity * currentMaterialValues.albedo[1];
      currentSpecularValues.specular[0] += diffuse[0];
      currentSpecularValues.specular[1] += diffuse[1];
      currentSpecularValues.specular[2] += diffuse[2];
   }

   void render()
   {
      int fov = (int)(PI / 2);
      for (int y = 0, counter = 0; y < height; y++)
      {
         for (int x = 0; x < width; x++, counter += 3)
         {
            double newfov = (double)fov;
            double i = (2 * (x + 0.5) / width - 1) * tan(newfov / 2) * width / height;
            double j = (2 * (y + 0.5) / height - 1) * tan(newfov / 2);
            double dirnorm[3] = {i, j, -1};
            double direction[3] = {-0.010379, -0.2930086, -0.956058};
            unsigned char color[3];
            double temp[3] = {};
            cast_ray(temp, direction);
            color[0] = currentSpecularValues.specular[0];
            color[1] = currentSpecularValues.specular[1];
            color[2] = currentSpecularValues.specular[2];
            unsigned char red = color[2], green = color[1], blue = color[0];
            pixels[counter] = blue;      // TODO: BLUE VALUE FOR BMP
            pixels[counter + 1] = green; // TODO: GREEN VALUE FOR BMP
            pixels[counter + 2] = red;   // TODO: RED VALUE FOR BMP
         }
      }
   }

   unsigned char *return_pixel_array()
   {
      return pixels;
   }
};

int main()
{
   Renderer r(1000, 1000);
   currentLightValues.intensity = 1.5;
   currentLightValues.position[0] = -20;
   currentLightValues.position[1] = 20;
   currentLightValues.position[2] = 20;

   currentSpecularValues.backgroundColor[0] = 50;
   currentSpecularValues.backgroundColor[1] = 50;
   currentSpecularValues.backgroundColor[2] = 200;
   materialstruct ivory;
   ivory.diffuse[0] = (unsigned char)100;
   ivory.diffuse[1] = (unsigned char)100;
   ivory.diffuse[2] = (unsigned char)80;

   ivory.albedo[0] = 0.6;
   ivory.albedo[1] = 0.3;

   ivory.spec = 50;

   materialstruct rubber;
   rubber.diffuse[0] = (unsigned char)80;
   rubber.diffuse[1] = (unsigned char)0;
   rubber.diffuse[2] = (unsigned char)0;
   rubber.albedo[0] = 0.9;
   rubber.albedo[1] = 0.1;
   rubber.spec = 10;

   double sphereCenter1[3] = {0, 0, 0};
   double sphereCenter2[3] = {-2, -1, -12};
   double sphereCenter3[3] = {1, 1, -8};
   Sphere s1(sphereCenter1, 1.5, ivory);
   Sphere s2(sphereCenter2, 1, rubber);
   Sphere s3(sphereCenter3, 1, rubber);
   // TODO: CHANGE THIS TO ACTUAL VECTOR
   currentSceneValues.objects[0] = s1;
   currentSceneValues.objects[1] = s2;
   currentSceneValues.objects[2] = s3;
   r.render();
   char *imageFileName = (char *)"out3.bmp";
   r.generateBMP(imageFileName);
   printf("Finished.\n");
}
