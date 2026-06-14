#include "fx_api.h"
#include "rng.h"
#include "sqlite3.h"
#include "stm32h7xx_hal_rng.h"
#include <string.h>

/* Reference to your initialized FileX Media disk component */
extern FX_MEDIA sdio_disk;

/* Custom sub-structure extending SQLite basic file type to retain FileX
 * specific tokens */
typedef struct {
  sqlite3_file base;  /* Base SQLite class tracker. MUST be first item */
  FX_FILE fx_file;    /* Internal FileX descriptor handle */
  int identity_flags; /* Read/Write tracking token flags */
} FileX_File;

/* --- I/O Operations on Open Files --- */

static int filex_Close(sqlite3_file *id) {
  FileX_File *p = (FileX_File *)id;
  if (fx_file_close(&p->fx_file) != FX_SUCCESS)
    return SQLITE_IOERR_CLOSE;
  return SQLITE_OK;
}

static int filex_Read(sqlite3_file *id, void *pBuf, int amt,
                      sqlite3_int64 offset) {
  FileX_File *p = (FileX_File *)id;
  ULONG actual_bytes;

  /* Move internal pointer directly to target offset location */
  if (fx_file_seek(&p->fx_file, (ULONG)offset) != FX_SUCCESS)
    return SQLITE_IOERR_SEEK;

  /* Fetch binary sequence block data directly */
  UINT status = fx_file_read(&p->fx_file, pBuf, (ULONG)amt, &actual_bytes);
  if (status == FX_END_OF_FILE) {
    memset((char *)pBuf + actual_bytes, 0, amt - actual_bytes);
    return SQLITE_IOERR_SHORT_READ; /* Partial match tracking warning */
  }
  if (status != FX_SUCCESS)
    return SQLITE_IOERR_READ;
  return SQLITE_OK;
}

static int filex_Write(sqlite3_file *id, const void *pBuf, int amt,
                       sqlite3_int64 offset) {
  FileX_File *p = (FileX_File *)id;

  if (fx_file_seek(&p->fx_file, (ULONG)offset) != FX_SUCCESS)
    return SQLITE_IOERR_SEEK;
  if (fx_file_write(&p->fx_file, (void *)pBuf, (ULONG)amt) != FX_SUCCESS)
    return SQLITE_IOERR_WRITE;
  return SQLITE_OK;
}

static int filex_Truncate(sqlite3_file *id, sqlite3_int64 size) {
  FileX_File *p = (FileX_File *)id;
  /* FileX does not support direct arbitrary mid-file truncation via API. */
  /* For simple logs, truncating entirely to zero size is emulated by
   * reallocation */
  if (size == 0) {
    fx_file_allocate(&p->fx_file, 0);
    return SQLITE_OK;
  }
  return SQLITE_OK;
}

static int filex_Sync(sqlite3_file *id, int flags) {
  //   FileX_File *p = (FileX_File *)id;
  /* Force internal sector caches out to the hardware physical boundaries
   * immediately */
  if (fx_media_flush(&sdio_disk) != FX_SUCCESS)
    return SQLITE_IOERR_DIR_FSYNC;
  return SQLITE_OK;
}

static int filex_FileSize(sqlite3_file *id, sqlite3_int64 *pSize) {
  FileX_File *p = (FileX_File *)id;
  *pSize = (sqlite3_int64)p->fx_file.fx_file_current_file_size;
  return SQLITE_OK;
}

/* Dummy implementations for features not typically supported by basic FAT file
 * systems */
static int filex_Lock(sqlite3_file *id, int lockType) { return SQLITE_OK; }
static int filex_Unlock(sqlite3_file *id, int lockType) { return SQLITE_OK; }
static int filex_CheckReservedLock(sqlite3_file *id, int *pResOut) {
  *pResOut = 0;
  return SQLITE_OK;
}
static int filex_FileControl(sqlite3_file *id, int op, void *pArg) {
  return SQLITE_NOTFOUND;
}
static int filex_SectorSize(sqlite3_file *id) {
  return 512;
} /* Default structural FAT sector sizing metric */
static int filex_DeviceCharacteristics(sqlite3_file *id) { return 0; }

/* IO Method Mapping Architecture Definitions */
static const sqlite3_io_methods filex_io_methods = {
    1,
    filex_Close,
    filex_Read,
    filex_Write,
    filex_Truncate,
    filex_Sync,
    filex_FileSize,
    filex_Lock,
    filex_Unlock,
    filex_CheckReservedLock,
    filex_FileControl,
    filex_SectorSize,
    filex_DeviceCharacteristics,
};

/* --- Global VFS Management Functions --- */

static int filex_Open(sqlite3_vfs *pVfs, const char *zPath, sqlite3_file *id,
                      int flags, int *pOutFlags) {
  FileX_File *p = (FileX_File *)id;
  UINT status;

  memset(p, 0, sizeof(FileX_File));
  p->base.pMethods = &filex_io_methods;

  /* Handle dynamic open vs create parameters */
  if (flags & SQLITE_OPEN_CREATE) {
    /* Attempt creation first (returns error if file already exists, which we
     * safely ignore) */
    fx_file_create(&sdio_disk, (CHAR *)zPath);
  }

  /* Set open mode permissions */
  UINT open_mode =
      (flags & SQLITE_OPEN_READWRITE) ? FX_OPEN_FOR_WRITE : FX_OPEN_FOR_READ;

  status = fx_file_open(&sdio_disk, &p->fx_file, (CHAR *)zPath, open_mode);
  if (status != FX_SUCCESS)
    return SQLITE_CANTOPEN;

  if (pOutFlags)
    *pOutFlags = flags;
  return SQLITE_OK;
}

static int filex_Delete(sqlite3_vfs *pVfs, const char *zPath, int syncDir) {
  if (fx_file_delete(&sdio_disk, (CHAR *)zPath) != FX_SUCCESS)
    return SQLITE_IOERR_DELETE;
  return SQLITE_OK;
}

static int filex_Access(sqlite3_vfs *pVfs, const char *zPath, int flags,
                        int *pResOut) {
  FX_FILE dummy;
  /* Test file existence by attempting to open it in read-only mode */
  if (fx_file_open(&sdio_disk, &dummy, (CHAR *)zPath, FX_OPEN_FOR_READ) ==
      FX_SUCCESS) {
    *pResOut = 1;
    fx_file_close(&dummy);
  } else {
    *pResOut = 0;
  }
  return SQLITE_OK;
}

/* Dummy time engine tracker setup linking system clock to relative queries */
static int filex_CurrentTime(sqlite3_vfs *pVfs, double *pTime) {
  *pTime = 2440587.5; /* Hardcoded Julian date anchor fallback constant */
  return SQLITE_OK;
}

static int filex_FullPathname(sqlite3_vfs *, const char *zName, int nOut, char *zOut) {
  strncpy(zOut, zName, nOut);
  return SQLITE_OK;
}

static int filex_Randomness(sqlite3_vfs *pVfs, int nByte, char *zOut) {
  uint32_t *random32bit = (uint32_t *)zOut;
  for (int i = 0; i < nByte; i += 4) {
    HAL_RNG_GenerateRandomNumber(&hrng, random32bit);
    random32bit++;
  }
  return 0;
}

/* Construct the global SQLite VFS structural descriptor */
static sqlite3_vfs filex_vfs = {
    1,
    sizeof(FileX_File),
    FX_MAX_LONG_NAME_LEN,
    0,
    "filex",
    0,
    filex_Open,
    filex_Delete,
    filex_Access,
    filex_FullPathname,
    NULL,
    NULL,
    NULL,
    NULL,
    filex_Randomness,
    NULL, /* Full path and randomness features skipped */
    filex_CurrentTime,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

int sqlite3_os_init(void) {
  sqlite3_vfs_register(&filex_vfs,
                       1); /* 1 makes it the default VFS interface */
}