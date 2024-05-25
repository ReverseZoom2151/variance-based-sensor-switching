# Investigating Line-Following Robot Navigation through Variance-Based Sensor Switching: A Hybrid Approach to Optimizing Sensor Performance

## Description
The project is an Arduino-based initiative focused on developing and implementing various robotic algorithms and simulations. It explores a novel, cost-effective switching mechanism that dynamically selects between analog and digital sensor arrays based on real-time variance assessments, optimizing sensor performance without additional hardware. The project serves as an educational tool for those interested in robotics, offering practical examples and interactive experimentation.

## Features
### Variance-Based Sensor Switching 🌟
This project introduces an innovative variance-based sensor switching mechanism that dynamically selects between analog and digital sensor arrays. This approach leverages the high-resolution data from analog sensors for precise control and the straightforward binary outputs from digital sensors for reliable navigation, enhancing sensor reading accuracy and adaptability to varying track conditions.
### Motor Control 🏎️
The motor control system is managed by the motors.h file, allowing for precise control of the robot's movements, including:
- Motor Initialization: Setting up motor pins and configurations.
- Speed Control: Functions to set the speed of both motors independently.
- Stopping Motors: A function to stop the motors, ensuring controlled halts.
### Kinematics Calculations 📐
The kinematics.h file includes functions to calculate:
- Position Calculation: Computes the robot's current position based on encoder data.
- Orientation Calculation: Determines the robot's orientation angle, essential for path planning.
### Encoder Data Processing 📊
Encoders provide feedback on the robot's wheel rotations, which is vital for accurate movement control. The encoders.h file processes this data with functions to:
- Initialize Encoders: Sets up encoder pins and configurations.
- Read Encoder Values: Continuously reads the values from the encoders to track the robot's movements.
### Line Sensor Integration 🚦
The project uses both digital and analog line sensors to detect the robot's position relative to a line, which is essential for line-following tasks.
1. Digital Line Sensors (digitallinesensors.h)
- Initialization: Sets up digital line sensor pins.
- Reading Values: Functions to read the values from the digital line sensors.
2. Analog Line Sensors (analoglinesensors.h)
- Initialization: Sets up analog line sensor pins.
- Reading Values: Functions to read the values from the analog line sensors, providing a continuous range of values for more precise detection.
### Data Analysis and Visualization 📈
The RT_Results.ipynb Jupyter Notebook contains data analysis and visualization of the robot's performance across various scenarios. Key analysis includes:
- Theta Over Time Analysis: Examines the robot's orientation changes over time.
- Variance Analysis: Analyzes the stability and performance of the robot through sensor data variance.
- Curve Analysis: Inspects how the robot handles curved paths, identifying potential issues and areas for improvement.
### Event Handling and Interactive Analysis 🕹️
Interactivity and real-time analysis are crucial for robotics development. The project implements interactive elements using Jupyter Notebook, allowing users to interact with the data and visualize the robot's performance dynamically.
