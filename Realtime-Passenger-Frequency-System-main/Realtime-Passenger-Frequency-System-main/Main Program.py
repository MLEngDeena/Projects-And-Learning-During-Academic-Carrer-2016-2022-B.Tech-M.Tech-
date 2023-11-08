# -*- coding: utf-8 -*-
"""
Created on Thu Apr  4 12:16:32 2019

@author: Study
"""
import random
import sys
import cv2
from firebase import firebase
import requests 
import json

request = None
JsonResult = None

img_counter = 0

total = 0

bus_route = ['Vijay Nagar','Malviya Nagar','Dainik Bhaskar','lig ','Industry House','Palasia','Geeta Bhawan','AIctsl','Gpo','Zoo','Navalakha Churaha','Holkar Subway','Bhawar Kuan']

journey_type = ['going','coming']

print("Going :- Vijay Nagar To Bhawar Kuan And Coming :- Bhawar Kuan To Vijay Nagar")

def number_of_faces( path ):
    global total 

    print(path)
    
    face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')
    
    image = cv2.imread(path)
    grayImage = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    faces = face_cascade.detectMultiScale(grayImage)

    if len(faces) == 0:
        print ("No faces found")
 
    else:
#        count_face = 0
#        for (x,y,w,h) in faces:
#            count_face = count_face + 1
#        print(count_face)
        total_faces = str(faces.shape[0])
        print ("Number of faces detected: " + total_faces)
        total = total + int(total_faces)

while True :
    index1 = random.randrange(len(bus_route))
    CurrentStop = bus_route[index1]
    print("current Bus Stop is : " ,CurrentStop)

    index2 = random.randrange(len(journey_type))
    Journey_Type_Selecton = journey_type[index2]
    print("Travel Mode is  : " ,Journey_Type_Selecton)

    if (Journey_Type_Selecton == 'coming') :
        if  (CurrentStop != 'Vijay Nagar') :
            index1 = index1-1
            print("Next Bus Stop is : " ,bus_route[index1])
        else:
            sys.exit("this is last stop")
    
    else:
        if  (CurrentStop != 'Bhawar Kuan') :
            index1 = index1 + 1
            print("Next Bus Stop is : " ,bus_route[index1])
        else:
            sys.exit("this is last stop")

    

    cam = cv2.VideoCapture(0)

    cv2.namedWindow("Hit ENTER to take Picture And ESC to quit")



    while True:
        ret, frame = cam.read()
        cv2.imshow("Hit ENTER to take Picture And ESC to quit", frame)
        if not ret:
            break
        k = cv2.waitKey(1)

        if k%256 == 27:
            # ESC pressed
            print("Escape hit, closing...")
            break
        elif k%256 == 32:
            # SPACE pressed
            img_name = "C:\\Users\\deena\\Desktop\\Minor\\Face Detection and Counting\\cam picture\\opencv_frame_{}.png".format(img_counter)
        
            cv2.imwrite(img_name, frame)
            print("{} written!".format(img_name))
            number_of_faces(img_name)
            img_counter += 1

    cam.release()
    cv2.destroyAllWindows()

    average = total/img_counter
    total_people = int (average)
    print("The Total People is : " + str(total_people))
    
    img_counter = 0

    total = 0
    
    firebase = firebase.FirebaseApplication('https://transportmanagementsyste-628b3.firebaseio.com/')

    resultput = firebase.put('','TotalPeople',total_people)
    
    RequestToThingspeak = 'https://api.thingspeak.com/update?api_key=L9BCQF5WS5QW7S3H&field1='
    RequestToThingspeak +=str(total_people)
    request = requests.get(RequestToThingspeak)

    print("Data is sended to Google Firebase and ThingSpeak" )
    
    print("Here we will show after retrieving from Firebase : ")
    print("The No. of people is : ")
    print(firebase.get('TotalPeople',None))
    print("Here we will show after retrieving from ThingSpeak : ")
    req = requests.get("http://api.thingspeak.com/channels/750652/feeds/last.json?api_key=QO6LRPOS2CACC77G")
    JsonResult = json.loads((req.text))
    print("The No. of people is : ")
    print(JsonResult['field1'])
    
    print("one execution ended")
    
    print("*"*50)
#    print("Do you want to continue then hit ENTER and to terminate hit ESC")
    
