#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>
//#include "plateRec.h"

typedef struct settings {
	int tot_sez;
	int ImageThreshold;
	int MatrixThreshold;
	int s_black;
	int s_patt;
} settings;

// Algoritmo  che permette di andare a trovare i pattern della targa lungo le
// righe dell'immagine.

int platePatternRec(int **m, int width, int height,
		     	      int s_white, int s_black, int s_patt,
			      int maxCandidate,
			      plateCandidate **c)
{

	int i, sec;
	int w, b, p;
	int reading = 0; // Se è vero vuol dire che stiamo leggendo alla ricerca dell patt.
	int cand_find = 0;
	int start = 0;

	// Inizializzazione della lista delle sezioni candidate.
	*c = (plateCandidate *) malloc (sizeof(plateCandidate) * maxCandidate);
	for (i = 0; i < maxCandidate; i++) {
		(*c)[i].sec = 0;
		(*c)[i].start = 0;
		(*c)[i].end = 0;
	}

	// Ricerca dei candidati.
	for (sec = 0; sec < height; sec++) {
		// Comincia la ricerca sulla riga
		w = 0;
		b = 0;
		p = 0;
		reading = 0;
		start = 0;
		for (i = 0; i < width; i++) {
			if (m[i][sec] == 255) {		//-White
				if (!reading)
					start = i;
				reading = 1;		//   Siamo in lettura
				b = 0;			//   Resettiamo il contatore black
				w++;			//   ++ white countunter
				p++;			//   ++ pattern count.
			} else {			//-Black
				if (reading) { 		// * Se siamo in lettura
					b++;
					w = 0;
					p++;
					if (b > s_black) {
						reading = 0;
						if (p > s_patt) {
							// salva il candidato se
							// possibile.
							if (cand_find < maxCandidate) {
								//printf("\n Candidate %d:"
								//	" S: %d, St: %d, En: %d\n",
								//	 cand_find, sec, start,
								//	 i - (3 * s_black / 4));
								(*c)[cand_find].sec = sec;
								(*c)[cand_find].start = start;
								(*c)[cand_find].end = i - (4 * s_black / 5);
								cand_find++;

							}
						}
						b = 0;
						p = 0;
						start = 0;
					}
				}
			}
		}
	}
	return cand_find;
}


// Segmentazione dell'immagine (non più utilizzata), creazione della matrice
// delta ed infine ricerca del pattern della targa.
int segmentation(IplImage *img_8uc1, char *label, settings *var, plateCandidate **candidates)
{

	IplImage* img_edge = cvCreateImage( cvGetSize(img_8uc1), 8, 1 );	// Image for Threshold
	IplImage* img_delta = cvCreateImage( cvGetSize(img_8uc1), 8, 1 );	// Image to draw DeltaM

	int height,width,step; 							// img_edge infos
	int sec_height;								// section height
	uchar *data;								// img_edge data
	int i, j, sez;								// some counter for loops
	int **matrice; 								// Matrix of colour average
	int **deltam;								// Matrix of colour delta
	int label_step = 0;

	int imageThreshold = var->ImageThreshold;
	int tot_sez = var->tot_sez;
	int matrixThreshold = var->MatrixThreshold;

	cvSmooth(img_8uc1, img_edge, CV_GAUSSIAN, 3, 0, 0, 0);				// Clean Image using smooth
	cvThreshold(img_edge, img_edge, imageThreshold, 255, CV_THRESH_BINARY );	// Magic Threshold :D
	saveImage(img_edge, label, ++label_step);					// Save first step

	height = img_edge->height;
	width = img_edge->width;
	step = img_edge->widthStep;
	data = (uchar *)img_edge->imageData;
	sec_height = height / tot_sez;

	// Create and Initialize "matrice" data
	matrice = (int **) malloc (sizeof(int *) * width);
	for (i = 0; i < width; i++)
		matrice[i] = (int *) malloc (sizeof(int) * tot_sez);

	for (i = 0;i < width; i++)
		for (j = 0; j < tot_sez; j++)
			matrice[i][j] = 0;

	// Create and Initialize "deltam" data
	deltam = (int **) malloc (sizeof(int *) * width);
	for (i = 0; i < width; i++)
		deltam[i] = (int *) malloc (sizeof(int) * tot_sez);

	for (i = 0; i < width; i++)
		for(j = 0; j < tot_sez; j++)
			deltam[i][j] = 0;

	// Fill up "matrice"
	for (sez = 0; sez < tot_sez; sez++) {
		for (j = 0; j < width; j++) {
			for (i = sez * sec_height; i < (sec_height * (sez + 1)); i++) {
				matrice[j][sez] += data[i*step + j];
			}
			matrice[j][sez] = matrice[j][sez] / sec_height;
		}
	}
	matrixBinaryThreshold(matrice, width, tot_sez, matrixThreshold, 0, 255);

	// Fill up "DeltaM"
	for (sez = 0; sez < tot_sez; sez++)
		for (i = 0; i < (width - 1); i++)
			deltam[i][sez] = abs(matrice[i][sez] -
					     matrice [i+1][sez]);
	// Draw "DeltaM"
	for (sez = 0; sez < tot_sez; sez++) {
		for (i = 0; i < (width-1); i++){
				j = deltam[i][sez];
				cvLine(img_delta, cvPoint(i, sez * sec_height),
			       		cvPoint(i, (sez+1) * sec_height),
			                cvScalar(j, 0, 0, 0),
		       			1, 8, 0);
		}
	}
	saveImage(img_delta, label, ++label_step);

	// Ricerca della posizione della targa
	return platePatternRec(deltam, width, tot_sez,
		     	      100, var->s_black, var->s_patt,
			      500, candidates);
}

// Funzione che permette di unire le righe candidate così da generare le aree
// candidate ad essere targhe.
int uniteCandidates(areaCandidate **areas, plateCandidate *candidates, const int tot_cand,
		    const int sec_height)
{
	int tot_areas = 0;
	int i, sec, j;
	int delta, startx, starty, endx, endy, lenght;

	*areas = (areaCandidate *) malloc (sizeof(areaCandidate) * tot_cand);

	for (i = 0; i < tot_cand; i++) {
		if (candidates[i].sec > 0) {
			// Inizializzazione dei dati.
			startx = candidates[i].start;
			starty = candidates[i].sec * sec_height;
			endx = candidates[i].end;
			endy = (candidates[i].sec + 1) * sec_height;
			lenght = endx - startx;
			sec = candidates[i].sec;

			// Ricerca dei candidati che è possibile unire.
			for (j = i + 1; j < tot_cand; j++) {
				if (candidates[j].sec > 0) {
					if (candidates[j].sec <= (sec + 10)) {
						delta = abs(candidates[j].start - startx);
						if (delta < lenght ) {
							// Unione dei candidati
							// Aggiornamento dei
							// dati dell'area
							startx = minNumber(startx, candidates[j].start);
							endx = maxNumber(endx, candidates[j].end);
							endy = (candidates[j].sec + 1) * sec_height;
							sec = candidates[j].sec;
							candidates[j].sec *= -1;
						}

					}
				}
			 }
			// Create Area
			starty -= sec_height;
			endy += sec_height;
			(*areas)[tot_areas].start = cvPoint(startx, starty);
			(*areas)[tot_areas].end = cvPoint(endx, endy);
			tot_areas++;
		}
	}
	return tot_areas;
}

// Conversione e selezione delle aree candidate.
int selectRectsFromAreas(areaCandidate *areas, int tot_areas, CvRect **rects,
			int ratio, int min_height, int max_height, int min_width, int max_width)
{
	int i;
	int tot_selected = 0;
	CvRect *rect;

	*rects = (CvRect *) malloc (sizeof(CvRect) * tot_areas);

	for (i = 0; i < tot_areas; i++) {
		rect = areaToRect(&areas[i]);
		if (rect->width < max_width && rect->width > min_width) {
			if (rect->height > min_height && rect->height < max_height) {
				if (rect->width / rect->height < ratio) {
					(*rects)[tot_selected].x = rect->x;
					(*rects)[tot_selected].y = rect->y;
					(*rects)[tot_selected].height = rect->height;
					(*rects)[tot_selected].width = rect->width;
					printf("\n Selected area %d (%d, %d) [h:%d, w:%d]", i,
							                                     rect->x,
											     rect->y,
											     rect->height,
											     rect->width);
					tot_selected++;
				}
			}
		}
		free(rect);
	}
	return tot_selected;
}

// Funzione in cui vengono ritagliate le porzioni di immagine candidate ad
// essere targhe

int cutCandidateImage(IplImage *img_8uc1, IplImage *img_8uc3, IplImage ***dest , settings *var)
{

	IplImage *img_candidates = cvCreateImage( cvGetSize(img_8uc1), 8, 3 );
	IplImage *img_areas = cvCreateImage( cvGetSize(img_8uc1), 8, 3 );
	IplImage **plates;
	CvRect *rects;

	plateCandidate *candidates;
	areaCandidate *areas;
	int i, img_height, sec_height;
	int tot_cand = 0;
	int tot_areas = 0;
	int tot_rects = 0;
	int tot_sez = var->tot_sez;

	img_height = img_8uc1->height;
	if (img_height < tot_sez) {
		tot_sez = img_height;
		var->tot_sez = img_height;
	}
	sec_height = img_height / tot_sez;

	tot_cand = segmentation(img_8uc1, 		// start image
				"first",		// filename label
				var,			// var
				&candidates);


	// Disegna le linee candidate
	cvCvtColor(img_8uc1, img_candidates, CV_GRAY2BGR );
	for (i = 0; i < tot_cand; i++)
		cvRectangle(img_candidates,
			    cvPoint(candidates[i].start, (candidates[i].sec) * sec_height),
			    cvPoint(candidates[i].end, ((candidates[i].sec) + 1) * sec_height),
			    CV_RGB(250,0,0), 1, 8, 0);
	saveImage(img_candidates, "main", 0);

	tot_areas = uniteCandidates(&areas, candidates, tot_cand, sec_height);

	tot_rects = selectRectsFromAreas(areas, tot_areas, &rects,
					 10, 20, 200, 50, 400);

	// Disegna i rettangoli che potrebbero rappresentare la targa.
	cvCvtColor(img_8uc1, img_areas, CV_GRAY2BGR );
	for (i = 0; i < tot_rects; i++)
		cvRectangle(img_areas,
			    cvPoint(rects[i].x, rects[i].y),
			    cvPoint(rects[i].x + rects[i].width, rects[i].y + rects[i].height),
			    CV_RGB(250,0,0), 1, 8, 0);
	saveImage(img_areas, "main", 1);


	// Crea nuove le nuove immagini dai rettangoli candidati.
	plates = (IplImage **) malloc (sizeof(IplImage *) * tot_rects);
	for (i = 0; i < tot_rects; i++) {
		plates[i] = cutImageFromRect(img_8uc3, &rects[i], 3);
	}

	*dest = plates;
	return tot_rects;

}


// Funzione di interfaccia per il main.c
int platesCandidates(IplImage *img_8uc3, settings *v, IplImage ***p)
{

	IplImage *img_8uc1 = cvCreateImage(cvGetSize(img_8uc3), 8, 1);			// Loaded Image
	int tot_plates = 0;
	int i;

	// Converti l'immagine in scala di grigi
	cvCvtColor(img_8uc3, img_8uc1, CV_RGB2GRAY);

	// Scaling delle immagini alle dimensioni 640x480
	img_8uc1 = scaleImage(img_8uc1, 640, 480, 1);
	img_8uc3 = scaleImage(img_8uc3, 640, 480, 3);

	// Ricerca delle aree candidate ad essere la targa
	tot_plates = cutCandidateImage(img_8uc1, img_8uc3, p, v);

	// Salvataggio dei candidati
	for (i = 0; i < tot_plates; i++) {
		saveImage((*p)[i], "plants", i);
	}

	return tot_plates;
}

