/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ***   To edit the content of this header, modify the corresponding
 ***   source file (e.g. under external/kernel-headers/original/) then
 ***   run bionic/libc/kernel/tools/update_all.py
 ***
 ***   Any manual change here will be lost the next time this script will
 ***   be run. You've been warned!
 ***
 ****************************************************************************
 ****************************************************************************/
#ifndef __DECON_FB_H__
#define __DECON_FB_H__

#define MAX_BUF_PLANE_CNT	3
typedef unsigned int u32;

struct decon_user_window {
	int x;
	int y;
};

struct s3c_fb_user_plane_alpha {
	int channel;
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

struct s3c_fb_user_chroma {
	int enabled;
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

struct s3c_fb_user_ion_client {
	int	fd[MAX_BUF_PLANE_CNT];
	int	offset;
};

enum decon_pixel_format {
	DECON_PIXEL_FORMAT_RGBA_8888 = 0,
	DECON_PIXEL_FORMAT_RGBX_8888 = 1,
	DECON_PIXEL_FORMAT_RGBA_5551 = 2,
	DECON_PIXEL_FORMAT_RGB_565 = 3,
	DECON_PIXEL_FORMAT_BGRA_8888 = 4,
	DECON_PIXEL_FORMAT_BGRX_8888 = 5,
	DECON_PIXEL_FORMAT_MAX = 6,
};

enum decon_blending {
	DECON_BLENDING_NONE = 0,
	DECON_BLENDING_PREMULT = 1,
	DECON_BLENDING_COVERAGE = 2,
	DECON_BLENDING_MAX = 3,
};

enum decon_idma_type {
	IDMA_G0 = 0,	/* Dedicated to WIN7 */
	IDMA_G1,
	IDMA_VG0,
	IDMA_VG1,
	IDMA_G2,
	IDMA_G3,
	IDMA_VGR0,
	IDMA_VGR1,
	ODMA_WB,
	IDMA_G0_S,
};

struct decon_win_rect {
	int x;
	int y;
	u32 w;
	u32 h;
};

struct decon_frame {
	int x;
	int y;
	u32 w;
	u32 h;
	u32 f_w;
	u32 f_h;
};

struct decon_win_config {
	enum {
		DECON_WIN_STATE_DISABLED = 0,
		DECON_WIN_STATE_COLOR,
		DECON_WIN_STATE_BUFFER,
		DECON_WIN_STATE_UPDATE,
	} state;

	/* Reusability:This struct is used for IDMA and ODMA */
	union {
		__u32 color;
		struct {
			int				fd_idma[3];
			int 				fd;
			__u32				offset;
			__u32				stride;
			int				fence_fd;
			int				plane_alpha;
			enum decon_blending		blending;
			enum decon_idma_type		idma_type;
			enum decon_pixel_format		format;
			/* no read area of IDMA */
			struct decon_win_rect		block_area;
			struct decon_win_rect		transparent_area;
			struct decon_win_rect		opaque_area;
			/* source framebuffer coordinates */
			struct decon_frame		src;
		};
 	};

	int x;
	int y;
	__u32 w;
	__u32 h;
	/* destination OSD coordinates */
	struct decon_frame dst;
	bool protection;
	bool compression;
};

#define MAX_DECON_WIN (8)
#define DECON_WIN_UPDATE_IDX	(8)
#define DEV_DECON	6
struct decon_win_config_data {
	int	fence;
	int	fd_odma;
	struct decon_win_config config[MAX_DECON_WIN + 1];
};
/* IOCTL commands */
#define S3CFB_WIN_POSITION		_IOW('F', 203, \
						struct decon_user_window)
#define S3CFB_WIN_SET_PLANE_ALPHA	_IOW('F', 204, \
						struct s3c_fb_user_plane_alpha)
#define S3CFB_WIN_SET_CHROMA		_IOW('F', 205, \
						struct s3c_fb_user_chroma)
#define S3CFB_SET_VSYNC_INT		_IOW('F', 206, __u32)

#define S3CFB_GET_ION_USER_HANDLE	_IOWR('F', 208, \
						struct s3c_fb_user_ion_client)
#define S3CFB_WIN_CONFIG		_IOW('F', 209, \
						struct decon_win_config_data)
#define S3CFB_WIN_PSR_EXIT		_IOW('F', 210, int)

#define DECON_IOC_LPD_EXIT_LOCK		_IOW('L', 0, u32)
#define DECON_IOC_LPD_UNLOCK		_IOW('L', 1, u32)
#ifdef CONFIG_LCD_DOZE_MODE
#define S3CFB_POWER_MODE		_IOW('F', 223, __u32)
enum disp_pwr_mode {
	DECON_POWER_MODE_OFF = 0,
	DECON_POWER_MODE_DOZE,
	DECON_POWER_MODE_NORMAL,
	DECON_POWER_MODE_DOZE_SUSPEND,
};
#endif
#endif
