#ifndef __SDFATSEQUENTIALFILERK_H
#define __SDFATSEQUENTIALFILERK_H

#include "Particle.h"
#include "SdFat.h"

#include <set>

/**
 * @brief Class for sequentially numbered files on an SD card
 *
 * You typically instantiate one of these objects as a global variable. If you have different directories
 * with different sequentially number files you can inc
 */
class SdFatSequentialFile {
public:
	/**
	 * @brief Constructor
	 *
	 * @param sd The SdFat object for the SD card
	 *
	 * @param csPin The GPIO pin used for the SD card CS pin
	 *
	 * @param spiSettings Normally pass SPI_FULL_SPEED.
	 */
	SdFatSequentialFile(SdFat &sd, uint8_t csPin, SPISettings spiSettings);

	/**
	 * @brief Destructor
	 */
	virtual ~SdFatSequentialFile();

	/**
	 * @brief Sets the directory name to store files in
	 */
	SdFatSequentialFile &withDirName(const char *dirName) { this->dirName = dirName; return *this; };

	/**
	 * @brief Sets the maximum number of files to keep. Default is all files (0).
	 *
	 * @param maxFilesToKeep For example, if 10 then 10 file are kept and when the 11th is created, the
	 * lowest numbered is deleted so there are still 10. Set to 0 to keep all files (limited by disk space).
	 */
	SdFatSequentialFile &withMaxFilesToKeep(size_t maxFilesToKeep) { this->maxFilesToKeep = maxFilesToKeep; return *this; };


	/**
	 * @brief Sets the sprintf-style name pattern to use for filename generation. Default: "%06d.txt"
	 *
	 * @param namePattern The name pattern to use
	 *
	 * The name must be an 8.3 DOS filename. Also, namePattern is not copied. The intention is that you'd
	 * pass a string constant. If you dynamically generate the pattern, make sure the buffer does not go away
	 * (don't allocate it on the stack).
	 */
	SdFatSequentialFile &withNamePattern(const char *namePattern) { this->namePattern = namePattern; return *this; };

	/**
	 * @brief Open and scan the SD card
	 *
	 * You don't need to call this; if the card has not yet been scanned it will scan on openFile.
	 * The card will also be scanned again if an error occurs; this is common when the card is ejected.
	 */
	bool scanCard();

	/**
	 * @brief Open a log file
	 *
	 * @param pFile the file is opened on this object. You can pass either a FatFile or PrintFile object
	 * pointer here; PrintFile is handy because it supports Wiring things like println and printlnf.
	 *
	 * @param nextFile Pass true to use the next higher numbered file. Pass false to use the current
	 * highest numbered file.
	 */
	bool openFile(FatFile *pFile, bool nextFile = false);


	/**
	 * @brief Check to make sure there aren't too many files
	 *
	 * If withMaxFilesToKeep() is used to set the maximum number of files, when that number of files
	 * is exceeded, the oldest (smallest filename sequential number) is deleted. The default is to
	 * keep all files.
	 */
	void checkMaxFiles();

	/**
	 * @brief Convert a file number to a name using namePattern
	 *
	 * @param num The file number (positive integer)
	 *
	 * The default pattern is "%06d.txt" which allows for numbers 0 - 999999 while still staying
	 * in a valid 8.3 filename pattern. You don't need to add the leading zeros, but since since
	 * the numbers are sequential and increasing, it keeps numeric and alphabetical order the same.
	 *
	 * The name pattern must contain a number. It can have a prefix, like "f%07d.txt" which would
	 * generate f0000000.txt, f0000001.txt, ...
	 */
	virtual const char *getName(int num);

    /**
     * Callback passed to SdFat so it can determine the time for file timestamps
     *
     * Note: Timestamps will be in UTC not local time, and are unaffected by daylight saving.
     */
    static void dateTimeCallback(uint16_t* date, uint16_t* time);

protected:
    SdFat &sd; //!< The SdFat object (typically a globally allocated object passed into the constructor for this object
    uint8_t csPin; //!< The CS/SS pin for the SD card reader passed into the constructor for this object
    SPISettings spiSettings; //!< SPI_FULL_SPEED or SPI_HALF_SPEED passed into the constructor for this object

    const char *dirName = "files"; //!< Name of the logs directory, override using withDirName()
    size_t maxFilesToKeep = 0; //!< Maximum number of files to keep, override using withMaxFilesToKeep() (0 = keep all)
    const char *namePattern = "%06d.txt"; //!< Pattern (used by snprintf) to format the name in getName().

    char nameBuf[12]; //!< Shared buffer returned by getName()
    bool lastBeginResult = false; //!< Last result from sd.begin(). Will be false if there's not a valid SD card inserted.
    int lastFileNum = 1; //!< Last file number found, and the one we're writing to.
	FatFile logsDir; //!< FatFile for the logs directory (dirName, default is "files")
    FatFile curLogFile; //!< FatFile for the file we're currently writing to
	std::set<int> fileNums; //!< set of file numbers in the logs directory
	bool needsScanCard = true; //!< True if the SD card needs to be scanned again

};

#endif /* __SDFATSEQUENTIALFILERK_H */
