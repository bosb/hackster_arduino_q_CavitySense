.PHONY: flash monitor flash-audio-test flash-led-test flash-ml-test clean setup

SKETCH_DIR = 05_firmware_integration/cavitysense

flash:
	./flash.sh

monitor:
	./flash.sh --monitor

flash-audio-test:
	./flash.sh --sketch 02_firmware_audio/examples/audio_test

flash-led-test:
	./flash.sh --sketch 03_firmware_led_matrix/examples/led_test

flash-ml-test:
	./flash.sh --sketch 04_firmware_classifier/examples/inference_test

clean:
	rm -rf /tmp/arduino-build-cavitysense
	rm -rf /tmp/arduino-build-*

setup:
	bash 09_scripts/setup.sh
