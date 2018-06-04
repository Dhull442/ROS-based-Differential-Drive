import numpy as np
import cv2
import pickle
import os

eye_cascade = cv2.CascadeClassifier("data/haarcascade_eye.xml")
face_cascade = cv2.CascadeClassifier("haarcascade_frontalface_alt2.xml")
smile_cascade = cv2.CascadeClassifier("site-packages/cv2/data/haarcascade_smile.xml")
recognizer = cv2.face.LBPHFaceRecognizer_create()
recognizer.read('/trainner.yml')
labels = {}
with open("labels.pickle", "rb") as f:
    og_labels = pickle.load(f)
    labels = {v:k for k,v in og_labels.items()}

cap = cv2.VideoCapture(0)

while(True):
    ret, frame = cap.read()
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, scaleFactor=1.5, minNeighbors=5)
    for (x,y,w,h) in faces:
        roi_gray = gray[y:y+h, x:x+w]
        roi_color = frame[y:y+h, x:x+h]

        id_,conf = recognizer.predict(roi_gray)
        if conf >= 10:
            print(labels[id_], conf)
            font = cv2.FONT_HERSHEY_SIMPLEX
            name = labels[id_] + "," + str(conf)
            color = (255,255,255)
            stroke = 2
            cv2.putText(frame, name, (x,y), font, 1, color, stroke, cv2.LINE_AA)

        img_item = "my-image.png"
        cv2.imwrite(img_item, roi_gray)

        color = (255,0,0) #BGR 0-255
        stroke = 2
        end_cord_x = x+w;
        end_cord_y = y+h;
        cv2.rectangle(frame, (x,y), (end_cord_x, end_cord_y), color, stroke)
        eyes = eye_cascade.detectMultiScale(roi_gray)
        for (ex, ey, ew, eh) in eyes:
            cv2.rectangle(roi_color, (ex, ey), (ex+ew, ey+eh), (0,255,0),2)

    cv2.imshow('frame', frame)
    if cv2.waitKey(20) & 0xFF == ord('x'):
        break

cap.release()
cv2.destroyAllWindows()