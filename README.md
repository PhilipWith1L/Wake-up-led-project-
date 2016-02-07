# Wake-up-led-project-
Arduino runs sketch to slowly cycle on lights to help wake you up 
Use RTC. Library. https://github.com/adafruit/RTClib
DS1307 and some MOSFET driving the LED's. 
With the RTCLib: https://github.com/adafruit/RTClib

From an example:

// following line sets the RTC to the date & time this sketch was compiled
rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

That code should be issued only once. After that, the battery keeps the time for years.
