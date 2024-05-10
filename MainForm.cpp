//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainForm.h"
#include "lodepng.h"
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_COMPONENTS 1000
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{

}
//------------------------------------------------------------------------------------
Node* create_image_graph(const char *filename, unsigned int *width, unsigned int *height)
{
	unsigned char *image = NULL;
	unsigned int var1, var2;
	Node* nodes;
	int error = lodepng_decode32_file(&image, width, height, filename);
	if (error)
	{
		printf("error %u: %s\\n", error, lodepng_error_text(error));
		return NULL;
	}


	var1 =  sizeof(Node);
	nodes = (Node*)malloc(*width * *height *var1 );

	//nodes = (Node)var2;
	if (!nodes)
	{
		free(image);
		return NULL;
	}

	for (unsigned y = 0; y < *height; ++y)
	{
		for (unsigned x = 0; x < *width; ++x)
		{
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
//----------------------------------------------------------------------

void segment_image(Node* nodes, int width, int height, double epsilon) {
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Node* node = &nodes[y * width + x];
			if (node->up)
			{
				Form1->union_sets(nodes, node, node->up, epsilon);
			}
			if (node->down)
			{
				Form1->union_sets(nodes, node, node->down, epsilon);
			}
			if (node->left)
			{
				Form1->union_sets(nodes, node, node->left, epsilon);
			}
			if (node->right)
			{
				Form1->union_sets(nodes, node, node->right, epsilon);


			}
		}
	}
}
//---------------------------------------------------------------
void __fastcall TForm1::FormActivate(TObject *Sender)
{
   SetCurrentDir("../../");
   OpenDialog1->InitialDir = GetCurrentDir();
   PathToInitDir = OpenDialog1->InitialDir ;
   DirToImage    = PathToInitDir + "\\img\\";

}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormResize(TObject *Sender)
{
  GroupBox1->Width = Form1->ClientWidth / 3 - 30;
  Button1->Top     = GroupBox1->Height / 2;
  Button1->Left    = GroupBox1->Left + GroupBox1->Width +4;
  GroupBox2->Left  = Button1->Left + Button1->Width + 4;
  GroupBox2->Width = GroupBox1->Width;

  Label1->Top      =  Button1->Top -40;
  Label1->Left     = GroupBox2->Left + GroupBox2->Width +4;
  Edit1->Top       = Label1->Top + Label1->Height +2;
  Edit1->Left      = Label1->Left;
  Button2->Top     = Button1->Top ;
  Button2->Left    = GroupBox2->Left + GroupBox2->Width +4;
  GroupBox3->Left  = Button2->Left + Button2->Width + 4;
  GroupBox3->Width = GroupBox1->Width;

}
//---------------------------------------------------------------------------
void __fastcall TForm1::N2Click(TObject *Sender)
{
   if(Sender ==  N2 || Sender ==  Image1)
   {
	  if(OpenDialog1->Execute())
	  {
		Image1->Picture->LoadFromFile(OpenDialog1->FileName );
		Button1->Enabled = true;
	  }
   }

   if(Sender ==  N3 || Sender ==  ToolButton4)
   {
	 Image1->Picture->LoadFromFile( PathToInitDir + "//открыть_73704.png");
	 Image2->Picture->LoadFromFile( PathToInitDir + "//ZagruziFoto.png");
	 Image3->Picture->LoadFromFile( PathToInitDir + "//Vtikay.png");
	 Button1->Enabled = false;
	 Button2->Enabled = false;
   }

	if(Sender ==  C1)
	  Form1->Close();

}
//---------------------------------------------------------------------------
 Node* find_root(Node* nodes, Node* x)
{

	if (x->parent != x)
	{
		x->parent = find_root(nodes, x->parent);
	}
	return x->parent;
}
//-------------------------------------------------------
void color_components_and_count(Node* nodes, int width, int height)
{
	unsigned char* output_image = (unsigned char*)malloc(width * height * 4 * sizeof(unsigned char));
	int* component_sizes = (int*)calloc(width * height, sizeof(int));
	int total_components = 0;
	char FilName[200];
	StrPCopy(FilName,Form1->DirToImage+"coloring.png");


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

	char *output_filename =FilName;
	lodepng_encode32_file(output_filename, output_image, width, height);



	free(output_image);
	free(component_sizes);
}
//-------------------------------------------------------------------------------------
void free_image_graph(Node* nodes)
{
	free(nodes);
}
//-----------------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
  unsigned int width, height;
  char FilName[200];
  double epsilon = 60.0;

  StrPCopy(FilName,DirToImage+"grayscale.png");

  if(Sender == Button1)
  {
	StrPCopy(FilName,OpenDialog1->FileName);
	grayscale(FilName);
	StrPCopy(FilName,DirToImage+"grayscale.png");
	Image2->Picture->LoadFromFile(FilName);
	Button2->Enabled = true;
	Label1->Enabled  = Button2->Enabled ;
	Edit1->Enabled   = Button2->Enabled ;
  }
  if(Sender == Button2)
  {
	StrPCopy(FilName,DirToImage+"grayscale.png");
	process_image_with_sobel(FilName);
	epsilon = StrToFloat(Edit1->Text);
	StrPCopy(FilName,DirToImage+"sobelimage.png");
	Node* nodes = create_image_graph(FilName, &width, &height);
	//if (!nodes)
	//{
	//	return 1;
	//}


	segment_image(nodes, width, height, epsilon);
	color_components_and_count(nodes, width, height);

	free_image_graph(nodes);

	//return 0;
  }
}
//--------------------------------------------------------------------------------
//int i, j;
char* TForm1::load_png_file(const char *filename,unsigned int *width,unsigned int *height)
{
    unsigned char *image = NULL;
	int error = lodepng_decode32_file(&image, width, height, filename);
	if (error) {
        printf("error %u: %s\n", error, lodepng_error_text(error));
        return NULL;
    }
    return (image);
}
//------------------------------------------
void TForm1::save_png_file(const char *filename, struct pixel* pixels, int width, int height)
{
	unsigned error = lodepng_encode32_file(filename, (unsigned char*)pixels, width, height);
    if (error) {
		printf("error %u: %s\\n", error, lodepng_error_text(error));
	}

}
//---------------------------------------------------------------------------
 void TForm1::grayscale(const char *filename)
{
	unsigned int w = 0, h = 0;
	unsigned int i;
	unsigned char *picture ;
	struct pixel* grayscale_image;
	unsigned char grayscale_value;
	char FilName[200];
	StrPCopy(FilName,DirToImage+"grayscale.png");

	picture = load_png_file(filename, &w, &h);

	if (picture == NULL)
	{
        printf("I can't read the picture %s. Error.\n", filename);
        return;
    }

	grayscale_image = (struct pixel*)malloc(w * h * sizeof(struct pixel));

	for (i = 0; i < h * w; i++)
	{
		grayscale_value = (unsigned char)((picture[i * 4] + picture[i * 4 + 1] + picture[i * 4 + 2]) / 3);
        grayscale_image[i].R = grayscale_value;
		grayscale_image[i].G = grayscale_value;
		grayscale_image[i].B = grayscale_value;
		grayscale_image[i].alpha = picture[i * 4 + 3];
    }
	save_png_file(FilName, grayscale_image, w, h);

    free(picture);
	free(grayscale_image);

}
 //-------------------------------------------------
  void TForm1::process_image_with_sobel(const char *filename)
{
	unsigned int w = 0, h = 0;
	unsigned char *picture;
	unsigned int magnitude;
	unsigned int gx = 0, gy = 0;
	struct pixel* sobel_image;
	double var1;
	char FilName[200];
	//StrPCopy(FilName,DirToImage+"sobelimage.png");

	if (picture == NULL)
	{
		printf("I can't read the picture %s. Error.\n", filename);
		return;
	}

	  picture = load_png_file(filename, &w, &h);
	  sobel_image = (struct pixel*)malloc(w * h * sizeof(struct pixel));
	for (int y = 1; y < h - 1; y++)
	{
		for (int x = 1; x < w - 1; x++)
		{
			// sobel

			// gradient X
			gx += (picture[(y-1)*w*4 + (x+1)*4] - picture[(y-1)*w*4 + (x-1)*4]) * 1;
			gx += (picture[y*w*4 + (x+1)*4] - picture[y*w*4 + (x-1)*4]) * 2;
			gx += (picture[(y+1)*w*4 + (x+1)*4] - picture[(y+1)*w*4 + (x-1)*4]) * 1;
			// gradient Y
			gy += (picture[(y+1)*w*4 + (x-1)*4] - picture[(y-1)*w*4 + (x-1)*4]) * 1;
			gy += (picture[(y+1)*w*4 + x*4] - picture[(y-1)*w*4 + x*4]) * 2;
			gy += (picture[(y+1)*w*4 + (x+1)*4] - picture[(y-1)*w*4 + (x+1)*4]) * 1;

			var1 = gx*gx + gy*gy;
			magnitude = sqrt(var1);
			magnitude = fmin(fmax(magnitude,0), 255);
			sobel_image[y*w + x].R = magnitude;
			sobel_image[y*w + x].G = magnitude;
			sobel_image[y*w + x].B = magnitude;
			sobel_image[y*w + x].alpha = 255;

		}
	}
	StrPCopy(FilName,DirToImage+"sobelimage.png");
	save_png_file(FilName, sobel_image, w, h);

	free(picture);
    free(sobel_image);
}

//----------------------------------------------
double TForm1::fmin(double x, double y)
{
   return (x<y)? x:y;
}
//------------------------------------
double TForm1::fmax(double x, double y)
{
   return (x>y)? x:y;
}
//------------------------------------
void TForm1::union_sets(Node* nodes, Node* x, Node* y, double epsilon)
{
	double var1, var2, var3, var4;

	if (x->r < 40 && y->r < 40)
	{
        return;
    }
    Node* px = find_root(nodes, x);
    Node* py = find_root(nodes, y);
	var2 = x->r - y->r;
	var3 = x->g - y->g;
	var4 = x->b - y->b;
	var1 =  pow(var2, 2) + pow(var3, 2) + pow(var4, 2);
	double color_difference = sqrt(var1);
	if (px != py && color_difference < epsilon)
	{
		if (px->rank > py->rank)
		{
			py->parent = px;
		} else
		{
			px->parent = py;
			if (px->rank == py->rank)
			{
				py->rank++;
			}
		}
	}
}
//----------------------------------------------------------------------------

