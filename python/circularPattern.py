from PIL import Image, ImageDraw

imgC = Image.new("RGBA", (10,10), "White")
width, height = imgC.size
imgCCenter = (round(width/2),
              round(height/2))
allPPxls = [imgCCenter]
#prevPPxls = [imgCCenter]
currPPxls = [imgCCenter]

# Create first point as black
draw = ImageDraw.Draw(imgC)
draw.point(imgCCenter, fill="Black")


# Append current pxl to both pxl lists
def appendToPxlList(newPxl, currPPxls, allPPxls):
    allPPxls.append(newPxl)
    currPPxls.append(newPxl)


x = imgCCenter[0]
y = imgCCenter[1]
colorRand = 0

while(len(currPPxls) < 20):#width * height
    for currPxl in currPPxls:
        for each in [(0,1),(0,-1),(1,0),(-1,0)]:
                xPos = currPxl[0]+each[0]
                yPos = currPxl[1]+each[1]
                if((xPos,yPos) not in allPPxls):
                    draw.point((xPos,yPos),fill=(colorRand,colorRand,colorRand,255))
                    colorRand +=1
                    #Append current pxl to both pxl lists
                    appendToPxlList((xPos,yPos),currPPxls,allPPxls)
                #print((x+each[0],y+each[1]))
                #if(checkPxlExists((xPxl,yPxl),allPPxls)==False):
                #    appendToPxlList((xPxl,yPxl),currPPxls,allPPxls)
        #print(currPPxls)
        if(len(currPPxls)>200):
            break

imgC.save("result.png")



'''


####Current function doesn't relate startPixel to anything else in the function ####
def checkLocalPxlArea(startPixel,currPPxls, allPPxls):
    def checkPxlExists(pixel, allPPxls):
        if pixel in allPPxls:
            return True
        else:
            return False
         
while(len(allPPxls) < (width * height)):
    #While the pixes processed is less than the size of the iage
    for eachPxl in prevPPxls:
        checkLocalPxlArea(imgCCenter,currPPxls,allPPxls)
        
'''
