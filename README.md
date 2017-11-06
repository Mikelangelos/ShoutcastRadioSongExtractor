# Shoutcast Radio Song Extractor
### ! A word of Caution !
This is a project I slammed in a few hours and my only concern was for it be functional (so that I can get the song titles). It's not ment to be used as a generic song title extractor so its features are bare minimal for it to be functional. Further more, the function names and the entire structure of the program is not thought through at all.
### What does it do?
Song extractor is a very simple tiny program that connects to a radio station from [Shoutcast.com](http://www.shoutcast.com) and saves the songs the station plays on a .txt file (if the station shares the current song title of course).
### How does it do it?
The way it works is super minimal and light. It has a hardcoded station of interest and it simply opens a TCP connection with the server, requests an html file that contains the current song playing, parses that file to get only the song title and after checking whether this song title is different from the one it got last time, it saves that into a file. After saving, the program sleeps for 2 minutes (assuming all songs have duration > 2 mins) and then goes through the same loop again.
### How to use it on the station you like?
1. Go to Shoutcast and get station IP:PORT you would like to connect. (A simple way to do this is by downloading the radio station you want and then opening that file with a text editor)
2. Go to main.c and in the function `get_server_response_for_msg` change the Ip and Port to the desired ones
3. Recompile and Run.
### Final notes:
- On the console you will see a number next to the song names. That number shows how many times the program requested the current song title and got the same title as last time. I used it for tweaking the amount of time the program has to sleep in order to not lose a song but also not spam the server with requests.
- I have included a compiled version of the program (the main.exe) that connects to a radio station called "Nebula" in case you want a quick demonstration of what it does.
