# **Readme File**

## **AI Based Waste Segregation**

## **Description**
  This project aims to automate garbage segregation using AI and embedded systems. The system classifies waste into specific categories to promote efficient recycling and waste management. The AI model is trained using Roboflow and deployed for real-time detection in VS Code, while Google Colab is used for training the machine learning model. Hardware components facilitate the movement and sorting of the waste based on its classification.

## **Features:**
1. AI-Powered Waste Classification: Classifies objects into Biodegradable, Non-Recyclable, Recyclable, and E-Waste categories.
2. Real-Time Detection: Uses a camera triggered by an IR sensor for live object detection.
3. Hardware Integration: Stepper motor rotates bins for sorted waste.
   . Servo motor opens gates for proper disposal.
   . OLED display provides user feedback on detected objects.
4. Sustainable Waste Management: Promotes proper recycling and reduces manual sorting efforts.

**Hardware Components**
1. OLED Display: Displays the type of detected waste.
2. Stepper Motor: Rotates the appropriate bin based on waste classification.
3. Servo Motor: Opens and closes gates for waste disposal.
4. IR Sensor: Detects the presence of an object and triggers the camera.
5. Power Adapter: Supplies power to the components.

**Software Tools**
1. Roboflow: For dataset preparation and model training.
2. Google Colab: For training the machine learning model.
3. VS Code: For deploying real-time detection and integrating the system.
4. Arduino IDE: For coding and controlling hardware components.

## **File Structure**
AI-Based-Garbage-Segregation/
│
├── model_training/               # Code and notebooks for training the AI model
├── real_time_detection/          # Scripts for real-time object detection
├── arduino_code/                 # Arduino IDE files for hardware control
├── images/                       # Sample input and output images
├── README.md                     # Documentation
└── requirements.txt              # Python dependencies

## **Future Enhancements**
Future Enhancements
1. Add support for more waste categories.
2. Integrate IoT for remote monitoring of bin status.
3. Improve classification accuracy with larger datasets.
