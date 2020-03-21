# https://pypi.org/project/opencv-python/
# https://github.com/skvark/opencv-python
#
# pip install opencv-python
import cv2

# https://pillow.readthedocs.io/en/5.3.x/
# pip install Pillow
from PIL import Image

def main():

	img = cv2.imread("./mytool.bmp")
	height, width, channels = img.shape

	filesTotals = []
	index = 1
	for yIndex in range(height // 16):
		filesLine = []
	
		for xIndex in range(width // 16):
			yStart = yIndex		* 16
			yEnd   = (yIndex+1) * 16
			xStart = xIndex		* 16
			xEnd   = (xIndex+1) * 16
		
			fileName = "./out-" + str(index) + ".bmp"
			clp = img[yStart:yEnd, xStart:xEnd]
			cv2.imwrite(fileName, clp)
			
			filesLine.append(fileName)
			index = index + 1
			
		filesTotals.append(filesLine)
	
	imgsTotals = []
	for filesLine in filesTotals:
		imgs = []
	
		for fileName in filesLine:
			img = cv2.imread(fileName)
			imgs.append(img)
			
		imgsHorz = cv2.hconcat(imgs)
		imgsTotals.append(imgsHorz)

	outName2 = "./mytool2.bmp"
	imgsTotal = cv2.vconcat(imgsTotals)
	cv2.imwrite(outName2, imgsTotal)
	
	# https://stackoverflow.com/questions/32323922/how-to-convert-a-24-color-bmp-image-to-16-color-bmp-in-python
	outName3 = "./mytool3.bmp"
	img = Image.open(outName2)
	newimg = img.convert(mode='P', colors=16)
	newimg.save(outName3)

if __name__ == "__main__":
	main()
