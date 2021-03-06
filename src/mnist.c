#include <stdio.h>
#include "mnist.h"
#include <stdlib.h>

/*Los numeros en el dataset se encunetran de manera invertida
  para usarse se deben de voltear se tienen que leer en formato
  de bytes*/

uint32_t flipBytes(uint32_t n){
    uint32_t b0, b1, b2, b3;

    b0 = (n & 0x000000ff) <<  24u;
    b1 = (n & 0x0000ff00) <<   8u;
    b2 = (n & 0x00ff0000) >>   8u;
    b3 = (n & 0xff000000) >>  24u;

    return (b0 | b1 | b2 | b3);
}

//Se lee el encabezado del archivo de imagenes
void readImageFileHeader(FILE *imageFile, MNIST_ImageFileHeader *ifh){
    //Inicializacion
    ifh->magicNumber = 0;
    ifh->maxImages = 0;
    ifh->imgWidth = 0;
    ifh->imgHeight = 0;

    //Se lee cada uno de los datos del encabezado y se voltean
    fread(&ifh->magicNumber, 4, 1, imageFile);
    ifh->magicNumber = flipBytes(ifh->magicNumber);

    fread(&ifh->maxImages, 4, 1, imageFile);
    ifh->maxImages = flipBytes(ifh->maxImages);

    fread(&ifh->imgWidth, 4, 1, imageFile);
    ifh->imgWidth = flipBytes(ifh->imgWidth);

    fread(&ifh->imgHeight, 4, 1, imageFile);
    ifh->imgHeight = flipBytes(ifh->imgHeight);
}

//Se lee el encabezado del archivo de etiquetas
void readLabelFileHeader(FILE *imageFile, MNIST_LabelFileHeader *lfh){

    //Inicializacion
    lfh->magicNumber = 0;
    lfh->maxImages   = 0;

    //Se lee cada uno de los datos del encabezado y se voltean
    fread(&lfh->magicNumber, 4, 1, imageFile);
    lfh->magicNumber = flipBytes(lfh->magicNumber);

    fread(&lfh->maxImages, 4, 1, imageFile);
    lfh->maxImages = flipBytes(lfh->maxImages);

}

/* Se lee el archivo de imagenes y el encabezado despues el puntero
   se mueve a la primera imagen
*/
FILE *openMNISTImageFile(char *fileName){
    FILE *imageFile;
    imageFile = fopen(fileName, "rb");//Se abre el archivo
    if(imageFile == NULL){
        printf("ERROR, no se ecnontro el archivo: %s\n", fileName);
        exit(0);
    }
    MNIST_ImageFileHeader imageFileHeader;
    readImageFileHeader(imageFile, &imageFileHeader);

    return imageFile;
}

/* Se lee el archivo de etiquetas y el encabezado despues el puntero
   se mueve a la primera etiqueta
*/
FILE *openMNISTLabelFile(char *fileName){
    FILE *labelFile;
    labelFile = fopen(fileName, "rb");
    if(labelFile == NULL){
        printf("ERROR, no se encontro el archivo: %s\n", fileName);
        exit(0);
    }
    MNIST_LabelFileHeader labelFileHeader;
    readLabelFileHeader(labelFile, &labelFileHeader);

    return labelFile;
}

//Se regresa la siguiente imagen
MNIST_Image getImage(FILE *imageFile){
    MNIST_Image img;
    size_t result;
    result = fread(&img, sizeof(img), 1, imageFile);
    if(result != 1){
        printf("Error al abrir la imagen\n");
        exit(1);
    }

    return img;
}

//Se regresa la siguiente etiqueta
MNIST_Label getLabel(FILE *labelFile){
    MNIST_Label lbl;
    size_t result;
    result = fread(&lbl, sizeof(lbl), 1, labelFile);
    if(result != 1){
        printf("Error al abrir la etiqueta\n");
        exit(1);
    }

    return lbl;
}

//Dibujamos el recuadro donde van a estar los numeros
void displayImageFrame(int row, int col){
    printf("------------------------------\n");

    for (int i = 0; i < MNIST_IMG_HEIGHT; i++){
        for (int o = 0; o < col-1; o++) printf(" ");
        printf("|                            |\n");
    }

    for (int o = 0; o < col-1; o++) printf(" ");
    printf("------------------------------\n");

}

//Desplegamos la imagen dentro del recuadro
void displayImage(MNIST_Image *img, int lbl, int cls, int row, int col){

    //Obtenemos un arreglo en que alamacenamos la posicion
    char imgStr[(MNIST_IMG_HEIGHT * MNIST_IMG_WIDTH)+((col+1)*MNIST_IMG_HEIGHT)+1];
    strcpy(imgStr, "");

    for (int y = 0; y < MNIST_IMG_HEIGHT; y++){

        for (int o = 0; o < col-2; o++) strcat(imgStr," ");
        strcat(imgStr, "|");

        for (int x = 0; x < MNIST_IMG_WIDTH; x++){
            //Con la posicion obtenido verificamos si existe un numero si es así ponemos una 'X' si no '.'
            strcat(imgStr, img->pixel[y*MNIST_IMG_HEIGHT+x] ? "X" : "." );
        }
        strcat(imgStr,"\n");
    }

    printf("%s",imgStr);

    printf("     Actual:%d   Prediccion:%d\n\n",lbl,cls);

}
