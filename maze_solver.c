#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
 
#define MAKS_YUKSEKLIK 15
#define MAKS_GENISLIK  70

#define HUCRE_BOS   ' '
#define HUCRE_YOL   '*'
#define HUCRE_ZIY   '.'
#define HUCRE_DUVAR '#'

#define TUS_ESC     27
#define TUS_BOSLUK  32
#define HUCRE_BFS '.'

#define RENK_BEYAZ   (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define RENK_KIRMIZI (FOREGROUND_RED | FOREGROUND_INTENSITY)
#define RENK_GRI     (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define RENK_SARI    (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define RENK_YESIL (FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define RENK_DUVAR (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)

#define IC_X  2
#define IC_Y  1
#define HUCRE_YESIL '+'

typedef enum {
    MOD_CIZ = 1,
    MOD_SIL = 2,
    MOD_GEZ = 3,
    MOD_DUVAR = 4,
    MOD_BFS = 99,
    MOD_BFS_YOL = 100,
    MOD_DFS = 101
} Mod;

typedef struct {
    int s, t;
} Dugum;

Dugum kuyruk[5000];
int qBas, qSon;

char labirent[MAKS_YUKSEKLIK][MAKS_GENISLIK];
unsigned char cozum[MAKS_YUKSEKLIK][MAKS_GENISLIK];
char labirentYedek[MAKS_YUKSEKLIK][MAKS_GENISLIK];

int genislik = 30;
int yukseklik = 5;

int baslangicSatir, baslangicSutun;
int cikisSatir, cikisSutun;

int imlecSatir, imlecSutun;

Mod aktifMod = MOD_CIZ;
int duraklatildi = 0;

char durumMesaji[120] = "Hazir.";

HANDLE konsol;

void renkAyarla(WORD renk);
int  sinirlarIcindeMi(int s, int t);

int bfsZiyaret[MAKS_YUKSEKLIK][MAKS_GENISLIK];
int parentS[MAKS_YUKSEKLIK][MAKS_GENISLIK];
int parentT[MAKS_YUKSEKLIK][MAKS_GENISLIK];
int bfsBulundu = 0;
int yolS, yolT;
int yolUzunluk = 0;
int yolDiziS[5000];
int yolDiziT[5000];
int yolIndex = 0;
int toplamKesif = 0;
int siradakiAlgoritma = 0;  


int dfsZiyaret[MAKS_YUKSEKLIK][MAKS_GENISLIK];
int dfsBulundu = 0;
int dfsYolUzunluk = 0;
int dfsKesif = 0;


int dfsStackS[5000];
int dfsStackT[5000];
int dfsTop = -1;

void labirentiTemizle(void);
void labirentiBaslat(void);
void yeniBoyutAl(void);

void ekraniCiz(void);

void imleciHareketEttir(int ds, int dt);
void cizimUygula(void);
void silmeUygula(void);
void tumYollariTemizle(void);

int  genisTusOku(int *kod);
void konsolImleciniGizle(void);
void imlecGit(int s, int t);
void hucreyiYenidenCiz(int s,int t);
void disCerceveCiz(void);
void duvarUygula(void);
void imleciCiz(void);
void durumYazisiniCiz(void);
void altMenuCiz(void);
void tumBosluklariDuvarYap(void);
void bfsBaslat(void);
int bfsAdim(void);
void bfsYoluCiz(void);
void bfsNoktalariTemizle(void);
void cozumTemizle(void);
void dfsBaslat(void);
int dfsAdim(void);
void labirentiYedekle(void);
void labirentiGeriYukle(void);
void rastgeleLabirentOlustur(void);
void labirentiKaydet(void);
void labirentiYukle(void);
void cikisEkraniGoster(void);
void yaziAnimasyonluYaz(const char *metin, int gecikme);

//MAIN FONSÝYONU
int main(void)
{   
    srand((unsigned)time(NULL));
    konsol = GetStdHandle(STD_OUTPUT_HANDLE);
    konsolImleciniGizle();
    yeniBoyutAl();

    system("cls");
    imlecGit(0, 0);

    disCerceveCiz();              /* Dis cerceve + ilk cizim */
    ekraniCiz();
    durumYazisiniCiz();
    altMenuCiz();
    imleciCiz();

    while (1) {
        if (aktifMod == MOD_BFS && !duraklatildi) {        /* 1) BFS yayýlmasý */
            if (kbhit()) {
                int kod;
                int genis = genisTusOku(&kod);
                    if (!genis && kod == TUS_BOSLUK) {
                       duraklatildi = 1;
                       strcpy(durumMesaji, "Duraklatildi.");
                       durumYazisiniCiz();
                       altMenuCiz();
                    continue;  /* BFS bloðundan çýk, ana döngüye dön */
                    }
            }
            if (!bfsAdim()) {
               if (bfsBulundu)
                   aktifMod = MOD_BFS_YOL;
                else {
                    strcpy(durumMesaji, "BFS: Yol bulunamadi!");
                    aktifMod = MOD_GEZ;
               }
               altMenuCiz();
            }

           ekraniCiz();
           altMenuCiz();
           Sleep(30);
           continue;
        }
       
       if (aktifMod == MOD_BFS_YOL && !duraklatildi) {       /* 2) BFS yeþil yol animasyonu */
       	    if (kbhit()) {
                int kod;
                int genis = genisTusOku(&kod);
                if (!genis && kod == TUS_BOSLUK) {
                   duraklatildi = 1;
                   strcpy(durumMesaji, "Duraklatildi.");
                   durumYazisiniCiz();
                   altMenuCiz();
                continue;
               }
            }
            if (!bfsYolAdim()) {
               sprintf(durumMesaji,
               "BFS: Yol uzunlugu: %d | Toplam kesif: %d",
                yolUzunluk-1,
                toplamKesif);
                aktifMod = MOD_GEZ;
                duraklatildi = 1;
                altMenuCiz();
            }
        ekraniCiz();
        altMenuCiz();
        Sleep(50);
        continue;
        }
        
        if (aktifMod == MOD_DFS && !duraklatildi) {       /* 3) DFS animasyonu  <<< BURASI YENÝ */
        	if (kbhit()) {
               int kod;
               int genis = genisTusOku(&kod);
               if (!genis && kod == TUS_BOSLUK) {
                   duraklatildi = 1;
                   strcpy(durumMesaji, "Duraklatildi.");
                   durumYazisiniCiz();
                   altMenuCiz();
                continue;
                }
            }
            if (!dfsAdim()) {
                sprintf(durumMesaji,
                "DFS: Yol uzunlugu: %d | Toplam kesif: %d",
                dfsYolUzunluk,
                dfsKesif);
                aktifMod = MOD_GEZ;
                duraklatildi = 1;
                altMenuCiz();
            }
            ekraniCiz();
            altMenuCiz();
            Sleep(40);
            continue;
        }

        if (!kbhit()) {
            Sleep(10);
            continue;
        }

        int kod;
        int genis = genisTusOku(&kod);

        if (!genis) {
            if (kod == TUS_ESC){     /* NORMAL TUSLAR */
                cikisEkraniGoster();
                break;
            }    
            if (kod == TUS_BOSLUK) {
                if ((aktifMod == MOD_BFS ||                 /* SADECE algoritma çalýþýyorken duraklat */
                   aktifMod == MOD_BFS_YOL ||
                   aktifMod == MOD_DFS) && !duraklatildi) {

                    duraklatildi = 1;
                    strcpy(durumMesaji, "Duraklatildi.");
                    durumYazisiniCiz();
                    altMenuCiz();
                }
                else if ((aktifMod == MOD_BFS ||                      /* SADECE duraklatýlmýþken devam ettir */
                        aktifMod == MOD_BFS_YOL ||
                        aktifMod == MOD_DFS) && duraklatildi) {
                         duraklatildi = 0;
                         strcpy(durumMesaji, "Devam ediliyor.");
                         durumYazisiniCiz();
                         altMenuCiz();
                }
            }
        }
        else {         
              if (duraklatildi &&                                       /* duraklatildi iken sadece YON TUSLARI kilitli olsun */
                kod != 66 &&   /* F8 */
                kod != 63 &&   /* F5 */
                kod != 64 &&   /* F6 */
                kod != 68 &&   /* F10*/
                kod != TUS_BOSLUK)  /* SPACE */
                continue;
            
               if (kod == 72) imleciHareketEttir(-1, 0);      /* Yukari */             /* YON TUSLARI */ 
                else if (kod == 80) imleciHareketEttir(1, 0); /* Asagi */
                else if (kod == 75) imleciHareketEttir(0, -1);/* Sol */
                else if (kod == 77) imleciHareketEttir(0, 1); /* Sag */
                else if (kod == 59) { /* F1 */                                     /* MOD TUSLARI */
                    aktifMod = MOD_CIZ;
                    if (labirent[imlecSatir][imlecSutun] == HUCRE_BOS)
                       labirent[imlecSatir][imlecSutun] = HUCRE_YOL;
                       strcpy(durumMesaji, "Cizim modu: Yon tuslariyla (*) yolunu cizin.");
                       durumYazisiniCiz();
                       altMenuCiz();
                       imleciCiz();
                }
                else if (kod == 60) { /* F2 */
                    aktifMod = MOD_SIL;
                    strcpy(durumMesaji, "Silme modu: Hatali noktalari silin.");
                    durumYazisiniCiz();
                    altMenuCiz();
                    imleciCiz();
                }
                else if (kod == 61) { /* F3 */
                    aktifMod = MOD_GEZ;
                    strcpy(durumMesaji, "Gezme modu: Serbest dolasim.");
                    durumYazisiniCiz();
                    altMenuCiz();
                    imleciCiz();
                }
                else if (kod == 62) { /* F4 */
                    aktifMod = MOD_DUVAR;
                    strcpy(durumMesaji, "Duvar yerlestiriliyor...:");
                    tumBosluklariDuvarYap();
                    ekraniCiz();
                    durumYazisiniCiz();
                    altMenuCiz();
                    imleciCiz();
                }
                else if (kod == 63) { /* F5 */
                    labirentiKaydet();
                    durumYazisiniCiz();
                }
                else if (kod == 64) { /* F6 */
                    labirentiYukle();
                    durumYazisiniCiz();
                }
                else if (kod == 65) { /* F7 */
                    cozumTemizle();
                    tumYollariTemizle();
                    strcpy(durumMesaji, "Temizlendi.");
                    ekraniCiz();
                    durumYazisiniCiz();
                    altMenuCiz();
                    imleciCiz();
                }
                else if (kod == 66) { /* F8 */ 
                    if (duraklatildi) {
                       labirentiGeriYukle();
                    } else {
                       labirentiYedekle();
                    }
                    duraklatildi = 0;
                    /* algoritma seç */
                   if (siradakiAlgoritma == 0) {
                        strcpy(durumMesaji,
                        "Derinlik Oncelikli Arama (DFS)... Bosluk: durdur.");
                        dfsBaslat();
                        aktifMod = MOD_DFS;
                        siradakiAlgoritma = 1;
                   } else {
                        strcpy(durumMesaji,
                        "Genislik Oncelikli Arama (BFS)... Bosluk: durdur.");
                        bfsBaslat();
                        aktifMod = MOD_BFS;
                        siradakiAlgoritma = 0;
                    }
                    altMenuCiz();
                }
            
                else if (kod == 67) { /* F9 */
                    duraklatildi = 0;
                    cozumTemizle();
                    imlecSatir = baslangicSatir;
                    imlecSutun = baslangicSutun;
                    rastgeleLabirentOlustur();
                    aktifMod = MOD_GEZ;
                    strcpy(durumMesaji, "Rastgele labirent olusturuldu.");
                    ekraniCiz();
                    durumYazisiniCiz();
                    altMenuCiz();
                }
                
                else if (kod == 68) { /* F10 */
                    duraklatildi = 0;
                    aktifMod = MOD_GEZ;
                    cozumTemizle();
                    system("cls");
                    yeniBoyutAl();
                    system("cls");
                    disCerceveCiz();
                    ekraniCiz();
                    strcpy(durumMesaji, "Yeni boyut ayarlandi.");
                    durumYazisiniCiz();
                    altMenuCiz();
                    imleciCiz();
                }
	    }
        Sleep(20);
	}
    system("cls");
    printf("Programdan cikildi.\n");
    return 0;
}


    //Fonksiyonlar

int sinirlarIcindeMi(int s, int t)
{
    return (s >= 0 && s < yukseklik && t >= 0 && t < genislik);
}


void labirentiTemizle(void)
{
    int s, t;
    for (s = 0; s < MAKS_YUKSEKLIK; s++)
        for (t = 0; t < MAKS_GENISLIK; t++) {
            labirent[s][t] = HUCRE_BOS;
            cozum[s][t] = 0;
        }
}


void labirentiBaslat(void)
{
    labirentiTemizle();

    baslangicSatir = yukseklik - 2;
    baslangicSutun = 0;

    cikisSatir = 1;
    cikisSutun = genislik - 1;

    imlecSatir = baslangicSatir;
    imlecSutun =baslangicSutun;
    
    labirent[baslangicSatir][baslangicSutun] = HUCRE_YOL;

    strcpy(durumMesaji,"Cizim modu: yol cizin.");
}


void yeniBoyutAl(void)
{
    int d;
    while (1) {
        system("cls");
        printf("Labirent genisligi (30-70): ");
        if (scanf("%d",&d)!=1 || d<30 || d>70) {
            printf("Hata!\n"); getch(); while(getchar()!='\n');
            continue;
        }
        genislik = d;
        break;
    }
    while (1) {
        system("cls");
        printf("Labirent yuksekligi (5-15): ");
        if (scanf("%d",&d)!=1 || d<5 || d>15) {
            printf("Hata!\n"); getch(); while(getchar()!='\n');
            continue;
        }
        yukseklik = d;
        break;
    }
    while(getchar()!='\n');
    labirentiBaslat();
}


void cizimUygula(void)
{
    if (labirent[imlecSatir][imlecSutun] != HUCRE_DUVAR)
        labirent[imlecSatir][imlecSutun] = HUCRE_YOL;
}


void silmeUygula(void)
{
    if (labirent[imlecSatir][imlecSutun] == HUCRE_YOL)
        labirent[imlecSatir][imlecSutun] = HUCRE_BOS;
}


void tumYollariTemizle(void)
{
    int s,t;
    for (s=0;s<yukseklik;s++)
        for (t=0;t<genislik;t++)
            if (labirent[s][t] == HUCRE_YOL)
                labirent[s][t] = HUCRE_BOS;
}


void imleciHareketEttir(int ds, int dt)
{
    int eskiS = imlecSatir;
    int eskiT = imlecSutun;

    int yeniS = imlecSatir + ds;
    int yeniT = imlecSutun + dt;

    if (!sinirlarIcindeMi(yeniS, yeniT))
        return;

    imlecSatir = yeniS;           //ÝMLECÝ TASÝ
    imlecSutun = yeniT;
    
    if (aktifMod == MOD_CIZ) cizimUygula();           /*  Mod iþlemini UYGULA (önemli!) */
    else if (aktifMod == MOD_SIL) silmeUygula();
    else if (aktifMod == MOD_DUVAR)  duvarUygula();

    hucreyiYenidenCiz(eskiS, eskiT);       /*Eski hücreyi GERÇEK içeriðiyle geri çiz */
    
    imleciCiz();               /*Yeni imleci çiz */
    
    if (aktifMod != 99)
    imleciCiz();

}


void ekraniCiz(void)
{
    int s,t;
    char LU=(char)218, RU=(char)191, LD=(char)192, RD=(char)217;
    char Y=(char)196, D=(char)179;

    imlecGit(IC_X, IC_Y);  

    putchar(LU);                      //ust cerceve
	for(t=0;t<genislik;t++)putchar(Y);
    putchar(RU);

    for(s=0;s<yukseklik;s++){   
    	imlecGit(IC_X, IC_Y + 1 + s);               //ic alan
         
        if (s == baslangicSatir){         /* SOL DUVAR (giriþ burada) */
		 renkAyarla(RENK_YESIL);
		 putchar('>');
		}
        else {
        renkAyarla(RENK_GRI);	
		putchar(D);
		}

        for (t = 0; t < genislik; t++) {

        if (labirent[s][t] == HUCRE_YESIL) {
            renkAyarla(RENK_YESIL);
            putchar('*');
        }
        else if (labirent[s][t] == HUCRE_YOL) {
            renkAyarla(RENK_SARI);
            putchar('*');
        }
        else if (labirent[s][t] == HUCRE_BFS) {   /* <<< EKLE */
            renkAyarla(RENK_SARI);
            putchar('.');
        }
        else if (labirent[s][t] == HUCRE_DUVAR) {
                SetConsoleTextAttribute(konsol, RENK_DUVAR);
                putchar(' ');   // <<< DUVAR = BOÞLUK
                SetConsoleTextAttribute(konsol, RENK_BEYAZ);
        }
        else {
             renkAyarla(RENK_BEYAZ);
             putchar(' ');
        }
    }
        
        if (s == cikisSatir){            /* SAÐ DUVAR (çýkýþ burada) */
        	renkAyarla(RENK_KIRMIZI);
		    putchar('>');
		}
		 
        else {
        	renkAyarla(RENK_GRI);
            putchar(D);
       }
   }

    imlecGit(IC_X, IC_Y + yukseklik + 1);
    renkAyarla(RENK_GRI);
    putchar(LD); 
	for(t=0;t<genislik;t++) putchar(Y);
    putchar(RD);
    
    durumYazisiniCiz();

    renkAyarla(RENK_BEYAZ);
    imlecGit(IC_X, IC_Y + yukseklik + 5);
    
    if (aktifMod != 99)
    
    if (aktifMod == MOD_CIZ ||
        aktifMod == MOD_SIL ||
        aktifMod == MOD_DUVAR) {
        imleciCiz();
    }

}


int genisTusOku(int *kod)
{
    int ch=getch();
    if(ch==0 || ch==224){ *kod=getch(); return 1; }
    *kod=ch; return 0;
}


void konsolImleciniGizle(void)
{
    CONSOLE_CURSOR_INFO imlec;
    imlec.dwSize = 1;        /* Boyutu önemsiz */
    imlec.bVisible = FALSE; /* Görünmez yap */
    SetConsoleCursorInfo(konsol, &imlec);
}


void imlecGit(int x, int y)
{
    COORD pozisyon;
    pozisyon.X = x;
    pozisyon.Y = y;
    SetConsoleCursorPosition(konsol, pozisyon);
}


void hucreyiYenidenCiz(int s, int t)
{
    imlecGit(IC_X + 1 + t, IC_Y + 1 + s);

    if (labirent[s][t] == HUCRE_YOL) {
        renkAyarla(RENK_SARI);
        putchar('*');
    }
    else if (labirent[s][t] == HUCRE_BFS) {
        renkAyarla(RENK_SARI);
        putchar('.');
    }
    else if (labirent[s][t] == HUCRE_DUVAR) {
        SetConsoleTextAttribute(konsol, RENK_DUVAR);
        putchar(' ');
        SetConsoleTextAttribute(konsol, RENK_BEYAZ);
    }
    else {
        renkAyarla(RENK_BEYAZ);
        putchar(' ');
    }
    renkAyarla(RENK_BEYAZ);
}


void renkAyarla(WORD renk)
{
    SetConsoleTextAttribute(konsol, renk);
}


void disCerceveCiz(void)
{
    int x, y;
    int gen = IC_X + (genislik + 2) + 4; /* saðda 2 boþluk */
    int yuk = IC_Y + (yukseklik + 2) + 8; /* yazýlar için alan */


    char LU = (char)201;
    char RU = (char)187;
    char LD = (char)200;
    char RD = (char)188;
    char Y  = (char)205;
    char D  = (char)186;

    renkAyarla(RENK_GRI);

    imlecGit(0,0);
    putchar(LU);
    for (x=0; x<gen; x++) putchar(Y);
    putchar(RU);

    for (y=1; y<yuk; y++) {
        imlecGit(0,y); putchar(D);
        imlecGit(gen+1,y); putchar(D);
    }

    imlecGit(0,yuk);
    putchar(LD);
    for (x=0; x<gen; x++) putchar(Y);
    putchar(RD);
}


void duvarUygula(void)
{
    if (labirent[imlecSatir][imlecSutun] == HUCRE_BOS)
        labirent[imlecSatir][imlecSutun] = HUCRE_DUVAR;
}


void imleciCiz(void)
{
if (aktifMod == MOD_BFS || aktifMod == MOD_BFS_YOL || aktifMod == MOD_DFS)
    return;

    char alt = labirent[imlecSatir][imlecSutun];

    imlecGit(IC_X + 1 + imlecSutun, IC_Y + 1 + imlecSatir);
    renkAyarla(RENK_KIRMIZI);

    if (aktifMod == MOD_GEZ) {
        /* boþsa +, yýldýzýn üstündeyse yýldýz KIRMIZI görünür */
        if (alt == HUCRE_YOL) putchar(HUCRE_YOL);   /* kýrmýzý * */
        else if (alt == HUCRE_BOS) putchar('+');   /* kýrmýzý + */
        else putchar(alt);                         /* duvar vs */
    }
    else if (aktifMod == MOD_SIL) {
        putchar('_'); /* kýrmýzý alt çizgi */
    }
    else if (aktifMod == MOD_CIZ) {
        putchar(HUCRE_YOL); /* kýrmýzý imleç yýldýzý */
    }
    else if (aktifMod == MOD_DUVAR) {
        putchar(' ');
    }
    else {
        putchar('+');
    }

    renkAyarla(RENK_BEYAZ);
}


void durumYazisiniCiz(void)
{
    int y = IC_Y + yukseklik + 3;
    char *ikiNokta;
    
    imlecGit(IC_X, y);   /* Satiri temizle */
    printf("                                                            ");

    imlecGit(IC_X, y);
    printf(">> ");
   
    if (strcmp(durumMesaji, "Duraklatildi.") == 0 ||          /* DURAKLAT / DEVAM yazýlarýný kýrmýzý yap */
        strcmp(durumMesaji, "Devam ediliyor.") == 0) {
            imlecGit(IC_X, y);
            printf(">> ");
            renkAyarla(RENK_KIRMIZI);
            printf("%s", durumMesaji);
            renkAyarla(RENK_BEYAZ);
        return;
    }
   
    ikiNokta = strchr(durumMesaji, ':');

    if (aktifMod == MOD_GEZ &&
        (strstr(durumMesaji, "BFS") != NULL ||
        strstr(durumMesaji, "DFS") != NULL)) {
        imlecGit(IC_X, y);
        printf(">> ");
        renkAyarla(RENK_YESIL);
        printf("%s", durumMesaji);
        renkAyarla(RENK_BEYAZ);
        return;
    }


    if (ikiNokta) {
        renkAyarla(RENK_KIRMIZI);       /* KIRMIZI: baslik kismi (iki noktaya kadar) */
        fwrite(durumMesaji, 1, ikiNokta - durumMesaji + 1, stdout);

        renkAyarla(RENK_BEYAZ);          /* BEYAZ: aciklama */
        printf("%s", ikiNokta + 1);
    }
    else {
        renkAyarla(RENK_BEYAZ);          /* ':' yoksa hepsi beyaz */
        printf("%s", durumMesaji);
    }

    renkAyarla(RENK_BEYAZ);
}


void altMenuCiz(void)
{
    int y = IC_Y + yukseklik + 5;
    imlecGit(IC_X, y);

    /* F1 */
    renkAyarla(aktifMod == MOD_CIZ ? RENK_KIRMIZI : RENK_BEYAZ);
    printf("F1: Ciz ");
    renkAyarla(RENK_BEYAZ);
    printf("| ");

    /* F2 */
    renkAyarla(aktifMod == MOD_SIL ? RENK_KIRMIZI : RENK_BEYAZ);
    printf("F2: Sil ");
    renkAyarla(RENK_BEYAZ);
    printf("| ");

    /* F3 */
    renkAyarla(aktifMod == MOD_GEZ ? RENK_KIRMIZI : RENK_BEYAZ);
    printf("F3: Gez ");
    renkAyarla(RENK_BEYAZ);
    printf("| ");

    /* F4 */
    renkAyarla(aktifMod == MOD_DUVAR ? RENK_KIRMIZI : RENK_BEYAZ);
    printf("F4: Duvar ");

    renkAyarla(RENK_BEYAZ);
    printf("| F5: Kaydet | F6: Yukle");

    imlecGit(IC_X, y + 1);
    printf("F7: Temizle | F8: En kisa yol bul | F9: Rastgele | F10: Yeni boyut");

   imlecGit(IC_X, y + 2);

    if (duraklatildi){
        renkAyarla(RENK_KIRMIZI);
        printf("Bosluk: Duraklat");
        renkAyarla(RENK_BEYAZ);
        printf(" | Esc: Cikis");
    }
    
    else {
        printf("Bosluk: Duraklat | Esc: Cikis");
    }
    renkAyarla(RENK_BEYAZ);
}


void tumBosluklariDuvarYap(void)
{
    int s, t;
    for (s = 0; s < yukseklik; s++) {
        for (t = 0; t < genislik; t++) {
            if (labirent[s][t] == HUCRE_BOS)
                labirent[s][t] = HUCRE_DUVAR;
        }
    }
}


void bfsBaslat(void)
{
	toplamKesif = 0;
    int s, t;
    tumYollariTemizle();
    
    for (s = 0; s < yukseklik; s++)   /* dizileri sýfýrla */
        for (t = 0; t < genislik; t++) {
            bfsZiyaret[s][t] = 0;
            parentS[s][t] = -1;
            parentT[s][t] = -1;
        }

    qBas = qSon = 0;

    int bs = baslangicSatir;
    int bt = baslangicSutun;

    kuyruk[qSon++] = (Dugum){bs, bt};
    bfsZiyaret[bs][bt] = 1;
    bfsBulundu = 0;     
    yolUzunluk = 0; 
    
}


int bfsAdim(void)
{
    if (bfsBulundu)
        return 0;

    if (qBas == qSon)
        return 0;   // kuyruk bitti  BFS bitti

    Dugum d = kuyruk[qBas++];
    int s = d.s;
    int t = d.t;

    int ds[4] = {-1, 1, 0, 0};
    int dt[4] = {0, 0, -1, 1};
    int i;
    for (i = 0; i < 4; i++) {
        int ns = s + ds[i];
        int nt = t + dt[i];

        if (!sinirlarIcindeMi(ns, nt))
            continue;

        if (bfsZiyaret[ns][nt])
            continue;

        if (labirent[ns][nt] == HUCRE_DUVAR)
            continue;

        bfsZiyaret[ns][nt] = 1;
        toplamKesif++;   
        parentS[ns][nt] = s;
        parentT[ns][nt] = t;

        kuyruk[qSon++] = (Dugum){ns, nt};

        /* ---- ÇIKIÞ BULUNDU ---- */
        if (ns == cikisSatir && nt == cikisSutun) {

            bfsBulundu = 1;
            yolUzunluk = 0;

            int s2 = ns;
            int t2 = nt;

            /* çýkýþtan baþlangýca parent ile geri git */
            while (!(s2 == baslangicSatir && t2 == baslangicSutun)) {
                yolDiziS[yolUzunluk] = s2;
                yolDiziT[yolUzunluk] = t2;
                yolUzunluk++;

                int ps = parentS[s2][t2];
                int pt = parentT[s2][t2];
                s2 = ps;
                t2 = pt;
            }

            /* baþlangýç hücresini de ekle */
            yolDiziS[yolUzunluk] = baslangicSatir;
            yolDiziT[yolUzunluk] = baslangicSutun;
            yolUzunluk++;

            /* yeþil çizim baþlangýçtan baþlasýn */
            yolIndex = yolUzunluk - 1;

            return 0;   // BFS yayýlmasý burada biter
        }

        /* sarý BFS noktasý */
        if (labirent[ns][nt] == HUCRE_BOS)
            labirent[ns][nt] = HUCRE_BFS;
    }

    return 1;   // BFS devam ediyor
}



void bfsNoktalariTemizle(void)
{
    int s, t;
    for (s = 0; s < yukseklik; s++)
        for (t = 0; t < genislik; t++)
            if (labirent[s][t] == HUCRE_BFS)
                labirent[s][t] = HUCRE_BOS;
}


void cozumTemizle(void)
{
    int s, t;
    for (s = 0; s < yukseklik; s++) {
        for (t = 0; t < genislik; t++) {

            /* DFS / BFS sonucu çizilen yeþiller */
            if (labirent[s][t] == HUCRE_YESIL)
                labirent[s][t] = HUCRE_YOL;

            /* BFS yayýlma noktalarý */
            else if (labirent[s][t] == HUCRE_BFS)
                labirent[s][t] = HUCRE_BOS;
        }
    }
}


void labirentiYedekle(void)
{
    int s, t;
    for (s = 0; s < yukseklik; s++)
        for (t = 0; t < genislik; t++)
            labirentYedek[s][t] = labirent[s][t];
}


void labirentiGeriYukle(void)
{
    int s, t;
    for (s = 0; s < yukseklik; s++)
        for (t = 0; t < genislik; t++)
            labirent[s][t] = labirentYedek[s][t];
}


int bfsYolAdim(void)
{
    if (yolIndex < 0)
        return 0;

    int s = yolDiziS[yolIndex];
    int t = yolDiziT[yolIndex];

    labirent[s][t] = HUCRE_YESIL;
    yolIndex--;

    return 1;
}


void dfsBaslat(void)
{
    int s, t;

    dfsBulundu = 0;
    dfsYolUzunluk = 0;
    dfsKesif = 0;

    tumYollariTemizle();

    for (s = 0; s < yukseklik; s++)
        for (t = 0; t < genislik; t++)
            dfsZiyaret[s][t] = 0;

    dfsTop = -1;

    dfsTop++;
    dfsStackS[dfsTop] = baslangicSatir;
    dfsStackT[dfsTop] = baslangicSutun;
    dfsZiyaret[baslangicSatir][baslangicSutun] = 1;
}


int dfsAdim(void)
{
    if (dfsTop < 0)
        return 0;   // stack boþ  DFS bitti

    int s = dfsStackS[dfsTop];
    int t = dfsStackT[dfsTop];
    
    if (s == cikisSatir && t == cikisSutun) {        /* çýkýþ bulundu mu? */
        dfsBulundu = 1;
        return 0;
    }

    int ds[4] = {0, 1, 0, -1};
    int dt[4] = {1, 0, -1, 0};

    int i;
    for (i = 0; i < 4; i++) {
        int ns = s + ds[i];
        int nt = t + dt[i];

        if (!sinirlarIcindeMi(ns, nt))
            continue;

        if (dfsZiyaret[ns][nt])
            continue;

        if (labirent[ns][nt] == HUCRE_DUVAR)
            continue;
       
        dfsTop++;                 /* ileri git */
        dfsStackS[dfsTop] = ns;
        dfsStackT[dfsTop] = nt;

        dfsZiyaret[ns][nt] = 1;
        dfsKesif++;

        labirent[ns][nt] = HUCRE_YESIL;  // YEÞÝL yol
        dfsYolUzunluk++;

        return 1;  // SADECE 1 adým
    }
 
    labirent[s][t] = HUCRE_BFS; // sarý nokta      /* çýkmaz sokak  geri dön */
    dfsKesif++;
    dfsTop--;

    return 1;
}


void rastgeleLabirentOlustur(void)
{
    int s, t;

    for (s = 0; s < yukseklik; s++)           /* 1) Her yeri duvar yap */
        for (t = 0; t < genislik; t++)
            labirent[s][t] = HUCRE_DUVAR;
    
    int stackS[5000], stackT[5000];          /* 2) DFS stack */
    int top = 0;

    stackS[top] = baslangicSatir;
    stackT[top] = baslangicSutun;
    top++;

    labirent[baslangicSatir][baslangicSutun] = HUCRE_YOL;

    int ds[4] = {-1, 1, 0, 0};
    int dt[4] = {0, 0, -1, 1};

    while (top > 0) {
        int curS = stackS[top - 1];
        int curT = stackT[top - 1];

        int dirs[4] = {0,1,2,3};
        int i;
        for (i = 0; i < 4; i++) {
            int r = rand() % 4;
            int tmp = dirs[i];
            dirs[i] = dirs[r];
            dirs[r] = tmp;
        }

        int moved = 0;

        for (i = 0; i < 4; i++) {
            int ns = curS + ds[dirs[i]] * 2;
            int nt = curT + dt[dirs[i]] * 2;

            if (!sinirlarIcindeMi(ns, nt))
                continue;

            if (labirent[ns][nt] == HUCRE_DUVAR) {
                labirent[curS + ds[dirs[i]]][curT + dt[dirs[i]]] = HUCRE_YOL;
                labirent[ns][nt] = HUCRE_YOL;

                stackS[top] = ns;
                stackT[top] = nt;
                top++;

                moved = 1;
                break;
            }
        }

        if (!moved)
            top--;
    }

    labirent[cikisSatir][cikisSutun] = HUCRE_YOL;       /* 3) Çýkýþý garanti et */
}


void labirentiKaydet(void)
{
	CONSOLE_CURSOR_INFO imlec;
    imlec.dwSize = 10;
    imlec.bVisible = TRUE;
    SetConsoleCursorInfo(konsol, &imlec);

    char dosyaAdi[64];
    FILE *fp;

    imlecGit(IC_X, IC_Y + yukseklik + 4);
    renkAyarla(RENK_KIRMIZI);
    printf("Kaydet - dosya adi: ");
    renkAyarla(RENK_BEYAZ);

    scanf("%63s", dosyaAdi);

    fp = fopen(dosyaAdi, "w");
    time_t simdi = time(NULL);
    struct tm *tm = localtime(&simdi);

    fprintf(fp, "# %02d.%02d.%04d %02d:%02d\n",          /* tarih - saat */
        tm->tm_mday,
        tm->tm_mon + 1,
        tm->tm_year + 1900,
        tm->tm_hour,
        tm->tm_min);

    if (!fp) {
        strcpy(durumMesaji, "Hata: Dosya olusturulamadi!");
        return;
    }

    fprintf(fp, "%d %d\n", genislik, yukseklik);          /* boyutlarý yaz */
    int s,t;
    
    for (s = 0; s < yukseklik; s++) {              /* labirenti yaz */
        for (t = 0; t < genislik; t++) {
            char c = labirent[s][t];
            
            if (c == HUCRE_YESIL || c == HUCRE_BFS)       /* çözüm izlerini kaydetme */
                c = HUCRE_YOL;

            fputc(c, fp);
        }
        fputc('\n', fp);
    }

    fclose(fp);

    strcpy(durumMesaji, "Basarili: kaydedildi.");
    imlec.bVisible = FALSE;
    SetConsoleCursorInfo(konsol, &imlec);

}


void labirentiYukle(void)
{
	CONSOLE_CURSOR_INFO imlec;
    imlec.dwSize = 10;
    imlec.bVisible = TRUE;
    SetConsoleCursorInfo(konsol, &imlec);

    char dosyaAdi[64];
    FILE *fp;

    imlecGit(IC_X, IC_Y + yukseklik + 4);
    renkAyarla(RENK_YESIL);
    printf("Yukle - dosya adi: ");
    renkAyarla(RENK_BEYAZ);

    scanf("%63s", dosyaAdi);

    fp = fopen(dosyaAdi, "r");
    char tarihSatiri[64];
    fgets(tarihSatiri, sizeof(tarihSatiri), fp); // # tarih satýrý

    if (!fp) {
        strcpy(durumMesaji, "Hata: Dosya bulunamadi!");
        return;
    }

    fscanf(fp, "%d %d\n", &genislik, &yukseklik);
    int s,t;
    for (s = 0; s < yukseklik; s++) {
        for (t = 0; t < genislik; t++) {
            labirent[s][t] = fgetc(fp);
        }
        fgetc(fp); /* \n */
    }

    fclose(fp);

    baslangicSatir = yukseklik - 2;       /* baþlangýç / çýkýþ yeniden ayarlanýr */
    baslangicSutun = 0;
    cikisSatir = 1;
    cikisSutun = genislik - 1;

    aktifMod = MOD_GEZ;
    duraklatildi = 0;

    sprintf(durumMesaji, "Yuklendi:%s", tarihSatiri + 1);
    ekraniCiz();
    imlec.bVisible = FALSE;
    SetConsoleCursorInfo(konsol, &imlec);
}


void yaziAnimasyonluYaz(const char *metin, int gecikme)
{
    while (*metin) {
        putchar(*metin);
        fflush(stdout);      // anýnda ekrana bas
        Sleep(gecikme);      // ms cinsinden bekle
        metin++;
    }
}


void cikisEkraniGoster(void)
{
    system("cls");

    renkAyarla(RENK_BEYAZ);

    printf("\n\n");

    yaziAnimasyonluYaz(
        "  Belki dunyaya ilk adimi atan degilim,\n"
        "  Ama attigim her adimin arkasinda duruyorum.\n\n",
        40
    );

    yaziAnimasyonluYaz(
        "  Bazen duruyorum,"
        " bazen yanlis yone gidiyorum,\n"
        "  Bazen de duvara carpiyorum.\n"
        "  Ama her seferinde yeniden deniyorum.\n\n",
        40
    );

    yaziAnimasyonluYaz(
        "  Yol her zaman acik degil,\n"
        "  En kisa yol her zaman en dogru yol da degil.\n"
        "  Ama vazgecmek, hicbir yol degil.\n\n",
        40
    );

    yaziAnimasyonluYaz(
        "  Dunya donmeye devam ederken\n"
        "  Ben de ogrenmeye devam ediyorum.\n"
        "  Yanilarak, durarak, tekrar baslayarak.\n\n",
        40
    );

    yaziAnimasyonluYaz(
        "  Bu bir son degil.\n"
        "  Bu sadece bir nefes.\n\n",
        70
    );

    renkAyarla(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    yaziAnimasyonluYaz("  Cikmak icin herhangi bir tusa basin...", 20);
    renkAyarla(RENK_BEYAZ);

    getch();
}

