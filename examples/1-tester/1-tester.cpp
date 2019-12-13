#include "SdFatSequentialFileRK.h"

SerialLogHandler logHander;


const int SD_CHIP_SELECT = A5;
SdFat sd;
PrintFile file;

SdFatSequentialFile sequentialFile(sd, SD_CHIP_SELECT, SPI_FULL_SPEED);

bool generateFile = false;

// Forward declarations
void buttonHandler(system_event_t event, int data);


void setup() {
	// Register handler to handle clicking on the SETUP button
	System.on(button_click, buttonHandler);

}

void loop() {

	if (generateFile) {
		generateFile = false;

		Log.info("generating sequential file!");

		if (sequentialFile.openFile(&file, true)) {
			char name[14];
			file.getName(name, sizeof(name));

			Log.info("file opened successfully %s", name);

			file.println("testing!");

			file.close();
		}
		else {
			Log.info("file open failed");
		}

	}
}

// button handler for the SETUP/MODE button. Generates a new file each time it's pressed
void buttonHandler(system_event_t event, int data) {
	generateFile = true;
}

