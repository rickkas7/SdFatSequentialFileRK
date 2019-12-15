
#include "SdFatSequentialFileRK.h"

// Define the debug logging level here
// 0 = Off
// 1 = Normal
// 2 = High
#define SDCARD_LOGHANDLER_DEBUG_LEVEL 1

// Don't change these, just change the debugging level above
// Note: must use Serial.printlnf here, not Log.info, as these are called from the log handler itself!
#if SDCARD_LOGHANDLER_DEBUG_LEVEL >= 1
#define DEBUG_NORMAL(x) Log.info x
#else
#define DEBUG_NORMAL(x)
#endif

#if SDCARD_LOGHANDLER_DEBUG_LEVEL >= 2
#define DEBUG_HIGH(x) Log.info x
#else
#define DEBUG_HIGH(x)
#endif


SdFatSequentialFile::SdFatSequentialFile(SdFat &sd, uint8_t csPin, SPISettings spiSettings)  : sd(sd), csPin(csPin), spiSettings(spiSettings) {

}

SdFatSequentialFile::~SdFatSequentialFile() {

}

bool SdFatSequentialFile::scanCard() {
	DEBUG_HIGH(("scanCard"));

	// We set needsScanCard to false on success. All other paths leave it set so we check again.
	needsScanCard = true;

	if (!lastBeginResult) {
		// Set the date time callback
		SdFile::dateTimeCallback(dateTimeCallback);

		pinMode(csPin, OUTPUT);
		lastBeginResult = sd.begin(csPin, spiSettings);
		if (!lastBeginResult) {
			DEBUG_HIGH(("sd.begin failed (no card or no reader)"));
			return false;
		}
	}

	if (dirName != NULL && !sd.exists(dirName)) {
		DEBUG_NORMAL(("creating dir %s", dirName));
		if (!sd.mkdir(dirName)) {
			DEBUG_NORMAL(("mkdir failed"));
			return false;
		}
	}

	if (!logsDir.open(sd.vwd(), dirName, O_READ)) {
		DEBUG_NORMAL(("failed to open dir"));
		return false;
	}

	DEBUG_HIGH(("opened dir %s", dirName));

	logsDir.rewind();

	FatFile tempFile;

	while (tempFile.openNext(&logsDir, O_READ)) {
		char name[64], *cp;
		tempFile.getName(name, sizeof(name));
		DEBUG_HIGH(("file %s", name));

		// Skip non-digits at start
		cp = name;
		while(*cp && !isdigit(*cp)) {
			cp++;
		}

		// atoi skips non-digits afterwards
		int num = atoi(name);
		if (num != 0) {
			if (maxFilesToKeep > 0) {
				fileNums.insert(num);
			}
			if (num > lastFileNum) {
				lastFileNum = num;
			}
		}
		tempFile.close();
	}
	checkMaxFiles();

	// Success!
	needsScanCard = false;
	return true;
}

bool SdFatSequentialFile::openFile(FatFile *pFile, bool nextFile) {

	if (nextFile) {
		lastFileNum++;
	}

	for(int tries = 0; tries <= 1; tries++) {
		const char *name = getName(lastFileNum);
		if (pFile->open(&logsDir, name, O_RDWR | O_CREAT)) {
			if (maxFilesToKeep > 0) {
				fileNums.insert(lastFileNum);
			}
			DEBUG_HIGH(("using file %s", name));
			return true;
		}

		// Failed to open file. Try scanning directory again. This will reinitialize SdFat
		// if the card has been ejected.
		if (!scanCard()) {
			DEBUG_NORMAL(("open failed and scan card failed, cannot open"));
			return false;
		}
		if (nextFile) {
			lastFileNum++;
		}
	}
	return false;
}


void SdFatSequentialFile::checkMaxFiles() {
	if (maxFilesToKeep > 0) {
		auto it = fileNums.begin();

		while(fileNums.size() > maxFilesToKeep) {
			const char *name = getName(*it);
			DEBUG_NORMAL(("removing old file %s", name));
			FatFile::remove(&logsDir, name);
			it = fileNums.erase(it);
		}
	}
}


const char *SdFatSequentialFile::getName(int num) {
	snprintf(nameBuf, sizeof(nameBuf), namePattern, num);
	return nameBuf;
}



// [static]
void SdFatSequentialFile::dateTimeCallback(uint16_t* date, uint16_t* time) {
	if (Time.isValid()) {
		*date = FAT_DATE(Time.year(), Time.month(), Time.day());
		*time = FAT_TIME(Time.hour(), Time.minute(), Time.second());
	}
}




