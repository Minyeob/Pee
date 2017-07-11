
'CC := arm-none-linux-gnueabi-gcc'
CC := gcc
exe : app.o keypad.o textlcd.o sevensegment.o oled.o serial.o eightled.o cled.o http.o buzzer.o dipswitch.o dotmatrix.o touch.o camera.o
	$(CC) -o exe app.o keypad.o textlcd.o sevensegment.o oled.o serial.o eightled.o cled.o http.o buzzer.o dipswitch.o dotmatrix.o touch.o camera.o -lpthread
buzzer.o : buzzer.c
	$(CC) -c -o buzzer.o buzzer.c
http.o : http.c
	$(CC) -c -o http.o http.c	
keypad.o : keypad.c
	$(CC) -c -o keypad.o keypad.c
sevensegment.o : sevensegment.c
	$(CC) -c -o sevensegment.o sevensegment.c
textlcd.o : textlcd.c
	$(CC) -c -o textlcd.o textlcd.c
app.o : app.c
	$(CC) -c -o app.o app.c -pthread
oled.o : oled.c
	$(CC) -c -o oled.o oled.c
serial.o : serial.c
	$(CC) -c -o serial.o serial.c
eightled.o : eightled.c
	$(CC) -c -o eightled.o eightled.c
cled.o : cled.c
	$(CC) -c -o cled.o cled.c
dipswitch.o : dipswitch.c
	$(CC) -c -o dipswitch.o dipswitch.c
dotmatrix.o : dotmatrix.c
	$(CC) -c -o dotmatrix.o dotmatrix.c
touch.o : touch.c
	$(CC) -c -o touch.o touch.c
camera.o : camera.c
	$(CC) -c -o camera.o camera.c
clean :
	rm *.o exe
