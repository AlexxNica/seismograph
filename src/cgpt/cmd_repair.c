// Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <getopt.h>
#include <string.h>

#include "blkid_utils.h"
#include "cgpt.h"
#include "vboot_host.h"

static void Usage(void)
{
  printf("\nUsage: %s repair [OPTIONS] DRIVE\n\n"
         "Repair damaged GPT headers and tables.\n\n"
         "Options:\n"
         "  -v           Verbose\n"
         "\n", progname);
}

int cmd_repair(int argc, char *argv[]) {
  CgptRepairParams params;
  memset(&params, 0, sizeof(params));

  int c;
  int r;
  int errorcnt = 0;
  uint32_t partition = 0;

  opterr = 0;                     // quiet, you
  while ((c=getopt(argc, argv, ":hv")) != -1)
  {
    switch (c)
    {
    case 'v':
      params.verbose++;
      break;

    case 'h':
      Usage();
      return CGPT_OK;
    case '?':
      Error("unrecognized option: -%c\n", optopt);
      errorcnt++;
      break;
    case ':':
      Error("missing argument to -%c\n", optopt);
      errorcnt++;
      break;
    default:
      errorcnt++;
      break;
    }
  }
  if (errorcnt)
  {
    Usage();
    return CGPT_FAILED;
  }

  if (optind >= argc)
  {
    Error("missing drive argument\n");
    return CGPT_FAILED;
  }

  params.drive_name = strdup(argv[optind]);

  r = translate_partition_dev(&params.drive_name, &partition);
  if (r != CGPT_OK)
    goto out;

  r = CgptRepair(&params);

out:
  free(params.drive_name);
  return r;
}
