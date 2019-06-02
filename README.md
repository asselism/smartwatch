<meta charset="utf-8" emacsmode="-*- markdown -*-">

**Team 25 Final Report**
Overview
========
We built a wearable smart watch that has multiple apps installed. The watch have a home screen that displays some basic information, including time, date, weather, etc. After entering the main menu, the user can select an app and experience the more complex functionality of the app, while being able to go back to the home screen. The watch also has a sleep mode in which it does not display anything, but can be woken up and put back to sleep.

Functionality
=====

The watch have a number of independent features.  

* Time: display current local time (default at MIT when GPS is not valid) in analog or digital view. Also include a stop watch.

* Weather: display current local weather (default at MIT when GPS is not valid). 

* Steps: display the number of steps taken during the day. Additionally, the user can view the graph of steps taken in the past 10 days. The user can also view the data of any date by specifying year, month, and date. Searching history data is not allowed when wifi is not connected (it will tell the user wifi is required when the user try to search).

* Battery: display the amount of charge left in the battery. Both in an analog plot and a text.

* Search: use the microphone to search short paragraph of information on the microphone generated query. This is implemented using Wikipedia. The whole app is not available when wifi is not connected (it will tell the user wifi is required when the user try to enter the app).

* Sleep: the watch will autosleep after some amount of time if any button is not pressed in this time interval. It can be woke up by pressing any button. There is also an app that allow users to choose the time interval before autosleep, including an option "never".

* Heartbeat: measuring the heartbeat in terms of beats per minutes based on a home made oximeter.

* Friends/contacts: add new friends by typing their username, accept/refuse friends requests, show who is nearby (based on GPS), send/receive messages to/from friends. 

Pictures and Videos
=====
**Pictures**

* The front view of the bracelet:

![Front view of the bracelet.](https://i.postimg.cc/ydGhpPgj/front-view.jpg)

* The back view of the bracelet:

![Back view of the bracelet. The seperate board is the oximeter.](https://i.postimg.cc/kGtQ0bWm/back-view.jpg)

**Videos**

* The menu:
    * [Go around Menu](https://youtu.be/pjoP2Ma025A)
    * [Go back to Menu from App](https://youtu.be/OPSZmPinuPE)
* Sleep app:
    * [Sleep](https://youtu.be/ViZcesi3TiU)
* Time app:
    * [Time](https://youtu.be/rVNMX6QELyU)
* Search app:
    * [When we Search Nothing](https://youtu.be/yPlGkK1MHdg)
    * [When Something is Found](https://youtu.be/5zPHX3EAtK0)
    * [When Nothing is Found](https://youtu.be/FKS95fDg55I)
* Steps app:
    * [Steps](https://youtu.be/k6DIK-u9FzE)
* Health app:
    * [Health](https://youtu.be/291QxjgIKxA)
* Friends app:
    * [Send Friend Request from Device 2 to Device 1](https://youtu.be/BnnimqnKi-A)
    * [Accept Request on Device 1](https://youtu.be/ITXluAiLsHI)
    * [Send Message from Device 2 to Device 1 (also received a message from the system)](https://youtu.be/Z-jfgzUu_L0)
    * [Check Message on Device 1](https://youtu.be/CbJSKQJWtH8)
    * [Send Message to Stranger (not allowed)](https://youtu.be/s4mgrQZGDbY)
    * [Check Nearby Friends on Device 1](https://youtu.be/kdBN5QyVIfI)
    * [Check Nearby Friends on Device 2](https://youtu.be/pYG6wR7zO9I)

Documentation of the System
=====
* The wiring on our board is:

![Wiring on our board.](https://i.postimg.cc/4yK7KR3b/wiring.jpg) 

* The circuit for oximeter is:

![Circuit for Oximeter. Two high pass filter and two low pass filter are used.](https://i.postimg.cc/zGThSNFx/oximeter-wiring.png) 

* The function of buttons: 

    There are three buttons: 2 on the bracelet, and 1 on the oximeter.

    * Button connected to pin 16: switch between apps in the menu; go in and out of apps; wake the whole thing up. Have different functions for short press (<2s) and long press (>2s). 

    * Button connected to pin 5: used inside each apps; can also wake the whole thing up. Have different functions for short press (<2s) and long press (>2s).

    * Button on the oximeter: connect power supply to the oximeter. The oximeter is working only when the button is pressed.


Block Diagram and High Level State Machine
=====
* The functional block diagram is:

![The functional block diagram. Arrows shows the flow of information.](https://i.postimg.cc/5yPgJVRB/fxn-block.png) 

* The high level state machine:
    * State variable: (X, Y)
    * X is in {0, 1, 2}, Y = the selected App
    * 0 = in Home screen, 1 = in Menu, 2 = in App
    * LP = Long Press, SP = Short Press, AP = Any Press

![The high level state machine. Transitions between differents apps and menu/homepage are shown here.](https://i.postimg.cc/dtxnVxMK/fsm.png)

Technical Challenges 
======
Wearability

* Challenge: It would be quite unrealistic to have the user of our smart watch simply tape that heavy and bulky breadboard on their wrist; as a result, we need to make our smart watch actually wearable and its shape more adjustable and flexible.   

* Solution: To enable the smart watch to be secured onto the user's wrist, we got three separate perfboards and used pipe-cleaners to connect them side by side such that they form a "bendable breadboard" in a bracelet form that could go around the user's wrist. With the pipe-cleaners as the connectors between the boards, the user could even adjust the width between the boards. 

Circuit design

* Challenge: As we did not plan on using the breadboard we had in class but three separate perfboards, we would need to come up with a new arrangement of the parts and their wiring such that the connections across different boards are as simple and little as possible. Additionally, we need to consider what parts go on which board such that the smart watch would be convenient for the user to use. Furthermore, since the bracelet will be bent quite frequently, the wires connecting the boards should not break due to fatigue. 
     
* Solution: For parts arrangement, we settled on having the ESP32 on the middle perfboard since it has the most connections with all other parts, the LCD with the two buttons on the same perfboard on the right for convenience of the user, and all other parts on the perfboard on the other side. We designed the wiring such that most of the overlapping parts of the wiring would be taken care of on the board for ESP32 before the wires go cross to the other boards. We used wires of the same length for connections between the boards, so the changes in shape of the wires are similar when they are bent. We hot-glued the soldered joints in attempt to enhance the durability. 

Oximeter

* Challenge: There are a lot of noises we need to filter out, and we need to have a good algorithm to make it somewhat accurate and reliable. 

* Solution: To solve the problem of too much noise, we used 2 low-pass filter and 2 high-pass filter. Because we expect the heart rate to be about 1~2 Hz, we set the -3dB point of the high pass filter near 1Hz and the -3dB point of the low pass filter near 2Hz. (Max helped us doing this) After the filters, the noise is suppressed. 

    To detect the heart rate, we need to identify the increasing part and decreasing part of the signal and count the number of transitions from increasing to decreasing, and the transition from decreasing to increasing. However, there's a lot of fluctuation in the signal, and the average level of the signal also drift with time because of the small movement of finger. The former makes it hard to identify the increasing and decreasing part and the latter makes it hard to use a uniform threshold. To solve this problem, we used a clever algorithm: we average the signal over time to get the DC offset, and updating the variance of signal based on the difference of the signal and the DC offset (to know how much the signal change), then we identify the increasing part and decreasing part using the following algorithm: when the signal (signal here means raw signal minus DC offset) is increasing, we update the max value whenever a data larger than previous max arrives; when a signal less than the previous max minus the standard deviation arrives, we identify this as a transition from increasing part to decreasing part; then we begin to update the min value until a signal larger than previous min value plus standard deviation arrives, and we identify this as a transition from decreasing part to increasing part. By doing so, our algorithm is robust in the following sense: 

    * The fluctuation of signal will not affect our counting of transition, because we do not expect fluctuation to be as large as the standard deviation. 

    * The drift of DC offset will not affect our counting, because we compensated it. 

    * The strength of heartbeat will not affect our counting, because instead of using a hard code standard, we compared the signal with its standard deviation. 

Parts List
===== 

* Bracelet: 

    * pipe cleaners x2 

    * perfboard (8 cm*11 cm) x2 

    * perfboard (7 cm*9 cm) x1 

    * sockets: 

        * 20 pins x2 

        * 8 pins x3 

        * 6 pins x1 

        * 4 pins x2 

        * buttons x2 

* Oximeter: 

    * perfboard (22 holes*13 holes) x1 

    * button x1 

    * resistors:

        * 220 x1 

        * 470k x5 

        * 8.2k x2 

        * 1M x1 

        * 10M x1 

    * tantalum capacitor:

        * 1u x2 

    * ceramic capacitor:

        * 100n x1 

        * 10n x1 

    * Infarad LED 

    * photoresistor x1 

    * operational amplifier (MCP6002) x1 

Code Details
=====
**Overall Structure (Screens.ino)**

This is the main file of the code. It provides a lot of functions to draw things, like topBar, menu, homepage, etc. All the global variables, including the objects for the apps are defined in this file. At the beginning of each loop, it will check the status of wifi connection, and check whether the two buttons are not pressed, short pressed, or long pressed. It will also get the current time and the current steps from the apps at the beginning of each loop. After that, it will do state transition based on the button values as shown in the High-Level State Machine section. Then it will check if the device has been idle for a certain time period and whether it should be put to sleep. At the end of each loop, the "update" function of each object of apps will be called, based on the button values and the wifi connection status.

**Button (Button.h)**

Provide a class called "Button" to let us know whether a button is not pressed, short pressed, or long pressed. It serves as the foundation of other parts of the code.

* `Button(int p)` Initializer. p is the pin number to which the button is connected.

* `int update()` In every loop, int update() should be called. It will return 0 is button is not pressed, return 1 if the button is short pressed, return 2 if the button is long pressed.

**HTTP Request (Requests.h)**

Provide a function to do http requests. Serves as the foundation of other part of the code.

* `void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial)`  

    Arguments:
    * char* host: null-terminated char-array containing host to connect to
    * char* request: null-terminated char-arry containing properly formatted HTTP request
    * char* response: char-array used as output for function to contain response
    * uint16_t response_size: size of response buffer (in bytes)
    * uint16_t response_timeout: duration we'll wait (in ms) for a response from server
    * uint8_t serial: used for printing debug information to terminal (true prints, false doesn't)  

**Type (Type.h and Type_with_character.h)**

Provide a class called "Type_Module" to help us type with the imu. It serves as the foundation of other parts of the code. Type_Module can only time number while the version with character can also type characters from a to z.

* `Type_Module(char* message_to_type, MPU9255* imu_to_use)` Initializer. message_to_type is the the string store the typed characters. imu_to_use is a pointer to the imu.

* `int update()` It should be called when you are typing something. It returns 0 if message is not changed; return 1 if message is changed, but typing is not finished; return 2 if typing is finished. It changes the string message_to_type when some character is typed.

* `int reset()` Forget what is typed and get ready to type from the beginning. Used when you have not finish typing but you decide to exit the app.

**Icons (Icons.h)**

This library includes all three icons of the top bar, which are WiFi, GPS, and the battery (which also includes the estimated remaining time).

* Battery:
    * `Battery_icon(TFT_eSPI* tft, uint8_t x_loc, uint8_t y_loc, uint16_t color, uint16_t background, uint16_t text_c)`

        Arguments:
        * TFT_eSPI* tft: Pointer to the screen
        * uint8_t x_loc: The base x coordinate for drawing the battery
        * uint8_t y_loc: The base y coordinate for drawing the battery
        * uint16_t color: Battery icon color
        * uint16_t background: Background color
        * uint16_t text_c: Text color (for the system)

    * `void update()` This first will check the time to see if the battery icon needs to be redrawn, which is based on if the screen switched or last drawn was at least 500 milliseconds ago. The icon is drawn based on the level of voltage detected. Additionally, the estimation time is drawn right under the icon when the function called it is in the Home screen state.

* WiFi:
    * `WiFi_icon(TFT_eSPI* tft, uint8_t x_loc, uint8_t y_loc, uint16_t color_on, uint16_t color_off, uint16_t bg_c)`

        Arguments:
        * TFT_eSPI* tft: Pointer to the screen
        * uint8_t x_loc: The base x coordinate for drawing the WiFi
        * uint8_t y_loc: The base y coordinate for drawing the WiFi
        * uint16_t color_on: WiFi icon color when it's connected
        * uint16_t color_off: WiFi icon color when it's not connected
        * uint16_t bg_c: Background color

    * `void drawWiFi()` Based on the connection status, it will either draw the icon with color_on or color_off.

* GPS:
    * `GPS_icon(TinyGPSPlus* GPS_to_use, TFT_eSPI* tft, uint8_t x_loc, uint8_t y_loc, uint16_t color_on, uint16_t color_off, uint16_t text_c)`

        Arguments:
        * TinyGPSPlus* GPS_to_use Pointer to GPS
        * TFT_eSPI* tft Pointer to the screen
        * uint8_t x_loc The base x coordinate for drawing GPS icon
        * uint8_t y_loc The base y coordinate for drawing GPS icon
        * uint16_t color_on GPS icon color when it's connected
        * uint16_t color_off GPS icon color when it's not connected
        * uint16_t text_c Text color (for the system)

    * `void get_GPS_status()` This function gets the current GPS status, and reflect on whether it has changed from old_status.

    * `void drawGPS()` This function draws the icon like the WiFi icon, choose the color based on the connectivity.

    * `void update()` This checks whether the GPS icon needs to be drawn again based on get_GPS_status().

**Sleep (Sleep.h)**

A library for the class "Sleep" which contains several functions responsible for the auto-sleep functionality of the smart watch.

* `Sleep(TFT_eSPI* tft, uint8_t op, uint8_t* gss, int bg, int tc, int sc, int ba)`

        Arguments:
        * TFT_eSPI* tft: Pointer to the screen
        * uint8_t op: Output pin
        * uint8_t* gss: State for the fsm of the screen
        * int bg: Background color value
        * int tc: Text color value
        * int sc: Color for the square that indicates the current highlighted option 
        * int ba: Color for the back arrow

* `void drawChoice()` Draw out the menu for the auto-sleep setting app. Called every time the user enters the app.

* `void fsm(int btn1, int btn2)` The fsm that is responsible for putting the screen to sleep or turning it back on based on current conditions. It turns off the screen when the user has not pressed any buttons for a period of time that is longer than the set inactive-time, and turns back on the screen as soon as the user press any button. The arguments are the button value of the two buttons on the board (from the "update" function in "Button" class).

* `void set_fsm(int btn2)` The fsm for the app that allows user to choose his/her preferred auto-sleep time from one of the four options: 10 sec, 30 sec, 60 sec, and Never. It draws a square indicating the current highlighted option and a square of another color indicating the current set option. A value of 1 from the button lets the user go to the next option, and a value of 2 from the button set the auto-sleep time to the current highlighted option (except when the back arrow is highlighted). A value of 2 from the button when the back arrow is highlighted would let the user return to the main menu.

**Time (Time.h)**

For the time app, we extract the local time and date. We use the GPS coordinates and make a get request towards the 608 server, which acts as a proxy and makes a request towards timezoneDB API, which gives local time and date based on lontitude and latitude. The server side code returns this information to the watch.

A long press of the PIN_2 button changes the mode of the time app in a cyclic manner - analog, digital, stopwatch. A short press in the stopwatch mode starts and stops the stopwatch.

The Time app is implemented as a Time class. The class has variables that store the time, the time, and current mode of the app. There are three modes - analog display, digital display, and stopwatch. The constructor of the class defines the length of the hands of the watch, as well as the center of the watch's display. The different functions of the class are described below.

* The `start()` function gets the local coordinates through the GPS if there is GPS connection. If there is no connection, it defaults the coordinates to Boston.

* `print_analog()` displays an analog watch using the current hour, minute, and seconds values. It also displays the current date.

* `print_digital()` does the same in a digital format.

* `drawline(int center, double ang, int l)`` and ``coverline(int center, double ang, int l)`` are helper functions for the ``print_analog()`` function. They take care of drawing new watch hands and covering up the old ones. The parameters are there to make sure that we are covering all the lines that have been drawn during the previous step to avoid flickering of the screen (which would be caused by blacking out the whole screen). The angles passed to the ``coverline`` function are determined by the hour, minute and second from the previous steps (``old_hoursd``, ``old_minsd``, ``old_sec`). The minutes are the hours are floats in this case because we need to have precise angles to draw the hands of the watch correctly. 

* `stopwatch()` checks if the stopwatch counter is currently running and in case it is, it updates the seconds and the minutes in the stopwatch (I have assumed that the user will not be using the stopwatch for more than 60 min). It also prints the current stopwatch minutes, seconds, and milliseconds (calculated by subtacting the current millis from the last time we updated the stopwatch seconds). 

* `change_time()`` updates the current time based on ``clock_timer`, a variable that tells us when we last updated the time. 

* `display()` prints the time in the current mode or goes to the stopwatch function if we are in the stopwatch mode.

* `get_time()`` and ``get_date()`` are functions responsible for getting the local time and date based on the coordinates defined in ``start()`. They both make a GET request to my (asselism) sandbox on the server code that extracts the local time and date from timezoneDB API. 

**Weather (Weather.h)**

For the weather app, we get the current temperature and weather conditions (rain, clouds, sunny (you might not be able to observe this in Cambridge), etc.). We achieve this by getting coordinates from the GPS and making a GET request to the 608 server, which acts as a proxy ad sends a request to openweathermap API. The server-side code then returns the local temperature if the key was "temp" and the weater conditions if the key was "vis".  

Similarly to the time app, the weather app is implemented with a Weather class that keeps the local weather conditions, the temperature, and the coordinates. The functions of the class are described below.

* `start()` is the same as the function in the time app.

* `get_vis()` and `get_temp()` are similar to `get_time()` and `get_date`. They make a GET request to the same server code to obtain the local weather based on the GPS location coordinates. 

**Wiki Search (Speech_search.h)** 

The user records an audio file of length at most 5.5 sec by pressing and holding a button. Once the button is released or the recoding surpasses 5.5 sec, the recording is encoded using mulaw and send in base64 to Google Speech API. The watch recieves the transcript and makes a request to 608 server with the text of the user's recording. Then, the server-side code makes a request to Wikipedia API to find an article on this topic and extracts the information from the summary of this article. It extracts at most "len" characters and only considers full sentences that end in full stops. The information is displayed on the LED of the watch. 

The search is implemented using a Wiki_search class. The file with the code for this app also contains some functions that assist in encoding the microphone data before it is send out to Google Speech. The member class functions are described below.

* `start()` prints the instructions for how to use the app to the user.

* `record_audio()` gathers data using the microphone. It continue to get new data while the button is being pressed and the duration of the recording does not exceed 5.5 sec.

* `update(uint8_t button_state, bool is_wifi_connected)` is the function that sends the recorded and encoded audio recording to Google Speech and after this sends the transcript received from the API to 608 server code that makes a search in Wikipedia articles. The server-side code returns a short blurb on the requested topic. If there have been some exceptions along the way (no WiFi, the transcript is empty, or there is no Wiki article on this topic), the user is notified accordingly.

**Steps Counting (Steps.h)**

Code for steps counting. Provide a class called "Steps_Module".

* `Steps_Module(TFT_eSPI* tft_to_use, MPU9255* imu_to_use, char* username, Time* time_module_to_use)` Initializer. "tft_to_use" is a pointer to the screen. "imu_to_use" is a pointer to the imu. username is the user's username. "time_module_to_use" is a pointer to the time module. It gets time from the time module so that it can tell you today's steps.

* `void activate()` Tell the class that it is entered. Called when enter the app.

* `void update(int bv, bool is_wifi_connected)` Update based on button value. It will accumulate steps no matter whether it is selected or not. But it will only show the steps and allow you to check other date when it is selected. It will tell you that wifi connection is required to check other dates when you try to check other dates if the wifi is not connected.

* `void close()` Tell the class that it is no longer selected. Called when exit the app.

* `int get_steps()` Return today's steps. Used when communicate with the homepage.

**Friends (Friends.h)**

The code for the app friends. It provides a class called "Friends_Module"

* `Friends_Module(TFT_eSPI* tft_to_use, TinyGPSPlus* gps_to_use, MPU9255* imu_to_use, char* username)` Initializer. tft_to_use is the pointer to the screen (used to draw things). gps_to_use is the pointer to the gps (used to get your position). imu_to_use is the pointer to the imu (used to type messages). username is the user's username.

* `void activate(int a)` Tell the class that it is entered. a tells the class which part is selected: 1 means message, 3 means nearby, 4 means add.

* `void update(int bv, bool is_wifi_connected)` It should be called in every loop. bv tells the class whether the button is not pressed (0), or short pressed (1), or long pressed (2). is_wifi_connected tells the class whether wifi is connected. When the friends app is not selected, the update function will check if anyone send message or friends requests to you, and the information will be stored in the class; it will also upload your location to the database. When the app is selected, it will not only check incoming messages, but also send message to other people, send friends requests to other people, or check nearby friends list when user ask it to do. It will only try to make a request when wifi if connected. If wifi is not connected, it will tell you that wifi is required in this app.

* `void close()` Tell the class that it is no longer selected. Called when exit the app.

**Health (Health.h)**

The code for the heart beat measurement. It provides a class called "Health_Module".

* `Health_Module(TFT_eSPI* tft_to_use, int measurementPin)` Initializer. tft_to_use is a pointer to the screen. measurementPin is the pin number to which the output of oximeter is connected.

* `void activate()` Tell the class that it is entered. Called when enter the app.

* `void update(int bv)` Update based on button value. It will do nothing when the app is not selected. It will try to measure the heartbeat when the app is selected.

* `void close()` Tell the class that it is no longer selected. Called when exit the app.

Energy Management Strategies
======
We have several strategies that helps with energy management:

* We show the user how much battery is left both in a form of an icon and as hours. We have measured the current needed for the watch to be roughly 0.15A, so the estimated full battery life is 10h. We also show the estimated remaining baterry time.

* There is a sleep feature in the watch, which turns off the screen of the watch after a given amount of time of inactivity. This allows us to conserve the energy from the LCD.

* We only make updates to the screen when necessary. Thus, we do not draw or write to the screen excessively, which avoids blinking screen and saves battery life.
