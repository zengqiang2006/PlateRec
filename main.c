
/*
 * PlateRec: A "simply" and crazy way to stringify your car plate
 *
 * Copyright (C) Rosario Contarino <contarino {dot} rosario {at} gmail {dot} com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <math.h>

#include "plateRec.h"
#include "plateCandidates.h"


int main(int argc, char* argcv[])
{
	IplImage* img;
	IplImage **plates;

	settings var;

	int tot_plates = 0;

	// Viene caricata l'immagine come primo parametro del programma.
	// L'immagine viene caricata forzandola a 3 canali (RGB).
	img = cvLoadImage(argcv[1], 1);
	if (!img) {
		printf("\n \n  ##  IMPOSSIBILE APRIRE L'IMMAGINE ## \n \n");
		exit(1);
	}

	// Impostazioni che permettono di modificare il comportamento
	// dell'algoritmo di ricerca della posizione della targa
	var.tot_sez = img->height;	// Impostando il numero totale delle sezioni pari all'altezza
					// dell'immagine, queste avranno altezza
					// di un 1px
	var.ImageThreshold = 220;
	var.MatrixThreshold = 10;
	var.s_black = 40;
	var.s_patt = 100;

	// Il valore di var.ImageThreshold viene abbassato fino a quando
	// l'algoritmo non è capace di individuare la targa
	while (tot_plates == 0 && var.ImageThreshold > 0) {
		printf("\n\nvar.ImageThreshold: %d\n", var.ImageThreshold);

		// Ricerca nell'immagine di partenza le aree candidate ad essere
		// targhe mediante la ricerca del pattern della targa.
		tot_plates = platesCandidates(img, &var, &plates);
		printf("tot_plates: %d\n", tot_plates);
		var.ImageThreshold -= 20;
	}
	return 0;
}
