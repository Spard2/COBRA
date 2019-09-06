#ifndef __MODULESPATCH_H__
#define __MODULESPATCH_H__

#include <lv2/process.h>
#include <lv2/thread.h>

#if defined(FIRMWARE_4_84) || defined(FIRMWARE_4_84DEX) || defined(FIRMWARE_4_85) || defined(FIRMWARE_4_85DEX)

#define VSH_DEX_HASH					0xb6b6d000002e6000
#define VSH_CEX_HASH					0xb6b6d000002e3000
#define BDP_DISC_CHECK_PLUGIN_HASH		0x9940000000003000
#define BASIC_PLUGINS_HASH				0x55f480000001f000
#define EXPLORE_PLUGIN_HASH				0xc50d0000000ec000
#define EXPLORE_CATEGORY_GAME_HASH		0xde52c00000056000
#define PS1_EMU_HASH					0xcc2840000009b000
#define PS1_NETEMU_HASH 				0xcc284000000be000
#define GAME_EXT_PLUGIN_HASH			0xcc2680000001e000
#define PSP_EMULATOR_HASH				0xcc29b00000023000
#define PEMUCORELIB_HASH				0x40425000000c0000
#define EMULATOR_API_HASH				0x8409f0000001b000
#define EMULATOR_DRM_HASH				0xbbb8800000005000
#define EMULATOR_DRM_DATA_HASH			0x2f3ab0000001b000
#define LIBSYSUTIL_SAVEDATA_PSP_HASH	0x0dfdc00000003000
#define LIBFS_EXTERNAL_HASH				0x05fd000000006000

// the following hashes are the values used on PS3HEN
//#define LIBAUDIO_HASH					0x2200600000002000
//#define NAS_PLUGIN_HASH				0xacf4af2b00027000
//#define BDP_BDMV_HASH					0x7be3b69b0037c000
//#define BDP_BDVD_HASH					0x7bf44e9200437000
//#define DOWNLOAD_PLUGIN_HASH			0xe274af7b00021000
//#define AUTODOWNLOAD_PLUGIN_HASH		0x00f86a7d00044000
//#define PREMO_PLUGIN_HASH				0x595f6f2b00012000
//#define PREMO_GAME_PLUGIN_HASH		0x010b867f00012000

/* vsh */
#define dex_vsh_text_size				0x6C0000
#define dex_ps2tonet_patch				0xCA380
#define dex_ps2tonet_size_patch 		0xCA374
#define dex_psp_drm_patch1				0x24A7E8
#define dex_psp_drm_patch2				0x24B288
#define dex_psp_drm_patch3				0x24AEC4
#define dex_psp_drm_patch4				0x24B6D0
#define dex_psp_drm_patchA				0x24A904
#define dex_psp_drm_patchB				0x24B1A4
#define dex_psp_drm_patchC				0x24A33C
#define dex_psp_drm_patchD				0x24A8EC
#define dex_psp_drm_patchE				0x24A8F0
#define dex_psp_drm_patchF				0x24B2BC
#define dex_vmode_patch_offset			0x44ADD0

#define cex_vsh_text_size				0x6B0000
#define cex_ps2tonet_patch				0xC4E34
#define cex_ps2tonet_size_patch 		0xC4E28
#define cex_psp_drm_patch1				0x242FC0
#define cex_psp_drm_patch2				0x243A60
#define cex_psp_drm_patch3				0x24369C
#define cex_psp_drm_patch4				0x243EA8
#define cex_psp_drm_patchA				0x2430DC
#define cex_psp_drm_patchB				0x24397C
#define cex_psp_drm_patchC				0x242B14
#define cex_psp_drm_patchD				0x2430C4
#define cex_psp_drm_patchE				0x2430C8
#define cex_psp_drm_patchF				0x243A94
#define cex_vmode_patch_offset			0x4431DC

/* basic_plugins */
//#define ps1emu_type_check_offset		0x20234
//#define pspemu_path_offset			0x4BBC8
//#define psptrans_path_offset			0x4C830

/* explore_plugin */
#define app_home_offset					0x2514C0
#define ps2_nonbw_offset				0xDDDC4
//#define whatsnew_offset				0x26E9B0

/* nas_plugin */
//#define patch1_nas					0x2eae4
//#define patch2_nas					0x2ec48
//#define patch3_nas					0x1F75C // Install ALL pkgs

/*Cinavia patch*/
//#define bdp_cinavia_patch				0x15ecd0
//#define bdp_cinavia1_patch			0x15ecfc

/* download_plugin */
//#define elf_patch1_download			0x42DC0 // 0x42e2c
//#define elf_patch2_download			0x42DC8 // 0x42e34
///// advanced download patches found by Devil303
//#define elf_patch3_download			0x4D628 // 0x4D708
//#define elf_patch4_download			0x4E448 // 0x4E528
//#define elf_patch5_download			0x4E468 // 0x4E548
//#define elf_patch6_download			0x4E544 // 0x4E624

/* autodownload_plugin */
//#define elf_patch1_autodownload		0x824B8 // 0x82650
//#define elf_patch2_autodownload		0x824C0 // 0x82658

/* premo_plugin */
//#define pcremote_play_offset1			0xb6fc

/* premo_game_plugin */
//#define pcremote_play_offset2			0xc8fc

/* libaudio */
//#define bt_usb_audio_offset			0xF80

/* explore_category_game */
#define ps2_nonbw_offset2				0x68264

/* bdp_disccheck_plugin */
#define dvd_video_region_check_offset	0x152C

/* ps1_emu */
#define ps1_emu_get_region_offset		0x3E74

/* ps1_netemu */
#define ps1_netemu_get_region_offset	0xA4E74 // seg003:00000000000A4E74                 lwzu      r11, 4(r4)

/* game_ext_plugin */
#define sfo_check_offset				0x23DAC
#define ps2_nonbw_offset3				0x172F0
#define ps_region_error_offset			0x687C
#define game_exit_popup_patch       	0xE418

/* psp_emulator */
#define psp_set_psp_mode_offset 		0x1C18

/* emulator_api */
#define psp_read						0x102D8
#define psp_read_header 				0x1125C
#define psp_drm_patch5					0x11080
#define psp_drm_patch6					0x110B0
#define psp_drm_patch7					0x110C8
#define psp_drm_patch8					0x110CC
#define psp_drm_patch9					0x1120C
#define psp_drm_patch11 				0x11210
#define psp_drm_patch12 				0x11220
#define psp_product_id_patch1			0x11320
#define psp_product_id_patch3			0x115F8
#define umd_mutex_offset				(0x64480+0x38C)

/* pemucorelib */
#define psp_eboot_dec_patch				0x5E6BC
#define psp_prx_patch					0x577D8
#define psp_savedata_bind_patch1		0x7A4BC
#define psp_savedata_bind_patch2		0x7A514
#define psp_savedata_bind_patch3		0x7A030
#define psp_extra_savedata_patch		0x87540
#define psp_prometheus_patch			0x12EA28
#define prx_patch_call_lr				0x5892C

/* emulator_drm */
#define psp_drm_tag_overwrite			0x4C68
#define psp_drm_key_overwrite			(0x27600-0xBE80)

/* libsysutil_savedata_psp */
#define psp_savedata_patch1				0x46CC
#define psp_savedata_patch2				0x46A4
#define psp_savedata_patch3				0x4504
#define psp_savedata_patch4				0x453C
#define psp_savedata_patch5				0x4550
#define psp_savedata_patch6				0x46B8

/* libfs */
#define aio_copy_root_offset			0xD658

#endif /* FIRMWARE */

typedef struct
{
	uint32_t offset;
	uint32_t data;
	uint8_t *condition;
} SprxPatch;

extern uint8_t condition_ps2softemu;
extern uint8_t condition_apphome;
extern uint8_t condition_psp_iso;
extern uint8_t condition_psp_dec;
extern uint8_t condition_psp_keys;
extern uint8_t condition_psp_change_emu;
extern uint8_t condition_psp_prometheus;
extern uint64_t vsh_check;

extern process_t vsh_process;
process_t get_vsh_process(void); //NzV
extern int vsh_type;

extern uint8_t safe_mode;

/* Functions for kernel */
void modules_patch_init(void);
void load_boot_plugins(void);
int prx_load_vsh_plugin(unsigned int slot, char *path, void *arg, uint32_t arg_size);
int prx_unload_vsh_plugin(unsigned int slot);

void load_boot_plugins_kernel(void);

int bc_to_net();

/* Syscalls */
int sys_prx_load_vsh_plugin(unsigned int slot, char *path, void *arg, uint32_t arg_size);
int sys_prx_unload_vsh_plugin(unsigned int slot);
int sys_thread_create_ex(sys_ppu_thread_t *thread, void *entry, uint64_t arg, int prio, uint64_t stacksize, uint64_t flags, const char *threadname);

// PS3Mapi v1.2.1
int ps3mapi_unload_vsh_plugin(char* name);
void unhook_all_modules(void);
int ps3mapi_get_vsh_plugin_info(unsigned int slot, char *name, char *filename);

#endif /* __MODULESPATCH_H__ */
