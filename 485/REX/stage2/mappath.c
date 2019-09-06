#include <lv2/lv2.h>
#include <lv2/libc.h>
#include <lv2/memory.h>
#include <lv2/patch.h>
#include <lv2/syscall.h>
#include <lv2/security.h>
#include <lv2/io.h>
#include <lv2/error.h>
#include "common.h"
#include "mappath.h"
#include "modulespatch.h"
#include "syscall8.h"
#include "ps3mapi_core.h"

#define MAX_TABLE_ENTRIES 32

typedef struct _MapEntry
{
	char *oldpath;
	char *newpath;
	int16_t oldpath_len;
	int16_t newpath_len;
	uint32_t flags;
} MapEntry;

MapEntry map_table[MAX_TABLE_ENTRIES];

#define MAX_PATH_OLD	128
#define MAX_PATH_NEW	384

// TODO: map_path and open_path_hook should be mutexed...

f_desc_t open_path_callback;

uint8_t photo_gui = 1;

static int8_t avoid_recursive_calls = 0;
static int8_t max_table_entries = 0;
static int8_t first_slot = 0;
int8_t condition_apphome_index = 0;

static uint8_t *path_entries = NULL;

#define BASE_OLDPATH	(path_entries)
#define BASE_NEWPATH	(path_entries + (MAX_TABLE_ENTRIES * MAX_PATH_OLD))


static void init_map_entry(uint8_t index)
{
	map_table[index].oldpath = NULL;
	map_table[index].newpath = NULL;
	map_table[index].oldpath_len = 0;
	map_table[index].newpath_len = 0;
	map_table[index].flags = 0;
}

void map_first_slot(char *oldpath, char *newpath)
{
	first_slot = 0;

	init_map_entry(0);

	if(!oldpath || !newpath) return;

	map_table[0].oldpath = oldpath;
	map_table[0].oldpath_len = strlen(oldpath);

	map_table[0].newpath = newpath;
	map_table[0].newpath_len = strlen(newpath);

	first_slot = 1;
	if(condition_apphome_index == 0) condition_apphome_index = first_slot;

	return;
}

int map_path(char *oldpath, char *newpath, uint32_t flags)
{
	int8_t i, firstfree = -1, is_dev_bdvd = 0;

	if (!oldpath || *oldpath == 0)
		return FAILED;

	#ifdef DEBUG
	//DPRINTF("Map path: %s -> %s\n", oldpath, newpath);
	#endif

	// init array
	if(!path_entries)
	{
		for(int8_t i = first_slot; i < MAX_TABLE_ENTRIES; i++) init_map_entry(i);

		if(!path_entries) page_allocate_auto(NULL, (MAX_TABLE_ENTRIES * (MAX_PATH_OLD + MAX_PATH_NEW)), 0x2F, (void **)&path_entries);
		if(!path_entries) return EKRESOURCE;
	}

	// unmap if both paths are the same
	if (newpath && strcmp(oldpath, newpath) == 0)
		newpath = NULL;

	// sys_aio_copy_root
	if (strcmp(oldpath, "/dev_bdvd") == 0)
	{
		condition_apphome = (newpath != NULL);
		condition_apphome_index = first_slot, is_dev_bdvd = 1; //AV 20190405
	}

	for (i = first_slot; i < max_table_entries; i++)
	{
		if (map_table[i].oldpath)
		{
			if (strcmp(oldpath, map_table[i].oldpath) == 0)
			{
				if (newpath && (*newpath != 0))
				{
					int len = strlen(newpath); if(len >= MAX_PATH_NEW) return FAILED;

					// replace mapping
					strncpy(map_table[i].newpath, newpath, len);
					map_table[i].newpath[len] = 0;
					map_table[i].newpath_len = len;
					map_table[i].flags = (map_table[i].flags & FLAG_COPY) | (flags & (~FLAG_COPY));

					if(is_dev_bdvd) condition_apphome_index = i;
				}
				else
				{
					// delete mapping
					init_map_entry(i);

					// find last entry
					for(int8_t n = max_table_entries - 1; n >= first_slot; n--) {if(map_table[n].oldpath) break; else if(max_table_entries > 0) max_table_entries--;}
				}

				return SUCCEEDED;
			}
		}
		else if (firstfree < 0)
		{
			firstfree = i;
		}
	}

	if (firstfree < 0) firstfree = max_table_entries;

	// add new mapping
	if (firstfree < MAX_TABLE_ENTRIES)
	{
		if (!newpath || *newpath == 0)
			return SUCCEEDED;

		int newpath_len = strlen(newpath); if(newpath_len >= MAX_PATH_NEW) return FAILED;

		int len = strlen(oldpath); if(len >= MAX_PATH_OLD) return FAILED;

		map_table[firstfree].oldpath_len = len;

		map_table[firstfree].flags = flags;

		if (flags & FLAG_COPY)
		{
			map_table[firstfree].oldpath = (void*)(BASE_OLDPATH + (firstfree * MAX_PATH_OLD));
			strncpy(map_table[firstfree].oldpath, oldpath, len);
			map_table[firstfree].oldpath[len] = 0;
		}
		else
		{
			map_table[firstfree].oldpath = oldpath;
		}

		map_table[firstfree].newpath = (void*)(BASE_NEWPATH + (firstfree * MAX_PATH_NEW));
		strncpy(map_table[firstfree].newpath, newpath, newpath_len);
		map_table[firstfree].newpath[newpath_len] = 0;
		map_table[firstfree].newpath_len = newpath_len;

		if(is_dev_bdvd) condition_apphome_index = firstfree;

		max_table_entries++;
		return SUCCEEDED;
	}

	return FAILED; // table entries is full
}

int map_path_user(char *oldpath, char *newpath, uint32_t flags)
{
	char *oldp, *newp;

	#ifdef DEBUG
	//DPRINTF("map_path_user, called by process %s: %s -> %s\n", get_process_name(get_current_process_critical()), oldpath, newpath);
	#endif

	if (oldpath == 0)
		return FAILED;

	int ret = pathdup_from_user(get_secure_user_ptr(oldpath), &oldp);
	if (ret != 0)
		return ret;

	if (newpath == 0)
	{
		newp = NULL;
	}
	else
	{
		ret = pathdup_from_user(get_secure_user_ptr(newpath), &newp);
		if (ret != 0)
		{
			dealloc(oldp, 0x27);
			return ret;
		}
	}

	ret = map_path(oldp, newp, flags | FLAG_COPY);

	dealloc(oldp, 0x27);
	if (newp)
		dealloc(newp, 0x27);

	return ret;
}

LV2_SYSCALL2(int, sys_map_path, (char *oldpath, char *newpath))
{
	extend_kstack(0);
	return map_path_user(oldpath, newpath, 0);
}

int get_map_path(unsigned int num, char *path, char *new_path)
{
	if(num == 0xFFFF) return MAX_TABLE_ENTRIES; // return max remaps

	if(num >= max_table_entries) return max_table_entries; // return last index (e.g. num = 0xFFF)

	if(map_table[num].oldpath_len == 0 || map_table[num].newpath_len == 0 || !map_table[num].newpath || !map_table[num].oldpath || !path) return FAILED;

	copy_to_user(&path, get_secure_user_ptr(map_table[num].oldpath), map_table[num].oldpath_len);

	if(!new_path) return SUCCEEDED;
	copy_to_user(&new_path, get_secure_user_ptr(map_table[num].newpath), map_table[num].newpath_len);

	return SUCCEEDED;
}

int sys_map_paths(char *paths[], char *new_paths[], unsigned int num)
{
	if(num > MAX_TABLE_ENTRIES) num = MAX_TABLE_ENTRIES;

	uint32_t *u_paths = (uint32_t *)get_secure_user_ptr(paths);
	uint32_t *u_new_paths = (uint32_t *)get_secure_user_ptr(new_paths);

	int unmap = 0;
	int ret = 0;

	if (!u_paths)
	{
		unmap = 1;
	}
	else
	{
		if (!u_new_paths)
			return EINVAL;

		for (unsigned int i = 0; i < num; i++)
		{
			ret = map_path_user((char *)(uint64_t)u_paths[i], (char *)(uint64_t)u_new_paths[i], FLAG_TABLE);
			if (ret != 0)
			{
				unmap = 1;
				break;
			}
		}
	}

	if (unmap)
	{
		for (int8_t i = first_slot; i < max_table_entries; i++)
		{
			if (map_table[i].flags & FLAG_TABLE) init_map_entry(i);
		}

		max_table_entries = 0;
	}

	return ret;
}

static uint8_t libft2d_access = 0;

#ifdef DO_REACTPSN
#include "make_rif.h"
#endif
#include "homebrew_blocker.h"

LV2_HOOKED_FUNCTION_POSTCALL_2(void, open_path_hook, (char *path0, int mode))
{
	if (*path0 == '/')
	{
		if(avoid_recursive_calls) return;
		avoid_recursive_calls = 1;

		if(open_path_callback.addr)
		{
			int (*callback)(char *);
			callback = (void *)&open_path_callback;
			if(callback(path0)) return;
		}

		if(block_homebrew(path0)) {avoid_recursive_calls = 0; return;}

		char *path = path0;
		if(path[1] == '/') path++; if(!path) {avoid_recursive_calls = 0; return;}

		// Disabled due to the issue with multiMAN can't copy update files from discs.
		/*if (path && ((strcmp(path, "/dev_bdvd/PS3_UPDATE/PS3UPDAT.PUP") == 0)))  // Blocks FW update from Game discs!
		{
			char not_update[40];
			sprintf(not_update, "/dev_bdvd/PS3_NOT_UPDATE/PS3UPDAT.PUP");
			set_patched_func_param(1, (uint64_t)not_update);
			#ifdef DEBUG
			DPRINTF("Update from disc blocked!\n");
			#endif
			return;
		}
		*/

		if(*path == '/')
		{
			//DPRINTF("?: [%s]\n", path);

			////////////////////////////////////////////////////////////////////////////////////
			// Photo_GUI integration with webMAN MOD - DeViL303 & AV                          //
			////////////////////////////////////////////////////////////////////////////////////
			if(!libft2d_access)
			{
				libft2d_access = photo_gui && !strcmp(path, "/dev_flash/sys/internal/libft2d.sprx");
			}
			else
			{
				libft2d_access = 0;

				if(!strncmp(path, "/dev_hdd0/photo/", 16))
				{
					char *photo = path + 16; size_t len = strlen(photo);
					if (len < 8) ;
					else if(!strcmp(photo + len -4, ".PNG") || !strcmp(photo + len -4, ".JPG") || !strcmp(photo + len -8, "_COV.JPG") || !strncasecmp(photo + len -8, ".iso.jpg", 8) || !strncasecmp(photo + len -8, ".iso.png", 8))
					{
						#ifdef DEBUG
						DPRINTF("CREATING /dev_hdd0/tmp/wm_request\n");
						#endif
						int fd;
						if(cellFsOpen("/dev_hdd0/tmp/wm_request", CELL_FS_O_CREAT | CELL_FS_O_WRONLY | CELL_FS_O_TRUNC, &fd, 0666, NULL, 0) == 0)
						{
							cellFsWrite(fd, path, (len + 16), NULL);
							cellFsClose(fd);
						}
					}
				}
			}
			////////////////////////////////////////////////////////////////////////////////////

			for (int8_t i = max_table_entries - 1; i >= 0; i--)
			{
				if (map_table[i].oldpath)
				{
					int16_t len = map_table[i].oldpath_len;

					if(strncmp(path, map_table[i].oldpath, len) == 0)
					{
						strcpy(map_table[i].newpath + map_table[i].newpath_len, path + len);

						// -- AV: use partial folder remapping when newpath starts with double '/' like //dev_hdd0/blah...
						if(map_table[i].newpath[1] == '/')
						{
							CellFsStat stat;
							if( (cellFsStat(map_table[i].newpath, &stat) != 0) && (cellFsStat(path0, &stat) == 0) )
							{
								#ifdef DEBUG
								DPRINTF("open_path %s\n", path0);
								#endif
								avoid_recursive_calls = 0;
								return; // Do not remap / Use the original file when redirected file does not exist
							}
						}

						set_patched_func_param(1, (uint64_t)map_table[i].newpath);

						#ifdef DEBUG
						DPRINTF("open_path %s\n", map_table[i].newpath);
						#endif
						avoid_recursive_calls = 0;
						return;
					}
				}
			}
		}

		#ifdef DEBUG
		DPRINTF("open_path %s\n", path0);
		#endif

		avoid_recursive_calls = 0;
	}
}

int sys_aio_copy_root(char *src, char *dst)
{
	src = get_secure_user_ptr(src);
	dst = get_secure_user_ptr(dst);

	// Begin original function implementation
	if (!src || !dst)
		return EFAULT;

	if (src[0] != '/') return EINVAL;

	int len;
	len = strlen(src);

	if (len >= MAX_PATH_OLD || len <= 1)
		return EINVAL;

	strcpy(dst, src);

	// Get device name
	for (int i = 1; i < len; i++)
	{
		if (dst[i] == 0)
			break;

		if (dst[i] == '/')
		{
			dst[i] = 0;
			break;
		}

		if(i >= 0x10) return EINVAL;
	}

	// Here begins custom part of the implementation
	if (condition_apphome && (strcmp(dst, "/dev_bdvd") == 0)) // if dev_bdvd and jb game mounted
	{
		// find /dev_bdvd
		for (int8_t i = condition_apphome_index; i < max_table_entries; i++)
		{
			if (map_table[i].oldpath && strcmp(map_table[i].oldpath, "/dev_bdvd") == SUCCEEDED)
			{
				for (int j = 1; j < map_table[i].newpath_len; j++)
				{
					dst[j] = map_table[i].newpath[j];

					if (dst[j] == 0)
						break;

					if (dst[j] == '/' && (j >= 9))
					{
						dst[j] = 0;
						break;
					}
				}

				#ifdef DEBUG
				DPRINTF("AIO: root replaced by %s\n", dst);
				#endif

				break;
			}
		}
	}

	return SUCCEEDED;
}

void unhook_all_map_path(void)
{
	suspend_intr();
	unhook_function_with_postcall(open_path_symbol, open_path_hook, 2);
	resume_intr();
}

void map_path_patches(int syscall)
{
	hook_function_with_postcall(open_path_symbol, open_path_hook, 2);

	open_path_callback.addr = NULL;

	if (syscall)
		create_syscall2(SYS_MAP_PATH, sys_map_path);
}
