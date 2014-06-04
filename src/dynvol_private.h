/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 SwooshyCueb
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __DYNVOL_PRIVATE_H__
#define __DYNVOL_PRIVATE_H__

#include <glib.h>
#include "dynvol.h"

struct __attribute__((__packed__)) header
{
	gchar ident[4];
	guint32 val;
};

struct __attribute__((__packed__)) VBLK
{
	struct header header;
	gchar *data;
};

struct __attribute__((__packed__)) vols
{
	struct header header;
	GPtrArray *data;
};

struct __attribute__((__packed__)) volv
{
	struct header header;
	GArray *data;
};

struct __attribute__((__packed__)) vval
{
	guint32 field_1;
	guint32 field_2;
	guint32 field_3;
	guint32 field_4;
	guint8 endcap;
};

struct __attribute__((__packed__)) file
{
	gchar *name;
	guint32 size;
	gint32 packed_size;
	gboolean compressed;
	guint32 n_offset;
	guint32 b_offset;
	struct VBLK data;
};

struct __attribute__((__packed__)) footer
{
	struct vols unknown_vstr;
	struct volv unknown_vval;
	struct vols filenames;
	struct volv fileprops;
};


//This is VOL on the outside.
struct __attribute__((__packed__)) volume
{
	gchar *path;
	GIOChannel *volio;
	GArray *files;
	gboolean open;
	gboolean writable;
	struct header header;
	struct footer footer;
};

//Fetches metadata and stores it inside vol
void vol_getmetadata(VOL handle);

//Fetches volume footer and stores it inside vol
void vol_getfooter(VOL handle);

//Fetches a header
void vol_getheader(VOL handle, struct header *header, const guint32 offset);

//Functions for skipping the first two arrays in the footer
//Probably temporary
guint32 vol_getvstr(VOL handle, struct vols *vstrarr, const guint32 offset);
guint32 vol_getvval(VOL handle, struct volv *vvalarr, const guint32 offset);

//Functions for fetching file metadata
guint32 vol_getfilenames(VOL handle, struct vols *fnarr, const guint32 offset);
guint32 vol_getfileprops(VOL handle, struct volv *attrarr, const guint32 offset);


#endif