# defcon25-badge
Badge Related items for defcon 25


-----
Installation of IDE:
Follow the following instructions (step by step):  

http://gnuarmeclipse.github.io/install/

	Notes: on Window's have gotten both Mars and Luna to work
	on linux i've only gotten luna to work (on ubuntu 16.04 LTS)

Once eclipse is set up here's what the debug config needs to look like, notes the 2 -f openocd scripts that must be configured.

For 2017 development we are using this nucleo board, while it is not the exact MCU we plan to use it is close and the one we want to use does not have a dev kit.

http://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-eval-tools/stm32-mcu-eval-tools/stm32-mcu-nucleo/nucleo-f302r8.html

![alt text](https://github.com/thedarknet/defcon25-badge/blob/master/Debug-ScreenShot.png "Debug Config")


If you have the actual badge board you'll need debugging to look like this:


![alt text](https://github.com/thedarknet/defcon25-badge/blob/master/Debug-ScreenShot-OnBadge.png "Debug Config")
