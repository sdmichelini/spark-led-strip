spark-led-strip
===============

Author: sdmichelini, e-mail:sdmichelini@wpi.edu

Spark Core LED Strip Project

Features:

1. IFTTT Integration: Uses a Python Webserver to handle requests from a Wordpress Webhook generate by IFTTT. See http://captnemo.in/ifttt-webhook/ on how to generate the webhooks in IFTTT. Allows for many different commands on the LED strip
2. Custom Lighting Profiles: Different lighting profiles such as an automatic one which adjust the light temperature based on wheter the sun is up. Also has notification ability where the strip will rainbow for around a second.
3. I2C Communication: Spark Core recieves requests from the internet and adjust the high-level commands and sends them to the Arduino which is only responsible for driving the LEDs
