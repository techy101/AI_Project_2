/////////////////////////////////
//
// Program Bitmap_Parser
//
// CSCI 540
//
// Author: Mark A. Renslow markminn@mac.com
//
// Instructor: Dr. B. Julstrom
//
// I am sharing this code in the spirit of collaboration.  I
// have found that it serves my purposes, and I hope it
// improves your learning experience.  Please feel free to
// modify it for your own use.  I would appreciate any
// comments, questions or bug reports.

// I may be reached at: markminn@mac.com
//
// This program will read a binary file containing a bitmap
// and load it into a structure. Each pixel is assumed to be
// represented by 8 bits.  The binary data are maintained in
// the structure as bytes.  This makes reading and writing
// the files easy.  However, when integer values need to be
// extracted, the bytes need to be assembled into integers,
// using the Assemble_Integer() function.
//
// This code was written with the notion in mind that it
// could be scaled to deal with bitmaps other than the ones
// with 256 greys.  However, this version will only work
// with 256 greys. Additonally, extra work will need to be
// done to support bitmaps with bits-per-row not evenly
// divisible by 4.  (This is the "padding" issue)
//
// If you change the size of the image, there are three
// variables in the header which will need to be
// considered for updating.  They are:
//    bfSize
//    biWidth
//    biHeight
//
// Additionally, if the width of your new image is not
// evenly divisible by four bytes, you will have to add
// "padding" bytes to make it so.  This will change
// the size of the file (bfSize), so you should update
// that parameter to reflect padding. However, I do not
// think that biWidth is to be updated to reflect
// padding.  That is a question I have not had to
// answer yet.
//    
////////////////////

#include "stdafx.h"
#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <math.h>


using namespace std;

typedef unsigned char  byte_t;

// The following two structures were adapted from
// http://users.ece.gatech.edu/~slabaugh/personal/c/bmpwrite.html
struct bmpFILEHEADER
{
	byte_t 	bfType[2];  //Bitmap identifier,  Must be "BM"
	byte_t	bfSize[4];
	byte_t       bfReserved[4];
	byte_t       bfOffbits[4];  //specifies the location
								//(in bytes) in the file of the
								// image data. Should be equal to
								// sizeof(bmpFileHeader + sizeof(bmpInfoHeader)
								//        + sizeof(Palette) 
};

struct bmpINFOHEADER
{
	byte_t	biSize[4];    // Size of the bmpInfoHeader,
						  // i.e. sizeof(bmpInfoheader)
	byte_t	biWidth[4];   // Width of bitmap, in pixels
						  // change this if you change
						  // the size of the image. see ***** note below

	byte_t  	biHeight[4];  // Height of bitmap, in pixels
							  //change this if you change the
							  // size of the image. see ***** note below
	byte_t 	biPlanes[2];  // Should/must be 1.
	byte_t	biBitCount[2]; // The bit depth of the bitmap.
						   // For 8 bit bitmaps, this is 8
	byte_t  	biCompression[4];   // Should be 0 for
									// uncompressed bitmaps
	byte_t       biSizeImage[4];    //The size of the padded
									// image, in bytes
	byte_t       biXPelsPerMeter[4]; //Horizontal resolution,
									 // in pixels per meter.  Not signif.
	byte_t       biYPelsPermeter[4];  //Vertical resolution,
									  //as above.
	byte_t       biClrUsed[4];   //Indicates the number of
								 //colors in the palette.
	byte_t       biClrImportant[4]; //Indicates number of
									//colors to display the bitmap.
									// Set to zero to indicate all colors should be used.
};
// *****Note (from above) you will have to write
//         a function to do this.  I have not yet.
struct bmpPALETTE
{
	byte_t	palPalette[1024]; // this will need to be
							  //improved if the program is to scale.
							  // unless we change the palette,
							  // this will do.
};

struct bmpBITMAP_FILE	// note:  this structure may not be
						// written to file all at once.
						// the two headers may be written
						// normally, but the image requires
						// a write for each line followed
						//  by a possible 1 - 3 padding bytes.
{
	bmpFILEHEADER	fileheader;
	bmpINFOHEADER	infoheader;
	bmpPALETTE		palette;     //this implementation
								 // will not generalize.  Fixed at 256 shades of grey.
	byte_t		**image_ptr; //this points to the 
							 // image. Allows the allocation of a two dimensional
							 // array dynamically
};



//================= Open_input_file =======================
//
// Gets the name of the input file from the user and opens
// it for input
//

void open_input_file(ifstream &in_file);           //Pre:  none
								//Post: File name supplied by user

//================ Assemble_Integer ========================
//
// Accepts a pointer to an array of unsigned characters
// (there should be 4 bytes)
// Assembles them into a signed integer and returns the
// result

int Assemble_Integer
(
	unsigned char bytes[]	// Pre: 4 little-endian bytes
							// (least significant byte first)
);

//============= Display_FileHeader ==========================
//

void Display_FileHeader(bmpFILEHEADER &fileheader);

//============= Display_InfoHeader ==========================
//

void Display_InfoHeader(bmpINFOHEADER &infoheader);

//=============== Calc_Padding ==============================
//
// Returns the number of bytes of padding for an image
// (either 0,1,2,3).
// Each scan line must end on a 4 byte boundry.
// Threfore, if the pixel_width is not evenly divisible
// by 4, extra bytes are added (either 1, 2 or 3 extra
// bytes) when writing each line.  Likewise, when reading
// a bitmap file it may be helpful to remove the padding
// prior to any manipulations.
// This is not needed unless the number of bytes in a row
// are not evenly divisible by 4. See implementation
// section for details.
int Calc_Padding(int pixel_width);  //pre: the width of the
									//     image in pixels


									//================= load_Bitmap_File =======================
									//
void Load_Bitmap_File
(
	bmpBITMAP_FILE &image  //Post: structure is filled with
						   //      data from a  .bmp file
);


//============= NEW: Average_Bitmap_File ========================
//
void Copy_Then_Average_Bitmap_File(bmpBITMAP_FILE &image_orig, bmpBITMAP_FILE &image_copy);


//============== Display_Bitmap_File =======================
//
void Display_Bitmap_File(bmpBITMAP_FILE &image);

//================== Copy_Image ============================
//
// Pre: image_orig.byte_t points to a 2 dim array
//      image_copy does not have an array associated with
//      byte_t**
// Post: image_copy receives a copy of the structurs in
//       image_orig
void Copy_Image(bmpBITMAP_FILE &image_orig,
	bmpBITMAP_FILE &image_copy);

//================== Remove_Image ==========================
//
// Pre:  image.byte-t contains pointers to a 2-dim array
// Post:  memory that **byte_t points to is freed with the
//        delete operator image.bfType[] is set to "XX"
//
void Remove_Image(bmpBITMAP_FILE &image);

//================= Save_Bitmap_File =======================
//
void Save_Bitmap_File(bmpBITMAP_FILE &image);

//=================== Open_Output_File =====================
//
void Open_Output_File(ofstream &out_file);


int averaging_constant;


//======================  MAIN  ============================
//======================  MAIN  ============================
//
int main()
{
	bmpBITMAP_FILE orig_image;		// <= cleanup
	bmpBITMAP_FILE copy1;
	bmpBITMAP_FILE image_averaged;

	Load_Bitmap_File(orig_image);	

	Display_FileHeader(orig_image.fileheader);
	Display_InfoHeader(orig_image.infoheader);


	//copies from orig_image to copy1
	Copy_Image(orig_image, copy1);
	cout << endl << "A copy of the file has been " <<
		"made in main memory.";

	Remove_Image(orig_image);	// frees dynamic memory too	

	cout << endl << "The original image has been " <<
		"removed from main memory.";

	cout << endl << "To show that the copy starts as " <<
		"an exact copy of the original,";

	cout << endl << "Save the copy as a bitmap." << endl;
	Save_Bitmap_File(copy1);	// SO the image info is stored in copy1 

	// display the image. 256 scale.
	//Display_Bitmap_File(copy1);

	// do processing
	cout << endl << "Apply averaging: enter a constant that will average <constant-sqd> number of pixels" << endl <<
		" and replace those pixels with 1 averaged value. " << endl;
	cin >> averaging_constant;

	// begin with averaging
	Copy_Then_Average_Bitmap_File(copy1, image_averaged);

	// display innfo for averaged imqge
	Display_FileHeader(image_averaged.fileheader);
	Display_InfoHeader(image_averaged.infoheader);

	// display the image. 256 scale.
	/*cout << endl << endl << "Displaying image again to test effect of processing... " << endl;
	Display_Bitmap_File(copy1);*/

	cout << endl << "Save the processed copy as a bitmap." << endl;
	Save_Bitmap_File(image_averaged);

	Remove_Image(copy1);
	Remove_Image(image_averaged);

	// a bit for cleanup
	/*Load_Bitmap_File(orig_image);
	Remove_Image(orig_image);*/

	system("PAUSE");
	return 0;
}


//=============== END OF MAIN ==============================
//=============== END OF MAIN ==============================


//============== open_input_file ===========================
//

void open_input_file
(
	ifstream &in_file
)
{
	char in_file_name[80];

	cout << "Enter the name of the file" << endl
		<< "which contains the bitmap: ";
	cin >> in_file_name;

	//cout << "You entered: " << in_file_name << endl;

	in_file.open(in_file_name, ios::in | ios::binary);
	if (!in_file) {
		cerr << "Error opening file\a\a\n";
		system("PAUSE");
		exit(101);
	}
	return;
}

//================ Assemble_Integer ========================
//
int Assemble_Integer(unsigned char bytes[])
{
	int an_integer;

	an_integer =
		int(bytes[0])
		+ int(bytes[1]) * 256
		+ int(bytes[2]) * 256 * 256
		+ int(bytes[3]) * 256 * 256 * 256;
	return an_integer;
}

//============= Display_FileHeader =======================
//

void Display_FileHeader(bmpFILEHEADER &fileheader)
{
	cout << "bfType:        " << fileheader.bfType[0]
		<< fileheader.bfType[1]
		<< "\n";

	cout << "bfSize:        "
		<< Assemble_Integer(fileheader.bfSize) << "\n";
	cout << "bfReserved:     "
		<< Assemble_Integer(fileheader.bfReserved) << "\n";
	cout << "bfOffbits:      "
		<< Assemble_Integer(fileheader.bfOffbits) << "\n";
}

//================ Display_InfoHeader ======================
//

void Display_InfoHeader(bmpINFOHEADER &infoheader)
{
	cout << "\nThe bmpInfoHeader contains the following:\n";
	cout << "biSize:          "
		<< Assemble_Integer(infoheader.biSize) << "\n";
	cout << "biWidth:         "
		<< Assemble_Integer(infoheader.biWidth) << "\n";
	cout << "biHeight:        "
		<< Assemble_Integer(infoheader.biHeight) << "\n";
	cout << "biPlanes:        "
		<< int(infoheader.biPlanes[0]) +
		int(infoheader.biPlanes[1]) * 256 << "\n";

	cout << "biBitCount:      "
		<< int(infoheader.biBitCount[0]) +
		int(infoheader.biBitCount[1]) * 256 << "\n";
	cout << "biCompression:   "
		<< Assemble_Integer(infoheader.biCompression) << "\n";
	cout << "biSizeImage:     "
		<< Assemble_Integer(infoheader.biSizeImage) << "\n";
	cout << "biClrUsed:       "
		<< Assemble_Integer(infoheader.biClrUsed) << "\n";
	cout << "biClrImportant:  "
		<< Assemble_Integer(infoheader.biClrImportant) << "\n";

}

//==================== Calc_Padding ========================
//

int Calc_Padding(int pixel_width)
{
	// Each scan line must end on a 4 byte boundry.
	// Threfore, if the pixel_width is not evenly divisible
	// by 4, extra bytes are added (either 1 - 3 extra bytes)

	int remainder = 0;
	int padding = 0;

	remainder = pixel_width % 4;
	//cout << "\nPixel width: " << pixel_width << "\n";
	//cout << "Remainder:     " << remainder << "\n";

	switch (remainder)
	{
	case 0:	padding = 0;
		break;
	case 1: padding = 3;
		break;
	case 2: padding = 2;
		break;
	case 3: padding = 1;
		break;
	default: cerr << "Error:  Padding was set to "
		<< padding << endl;
		system("PAUSE");
		exit(101);
	}

	//cout << "Padding determined: " << padding << "\n";

	return padding;
}

//================== load_Bitmap_File ======================
//
void Load_Bitmap_File(bmpBITMAP_FILE &image)
{
	ifstream fs_data;

	int bitmap_width;
	int bitmap_height;

	int padding;
	long int cursor1; // used to navigate through the
					  // bitfiles

	open_input_file(fs_data);


	fs_data.read((char *)&image.fileheader,
		sizeof(bmpFILEHEADER));
	//fs_data.seekg(14L);
	fs_data.read((char *)&image.infoheader,
		sizeof(bmpINFOHEADER));

	fs_data.read((char *)&image.palette,
		sizeof(bmpPALETTE));	// this will need to
								// be dynamic, once 
								// the size of the palette can vary

	bitmap_height = Assemble_Integer(image.infoheader.biHeight);
	bitmap_width = Assemble_Integer(image.infoheader.biWidth);
	padding = Calc_Padding(bitmap_width);

	// allocate a 2 dim array
	image.image_ptr = new byte_t*[bitmap_height];
	for (int i = 0; i < bitmap_height; i++)
		image.image_ptr[i] = new byte_t[bitmap_width];

	cursor1 = Assemble_Integer(image.fileheader.bfOffbits);
	fs_data.seekg(cursor1);  //move the cursor to the
							 // beginning of the image data

							 //load the bytes into the new array one line at a time
	for (int i = 0; i < bitmap_height; i++)
	{
		fs_data.read((char *)image.image_ptr[i],
			bitmap_width);
		// insert code here to read the padding,
		// if there is any
	}

	fs_data.close();  //close the file
					  // (consider replacing with a function w/error checking)
}

//============== Copy_Then_Average_Bitmap_File ========================
//
void Copy_Then_Average_Bitmap_File(bmpBITMAP_FILE &image_orig, bmpBITMAP_FILE &image_copy)
{
	//int orig_height, orig_width,
	//	copy_height, copy_width;
	//int averaging_constant = 8;		// 16 max, thinking

	//image_copy.fileheader = image_orig.fileheader;
	//image_copy.infoheader = image_orig.infoheader;
	//image_copy.palette = image_orig.palette;

	//orig_height = Assemble_Integer(image_orig.infoheader.biHeight);
	//orig_width = Assemble_Integer(image_orig.infoheader.biWidth);
	//copy_height = Assemble_Integer(image_copy.infoheader.biHeight) / averaging_constant;		// resize, eh?
	//copy_width = Assemble_Integer(image_copy.infoheader.biWidth) / averaging_constant;

	//image_copy.image_ptr = new byte_t*[copy_height];

	//for (int i = 0; i < copy_height; i++)
	//	image_copy.image_ptr[i] = new byte_t[copy_width];


	//load the bytes into the new array one byte at a time
	//for (int i = 0; i < height; i++)
	//{
	//	for (int j = 0; j < width; j++)
	//		image_copy.image_ptr[i][j] =
	//		image_orig.image_ptr[i][j];				//********Can add values here and change image*****
	//}

	// *** old averaging code
	//for (int i = 0; i < orig_height; i += averaging_constant)		// step through original image one row at a time
	//{
	//	for (int j = 0; j < orig_width; j += averaging_constant)	// step through original image one row at a time
	//	{
	//		image_copy.image_ptr[i/averaging_constant][j/averaging_constant] = 0;

	//		// sum up pixel values
	//		for (int k = i; k < (i + averaging_constant); k++)		// go through each pixel of the block
	//		{
	//			for (int l = j; l < (j + averaging_constant); l++)
	//				image_copy.image_ptr[i/averaging_constant][j/averaging_constant] += image_orig.image_ptr[k][l];
	//		}

	//		// apply the averaging
	//		image_copy.image_ptr[i / averaging_constant][j / averaging_constant] /= averaging_constant;
	//	}
	//}

	int height, width;	

	image_copy.fileheader = image_orig.fileheader;
	image_copy.infoheader = image_orig.infoheader;
	image_copy.palette = image_orig.palette;

	// get dimensions
	height = Assemble_Integer(image_orig.infoheader.biHeight);
	width = Assemble_Integer(image_orig.infoheader.biWidth);

	// allocate the array
	image_copy.image_ptr = new byte_t*[height];

	for (int i = 0; i < height; i++)
		image_copy.image_ptr[i] = new byte_t[width];

	// new averaging code
	for (int i = 0; i < height / averaging_constant; i++)		// dimensions of resized image
	{
		for (int j = 0; j < width / averaging_constant; j++)
		{ 
			image_copy.image_ptr[i][j] = 0;		// initialize the average
	
			// sum up pixel values from original image one block at a time
			for (int k = i*averaging_constant; k < (i+1)*averaging_constant; k++)		
			{
				for (int l = j*averaging_constant; l < (j + 1)*averaging_constant; l++)	
				{
					image_copy.image_ptr[i][j] += image_orig.image_ptr[k][l];
				}
			}

			// apply the average
			image_copy.image_ptr[i][j] = image_copy.image_ptr[i][j] / averaging_constant;
		}
	}

	// fill the rest of the image with... 
	for (int i = height/averaging_constant; i < height; i++)
	{
		for (int j = width/averaging_constant; j < width; j++)
			image_copy.image_ptr[i][j] = 255;				// white
	}
}

//============== Display_Bitmap_File =======================
//
void Display_Bitmap_File(bmpBITMAP_FILE &image)
{
	int bitmap_width;
	int bitmap_height;

	Display_FileHeader(image.fileheader);
	Display_InfoHeader(image.infoheader);

	//display the palette here too, perhaps.

	bitmap_height = Assemble_Integer(image.infoheader.biHeight);
	bitmap_width = Assemble_Integer(image.infoheader.biWidth);

	for (int i = 0; i < 1; i++)
	{
		for (int j = 0; j < bitmap_width; j++)
			cout << setw(4) << int(image.image_ptr[i][j]);
		cout << "\n\nNew Line\n\n";
	}
}

//=============== Copy_Image ===============================
//
void Copy_Image(bmpBITMAP_FILE &image_orig,
	bmpBITMAP_FILE &image_copy)
{
	int height, width;

	image_copy.fileheader = image_orig.fileheader;
	image_copy.infoheader = image_orig.infoheader;
	image_copy.palette = image_orig.palette;

	height = Assemble_Integer(image_copy.infoheader.biHeight);
	width = Assemble_Integer(image_copy.infoheader.biWidth);

	image_copy.image_ptr = new byte_t*[height];

	for (int i = 0; i < height; i++)
		image_copy.image_ptr[i] = new byte_t[width];

	//load the bytes into the new array one byte at a time
	for (int i = 0; i<height; i++)
	{
		for (int j = 0; j < width; j++)
			image_copy.image_ptr[i][j] =
			image_orig.image_ptr[i][j];				//********Can add values here and change image*****
	}
}

//================== Remove_Image ==========================
//
void Remove_Image(bmpBITMAP_FILE &image)
{
	int height, width;

	height = Assemble_Integer(image.infoheader.biHeight);
	width = Assemble_Integer(image.infoheader.biWidth);

	//once the palette is dynamic, must delete the memory
	// allocated for the palatte here too

	// delete the dynamic memory
	for (int i = 0; i < height; i++)
		delete[] image.image_ptr[i];
	delete[] image.image_ptr;

	image.fileheader.bfType[0] = 'X';  // just to mark it as
	image.fileheader.bfType[1] = 'X';  // unused.
									   // Also, we may wish to initialize all the header
									   // info to zero.
}

//================== Save_Bitmap_File ======================
//
void Save_Bitmap_File(bmpBITMAP_FILE &image)
{
	ofstream fs_data;

	int width;
	int height;
	int padding;
	long int cursor1;	// used to navigate through the
						// bitfiles

	height = Assemble_Integer(image.infoheader.biHeight);
	width = Assemble_Integer(image.infoheader.biWidth);

	Open_Output_File(fs_data);

	fs_data.write((char *)&image.fileheader,
		sizeof(bmpFILEHEADER));
	if (!fs_data.good())
	{
		cout << "\aError 101 writing bitmapfileheader";
		cout << " to file.\n";
		system("PAUSE");
		exit(101);
	}

	fs_data.write((char *)&image.infoheader,
		sizeof(bmpINFOHEADER));
	if (!fs_data.good())
	{
		cout << "\aError 102 writing bitmap";
		cout << " infoheader to file.\n";
		exit(102);
	}

	fs_data.write((char *)&image.palette,
		sizeof(bmpPALETTE));
	if (!fs_data.good())
	{
		cout << "\aError 103 writing bitmap palette to";
		cout << "file.\n";
		exit(103);
	}
	//this loop writes the image data
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			fs_data.write((char *)&image.image_ptr[i][j], sizeof(byte_t));	// error here with previous code
			if (!fs_data.good())
			{
				cout << "\aError 104 writing bitmap data";
				cout << "to file.\n";
				exit(104);
			}
		}
	}

	fs_data.close();
}

//================== Open_Output_File ===================
//
void Open_Output_File(ofstream &out_file)
{
	char out_file_name[80];

	cout << "Save file as: ";
	cin >> out_file_name;

	out_file.open(out_file_name, ios::out | ios::binary);
	if (!out_file)
	{
		cout << "\nCannot open " << out_file_name << endl;
		system("PAUSE");
		exit(101);
	}

	return;
}




int openBitmapAndGetInts(char fileName[], char **header, unsigned char **data)
{

	int headerSize = /*sizeof(BITMAPFILEHEADER)*/14 +
		/*sizeof(BITMAPINFOHEADER)*/ 40 + 1026/*PALLET*/;
	FILE *file = fopen(fileName, "rb");
	int num = 0;

	//problem opening the file
	if (file == 0)
		return 0;

	//make the room for the header
	(*header) = (char *)malloc(headerSize);
	memset(*header, 0, headerSize);

	//make the room for the data
	(*data) = (unsigned char *)malloc(1024 * 768);
	memset(*data, 0, 1024 * 768);

	//problem getting the space for the header
	if ((*header) == 0)
		return 0;

	if ((*data) == 0)
		return 0;

	//read in the header for the bitmap
	num = fread((*header), 1, headerSize, file);

	//error reading in header
	if (num != headerSize)
		return 0;

	//read in the integer data
	num = fread((*data), 1, 1024 * 768, file);

	//error reading in data
	if (num != 1024 * 768)
		return 0;

	//close the file
	fclose(file);

	return 1;
}

