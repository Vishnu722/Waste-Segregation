from ultralytics import YOLO
import cv2
import serial
import time

# Load a pretrained YOLO model
try:
    model = YOLO("segvil.pt")
    print("YOLO model loaded successfully.")
except Exception as e:
    print("Error loading YOLO model:", e)
    exit()

# Open the webcam (adjust index as needed)
cap = cv2.VideoCapture(1)
if not cap.isOpened():
    print("Error: Could not open webcam. Check if index is correct or try index 0.")
    exit()
else:
    print("Webcam opened successfully.")

# Initialize serial communication (adjust COM port and baud rate if needed)
try:
    ser = serial.Serial('COM12', 9600)
    print("Serial communication initialized successfully.")
except Exception as e:
    print("Error initializing serial communication:", e)
    exit()

# Set the confidence threshold
confidence_threshold = 0.65

# Define a dictionary to map class indices to labels
class_labels = {
    0: 'ewaste',
    1: 'nonbiodegradable',
    2: 'recyclable',
    3: 'nonrecyclable'
}

# Flag to control detection based on Arduino signal
wait_for_detection = True

# Loop to continuously get frames from the webcam
while True:
    # Wait for 'D' signal from Arduino before detection
    if wait_for_detection and ser.in_waiting > 0:
        incoming_signal = ser.read().decode('utf-8')
        if incoming_signal == 'D':
            print("Detection signal received from Arduino.")
            wait_for_detection = False  # Allow detection

    # If detection allowed, process webcam frames
    if not wait_for_detection:
        ret, frame = cap.read()
        if not ret:
            print("Error: Failed to capture image.")
            break

        # Perform object detection on the current frame
        results = model(frame)

        # Check if any detection with a score >= confidence_threshold occurred
        detection_found = False
        for box in results[0].boxes:
            if box.conf >= confidence_threshold:
                class_id = int(box.cls)  # Get class index
                class_name = class_labels.get(class_id, "unknown")
                print(f"Detected: {class_name} with confidence {box.conf}")

                # Check for specific classes and send corresponding signals
                if class_name == 'yoo':
                    time.sleep(4)
                    ser.write(b'f')
                elif class_name == 'ewaste':
                    time.sleep(4)
                    print("Sent 'e' for ewaste")
                    ser.write(b'e')
                elif class_name == 'nonrecyclable':
                    time.sleep(4)
                    print("sent non")
                    ser.write(b'3')
                elif class_name == 'biodegradable':
                    time.sleep(4)
                    print("sent bio")
                    ser.write(b'1')
                elif class_name == 'recyclable':
                    time.sleep(4)
                    print("sent recyc")
                    ser.write(b'2')
                
                detection_found = True
                break  # Stop further detections in this frame

        # If detection occurred, reset and wait for next Arduino signal
        if detection_found:
            wait_for_detection = True  # Wait for the next detection signal

        # Visualize the results on the frame
        annotated_frame = results[0].plot()

        # Display the annotated frame
        cv2.imshow("YOLO Real-Time Detection", annotated_frame)

    # Break the loop if 'q' is pressed
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

# Release the webcam, close all OpenCV windows, and close the serial connection
cap.release()
cv2.destroyAllWindows()
ser.close()
