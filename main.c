#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include<string.h>
struct Pixel
{
    unsigned char red,green,blue;
};

void XORSHIFT32(unsigned int R0,unsigned int W,unsigned int H,unsigned int *R)
{

    unsigned int n=2*W*H;
    unsigned int r,k;
    r=R0;
    for(k=0; k<n; k++)
    {
        r=r^r<<13;
        r=r^r>>17;
        r=r^r<<5;
        R[k]=r;
    }
}


void Durstenfeld(unsigned int n,unsigned int *R,unsigned int *p)
{
    unsigned int r,k,aux;
    for(k=0; k<n; k++)
        p[k]=k;

    for(k=n-1; k>=1; k--)
    {
        r=R[k]%(k+1);
        aux=p[r];
        p[r]=p[k];
        p[k]=aux;
    }
}

struct Pixel * citire(char *nume,long int *dimensiune,unsigned int *H,unsigned int *W,int *k,unsigned int * padding,unsigned char *header,int gray)
{
    *k=0;
    struct Pixel *pixel;
    pixel=malloc(sizeof(struct Pixel)*(*dimensiune));
    FILE *datein=fopen(nume,"rb+");
    if(datein==NULL)
    {
        printf("Fisierul %s nu a fost gasit",nume);
        exit(0);
    }
    fseek(datein,0,2);
    *dimensiune=ftell(datein);
    fseek(datein,18,0);
    fread(W,sizeof(unsigned int),1,datein);
    fread(H,sizeof(unsigned int),1,datein);

    if((*W)%4!=0)
        *padding=4-(((*W)*3)%4);
    else *padding=0;

    fseek(datein,0,0);
    unsigned char c,c2,c3;

    for(int i=0; i<54; i++)
    {
        fread(&c,1,1,datein);
        header[i]=c;
    }

    fseek(datein,54,0);
    for(int i=0; i<*H; i++)
    {
        for(int j=0; j<*W; j++)
        {
            fread(&c,1,1,datein);
            fread(&c2,1,1,datein);
            fread(&c3,1,1,datein);
            if(gray==0)
            {

                pixel[*k].blue=c;
                pixel[*k].green=c2;
                pixel[*k].red=c3;
            }
            else
            {


                pixel[*k].red=c*0.114+0.587*c2+0.299*c3;
                pixel[*k].blue=c*0.114+0.587*c2+0.299*c3;
                pixel[*k].green=c*0.114+0.587*c2+0.299*c3;
            }

            (*k)++;
        }
        fseek(datein,*padding,SEEK_CUR);
    }
    return pixel;
    fclose(datein);
}

unsigned char **citire2(char *nume,long int *dimensiune,unsigned int *H,unsigned int *W,unsigned int * padding,unsigned char *header)
{

    FILE *datein=fopen(nume,"rb+");
    if(datein==NULL)
    {
        printf("Fisierul %s nu a fost gasit",nume);
        exit(0);
    }
    fseek(datein,0,2);
    *dimensiune=ftell(datein);
    fseek(datein,18,0);
    fread(W,sizeof(unsigned int),1,datein);
    fread(H,sizeof(unsigned int),1,datein);

    if((*W)%4!=0)
        *padding=4-(((*W)*3)%4);
    else *padding=0;

    fseek(datein,0,0);
    unsigned char c,c2,c3;

    for(int i=0; i<54; i++)
    {
        fread(&c,1,1,datein);
        header[i]=c;
    }

    fseek(datein,54,0);
    unsigned char **a;
    a=malloc(sizeof(unsigned char *)*(*H));
    int i,j;
    for( i=0; i<*H; i++)
    {
        a[i]=malloc(sizeof(unsigned char)*(*W));
        for(j=0; j<*W; j++)
        {
            fread(&c,1,1,datein);
            fread(&c2,1,1,datein);
            fread(&c3,1,1,datein);
            a[i][j]=c*0.114+0.587*c2+0.299*c3;


        }
        fseek(datein,*padding,SEEK_CUR);
    }
    fclose(datein);
    return a;

}



void afisare(char *nume2,int W,int H,struct Pixel *pixel,int padding,unsigned char *header)
{
    int i,j=0;
    unsigned char c;
    FILE *scriere=fopen(nume2,"wb+");
    for(i=0; i<54; i++)
        fwrite(&header[i],1,1,scriere);
    for(i=0; i<H*W; i++)
    {
        fwrite(&pixel[j].blue,1,1,scriere);
        fwrite(&pixel[j].green,1,1,scriere);
        fwrite(&pixel[j].red,1,1,scriere);
        j++;
    }

    c=0;
    for(int p=0; p<padding; p++)
        fwrite(&c,1,1,scriere);


    fclose(scriere);
}

void afisare2(char *nume2,int W,int H,int padding,unsigned char *header,unsigned char **a)
{

    unsigned char c;
    FILE *scriere=fopen(nume2,"wb+");
    for(int i=0; i<54; i++)
        fwrite(&header[i],1,1,scriere);
    for(int i=0; i<H; i++)
    {
        for(int j=0; j<W; j++)
        {
            fwrite(&a[i][j],1,1,scriere);
            fwrite(&a[i][j],1,1,scriere);
            fwrite(&a[i][j],1,1,scriere);
        }

        c=0;
        for(int p=0; p<padding; p++)
            fwrite(&c,1,1,scriere);

    }
    fclose(scriere);
}
void criptare(char *nume,unsigned int W,unsigned int H,struct Pixel *pixel,char *imagine_initiala)
{
    FILE *key=fopen(nume,"r");
    unsigned int R0,SV;
    fscanf(key,"%u%u",&R0,&SV);
    unsigned int *R,*p;
    R=malloc(sizeof(unsigned int)*(2*W*H));
    XORSHIFT32(R0,W,H,R);
    p=malloc(sizeof(unsigned int)*H*W);
    Durstenfeld(W*H,R,p);
    struct Pixel *aux;
    aux=malloc(sizeof(struct Pixel)*W*H);

    for(int i=0; i<W*H; i++)
    {

        aux[p[i]].blue=pixel[i].blue;
        aux[p[i]].green=pixel[i].green;
        aux[p[i]].red=pixel[i].red;

    }

    for(int i=0; i<W*H; i++)
    {

        if(i==0)
        {
            pixel[0].red=SV^aux[0].red^R[W*H];
            pixel[0].green=SV^aux[0].green^R[W*H];
            pixel[0].blue=SV^aux[0].blue^R[W*H];
        }
        else
        {
            pixel[i].red=pixel[i-1].red^aux[i].red^R[W*H+i];
            pixel[i].green=pixel[i-1].green^aux[i].green^R[W*H+i];
            pixel[i].blue=pixel[i-1].blue^aux[i].blue^R[W*H+i];
        }
    }

    fclose(key);
    free(aux);
    free(p);
    free(R);
}

void decriptare(char *nume,unsigned int W,unsigned int H,struct Pixel *pixel,char *imagine_initiala)
{
    FILE *key=fopen(nume,"r");
    unsigned int R0,SV;
    fscanf(key,"%u%u",&R0,&SV);
    fclose(key);
    unsigned int *R,*p,*p2;

    R=malloc(sizeof(unsigned int)*(2*W*H));
    XORSHIFT32(R0,W,H,R);
    p=malloc(sizeof(unsigned int)*H*W);
    Durstenfeld(W*H,R,p);

    p2=malloc(sizeof(unsigned int)*W*H);
    for(int i=0; i<W*H; i++)
        p2[p[i]]=i;

    struct Pixel *aux;
    aux=malloc(sizeof(struct Pixel)*W*H);

    for(int i=0; i<W*H; i++)
    {

        if(i==0)
        {
            aux[0].blue=SV^pixel[0].blue^R[W*H];
            aux[0].green=SV^pixel[0].green^R[W*H];
            aux[0].red=SV^pixel[0].red^R[W*H];
        }
        else
        {

            aux[i].blue=pixel[i-1].blue^pixel[i].blue^R[W*H+i];
            aux[i].green=pixel[i-1].green^pixel[i].green^R[W*H+i];
            aux[i].red=pixel[i-1].red^pixel[i].red^R[W*H+i];
        }
    }
    for(int i=0; i<W*H; i++)
        pixel[p2[i]]=aux[i];

    free(aux);
    free(p);
    free(p2);
    free(R);
}
void test_frecventa(struct Pixel*pixel,unsigned int W,unsigned int H)
{
    double R,G,B,x,R2=0,B2=0,G2=0;
    x=W*H/256;
    for(int i=0; i<256; i++)
    {
        R=0;
        G=0;
        B=0;
        for(int j=0; j<W*H; j++)
        {
            if(i==pixel[j].red)
                R++;
            if(i==pixel[j].green)
                G++;
            if(i==pixel[j].blue)
                B++;
        }
        R2+=(R-x)*(R-x)/x;
        G2+=(G-x)*(G-x)/x;
        B2+=(B-x)*(B-x)/x;
    }
    printf("R:%.2f G:%.2f B:%.2f\n",R2,G2,B2);
}

struct Detectie
{
    float valoare;
    int linie,coloana;
};

float medie(unsigned char **a,unsigned int linie,unsigned int coloana)
{
    float k=0;
    for(int i=0; i<linie; i++)
        for(int j=0; j<coloana; j++)
            k=k+a[i][j];
    k=k/(linie*coloana);
    return k;
}


int compare(const void *x,const void* y)
{
    if((((struct Detectie*)y)->valoare)>(((struct Detectie*)x)->valoare))
        return -1;
    else
        return 1;
}


void colorare(struct Pixel *culoare,struct Pixel *imagine,int p,struct Detectie *detectie,unsigned int H2,unsigned int W2,int cifra)
{
    int j;
    for(int i=0; i<p; i++)
    {
        for(j=detectie[i].linie; j<detectie[i].linie+H2; j++)
        {
            imagine[j].red=culoare[cifra].red;
            imagine[j].green=culoare[cifra].green;
            imagine[j].blue=culoare[cifra].blue;
            imagine[j+W2].red=culoare[cifra].red;
            imagine[j+W2].green=culoare[cifra].green;
            imagine[j+W2].blue=culoare[cifra].blue;
        }
        for(j=detectie[i].coloana; j<detectie[i].coloana+W2; j++)
        {
            imagine[j].red=culoare[cifra].red;
            imagine[j].green=culoare[cifra].green;
            imagine[j].blue=culoare[cifra].blue;
            imagine[H2+j].red=culoare[cifra].red;
            imagine[j+H2].green=culoare[cifra].green;
            imagine[H2+j].blue=culoare[cifra].blue;
        }

    }

}


void template_matching(unsigned int W,unsigned int H,unsigned int H2,unsigned int W2,unsigned char **imagine,unsigned char**sablon,int cifra,struct Pixel *culori)
{

    int i,j,k,k2,p=0;
    float deviatie_imagine,deviatie_sablon,medie_imagine,medie_sablon,corr;
    struct Detectie *detectie;
    for(i=0; i<H; i++)
        for(j=0; j<W; j++)
        {
            medie_sablon=medie(sablon,H2,W2);
            medie_imagine=0;
            for(k=i; k<i+H2; k++)
                for(k2=j; k2<j+W2; k2++)
                    medie_imagine+=imagine[k][k2];
            medie_imagine/=H2*W2;

            for(k=i; k<i+H2; k++)
                for(k2=j; k2<j+W2; k2++)
                {
                    deviatie_imagine+=(imagine[k][k2]-medie_imagine)*(imagine[k][k2]-medie_imagine);
                    deviatie_sablon+=(sablon[k-i][k2-j]-medie_sablon)*(sablon[k-i][k2-j]-medie_sablon);
                }
            deviatie_imagine=sqrt(deviatie_imagine/(W2*H2-1));
            deviatie_sablon=sqrt(deviatie_sablon/(W2*H2-1));
            for(k=i; k<i+H2; k++)
                for(k2=j; k2<j+W2; k2++)
                    corr+=1/(deviatie_imagine*deviatie_sablon)*(imagine[k][k2]-medie_imagine)*(sablon[k-i][k2-j]-medie_sablon);
            corr/=W2*H2;


            detectie=malloc(sizeof(struct Detectie)*(W*H)/(W2*H2));
            if(corr>=0.5)
            {
                detectie[p].linie=i;
                detectie[p].coloana=j;
                detectie[p].valoare=corr;

            }
        }

     qsort(detectie,p,sizeof(struct Detectie),compare);
     colorare(culori,imagine,p,detectie,H2,W2,cifra);

}

int main()
{
    unsigned int H,W,padding=0,*R,*p,H2,W2,padding2;
    int k=0,k2=0;
    long int dimensiune,dimensiune2;

    unsigned char *header,*header2;
    struct Pixel *pixel;
    unsigned char **matrice;

    //Partea 1

    header=malloc(sizeof(unsigned char)*54);
    pixel=citire("peppers.bmp",&dimensiune,&H,&W,&k,&padding,header,0);


    printf("Imaginea initiala: ");
    test_frecventa(pixel,W,H);
    criptare("secret_key.txt",W,H,pixel,"peppera.bmp");

    afisare("afisare_criptat.bmp",W,H,pixel,padding,header);
    printf("Imaginea criptata: ");
    test_frecventa(pixel,W,H);

    decriptare("secret_key.txt",W,H,pixel,"peppera.bmp");
    afisare("afisare_decriptat.bmp",W,H,pixel,padding,header);
    free(pixel);
    free(header);

    //Partea 2


    int i,j;
    unsigned char **a,**cifra;
    header=malloc(sizeof(unsigned char)*54);
    pixel=citire("test.bmp",&dimensiune,&H,&W,&k,&padding,header,1);
    a=citire2("test.bmp",&dimensiune,&H,&W,&padding,header);

    char nume[11][11];

    strcpy(nume[0],"cifra0.bmp");
    strcpy(nume[1],"cifra1.bmp");
    strcpy(nume[2],"cifra2.bmp");
    strcpy(nume[3],"cifra3.bmp");
    strcpy(nume[4],"cifra4.bmp");
    strcpy(nume[5],"cifra5.bmp");
    strcpy(nume[6],"cifra6.bmp");
    strcpy(nume[7],"cifra7.bmp");
    strcpy(nume[8],"cifra8.bmp");
    strcpy(nume[9],"cifra9.bmp");
    header2=malloc(sizeof(unsigned char)*54);
    cifra=malloc(sizeof(unsigned char*)*15);
    struct Pixel *culori;
    culori=malloc(sizeof(struct Pixel)*10);
    for(i=0; i<=9; i++)
        culori[i].red=culori[i].green=culori[i].blue=0;
    culori[0].red=255;
    culori[1].red=255;
    culori[1].green=255;
    culori[2].green=255;
    culori[3].blue=255;
    culori[3].green=255;
    culori[4].red=255;
    culori[4].blue=255;
    culori[5].blue=255;
    culori[6].green=192;
    culori[6].red=192;
    culori[6].blue=192;
    culori[7].green=140;
    culori[7].red=255;
    culori[8].blue=128;
    culori[8].red=128;
    culori[9].red=128;

    for(i=0; i<=9; i++)
    {
        cifra[i]=malloc(sizeof(unsigned char)*11);
        cifra[i]=citire2(nume[i],&dimensiune2,&H2,&W2,&padding2,header2);
        //template_matching(W,H,H2,W2,a,cifra[i],i,culori[i]);
    }
    afisare2("img_gri.bmp",W,H,padding,header,a);
    return 0;
}
