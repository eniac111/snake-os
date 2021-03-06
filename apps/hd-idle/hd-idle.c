/*
 * hd-idle.c - external disk idle daemon
 *
 * Copyright (c) 2007 Christian Mueller.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * hd-idle is a utility program for spinning-down external disks after a period
 * of idle time. Since most external IDE disk enclosures don't support setting
 * the IDE idle timer, a program like hd-idle is required to spin down idle
 * disks automatically.
 *
 * A word of caution: hard disks don't like spinning-up too often. Laptop disks
 * are more robust in this respect than desktop disks but if you set your disks
 * to spin down after a few seconds you may damage the disk over time due to the
 * stress the spin-up causes on the spindle motor and bearings. It seems that
 * manufacturers recommend a minimum idle time of 3-5 minutes, the default in
 * hd-idle is 10 minutes.
 *
 * Please note that hd-idle can spin down any disk accessible via the SCSI
 * layer (USB, IEEE1394, ...) but it will NOT work with real SCSI disks because
 * they don't spin up automatically. Thus it's not called scsi-idle and I don't
 * recommend using it on a real SCSI system unless you have a kernel patch that
 * automatically starts the SCSI disks after receiving a sense buffer indicating
 * the disk has been stopped. Without such a patch, real SCSI disks won't start
 * again and you can as well pull the plug.
 *
 * You have been warned...
 *
 * CVS Change Log:
 * ---------------
 *
 * $Log: hd-idle.c,v $
 * Revision 1.2  2007/04/23 22:14:27  cjmueller
 * Bug fixes
 * - Comment changes; no functionality changes...
 *
 * Revision 1.1.1.1  2007/04/23 21:49:43  cjmueller
 * initial import into CVS
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <scsi/scsi.h>

#define STAT_FILE "/proc/diskstats"
#define dprintf if (debug) printf

/* typedefs and structures */
typedef struct DISKSTATS {
  struct DISKSTATS  *next;
  char               name[50];
  time_t             last_io;
  time_t             spindown;
  time_t             spinup;
  unsigned int       spun_down : 1;
  unsigned int       reads;
  unsigned int       writes;
} DISKSTATS;

/* function prototypes */
static void        daemonize       (void);
static DISKSTATS  *get_diskstats   (const char *name);
static void        spindown_disk   (const char *name);
static void        log_spinup      (DISKSTATS *ds);

/* global/static variables */
DISKSTATS *ds_root;
char *logfile = "/dev/null";
int debug;

/* main function */
int main(int argc, char *argv[])
{
  int have_logfile = 0;
  int idle_time = 600;
  int sleep_time;
  int opt;

  /* process command line options */
  while ((opt = getopt(argc, argv, "t:i:l:dh")) != -1) {
    switch (opt) {

    case 't':
      /* just spin-down the specified disk and exit */
      debug = 1;
      spindown_disk(optarg);
      return(0);

    case 'i':
      idle_time = atoi(optarg);
      break;

    case 'l':
      logfile = optarg;
      have_logfile = 1;
      break;

    case 'd':
      debug = 1;
      break;

    case 'h':
      printf("usage: hd-idle [-t <disk>] [-i <idle_time>] [-l <logfile>] [-d] [-h]\n");
      return(0);

    case ':':
      fprintf(stderr, "error: option -%c requires an argument\n", optopt);
      return(1);

    case '?':
      fprintf(stderr, "error: unknown option -%c\n", optopt);
      return(1);
    }
  }

  /* set sleep interval */
  if ((sleep_time = idle_time / 10) == 0) {
    sleep_time = 1;
  }

  /* daemonize unless we're running in debug mode */
  if (!debug) {
    daemonize();
  }

  /* main loop: probe for idle disks and stop them */
  for (;;) {
    DISKSTATS tmp;
    FILE *fp;
    char buf[200];

    if ((fp = fopen(STAT_FILE, "r")) == NULL) {
      perror(STAT_FILE);
      return(2);
    }

    memset(&tmp, 0x00, sizeof(tmp));

    while (fgets(buf, sizeof(buf), fp) != NULL) {
      if (sscanf(buf, "%*d %*d %s %*u %*u %u %*u %*u %*u %u %*u %*u %*u %*u",
                 tmp.name, &tmp.reads, &tmp.writes) == 3) {
        DISKSTATS *ds;
        time_t now = time(NULL);

        /* make sure this is a SCSI disk (sd[a-z]) */
        if (tmp.name[0] != 's' ||
            tmp.name[1] != 'd' ||
            !isalpha(tmp.name[2]) ||
            tmp.name[3] != '\0') {
          continue;
        }

        dprintf("probing %s: reads: %d, writes: %d\n", tmp.name, tmp.reads, tmp.writes);

        /* get previous statistics for this disk */
        ds = get_diskstats(tmp.name);

        if (ds == NULL) {
          /* new disk; just add it to the linked list */
          ds = malloc(sizeof(*ds));
          memcpy(ds, &tmp, sizeof(*ds));
          ds->last_io = time(NULL);
          ds->spinup = ds->last_io;
          ds->next = ds_root;
          ds_root = ds;

        } else if (ds->reads == tmp.reads && ds->writes == tmp.writes) {
          if (!ds->spun_down) {
            /* no activity on this disk and still running */
            if (now - ds->last_io > idle_time) {
              spindown_disk(ds->name);
              ds->spindown = time(NULL);
              ds->spun_down = 1;
            }
          }

        } else {
          /* disk had some activity */
          if (ds->spun_down) {
            /* disk was spun down, thus it has just spun up */
            if (have_logfile) {
              log_spinup(ds);
            }
            ds->spinup = time(NULL);
          }
          ds->reads = tmp.reads;
          ds->writes = tmp.writes;
          ds->last_io = time(NULL);
          ds->spun_down = 0;
        }
      }
    }

    fclose(fp);
    sleep(sleep_time);
  }

  return(0);
}

/* become a daemon */
static void daemonize(void)
{
  int maxfd;
  int i;

  /* fork #1: exit parent process and continue in the background */
  if ((i = fork()) < 0) {
    perror("couldn't fork");
    exit(2);
  } else if (i > 0) {
    _exit(0);
  }

  /* fork #2: detach from terminal and fork again so we can never regain
   * access to the terminal */
  setsid();
  if ((i = fork()) < 0) {
    perror("couldn't fork #2");
    exit(2);
  } else if (i > 0) {
    _exit(0);
  }

  /* change to root directory and close file descriptors */
  chdir("/");
  maxfd = getdtablesize();
  for (i = 0; i < maxfd; i++) {
    close(i);
  }

  /* use /dev/null for stdin, stdout and stderr */
  open("/dev/null", O_RDONLY);
  open("/dev/null", O_WRONLY);
  open("/dev/null", O_WRONLY);
}

/* get DISKSTATS entry by name of disk */
static DISKSTATS *get_diskstats(const char *name)
{
  DISKSTATS *ds;

  for (ds = ds_root; ds != NULL; ds = ds->next) {
    if (!strcmp(ds->name, name)) {
      return(ds);
    }
  }

  return(NULL);
}

/* spin-down a disk */
static void spindown_disk(const char *name)
{
  struct sg_io_hdr io_hdr;
  unsigned char sense_buf[255];
  char dev_name[100];
  int fd;
  unsigned char stop_disk[] = { 0x1b, 0, 0, 0, 0, 0};

  dprintf("spindown: %s\n", name);

  /* fabricate SCSI IO request */
  memset(&io_hdr, 0x00, sizeof(io_hdr));
  io_hdr.interface_id = 'S';
  io_hdr.dxfer_direction = SG_DXFER_NONE;

  /* SCSI stop unit command */
  io_hdr.cmdp = stop_disk;

  io_hdr.cmd_len = 6;
  io_hdr.sbp = sense_buf;
  io_hdr.mx_sb_len = (unsigned char) sizeof(sense_buf);

  /* open disk device (kernel 2.4 will probably need "sg" names here) */
  snprintf(dev_name, sizeof(dev_name), "/dev/%s", name);
  dprintf("spindown: %s\n", dev_name);
  if ((fd = open(dev_name, O_WRONLY)) < 0) {
    perror(dev_name);
    return;
  }

  /* execute SCSI request */
  if (ioctl(fd, SG_IO, &io_hdr) < 0) {
    char buf[100];
    snprintf(buf, sizeof(buf), "ioctl on %s:", name);
    perror(buf);

  } else if (io_hdr.masked_status != 0) {
    fprintf(stderr, "error: SCSI command failed with status 0x%02x\n",
            io_hdr.masked_status);
  }

  close(fd);
}

/* write a spin-up event message to the log file */
static void log_spinup(DISKSTATS *ds)
{
  FILE *fp;

  if ((fp = fopen(logfile, "a")) != NULL) {
    /* Print statistics to logfile
     *
     * Note: This doesn't work too well if there are multiple disks
     *       because the I/O we're dealing with might be on another
     *       disk so we effectively wake up the disk the log file is
     *       stored on as well. Then again the logfile is a debugging
     *       option, so what...
     */
    time_t now = time(NULL);
    char tstr[20];
    char dstr[20];

    strftime(dstr, sizeof(dstr), "%Y-%m-%d", localtime(&now));
    strftime(tstr, sizeof(tstr), "%H:%M:%S", localtime(&now));
    fprintf(fp,
            "date: %s, time: %s, disk: %s, running: %ld, stopped: %ld\n",
            dstr, tstr, ds->name,
            (long) ds->spindown - (long) ds->spinup,
            (long) time(NULL) - (long) ds->spindown);

    /* Sync to make sure writing to the logfile won't cause another
     * spinup in 30 seconds (or whatever bdflush uses as flush interval).
     * Since there's already some I/O which caused the spin-up and we don't
     * want to cause even more I/O, wait some time before doing this.
     */
    fclose(fp);
    sleep(3);
    sync();
  }
}
