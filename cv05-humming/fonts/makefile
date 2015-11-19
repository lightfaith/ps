all: fancyprint i2c_led

i2c_led:i2c_led.c
	g++ i2c_led.c -o i2c_led
clean:
	rm i2c_led fancyprint

fancyprint: fancyprint.c
	g++ -o fancyprint fancyprint.c
