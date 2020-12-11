# esp-idf-m5stack-speaker
Example to ring the speaker of M5Stack with esp-idf.   
I ported from Arduino core for ESP32.   

__The buzzer needs enough current.__   
If there is not enough current, Brownout will occur.   
ESP32 has a built-in brownout detector, which is enabled by default.    
Brownout detector can trigger system reset if supply voltage goes below safe level.    

![M5Stack-speaker](https://user-images.githubusercontent.com/6020549/101963744-cd0b8080-3c52-11eb-891b-2c35d0237af5.JPG)

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

# Firmware configuration
You have to set this config value using menuconfig.   

- CONFIG_BEEP_FREQUENCY   
 BEEP frequency.
- CONFIG_BEEP_DURATION   
 BEEP duration.
- CONFIG_ESP_FONT   
 The font to use.

![config-main](https://user-images.githubusercontent.com/6020549/101963344-d34d2d00-3c51-11eb-95f7-c9b30b03cd25.jpg)

![config-app](https://user-images.githubusercontent.com/6020549/101963350-d6481d80-3c51-11eb-8fd6-c1d7c15644fc.jpg)


# Operation
- Left button is beep.   

- Middle button play Doremi.   

- Right button start alarm.   
 Stops when pressed again.   


