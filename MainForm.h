//---------------------------------------------------------------------------

#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <System.ImageList.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>
//---------------------------------------------------------------------------

    typedef struct Node
  {
	unsigned char r, g, b, a;
	struct Node *up, *down, *left, *right;
	struct Node *parent;
	int rank;
  } Node;

    struct pixel
  {
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char alpha;
  };

class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TOpenDialog *OpenDialog1;
	TMainMenu *MainMenu1;
	TMenuItem *N1;
	TMenuItem *N2;
	TMenuItem *N3;
	TImageList *ImageList1;
	TToolBar *ToolBar1;
	TToolButton *ToolButton1;
	TToolButton *ToolButton2;
	TToolButton *ToolButton3;
	TGroupBox *GroupBox1;
	TButton *Button1;
	TGroupBox *GroupBox2;
	TImage *Image1;
	TImage *Image2;
	TButton *Button2;
	TGroupBox *GroupBox3;
	TImage *Image3;
	TMenuItem *C1;
	TToolButton *ToolButton4;
	TEdit *Edit1;
	TLabel *Label1;
	void __fastcall FormResize(TObject *Sender);
	void __fastcall N2Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);



private:	// User declarations


double fmin(double x, double y);
double fmax(double x, double y);
public:		// User declarations



	__fastcall TForm1(TComponent* Owner);

 char* load_png_file(const char *filename,unsigned int *width,unsigned int *height);
 void save_png_file(const char *filename, struct pixel* pixels, int width, int height);
 void grayscale(const char *filename);
 void process_image_with_sobel(const char *filename);
 void union_sets(Node* nodes, Node* x, Node* y, double epsilon);


AnsiString PathToInitDir;
AnsiString DirToImage;

};



//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
