import cv2
import numpy as np
import matplotlib as plt
import pickle
import os
upper_cascade= cv2.CascadeClassifier("haarcascade_upperbody.xml")

cap = cv2.VideoCapture(0)
# labels = {}
# with open("labels.pickle", "rb") as f:
#     og_labels = pickle.load(f)
#     labels = {v:k for k,v in og_labels.items()}
#
# cap = cv2.VideoCapture(0)

while(True):
    ret, frame = cap.read()
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    # ub = upper_cascade.detectMultiScale(gray, scaleFactor=1.5, minNeighbors=5)
    ub = upper_cascade.detectMultiScale(
        gray,
        scaleFactor=1.1,
        minNeighbors=5,
        minSize=(30, 30),
        # flags=cv2.CASCADE_SCALE_IMAGE
    )
    for (x,y,w,h) in ub:
        roi_gray = gray[y:y+h, x:x+w]
        roi_color = frame[y:y+h, x:x+h]



        img_item = "my-image.png"
        cv2.imwrite(img_item, roi_gray)

        color = (255,0,0) #BGR 0-255
        stroke = 2
        end_cord_x = x+w;
        end_cord_y = y+h;
        cv2.rectangle(frame, (x,y), (end_cord_x, end_cord_y), color, stroke)

    cv2.imshow('frame', frame)
    if cv2.waitKey(20) & 0xFF == ord('x'):
        break

cap.release()
cv2.destroyAllWindows()
