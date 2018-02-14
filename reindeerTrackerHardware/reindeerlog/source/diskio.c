/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */

#include "sdcard_io.h"
#include <stdio.h>

/* Definitions of physical drive number for each drive */
#define DEV_RAM		3	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		0	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv /* Physical drive nmuber to identify the drive */
) {
	DSTATUS stat;

	switch (pdrv) {
	case DEV_RAM:
		//result = RAM_disk_status();

		// translate the reslut code here

		return STA_NOINIT;

	case DEV_MMC:
		//result = MMC_disk_status();

		stat = RES_OK;

		return stat;

	case DEV_USB:
		//result = USB_disk_status();

		// translate the reslut code here

		return STA_NOINIT;
	}
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv /* Physical drive nmuber to identify the drive */
) {
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_RAM:
		//result = RAM_disk_initialize();

		return STA_NOINIT;

	case DEV_MMC:

		result = cardInit();
		if (result == 0) {
			stat = RES_OK;
		}

		return stat;

	case DEV_USB:
		//result = USB_disk_initialize();

		// translate the reslut code here

		return STA_NOINIT;
	}
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv, /* Physical drive nmuber to identify the drive */
BYTE *buff, /* Data buffer to store read data */
DWORD sector, /* Start sector in LBA */
UINT count /* Number of sectors to read */
) {
	DRESULT res;res = RES_ERROR;

	switch (pdrv) {
	case DEV_RAM:
		// translate the arguments here

		//result = RAM_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC:
		// translate the arguments here

		//result = MMC_disk_read(buff, sector, count);

		if (count == 1) {
			res = SD_readBlock(sector, buff);
		} else {
			res = SD_readMultipleBlock(sector, buff, count);
		}

		if(res == 0)
		{
			return RES_OK;
		}
		else
		{
			return RES_ERROR;
		}

	case DEV_USB:
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write(BYTE pdrv, /* Physical drive nmuber to identify the drive */
const BYTE *buff, /* Data to be written */
DWORD sector, /* Start sector in LBA */
UINT count /* Number of sectors to write */
) {
	DRESULT res;res = RES_ERROR;

	switch (pdrv) {
	case DEV_RAM:
		// translate the arguments here

		//result = RAM_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC:
		// translate the arguments here

		if (count == 1) {
			res = SD_writeBlock(sector, buff);
		} else {
			res = SD_writeMultipleBlock(sector, buff, count);
		}

		// translate the reslut code here

		return res;

	case DEV_USB:
		// translate the arguments here

		//result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(BYTE pdrv, /* Physical drive nmuber (0..) */
BYTE cmd, /* Control code */
void *buff /* Buffer to send/receive control data */
) {

	printf("disk_ioctl \r\n");
	DRESULT res;
	res = RES_ERROR;
	BYTE n, csd[16];
	DWORD csize;

	switch (pdrv) {
	case DEV_RAM:

		// Process of the command for the RAM drive

		return res;

	case DEV_MMC:

		/*if (pdrv) {
			return RES_PARERR;
		}*/

		res = RES_ERROR;
		switch (cmd) {

		case CTRL_SYNC:
			res = RES_OK;
			break;

		case GET_SECTOR_COUNT: /* Get number of sectors on the disk (WORD) */

			if (SD_getCID_CSD(0x09,csd) == 0) { //Send CMD9 and read 16 bytes of CSD data to csd

				if ((csd[0] >> 6) == 1) /* SDC ver 2.00 */
				{
					csize = csd[9] + ((WORD) csd[8] << 8) + 1;
					*(DWORD*) buff = (DWORD) csize << 10;
				}

				else /* MMC or SDC ver 1.XX */
				{

					n = (csd[5] & 15) + ((csd[10] & 128) >> 7)
							+ ((csd[9] & 3) << 1) + 2;

					csize = (csd[8] >> 6) + ((WORD) csd[7] << 2)
							+ ((WORD) (csd[6] & 3) << 10) + 1;

					*(DWORD*) buff = (DWORD) csize << (n - 9);

				}

				res = RES_OK;

			}

			break;

		case GET_SECTOR_SIZE: /* Get sectors on the disk (WORD) */

			*(WORD*) buff = 512;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE:

			printf("Get blocksz\r\n");

			if (SD_getCID_CSD(0x09,csd) == 0) /* Read CSD */
			{

				*(DWORD*) buff = (((csd[10] & 63) << 1)
						+ ((WORD) (csd[11] & 128) >> 7) + 1)
						<< ((csd[13] >> 6) - 1);

				res = RES_OK;
			}

			break;

		default:
			res = RES_PARERR;
			break;

		}

		return res;

	case DEV_USB:

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
}

DWORD get_fattime(void)
{
	return 0;

}

