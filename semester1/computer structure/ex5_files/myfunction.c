//208994962 Shachar Sirotkin

/* Compute min and max of two integers, respectively */
//calculating max and min inline instead of calling another function
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define KERNEL_LOW_LIMIT 3

/*
* Apply the kernel over each pixel.
* Ignore pixels where the kernel exceeds bounds. These are pixels with row index smaller than kernelSize/2 and/or
* column index smaller than kernelSize/2
*/
void blurSmooth(register unsigned char *src, register unsigned char *dst) {
	//using registers saves the time of using the memory 
	register int i;
	register int j;
	register int dimChars = (m << 1) + m;
	register int jLimit = dimChars - KERNEL_LOW_LIMIT ;
	register int currRow;
	register int prevRow;
	register int nextRow;
	register int sumRed;
	register int sumGreen;
	register int sumBlue;
	register int prevR;
	register int nextR;
	register int currG;
	register int prevG;
	register int nextG;
	register int currB;
	register int prevB;
	register int nextB;

	for (i = 1 ; i < m - 1; i++) {

		//calculating constant value before the loop instead of every iteration
		prevRow = (i-1) * dimChars;
		currRow = i * dimChars;
		nextRow = (i+1) * dimChars;

		for (j = KERNEL_LOW_LIMIT ; j < jLimit; j += 3) {
		//calculating repeated values once
			prevR = j - 3;
			nextR = j + 3;
			prevG = j - 2;
			currG = j + 1;
			nextG = j + 4;
			currB = j + 2;
			prevB = j - 1;
			nextB = j + 5;
			
			//suuming each row of the image for preventing cache misses
			sumRed = (unsigned char)src[prevRow + prevR] + (unsigned char)src[prevRow + j]
				 + (unsigned char)src[prevRow + nextR];
			sumGreen = (unsigned char)src[prevRow + prevG] + (unsigned char)src[prevRow + currG]
				 + (unsigned char)src[prevRow + nextG];
			sumBlue = (unsigned char)src[prevRow + prevB] + (unsigned char)src[prevRow + currB]
				 + (unsigned char)src[prevRow + nextB];

			sumRed += (unsigned char)src[currRow + prevR] + (unsigned char)src[currRow + j]
				 + (unsigned char)src[currRow + nextR];
			sumGreen += (unsigned char)src[currRow + prevG] + (unsigned char)src[currRow + currG]
				 + (unsigned char)src[currRow + nextG];
			sumBlue += (unsigned char)src[currRow + prevB] + (unsigned char)src[currRow + currB]
				 + (unsigned char)src[currRow + nextB];

			sumRed += (unsigned char)src[nextRow + prevR] + (unsigned char)src[nextRow + j]
				 + (unsigned char)src[nextRow + nextR];
			sumGreen += (unsigned char)src[nextRow + prevG] + (unsigned char)src[nextRow + currG]
				 + (unsigned char)src[nextRow + nextG];
			sumBlue += (unsigned char)src[nextRow + prevB] + (unsigned char)src[nextRow + currB]
				 + (unsigned char)src[nextRow + nextB];

			dst[currRow + j] = (unsigned char) (sumRed/9);
			dst[currRow + currG] = (unsigned char) (sumGreen/9);	
			dst[currRow + currB] = (unsigned char) (sumBlue/9);
		}
	}
}

void sharpSmooth(register unsigned char *src, register unsigned char *dst) {
	//using registers saves the time of using the memory 
	register int i;
	register int j;
	register int dimChars = (m << 1) + m;
	register int jLimit = dimChars - KERNEL_LOW_LIMIT;
	register int currRow;
	register int prevRow;
	register int nextRow;
	register int sumRed;
	register int sumGreen;
	register int sumBlue;
	register int startR;
	register int startG;
	register int startB;
	register int prevR;
	register int nextR;
	register int currG;
	register int prevG;
	register int nextG;
	register int currB;
	register int prevB;
	register int nextB;

	for (i = 1 ; i < m - 1; i++) {

		//calculating constant value before the loop instead of every iteration
		prevRow = (i-1) *dimChars ;
		currRow = i * dimChars;
		nextRow = (i+1) * dimChars;

		for (j = KERNEL_LOW_LIMIT ; j < jLimit; j += 3) {
		//calculating repeated values once
			prevR = j - 3;
			nextR = j + 3;
			currG = j + 1;
			prevG = j - 2;
			nextG = j + 4;
			currB = j + 2;
			prevB = j - 1;
			nextB = j + 5;

			//suuming each row of the image for preventing cache misses
			startR = ((unsigned char)src[currRow + j] << 3) + (unsigned char)src[currRow + j];
			startG = ((unsigned char)src[currRow + currG] << 3) + (unsigned char)src[currRow + currG];
			startB = ((unsigned char)src[currRow + currB] << 3) + (unsigned char)src[currRow + currB];
			sumRed = (unsigned char)src[currRow + prevR] + (unsigned char)src[currRow + nextR];
			sumGreen = (unsigned char)src[currRow + prevG] + (unsigned char)src[currRow + nextG];
			sumBlue = (unsigned char)src[currRow + prevB] + (unsigned char)src[currRow + nextB];

			sumRed += (unsigned char)src[prevRow + prevR] + (unsigned char)src[prevRow + j]
				 + (unsigned char)src[prevRow + nextR];
			sumGreen += (unsigned char)src[prevRow + prevG] + (unsigned char)src[prevRow + currG]
			 	 + (unsigned char)src[prevRow + nextG];
			sumBlue += (unsigned char)src[prevRow + prevB] + (unsigned char)src[prevRow + currB]
				 + (unsigned char)src[prevRow + nextB];

			sumRed += (unsigned char)src[nextRow + prevR] + (unsigned char)src[nextRow + j]
				 + (unsigned char)src[nextRow + nextR];
			sumGreen += (unsigned char)src[nextRow + prevG] + (unsigned char)src[nextRow + currG]
				 + (unsigned char)src[nextRow + nextG];
			sumBlue += (unsigned char)src[nextRow + prevB] + (unsigned char)src[nextRow + currB]
				 + (unsigned char)src[nextRow + nextB];

			dst[currRow + j] = (char) (MIN(MAX((startR - sumRed), 0), 255));
			dst[currRow + currG] = (char) (MIN(MAX((startG - sumGreen), 0), 255));
			dst[currRow + currB] = (char) (MIN(MAX((startB - sumBlue), 0), 255));
		}
	}
}

void doBlurConvolution(register Image *image) {
	//calculating the full size once except of calculating every usage;
	register int tempSize = m*n;
	register int fullSize = tempSize + (tempSize << 1);

	unsigned char* ImgCopy = malloc(fullSize);

	//memcpy does the same as copying each cell separately
	memcpy(ImgCopy,image->data,fullSize);

	blurSmooth(ImgCopy, image->data);

	free(ImgCopy);
}

void doSharpConvolution(register Image *image) {
	//calculating the full size once except of calculating every usage;
	register int tempSize = m*n;
	register int fullSize = tempSize + (tempSize << 1);

	unsigned char* ImgCopy = malloc(fullSize);

	//memcpy does the same as copying each cell separately
	memcpy(ImgCopy,image->data,fullSize);

	sharpSmooth(ImgCopy, image->data);

	free(ImgCopy);
}

void myfunction(Image *image, char* srcImgpName, char* blurRsltImgName, char* sharpRsltImgName) {
	// blur image
	doBlurConvolution(image);

	// write result image to file
	writeBMP(image, srcImgpName, blurRsltImgName);

	// sharpen the resulting image
	doSharpConvolution(image);

	// write result image to file
	writeBMP(image, srcImgpName, sharpRsltImgName);
}

