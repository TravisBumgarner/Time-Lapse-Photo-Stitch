from PIL import Image, ImageDraw
import os
import time
def getImages():
    imageList = []
    for folderName, subfolders, filenames in os.walk(".\\imagesToStich"):
        for filename in filenames:
            if filename.endswith('.png') or filename.endswith('.jpg') or filename.endswith('.gif'):
                imageList.append(filename)
            else:
                print(filename + " is not a valid image file. Only use png, jpg, gif.")
    return imageList

def verifyImageSizes(images):
    #Takes in list of images an verifies they're all the same size
    finalImageSize = Image.open(".\\imagesToStich\\" + images[0]).size
    for image in images:
        if finalImageSize == Image.open(".\\imagesToStich\\" + image).size:
            continue
        else:
            print("Image " + image + " is not the same size as the other images.")
            return False
    return True
                
def stitchImages(images):
    #images is an list of image file names
    #Grab finalImageSize from first image in list
    finalImageSize = Image.open(".\\imagesToStich\\" + images[0]).size
    xPosition = 0
    #Creates a pixel width based on the number of images and the width of each image
    xWidth = int(finalImageSize[0]/len(images))
    finalImage = Image.new('RGBA',finalImageSize,"White")
    print("Now Creating image with size: " + str(finalImageSize) + " with a pixels//image of " + str(xWidth))
    for image in images:
        openImage = Image.open(".\\imagesToStich\\" + image)
        cropImage = openImage.crop((0,0,xWidth,finalImageSize[1]))
        finalImage.paste(cropImage,(xPosition,0))
        xPosition += xWidth
        openImage.close()
    finalImage.save("finalImage.png")

def main():
    images = getImages()
    startTime = time.time()
    stitchImages(images)
    print("Image created successfully in " + "--- %s seconds ---" % (time.time() - startTime))

if __name__ == '__main__':
    main()
