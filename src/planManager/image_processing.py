import sys
import numpy as np
from cv2 import imread
from cv2 import waitKey
from cv2 import bitwise_and
from cv2 import imshow
from cv2 import inRange
from cv2 import COLOR_BGR2HSV
from cv2 import cvtColor

if len(sys.argv) != 2:
    print('Argument number error !')
    sys.exit(-1)
    
# load the image
img = imread(str(sys.argv[1]))
#transform from RGB to HSV
hsv=cvtColor(img,COLOR_BGR2HSV)

# define the list of boundaries
white = np.array([255, 255, 255])
lowerBound = np.array([30,30,30])

mask = inRange(hsv, lowerBound, white) #The mask

res = bitwise_and(img, img, mask=mask) #transforming zones of clouds into black pixels

#percentage calculation
c=0
dim=np.shape(res)
Nl=dim[0]
Nc=dim[1]
for i in range(0,Nl):
	for j in range (0,Nc):
		for k in range(0,3):
			if res[i,j,k] == 0:
				c=c+1
				
pr=(c*100)/(Nl*Nc*3)

print(pr)
imshow("mywindow",res)

waitKey(0)
# Determine if the image is valid or not
S=70 #seuil

if pr > S:
	f = open("re_image_processing.txt", "w")
	f.write("0")
	f.close()
else:
    f = open("res_image_processing.txt", "w")
    f.write("1")
    f.close()
