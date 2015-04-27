Recently I have developed a GPS NMEA Parser for an embedded system using the ATMega128 microcontroller, and decided to pack everything together as a GPS open source library for AVR Microcontrollers. The code is highly optimized to load the microprocessor as low as possible (both in terms of storage and CPU load), so the serial data is parsed as it gets in, without using redundant data structures or operations.



<b>Demo video</b>
First we show latitude, number of satellites used, longitude and altitude (in meters) and on the next iteration we show speed, bearing UTC time and UTC date:

<a href='http://www.youtube.com/watch?feature=player_embedded&v=TCCFnY8f6Og' target='_blank'><img src='http://img.youtube.com/vi/TCCFnY8f6Og/0.jpg' width='425' height=344 /></a>

Project details available here: http://www.pocketmagic.net/?p=3821

[![](http://www.pocketmagic.net/wp-content/uploads/2013/02/atmega128_nmea_gps_library_0-300x199.jpg)](http://www.pocketmagic.net/?p=3821) [![](http://www.pocketmagic.net/wp-content/uploads/2013/02/atmega128_nmea_gps_library_1-300x199.jpg)](http://www.pocketmagic.net/?p=3821)



<b>License</b>
This library is free software, licensed under GPL v2; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
For other licensing options, feel free to contact me.

