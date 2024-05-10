#include "lodepng.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_COMPONENTS 1000
struct pixel {
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char alpha;
};

int i, j;
char* load_png_file(const char *filename, int *width, int *height) {
    unsigned char *image = NULL;
    int error = lodepng_decode32_file(&image, width, height, filename);
    if (error) {
        printf("error %u: %s\n", error, lodepng_error_text(error));
        return NULL;
    }
    return (image);
}
void save_png_file(const char *filename, struct pixel* pixels, int width, int height) {
    unsigned error = lodepng_encode32_file(filename, (unsigned char*)pixels, width, height);
    if (error) {
        printf("error %u: %s\\n", error, lodepng_error_text(error));
    }
}
void grayscale(const char *filename) {
    int w = 0, h = 0;
    int i;
    unsigned char *picture = load_png_file(filename, &w, &h);
    if (picture == NULL) {
        printf("I can't read the picture %s. Error.\n", filename);
        return;
    }
    struct pixel* grayscale_image = (struct pixel*)malloc(w * h * sizeof(struct pixel));
    for (i = 0; i < h * w; i++) {
        unsigned char grayscale_value = (unsigned char)((picture[i * 4] + picture[i * 4 + 1] + picture[i * 4 + 2]) / 3);
        grayscale_image[i].R = grayscale_value;
        grayscale_image[i].G = grayscale_value;
        grayscale_image[i].B = grayscale_value;
        grayscale_image[i].alpha = picture[i * 4 + 3];
    }
    save_png_file("C:\\Users\\Alexandr\\CLionProjects\\SERMENTMYIMG\\img\\grayscale.png", grayscale_image, w, h);

    free(picture);
    free(grayscale_image);
}
void process_image_with_sobel(const char *filename) {
    int w = 0, h = 0;
    unsigned char *picture = load_png_file(filename, &w, &h);

    if (picture == NULL) {
        printf("I can't read the picture %s. Error.\n", filename);
        return;
    }
    struct pixel* sobel_image = (struct pixel*)malloc(w * h * sizeof(struct pixel));

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            // sobel
            int gx = 0, gy = 0;
            // gradient X
            gx += (picture[(y-1)*w*4 + (x+1)*4] - picture[(y-1)*w*4 + (x-1)*4]) * 1;
            gx += (picture[y*w*4 + (x+1)*4] - picture[y*w*4 + (x-1)*4]) * 2;
            gx += (picture[(y+1)*w*4 + (x+1)*4] - picture[(y+1)*w*4 + (x-1)*4]) * 1;
            // gradient Y
            gy += (picture[(y+1)*w*4 + (x-1)*4] - picture[(y-1)*w*4 + (x-1)*4]) * 1;
            gy += (picture[(y+1)*w*4 + x*4] - picture[(y-1)*w*4 + x*4]) * 2;
            gy += (picture[(y+1)*w*4 + (x+1)*4] - picture[(y-1)*w*4 + (x+1)*4]) * 1;
            int magnitude = (int)sqrt(gx*gx + gy*gy);
            magnitude = fmin(fmax(magnitude, 0), 255);
            sobel_image[y*w + x].R = magnitude;
            sobel_image[y*w + x].G = magnitude;
            sobel_image[y*w + x].B = magnitude;
            sobel_image[y*w + x].alpha = 255;
        }
    }
    save_png_file("CC:\\Users\\Alexandr\\CLionProjects\\SERMENTMYIMG\\img\\sobelimage.png", sobel_image, w, h);

    free(picture);
    free(sobel_image);
}

// test+++++++++++++++++++++++++++++
typedef struct Node {
    unsigned char r, g, b, a;
    struct Node *up, *down, *left, *right;
    struct Node *parent;
    int rank;
} Node;


Node* find_root(Node* nodes, Node* x) {
    if (x->parent != x) {
        x->parent = find_root(nodes, x->parent);
    }
    return x->parent;
}

void union_sets(Node* nodes, Node* x, Node* y, double epsilon) {
    if (x->r < 40 && y->r < 40) {
        return;
    }
    Node* px = find_root(nodes, x);
    Node* py = find_root(nodes, y);

    double color_difference = sqrt(pow(x->r - y->r, 2) + pow(x->g - y->g, 2) + pow(x->b - y->b, 2));
    if (px != py && color_difference < epsilon) {
        if (px->rank > py->rank) {
            py->parent = px;
        } else {
            px->parent = py;
            if (px->rank == py->rank) {
                py->rank++;
            }
        }
    }
}

Node* create_image_graph(const char *filename, int *width, int *height) {
    unsigned char *image = NULL;
    int error = lodepng_decode32_file(&image, width, height, filename);
    if (error) {
        printf("error %u: %s\\n", error, lodepng_error_text(error));
        return NULL;
    }

    Node* nodes = malloc(*width * *height * sizeof(Node));
    if (!nodes) {
        free(image);
        return NULL;
    }

    for (unsigned y = 0; y < *height; ++y) {
        for (unsigned x = 0; x < *width; ++x) {
            Node* node = &nodes[y * *width + x];
            unsigned char* pixel = &image[(y * *width + x) * 4];
            node->r = pixel[0];
            node->g = pixel[1];
            node->b = pixel[2];
            node->a = pixel[3];
            node->up = y > 0 ? &nodes[(y - 1) * *width + x] : NULL;
            node->down = y < *height - 1 ? &nodes[(y + 1) * *width + x] : NULL;
            node->left = x > 0 ? &nodes[y * *width + (x - 1)] : NULL;
            node->right = x < *width - 1 ? &nodes[y * *width + (x + 1)] : NULL;
            node->parent = node;
            node->rank = 0;
        }
    }

    free(image);
    return nodes;
}

void segment_image(Node* nodes, int width, int height, double epsilon) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Node* node = &nodes[y * width + x];
            if (node->up) {
                union_sets(nodes, node, node->up, epsilon);
            }
            if (node->down) {
                union_sets(nodes, node, node->down, epsilon);
            }
            if (node->left) {
                union_sets(nodes, node, node->left, epsilon);
            }
            if (node->right) {
                union_sets(nodes, node, node->right, epsilon);


            }
        }
    }
}

void free_image_graph(Node* nodes) {
    free(nodes);
}

void color_components_and_count(Node* nodes, int width, int height) {
    unsigned char* output_image = malloc(width * height * 4 * sizeof(unsigned char));
    int* component_sizes = calloc(width * height, sizeof(int));
    int total_components = 0;

    srand(time(NULL));
    for (int i = 0; i < width * height; i++) {
        Node* p = find_root(nodes, &nodes[i]);
        if (p == &nodes[i]) {
            if (component_sizes[i] < 3) {
                p->r = 0;
                p->g = 0;
                p->b = 0;
            } else {
                p->r = rand() % 256;
                p->g = rand() % 256;
                p->b = rand() % 256;
            }

        }
        output_image[4 * i + 0] = p->r;
        output_image[4 * i + 1] = p->g;
        output_image[4 * i + 2] = p->b;
        output_image[4 * i + 3] = 255;
        component_sizes[p - nodes]++;
    }

    char *output_filename ="C:\\Users\\Alexandr\\CLionProjects\\SERMENTMYIMG\\img\\coloring.png";
    lodepng_encode32_file(output_filename, output_image, width, height);



    free(output_image);
    free(component_sizes);
}

//test++++++++++++++++++++++++++++++++++++



int main() {
    grayscale("C:\\Users\\Alexandr\\CLionProjects\\SERMENTMYIMG\\img\\startimg.png");
    process_image_with_sobel("C:\\Users\\Alexandr\\CLionProjects\\SERMENTMYIMG\\img\\grayscale.png");

    int width, height;
    char *filename = "C:\\Users\\Alexandr\\CLionProjects\\SERMENTMYIMG\\img\\sobelimage.png";
    Node* nodes = create_image_graph(filename, &width, &height);
    if (!nodes) {
        return 1;
    }

    double epsilon = 60.0;
    segment_image(nodes, width, height, epsilon);
    color_components_and_count(nodes, width, height);

    free_image_graph(nodes);

    return 0;
}