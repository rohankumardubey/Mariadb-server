/* Copyright (C) 2006 MySQL AB & MySQL Finland AB & TCX DataKonsult AB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

/*
  WL#3234 Maria control file
  First version written by Guilhem Bichot on 2006-04-27.
  Does not compile yet.
*/

#ifndef _control_file_h
#define _control_file_h

/*
  Not everybody needs to call the control file that's why control_file.h is
  not in maria_def.h. However, policy or habit may want to change this.
*/

#ifndef REMOVE_WHEN_SANJA_PUSHES_LOG_HANDLER
/*
  this is to get the control file to compile, until Sanja pushes the log
  handler which will supersede those definitions.
*/
typedef struct st_lsn {
  uint32 file_no;
  uint32 rec_offset;
} LSN;
#define maria_data_root "."
#endif

#define CONTROL_FILE_BASE_NAME "maria_control"
/*
  indicate absence of the log file number; first log is always number 1, 0 is
  impossible.
*/
#define CONTROL_FILE_IMPOSSIBLE_FILENO 0
/* logs always have a header */
#define CONTROL_FILE_IMPOSSIBLE_LOG_OFFSET 0
/*
  indicate absence of LSN.
*/
#define CONTROL_FILE_IMPOSSIBLE_LSN ((LSN){CONTROL_FILE_IMPOSSIBLE_FILENO,CONTROL_FILE_IMPOSSIBLE_LOG_OFFSET})

/* Here is the interface of this module */

/*
  LSN of the last checkoint
  (if last_checkpoint_lsn.file_no == CONTROL_FILE_IMPOSSIBLE_FILENO
  then there was never a checkpoint)
*/
extern LSN last_checkpoint_lsn;
/*
  Last log number (if last_logno ==
  CONTROL_FILE_IMPOSSIBLE_FILENO then there is no log file yet)
*/
extern uint32 last_logno;

/*
  Looks for the control file. If absent, it's a fresh start, create file.
  If present, read it to find out last checkpoint's LSN and last log.
  Called at engine's start.
*/
typedef enum enum_control_file_error {
  CONTROL_FILE_OK= 0,
  CONTROL_FILE_TOO_SMALL,
  CONTROL_FILE_TOO_BIG,
  CONTROL_FILE_BAD_MAGIC_STRING,
  CONTROL_FILE_BAD_CHECKSUM,
  CONTROL_FILE_UNKNOWN_ERROR /* any other error */
} CONTROL_FILE_ERROR;
CONTROL_FILE_ERROR ma_control_file_create_or_open();

/*
  Write information durably to the control file.
  Called when we have created a new log (after syncing this log's creation)
  and when we have written a checkpoint (after syncing this log record).
*/
#define CONTROL_FILE_UPDATE_ALL 0
#define CONTROL_FILE_UPDATE_ONLY_LSN 1
#define CONTROL_FILE_UPDATE_ONLY_LOGNO 2
int ma_control_file_write_and_force(const LSN *checkpoint_lsn, uint32 logno,
                                    uint objs_to_write);


/* Free resources taken by control file subsystem */
int ma_control_file_end();

#endif
