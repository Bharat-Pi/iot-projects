
import cv2
import mediapipe as mp
import requests

ESP32_IP = "http://172.20.10.2"

mp_hands = mp.solutions.hands
hands = mp_hands.Hands()
mp_drawing = mp.solutions.drawing_utils

def control_led(endpoint):
    url = f"{ESP32_IP}/led/{endpoint}"
    requests.get(url)

def count_fingers(hand_landmarks):
    finger_status = []

    thumb_up = hand_landmarks.landmark[mp_hands.HandLandmark.THUMB_TIP].x < hand_landmarks.landmark[mp_hands.HandLandmark.THUMB_IP].x
    index_up = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP].y < hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_PIP].y
    middle_up = hand_landmarks.landmark[mp_hands.HandLandmark.MIDDLE_FINGER_TIP].y < hand_landmarks.landmark[mp_hands.HandLandmark.MIDDLE_FINGER_PIP].y
    ring_up = hand_landmarks.landmark[mp_hands.HandLandmark.RING_FINGER_TIP].y < hand_landmarks.landmark[mp_hands.HandLandmark.RING_FINGER_PIP].y
    pinky_up = hand_landmarks.landmark[mp_hands.HandLandmark.PINKY_TIP].y < hand_landmarks.landmark[mp_hands.HandLandmark.PINKY_PIP].y

    if thumb_up:
        control_led("thumb/on")
    else:
        control_led("thumb/off")

    if index_up:
        control_led("index/on")
    else:
        control_led("index/off")

    if middle_up:
        control_led("middle/on")
    else:
        control_led("middle/off")

    if ring_up:
        control_led("ring/on")
    else:
        control_led("ring/off")

    if pinky_up:
        control_led("pinky/on")
    else:
        control_led("pinky/off")

    finger_status = [thumb_up, index_up, middle_up, ring_up, pinky_up]
    return finger_status

cap = cv2.VideoCapture(0)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break
    
    frame = cv2.flip(frame, 1)
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    
    results = hands.process(frame_rgb)
    
    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)
            fingers = count_fingers(hand_landmarks)
            if all(fingers):
                print("All fingers up")
            elif not any(fingers):
                print("All fingers down")
    
    cv2.imshow('Hand Gesture Recognition', frame)
    
    if cv2.waitKey(5) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()