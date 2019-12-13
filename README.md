# SdFatSequentialFileRK

*Library to make it easier to use sequentially number files (like log files) on SD cards on Particle Devices*


## Example

```
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
```

You need to specify the chip select pin for the SD card reader. You also need to create a `SdFat` object. You'll also probably want to create `PrintFile` or `FatFile` object. These are pretty typical for applications that use the SdFat library.

```
const int SD_CHIP_SELECT = A5;
SdFat sd;
PrintFile file;

SdFatSequentialFile sequentialFile(sd, SD_CHIP_SELECT, SPI_FULL_SPEED);
```

To use a file:

```
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
```

The `true` parameter to openFile means create a new sequentially numbered file. Pass `false` to continue using the largest numbered file.

Using `PrintFile` object allows you to use standard Wiring Print functions like you would to Serial, TCPClient, etc.. Printing numbers as well as functions like `println` and `printlnf` are supported.

Don't forget to close the file when done!


## Version History

#### 0.0.1 (2019-12-13)

- Initial version

