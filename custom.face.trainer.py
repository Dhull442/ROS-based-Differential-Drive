import os
from PIL import Image
import numpy as np
import cv2
import pickle

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
image_dir = os.path.join(BASE_DIR, "images")

face_cascade = cv2.CascadeClassifier("/Users/kunalgupta/PycharmProjects/opencv_first/venv/lib/python3.6/site-packages/cv2/data/haarcascade_frontalface_alt2.xml")
recognizer = cv2.face.LBPHFaceRecognizer_create()

current_id = 0
label_ids = {}
y_labels = []
x_train = []
max_ic = {}

for root, dirs, files in os.walk(image_dir):
    for file in files:
        if file.endswith("png") or file.endswith("jpg"):
            path = os.path.join(root, file)
            label = os.path.basename(root).replace(" ", "-").lower()
            if not label in label_ids:
                label_ids[label] = current_id
                current_id += 1
                max_ic[label_ids[label]] = 0
            max_ic[label_ids[label]] += 1
            id = label_ids[label]
            pil_image = Image.open(path).convert("L")
            size = (550, 550)
            final_image = pil_image.resize(size, Image.ANTIALIAS)
            image_array = np.array(final_image, "uint8")
            faces = face_cascade.detectMultiScale(image_array, scaleFactor=1.5, minNeighbors=5);
            for (x,y,w,h) in faces:
                roi = image_array[y:y+h, x:x+w]
                x_train.append(roi)
                y_labels.append(id)

n = int(input("How many people do you want to add?"))

for i in range(n):
    label = input("Enter the name")
    if not label in label_ids:
        label_ids[label] = current_id
        current_id += 1
        if not os.path.exists("/Users/kunalgupta/PycharmProjects/opencv_first/venv/bin/images/"+label):
            os.makedirs("/Users/kunalgupta/PycharmProjects/opencv_first/venv/bin/images/"+label)
        max_ic[label_ids[label]] = 0
    id = label_ids[label]
    cap = cv2.VideoCapture(0)
    ic = max_ic[id] + 1
    while (True):
        ret, frame = cap.read()
        frame2 = frame
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        size = (550, 550)
        final_image = cv2.resize(gray, size)
        image_array = np.array(final_image, "uint8")
        faces = face_cascade.detectMultiScale(gray, scaleFactor=1.5, minNeighbors=5)
        for (x, y, w, h) in faces:
            roi = image_array[y:y + h, x:x + w]
            color1 = (255, 0, 0)  # BGR 0-255
            color2 = (0, 0, 255)
            stroke = 2
            end_cord_x = x + w;
            end_cord_y = y + h;
            if cv2.waitKey(10) & 0xFF == ord('n'):
                cv2.rectangle(frame, (x, y), (end_cord_x, end_cord_y), color1, stroke)
                cv2.imwrite("/Users/kunalgupta/PycharmProjects/opencv_first/venv/bin/images/" + label + "/" + str(ic) + ".jpg", frame2)
                print(ic)
                ic += 1
            else:
                cv2.rectangle(frame, (x, y), (end_cord_x, end_cord_y), color2, stroke)
        cv2.imshow('frame', frame)
        if cv2.waitKey(10) & 0xFF == ord('x'):
            break
    cap.release()
    cv2.destroyAllWindows()

current_id = 0
label_ids = {}
y_labels = []
x_train = []
max_ic = {}

for root, dirs, files in os.walk(image_dir):
    for file in files:
        if file.endswith("png") or file.endswith("jpg"):
            path = os.path.join(root, file)
            label = os.path.basename(root).replace(" ", "-").lower()
            if not label in label_ids:
                label_ids[label] = current_id
                current_id += 1
                max_ic[label_ids[label]] = 0
            max_ic[label_ids[label]] += 1
            id = label_ids[label]
            pil_image = Image.open(path).convert("L")
            size = (550, 550)
            final_image = pil_image.resize(size, Image.ANTIALIAS)
            image_array = np.array(final_image, "uint8")
            faces = face_cascade.detectMultiScale(image_array, scaleFactor=1.5, minNeighbors=5);
            for (x,y,w,h) in faces:
                roi = image_array[y:y+h, x:x+w]
                x_train.append(roi)
                y_labels.append(id)


with open("labels.pickle", 'wb') as f:
    pickle.dump(label_ids, f)

recognizer.train(x_train, np.array(y_labels))
recognizer.save("trainner.yml")


