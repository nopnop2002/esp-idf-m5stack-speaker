# esp-idf-m5stack-speaker
Example to ring the speaker of M5Stack with esp-idf.   
I ported from Arduino core for ESP32.   

__The buzzer needs enough current.__   
If there is not enough current, Brownout will occur.   
ESP32 has a built-in brownout detector, which is enabled by default.    
Brownout detector can trigger system reset if supply voltage goes below safe level.    

```
Brownout detector was triggered

ets Jun  8 2016 00:22:57
```

# Software requirements
esp-idf ver4.1 or later.   

# Hardware requirements
M5Stack

# Install
```
git clone https://github.com/nopnop2002/esp-idf-m5stack-speaker
cd esp-idf-m5stack-speaker
make menuconfig
make flash monitor
```


# Operation
- Left button is beep.   

- Middle button play Doremi.   

- Right button start buzzer.   
 Stops when pressed again.   

![M5Stack-speaker](https://user-images.githubusercontent.com/6020549/101846453-4db77780-3b94-11eb-8bbc-3bd2e0e0f949.JPG)

